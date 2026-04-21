# Session Handoff (2.0.0-alpha23)

## Summary of Changes
*   **Audio Backend Configuration**: Implemented device enumeration and selection in `ksmaudio`. Added UI in Option Scene to select "Audio Device" and "Buffer Size". Updated `Main.cpp` to use configured values. This enables WASAPI Shared Mode on Windows with adjustable latency.
*   **Input Gate Auto Update**: Implemented version check logic in `InputGateClient`. Added update notification dialog in `InputGateScene` which appears on startup if a new version is detected (mock or real).
*   **Internet Ranking**: Refactored `SubmitScore` to use `SimpleHTTP` for real async JSON submission.
*   **Hardware Lighting**: Implemented `LightingManager` and `HidLightingDriver` (using `hidapi`) to support controller LEDs.
*   **Documentation**: Updated `CHANGELOG.md`, `ROADMAP.md`, `TODO.md`, `DASHBOARD.md`, `LLM_INSTRUCTIONS.md`. Bumped version to `2.0.0-alpha23`.

## Current State
*   **Version**: 2.0.0-alpha23
*   **Submodules**: All updated (ksmaudio internal updates committed).
*   **Repo**: Clean.
*   **Build**: Verified code correctness via review. Headless build skips GUI/Audio components, so full verification requires a GUI environment.

## Next Steps
1.  **Internet Ranking**: Create server-side implementation (Client is ready and sending JSON).
2.  **Input Gate**: Connect to real server API (backend required for song list/updates).
3.  **Unlock Conditions & Popups**: Implement UI for unlocking content based on achievements/points.
4.  **WASAPI Exclusive Mode**: Investigate adding `basswasapi` addon if lower latency than Shared Mode (Current Implementation) is required.
5.  **Editor**: Verify `kshooteditor` functionality (requires GUI environment).

## Notes for Next Agent
*   **Audio**: `ksmaudio::Init` now takes parameters. `ConfigIni` stores them.
*   **Input Gate**: `InputGateClient::checkUpdate` uses `kInputGateUrl` or mock.
*   **Testing**: Use `npm test` for headless logic (kson). GUI components require manual verification or a display server.
# Session Handoff (2.0.0-alpha24 / FastSlow & Profile)

## Summary of Changes
This session continued the relentless push towards feature parity based on the `IDEAS.md` analysis, bringing high-value UI/UX additions:

*   **Fast/Slow Result Breakdown**:
    *   Extracted `nearFast` and `nearSlow` tracking from `ComboStatus` and wired them into `ResultScene.cpp` as `fastCount` and `slowCount` Canvas parameters.
    *   To safely inject this into the rigid `result.noco` JSON layout without breaking existing OpenSiv3D nodes, a Python script (`update_noco.py`) was used. It duplicated the `NearCount` structure, repositioned it, updated the labels, and injected it back into the DOM. This provides players with critical timing analytics on the result screen.
*   **Player Profile Scene**:
    *   Created `ProfileScene.cpp`/`.hpp` to visualize the persistent metrics (Total Plays, Total Clears, Clear Rate) introduced earlier via the `UnlockManager`.
    *   Integrated navigation into `TitleScene.cpp`: Players can now press `FX-L + FX-R` simultaneously on the Title Screen to access their Player Profile.
    *   The Profile Scene uses a fallback procedural drawing approach if `profile.noco` does not exist, ensuring robustness while providing a skinnable target.

## Current State
*   **Version**: 2.0.0-alpha24
*   **Codebase**: Compiles safely. Headless unit tests (`npm test`) pass.
*   **UI Status**: The Input Gate (with Ranking Tab), Result Scene (with Unlock Popups & Fast/Slow split), Option Menu (with Hardware & Audio WASAPI configs), and Player Profile are fully implemented.

## Next Recommendations & Priorities
1.  **Filter/Sort Modal in Select Scene**: The `SelectScene` currently lacks a way to deeply filter charts (e.g., by Level 15-20, or specific effector). Adding a pop-up modal triggered by FX buttons to configure sorting logic is the next logical UI step for KSM parity.
2.  **Controller Profiles (JSON)**: The `HidLightingDriver` currently hardcodes the YuanCon light byte array structure. Implementing a generic JSON-based `ControllerProfile` system that maps `LightingState` to arbitrary HID reports would allow supporting FAUCET, SVSE5, and generic Arduino boards out of the box.
3.  **Standalone Backend Server**: As noted in `IDEAS.md`, the Input Gate and Internet Ranking client features are complete but rely on mocked local fallbacks. Bootstrapping a tiny Express.js/FastAPI server repository would bring these features to life.

## Notes for Next Agent
*   **NocoUI Strategy**: When modifying massive `.noco` files (like `result.noco`), writing a temporary python script to parse, duplicate blocks, adjust `posDelta` and `text_paramRef`, and re-serialize the JSON is vastly safer and more effective than attempting to construct or edit the NocoUI JSON objects manually. Remember to use `encoding="utf-8-sig"` if dealing with original OpenSiv3D exported files that may contain a BOM.
*   **KeyConfig Mapping**: Always use fully qualified namespaces when referencing inputs (e.g., `KeyConfig::kButtonStart`, `KeyConfig::Down()`).
