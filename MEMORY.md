# Development Memory

## Codebase Patterns
*   **Scenes**: Located in `kshootmania/src/Scenes`. Each scene typically has a `.cpp`, `.hpp`, and a `.noco` layout file.
*   **UI**: NocoUI is used for layout. Parameters are passed from C++ to Noco via `canvas->setParamValue` or `setSubCanvasParamValuesByTag`.
*   **Input**: `Input/KeyConfig.hpp` handles button mapping. `ksmaxis` handles analog input.
*   **Config**: `RuntimeConfig` and `ConfigIni` handle settings.
*   **Assets**: Located in `kshootmania/App/assets` or `imgs`.

## Current State Observations (2026-02-20)
*   **Missing Features**:
    *   Controller LED Output (Hardware IO).
    *   Course Mode UI Polish (Title Images).
    *   Real Backend for Input Gate / Internet Ranking (currently mocked).
*   **Build System**: CMake based. `HEADLESS_BUILD` added for CI.
*   **Submodules**: `ksmaxis` (Input), `ksmaudio` (Audio), `kson` (Chart Format), `NocoUI` (UI).
