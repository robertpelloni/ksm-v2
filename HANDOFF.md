# Session Handoff (2.0.0-alpha26)

## Summary of Changes
*   **Select Scene Filter**: Wired up `SelectFilterModal` level filter and sort properties directly to `SelectMenu::reloadCurrentDirectory()`. It now correctly modifies `m_folderState.sortMode` temporarily when loading folders and subsequently filters the array by `item->hasLevel(levelFilter)`.
*   Added `hasLevel` to `ISelectMenuItem` interface.

## Current State
*   **Version**: 2.0.0-alpha26
*   **Codebase**: `SelectMenu` now correctly processes Level and Title sorts. `kson` submodule headless builds fail due to unrelated casing issues in `#include` directives within the submodule.
*   **UI Status**: Result screen fast/slow, Input gate, Option menu WASAPI config, and Select filtering are largely finished.

## Next Recommendations & Priorities
1.  **Server Backend**: As the client is now heavily requesting `list.json` and sending Internet Ranking JSON posts, bootstrapping a fast/small Node.js/Python server to receive scores and serve downloads is highly recommended.
2.  **Controller Profiles (JSON)**: `HidLightingDriver` currently hardcodes lighting bytes. Using JSON will support many boards (FAUCET, SVSE5).
