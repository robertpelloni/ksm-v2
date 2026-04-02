# Deployment & Infrastructure Guide

This file provides instructions on building, running tests, and preparing the `kshootmania-v2` binaries.

## 1. Prerequisites

### Windows (Visual Studio 2022)
- Visual Studio 2022 with Desktop development with C++ workload.
- CMake 3.20+
- OpenSiv3D v0.6.x (installed to default directory or set `Siv3D_ROOT` environment variable).

### Linux (Ubuntu 22.04 LTS recommended)
- `sudo apt-get install build-essential cmake pkg-config xorg-dev libgl1-mesa-dev libasound2-dev libgtk-3-dev libhidapi-dev`
- OpenSiv3D v0.6.x Linux SDK.

### macOS (Xcode/Clang)
- Xcode Command Line Tools.
- OpenSiv3D v0.6.x macOS SDK.

## 2. Building the Project

The project is built using standard CMake commands:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release -j $(nproc)
```

### Headless/CI Build (No GUI)
For headless environments (e.g., GitHub Actions, automated testing tools) where an X11 server or macOS WindowServer is unavailable, you can build only the underlying logic libraries (e.g., `kson`):

```bash
mkdir build_headless
cd build_headless
cmake .. -DHEADLESS_BUILD=ON
cmake --build . -j $(nproc)
```

## 3. Running Tests

The project includes unit tests for core libraries like `kson`. You can run them via `ctest` or using the provided NPM wrapper (which handles the headless build setup automatically):

```bash
# Using standard CTest
cd build
ctest -V

# Using the NPM wrapper (requires Node.js)
npm test
```

## 4. Run Instructions

After a successful non-headless build, the executables will be located in the `App/` directory (or bundled app depending on the OS).

- **Game:** Run `kshootmania` (or `kshootmania.exe`)
- **Editor:** Run `kshooteditor` (or `kshooteditor.exe`)

*Note: For the application to find its resources (`ui/`, `imgs/`, `se/`, `VERSION`, `config.ini`), it must be executed with its working directory set to the `App/` folder.*

## 5. Deployment / Packaging

To package the application for distribution, ensure that the `App/` directory contains all necessary dynamic libraries (`bass.dll`, `bass_fx.dll`, `hidapi.dll` on Windows), and that the default `.noco` files and assets are present.

- On macOS, CMake will automatically create a `.app` bundle.
- On Linux, ensure `libbass.so` and `libbass_fx.so` are distributed alongside the executable and `LD_LIBRARY_PATH` is configured via a launch script.
- Update `VERSION` and `CHANGELOG.md` before finalizing a package.
