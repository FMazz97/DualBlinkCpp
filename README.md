# DualBlinkCpp

A minimal, non-blocking blink library that runs natively on **Arduino** and **ESP-IDF**. It is the reference example of the [Dual-Framework Library Guide](https://github.com/FMazz97/dual-framework-library-guide).

It does one thing, blinking a pin without blocking, so that the structure stays visible: one framework-agnostic core, a compile-time HAL for each framework, and an Arduino facade. The file-by-file walkthrough is in [Appendix E](https://github.com/FMazz97/dual-framework-library-guide/blob/main/appendix/E-minimal-example.md) of the guide.

> [!NOTE]
> This repository is an example and is not published to any registry: package names are permanent once published (see [5.4](https://github.com/FMazz97/dual-framework-library-guide/blob/main/chapters/05-publishing.md#54-the-case-sensitivity-trap-platformios-immutable-package-binding) of the guide).
>
> **Tested with:** Arduino IDE 2.3.x, PlatformIO (`framework = arduino` and `framework = espidf`, `espressif32@6.4.0` with IDF 5.1.1) and native ESP-IDF v6.1, on an Arduino Nano and on an ESP32 (`esp32dev`) mounted on a [Cheap Yellow Display](https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display) board.

---

## Layout

```text
dual_blink_cpp/                      ← repository root = component root
├── CMakeLists.txt                   ← ESP-IDF
├── idf_component.yml                ← ESP-IDF
├── library.properties               ← Arduino
├── library.json                     ← PlatformIO
├── src/
│   ├── DualBlinkCpp.h               ← Arduino facade
│   ├── core/                        ← framework-agnostic core
│   │   ├── dual_blink_cpp.h
│   │   └── dual_blink_cpp.cpp
│   └── hal/                         ← Hardware Abstraction Layer
│       ├── dual_blink_cpp_hal_arduino.h
│       └── dual_blink_cpp_hal_espidf.h
└── examples/
    ├── BasicExample/                ← Arduino sketch, also buildable with PlatformIO
    └── basic_example/               ← ESP-IDF project, also buildable with PlatformIO
```

The repository is named `DualBlinkCpp` (the Arduino library name), while the ESP-IDF component and the PlatformIO package are `dual_blink_cpp`. For local deployment, clone it into a folder with the lowercase name, which is the component name:

```bash
git clone https://github.com/FMazz97/DualBlinkCpp.git dual_blink_cpp
```

---

## Install

**Arduino IDE.** Download the repository as a ZIP and use *Sketch → Include Library → Add .ZIP Library*, or clone it into your `libraries/` folder.

**ESP-IDF.** Declare the dependency in the `idf_component.yml` of your component or of `main/`; the Component Manager fetches it straight from GitHub:

```yaml
dependencies:
  fmazz97/dual_blink_cpp:
    git: https://github.com/FMazz97/DualBlinkCpp.git
    version: "*"
    # override_path: "../../../" # for local deployment
```

Use either the `git` source or `override_path`, and remove the other. `override_path` is meant for local deployment, for example from the repository's own examples, where `"../../../"` points to the repository root.

**PlatformIO.** Add the library to your environment, with either framework:

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino          ; or espidf
lib_deps = https://github.com/FMazz97/DualBlinkCpp.git
; lib_deps = symlink://../.. ; for local deployment
```
---

## Usage

- **Arduino:** once the library is added from the ZIP, the example appears under *File → Examples → DualBlinkCpp*.
- **PlatformIO and ESP-IDF:** open the project directly in its folder under `examples/` (`BasicExample` or `basic_example`). Each project declares the library dependency, from GitHub or from the local repository (`symlink://` in `platformio.ini`, `override_path` in `main/idf_component.yml`): keep one of the two and remove the other.

---

## API

The core uses `snake_case` and is used directly by ESP-IDF; the Arduino facade forwards every call with `camelCase` names (see: [naming convention](https://github.com/FMazz97/dual-framework-library-guide/blob/main/appendix/C-naming-convention.md)).

| Core (ESP-IDF) | Facade (Arduino) | Description |
| :--- | :--- | :--- |
| `dual_blink_cpp(pin, interval_ms)` | `DualBlinkCpp(pin, intervalMs)` | Constructor, `500` ms by default |
| `begin()` | `begin()` | Configures the pin and starts the interval |
| `update()` | `update()` | Call it as often as possible: it toggles the pin when the interval has elapsed |
| `set_level(level)` / `get_level()` | `setLevel(level)` / `getLevel()` | Sets or reads the current pin level |
| `set_interval_ms(ms)` / `get_interval_ms()` | `setIntervalMs(ms)` / `getIntervalMs()` | Sets or reads the blink interval |

---

## How it follows the guide

| Rule | Guide |
| :--- | :--- |
| Repository root = component root, sources under `src/` | [Chapter 2](https://github.com/FMazz97/dual-framework-library-guide/blob/main/chapters/02-repo-structure.md) |
| Agnostic core, compile-time HAL, Arduino facade | [Chapter 3](https://github.com/FMazz97/dual-framework-library-guide/blob/main/chapters/03-agnostic-core-hal-architecture.md) |
| One example buildable with both toolchains | [Chapter 4](https://github.com/FMazz97/dual-framework-library-guide/blob/main/chapters/04-examples.md) |
| Manifests for the three registries | [Chapter 5](https://github.com/FMazz97/dual-framework-library-guide/blob/main/chapters/05-publishing.md) |
| Cooperating with the scheduler in a polling loop | [Appendix B](https://github.com/FMazz97/dual-framework-library-guide/blob/main/appendix/B-yield-watchdog.md) |

---

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
