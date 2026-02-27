# Deployment Instructions

## Windows
1.  Install Visual Studio 2022 with C++ Desktop Development workload.
2.  Install OpenSiv3D 0.6.16 (or latest supported).
3.  Clone repo with submodules: `git clone --recursive ...`
4.  Open `kshootmania.sln` or use CMake.
5.  Build `Release`.
6.  Run `kshootmania.exe`.

## macOS
1.  Install Xcode and Command Line Tools.
2.  Install OpenSiv3D.
3.  Use CMake:
    ```bash
    cmake -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build
    ```
4.  Run `kshootmania.app`.

## Linux
1.  Install dependencies: `libxi-dev`, `libxcursor-dev`, `libxinerama-dev`, `libasound2-dev`, `mesa-common-dev`.
2.  Use CMake:
    ```bash
    cmake -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build
    ```

## Post-Build
*   Ensure `assets`, `imgs`, `se`, `ui` folders are copied to the output directory (CMake handles this).
*   `songs` directory should be placed in the executable directory or configured in `config.ini`.
