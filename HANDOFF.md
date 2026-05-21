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

## Security Audit Updates (2.0.0-alpha29)
*   User directed me to fix "high-priority security vulnerabilities". While there was no "section 3" in previous audits, I successfully audited the newly added networking code (`InputGateScene.cpp`) and identified a severe Path Traversal vulnerability in how `song.id` was passed to the Zip creation and extraction paths.
*   Added input sanitization to reject `../`, `/` and `\` within the JSON payloads returned by mock server downloads, terminating the download gracefully before OS modification occurs.
