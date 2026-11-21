#!/usr/bin/env python3
"""Check that library.properties, library.json and idf_component.yml carry the
same version, and enforce the release rules against the existing git tags.

Usage:
    python scripts/check_versions.py                          # branch mode, working tree
    python scripts/check_versions.py --ref <commit>           # branch mode, manifests as stored in <commit>
    python scripts/check_versions.py 1.2.3 [--ref <commit>]   # tag mode

Rules
  all modes  the three manifests carry the same, valid semantic version (X.Y.Z[-pre]).
  branch     that version must be STRICTLY GREATER than the highest existing tag:
             bump all three manifests right after a release, before pushing anything else.
  tag        the tag name must equal the manifest version (a leading "v" is ignored)
             and must be greater than every other existing tag.
Tags that are not semantic versions are ignored. With no tags at all there is no
limit (first release).
Unusual version strings (see PlatformIO limits below) only produce warnings.

Exit code 0 = OK, 1 = a rule is broken or a manifest is unreadable.
Standard library only.
"""
import argparse
import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

SEMVER = re.compile(r"^v?(\d+)\.(\d+)\.(\d+)(?:-([0-9A-Za-z.-]+))?(?:\+[0-9A-Za-z.-]+)?$")


def from_properties(text: str) -> str:
    m = re.search(r"^version\s*=\s*(\S+)\s*$", text, re.M)
    if not m:
        raise ValueError("no 'version=' line")
    return m.group(1)


def from_json(text: str) -> str:
    return str(json.loads(text)["version"])


def from_yaml(text: str) -> str:
    # Top-level key only (no indentation): dependency versions are indented
    # under 'dependencies:' and must not be picked up.
    m = re.search(r"^version:\s*[\"']?([^\"'\s#]+)", text, re.M)
    if not m:
        raise ValueError("no top-level 'version:' key")
    return m.group(1)


MANIFESTS = [
    ("library.properties", from_properties),
    ("library.json", from_json),
    ("idf_component.yml", from_yaml),
]


# PlatformIO's documentation (older pages) says a library version may only contain
# a-z, digits, dots and dashes, up to 20 characters. Current `pio pkg publish` does
# not seem to enforce it, so these are warnings only: they never fail the check.
PIO_ALLOWED = re.compile(r"[a-z0-9.-]")
PIO_MAX_LEN = 20


def platformio_warnings(version):
    out = []
    bad = sorted({c for c in version if not PIO_ALLOWED.match(c)})
    if bad:
        out.append(
            f"WARNING  version '{version}' contains unusual characters ({' '.join(bad)}): "
            "PlatformIO's docs only allow a-z, 0-9, '.' and '-' "
            "(and ESP ignores anything after '+' when comparing versions)"
        )
    if len(version) > PIO_MAX_LEN:
        out.append(
            f"WARNING  version '{version}' is {len(version)} characters long: "
            f"PlatformIO's docs set a limit of {PIO_MAX_LEN}"
        )
    return out


def git(*cmd):
    return subprocess.run(["git", *cmd], cwd=ROOT, capture_output=True, text=True, encoding="utf-8")


def resolve_commit(ref):
    out = git("rev-parse", "--verify", "--quiet", f"{ref}^{{commit}}")
    return out.stdout.strip() if out.returncode == 0 else None


def short_commit(commit):
    """Abbreviated hash as Git shows it (at least 7 characters, longer only if ambiguous)."""
    out = git("rev-parse", "--short=7", commit)
    return out.stdout.strip() if out.returncode == 0 and out.stdout.strip() else commit[:7]


def read_text(name, commit):
    if commit is None:
        return (ROOT / name).read_text(encoding="utf-8")
    out = git("show", f"{commit}:{name}")
    if out.returncode != 0:
        raise FileNotFoundError(out.stderr.strip() or "not in commit")
    return out.stdout


def parse_semver(text):
    """Sort key following semver precedence (a prerelease is lower than its release)."""
    m = SEMVER.match(text.strip())
    if not m:
        return None
    major, minor, patch = (int(x) for x in m.group(1, 2, 3))
    pre = m.group(4)
    if pre is None:
        pre_key = (1,)
    else:
        ids = tuple((0, int(p), "") if p.isdigit() else (1, 0, p) for p in pre.split("."))
        pre_key = (0, ids)
    return (major, minor, patch, pre_key)


def semver_tags():
    """[(key, name)] for the existing tags that are semantic versions; None if git is unavailable."""
    try:
        out = git("tag", "--list")
    except FileNotFoundError:
        return None
    if out.returncode != 0:
        return None
    result = []
    for name in out.stdout.split():
        key = parse_semver(name)
        if key is not None:
            result.append((key, name))
    return result


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("tag", nargs="?", default="")
    parser.add_argument("--ref", default=None)
    args = parser.parse_args()
    tag = args.tag.strip() or None
    tag_version = tag[1:] if tag and tag.startswith("v") else tag

    commit = None
    if args.ref:
        commit = resolve_commit(args.ref)
        if commit is None:
            print(f"ERROR  cannot resolve '{args.ref}' to a commit")
            return 1

    found, problems = {}, []
    for name, reader in MANIFESTS:
        try:
            found[name] = reader(read_text(name, commit))
        except FileNotFoundError as exc:
            problems.append(f"{name}: file not found" + (f" ({exc})" if commit else ""))
        except Exception as exc:  # unreadable or malformed manifest
            problems.append(f"{name}: {exc}")

    rows = dict(found)
    notes = []
    versions = set(found.values())
    if len(versions) > 1:
        problems.append("the manifests carry different versions")

    if tag_version is not None:
        rows["tag"] = tag_version
        if len(versions) == 1 and tag_version not in versions:
            problems.append(f"the tag ({tag_version}) differs from the manifests ({next(iter(versions))})")

    # Release rules against the existing tags (only when the manifests agree).
    if len(versions) == 1 and not problems:
        version = next(iter(versions))
        key = parse_semver(version)
        notes.extend(platformio_warnings(version))
        tags = semver_tags()
        if key is None:
            problems.append(f"version '{version}' is not a valid semantic version (X.Y.Z)")
        elif tags is None:
            notes.append("WARNING  git tags unavailable: release rules skipped")
        elif tag_version is not None:
            tkey = parse_semver(tag_version)
            if tkey is None:
                problems.append(f"tag '{tag_version}' is not a valid semantic version (X.Y.Z)")
            else:
                others = [(k, n) for k, n in tags if k != tkey]
                if others:
                    top_key, top_name = max(others)
                    rows["highest other tag"] = top_name
                    if tkey <= top_key:
                        problems.append(f"tag {tag_version} must be greater than the highest other tag ({top_name})")
        else:
            if tags:
                top_key, top_name = max(tags)
                rows["highest tag"] = top_name
                if key <= top_key:
                    problems.append(
                        f"version {version} must be greater than the highest tag ({top_name}): "
                        "bump all three manifests before pushing"
                    )

    print(f"Commit {short_commit(commit)} reports the following versions:" if commit
          else "Working tree reports the following versions:")
    width = max((len(k) for k in rows), default=0)
    for name, value in rows.items():
        print(f"{name:<{width}}  {value}")
    for note in notes:
        print(note)
    for problem in problems:
        print(f"ERROR  {problem}")

    if problems:
        print("\nCheck FAILED.")
        return 1
    print("\nCheck passed.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
