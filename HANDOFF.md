# Session Handoff (2.0.0-alpha27)

## Summary of Changes
*   **Hardware Profiles**: Converted the hardcoded `HidLightingDriver` VID/PID initialization and byte structure mapping into a flexible `ControllerProfile` system that initializes by parsing `hardware/controller_profiles.json`. It will fallback to standard defaults if unavailable.
*   **Select Scene Filter**: Wired up `SelectFilterModal` level filter and sort properties directly to `SelectMenu::reloadCurrentDirectory()`. It now correctly modifies `m_folderState.sortMode` temporarily when loading folders and subsequently filters the array by `item->hasLevel(levelFilter)`.
*   Added `hasLevel` to `ISelectMenuItem` interface.

## Current State
*   **Version**: 2.0.0-alpha27
*   **Codebase**: `SelectMenu` filtering works. `LightingManager` iterates generic JSON HID bindings. `kson` submodule headless builds fail due to unrelated casing issues in `#include` directives within the submodule.
*   **UI Status**: Result screen fast/slow, Input gate, Option menu WASAPI config, and Select filtering are largely finished.

## Next Recommendations & Priorities
1.  **Server Backend**: As the client is now heavily requesting `list.json` and sending Internet Ranking JSON posts, bootstrapping a fast/small Node.js/Python server to receive scores and serve downloads is highly recommended.
2.  **Audio Backend**: Investigate `basswasapi.dll` integration to support Exclusive Mode backend if required by players for lowest latency.

### CMake Editor Investigation (Blocker)
I investigated `kshootmania/CMakeLists.txt` and confirmed that the `kshooteditor` target is missing. To achieve editor parity, `CMakeLists.txt` requires a major architectural rewrite to isolate `kshootmania` game logic into a shared static library that `EditorMain.cpp` can link against without dragging in `Main.cpp`. I am halting further iteration because this constitutes a major architectural decision and the next immediate steps are ambiguous without user input on CMake restructuring.
