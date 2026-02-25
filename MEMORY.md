# Development Memory

## Codebase Patterns
*   **Scenes**: Located in `kshootmania/src/Scenes`. Each scene typically has a `.cpp`, `.hpp`, and a `.noco` layout file.
*   **UI**: NocoUI is used for layout. Parameters are passed from C++ to Noco via `canvas->setParamValue` or `setSubCanvasParamValuesByTag`.
*   **Input**: `Input/KeyConfig.hpp` handles button mapping. `ksmaxis` handles analog input.
*   **Output**: `LightingManager` handles HID/Serial lighting.
*   **Audio**: `ksmaudio` wraps BASS. Config via `ConfigIni`.
*   **Network**: `SimpleHTTP` wraps libcurl or platform HTTP. `InputGateClient` and `InternetRanking` use this.
*   **Config**: `RuntimeConfig` and `ConfigIni` handle settings.
*   **Assets**: Located in `kshootmania/App/assets` or `imgs`.

## Current State Observations (2026-02-20)
*   **Recently Implemented**:
    *   **Controller LED Output (Hardware IO)**: Implemented `LightingManager` and `HidLightingDriver`.
    *   **Audio Backend Config**: WASAPI Shared Mode via BASS device selection and buffer tuning.
    *   **Input Gate**: Download manager working, Auto Update check added.
    *   **Internet Ranking**: Async JSON submission integrated.
*   **Missing Features**:
    *   Real Server Backend for Input Gate / Internet Ranking (currently points to dummy URL or uses mock).
    *   WASAPI Exclusive Mode (requires `basswasapi` addon).
    *   Unlock Conditions & Popups.
*   **Build System**: CMake based. `HEADLESS_BUILD` added for CI.
*   **Submodules**: `ksmaxis` (Input), `ksmaudio` (Audio), `kson` (Chart Format), `NocoUI` (UI), `CoTaskLib` (Coroutines).
