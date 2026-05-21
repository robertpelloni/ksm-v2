# Changelog

All notable changes to this project will be documented in this file.

## [Unreleased]

### Added

## [2.0.0-alpha27] - 2026-05-20

### Added
- **Hardware Lighting Profiles**:
    - Created `ControllerProfile.hpp` allowing JSON-based HID configurations (`hardware/controller_profiles.json`) instead of hardcoding light/byte structures. This supports custom controllers (SVSE, FAUCET, generic Arduinos).
    - `LightingManager::init()` now iterates through provided profiles and attempts to open matching VID/PID.
- **Select Scene**: Implemented logic in `SelectMenu::reloadCurrentDirectory` to properly apply `SelectFilterModal` sort mode and level filtering.

## [2.0.0-alpha25] - 2026-02-20

### Added
- **Hardware Lighting Profiles**:
    - Created `ControllerProfile.hpp` allowing JSON-based HID configurations (`hardware/controller_profiles.json`) instead of hardcoding light/byte structures. This supports custom controllers (SVSE, FAUCET, generic Arduinos).
- **Select Scene Filter Modal**:
    - Added `SelectFilterModal` toggled via `FX-L + FX-R` to configure Sort (Title/Level) and difficulty filters.
    - Wired modal directly to `SelectMenu::reloadCurrentDirectory()` to actively update the song wheel on close.

### Fixed
- **Build**: Fixed critical compilation errors by explicitly qualifying `KeyConfig::` enums in `Main.cpp` and `DifficultyIdx::` in `ResultScene.cpp`.
- **API Fixes**: Replaced hallucinated `task.get()` calls on `AsyncHTTPTask` with the proper `task.getResponse()` from the OpenSiv3D API, resolving headless unit test deviations.
- **Git Hygiene**: Prevented a script from truncating the `.gitignore` file, maintaining local repo cleanliness.

## [2.0.0-alpha24] - 2026-02-20

### Added
- **Project Documentation & Tooling**:
    - Created `IDEAS.md` for deep analysis and feature brainstorming.
    - Updated `VISION.md`, `ROADMAP.md`, `TODO.md`, and `DASHBOARD.md` to reflect the latest state of the project.
    - Created universal instruction files for all AI agents (`CLAUDE.md`, `GEMINI.md`, `GPT.md`, `copilot-instructions.md`) that link back to `LLM_INSTRUCTIONS.md`.
    - Added `DEPLOY.md` to guide project builds and infrastructure.
- **Unlock Manager**:
    - Replaced the mock unlock UI in `ResultScene` with a persistent `UnlockManager` class that reads/writes to `save/unlocks.json`.
    - Implemented a baseline condition system (e.g., unlocking secret content upon achieving a Perfect clear).
- **Result UI Analytics**:
    - Added Fast/Slow timing breakdown beneath the Near count on the Result screen.
    - Updated `result.noco` dynamically to support the new metrics without breaking layout integrity.
- **Player Profile**:
    - Added a new `ProfileScene` accessible from the Title screen by pressing `FX-L + FX-R`.
    - Visualizes persistent player statistics (Total Plays, Clears, Clear Rate) fetched from the `UnlockManager`.

## [2.0.0-alpha23] - 2026-02-20

### Added
- **Audio Configuration**:
    - Added "Audio Device" selection to Option Menu (supports WASAPI Shared Mode via BASS).
    - Added "Audio Buffer Size" selection to Option Menu (50ms - 200ms) to allow tuning latency vs stability.
    - Updated `ksmaudio` to support device enumeration and configurable initialization parameters.
    - Updated `Main.cpp` to initialize audio backend using saved configuration.

### Fixed
- **Build**: Fixed a typo in `CMakeLists.txt` that prevented `KSM_HIDAPI_ENABLED` from being defined, ensuring hardware lighting support is properly enabled when HIDAPI is found.

## [2.0.0-alpha22] - 2026-02-20

### Added
- **Input Gate**:
    - Implemented auto-update check on scene start.
    - Checks against `kInputGateUrl + "/version.json"` (or mock if URL empty).
    - Displays update notification dialog if a newer version is available.
    - Reads local version from `VERSION` file.

## [2.0.0-alpha21] - 2026-02-20

### Added
- **Internet Ranking**:
    - Implemented real score submission backend using asynchronous HTTP POST.
    - Sends JSON payload with Score, Combo, Critical/Near/Error counts, and Gauge Value.
- **Editor**:
    - Verified `kshooteditor` target existence and build process.
    - Confirmed current state is a stub with "Editor is unimplemented" message.

## [2.0.0-alpha20] - 2026-02-20

### Added
- **Hardware I/O**:
    - Added `LightingManager` to support controller LEDs.
    - Implemented `HidLightingDriver` using `hidapi` (supports generic HID lighting reports, currently hardcoded for standard YuanCon VID/PID 0x1973:0x2001).
    - Added `MockLightingDriver` for environments without hardware.
    - Integrated lighting state updates (BT/FX) into the main loop.
- **Input Gate**:
    - Implemented real download logic using `AsyncHTTPTask`.
    - Implemented ZIP extraction of downloaded songs to `songs/download/` using `ZIPReader`.
    - Client now fetches `list.json` from the configured `kInputGateUrl`.
- **Course Mode UI**:
    - Added display of Course Title Image (`title_img`) in Course Selection (replaces text title if image exists).
    - Added display of Course Title Image (`title_img`) in Course Result Screen.
- **Documentation**:
    - Consolidated LLM instructions into `LLM_INSTRUCTIONS.md`.
    - Updated `ROADMAP.md`, `TODO.md`, `VISION.md`, `DASHBOARD.md`.

## [2.0.0-alpha19] - 2026-02-20

### Added
- **Development / CI**:
    - Added `package.json` and `scripts/test_wrapper.sh` to enable standard `npm test` workflow.
    - Added `HEADLESS_BUILD` CMake option to allow building core libraries (e.g., kson) without graphical dependencies (OpenSiv3D).
    - Enabled running unit tests in headless environments (e.g., CI).
- **Option Menu Improvements**:
    - Added support for text-based menu items (bypassing the need for texture assets).
    - Added `Auto Sync` setting (Off/Low/Mid/High).
    - Added `Song Title/Artist Images` toggle (implemented display logic in Song Select).
    - Added `Songs Directory` display (showing the resolved path).
    - Improved `Laser Signal Sensitivity` display with proper suffixes (e.g., "0 (Default)").
- **Auto Sync**:
    - Implemented logic to calculate average judgment timing deviation during gameplay.
    - Added confirmation dialog at Results screen to update `Input Delay` if significant deviation is detected (enabled via Auto Sync option).
- **FileSystem**:
    - Added support for loading additional song directories via `songs/conf.ini`.
- **UI**:
    - Added NocoUI layout for "INPUT GATE" scene (tabs, list placeholder).
    - Populated Input Gate with mock song list and download simulation.
    - Added `Auto Sync` to in-game BT Option Panel.
- **Network**:
    - Added `InternetRanking` stub for future online features.
    - Added UI option to enable/disable Internet Ranking (stubbed backend).
    - Uncommented submission call in Result Scene.
    - Added `InputGateClient` mock for fetching song data.
- **I18n**:
    - Added missing localization keys for Auto Sync confirmation and Internet Ranking option.
    - Updated English and Japanese language files.
- **Input Gate**:
    - Implemented a more robust `InputGateClient` with support for file downloads and progress reporting.
    - Updated `InputGateScene` to display a song list with navigation and download progress overlay.
    - Prepared client-side logic for real API integration (currently using enhanced mocks).
- **Social**:
    - Added "Post to X" (Twitter) feature in Result Screen (press FX-L + FX-R).
    - Implemented `TwitterClient` using Web Intents for secure and simple sharing without requiring in-app login yet.
- **Options**:
    - Added missing critical Audio/Visual offset settings to Option Menu:
        - `Global Offset` (Universal offset for all charts).
        - `Visual Offset` (Shifts judgment line visually relative to audio).
        - `Audio FX Delay` (Compensates for audio processing latency).
        - `Auto Play SE` (Toggle for chip/slam sound auto-play).
        - `Mute in Inactive Window` (Toggles mute behavior when window loses focus).
- **Course Mode**:
    - Fixed Max Combo tracking in Course Mode (previously only tracked chart-local max combo, now tracks global max combo across charts).
    - Added support for displaying Course Title Image (`title_img`) in both Song Select and Course Result screens.
- **Internet Ranking**:
    - Refined submission logic to include a JSON payload with detailed score stats.
    - Added configuration for `internet_ranking_url` to allow pointing to custom servers.
- **Input Gate**:
    - Added configuration for `input_gate_url` to allow pointing to custom servers for song list fetching.
- **Refactoring**:
    - Cleaned up `AudioEffectMain` initialization logic to remove technical debt (comma operator in constructor).
    - Removed stale TODOs in `SelectMenuSongItem` as features were already implemented.
- **Documentation**:
    - Added `HANDOFF.md` and updated `DASHBOARD.md` to assist future development cycles.
- Initial setup of documentation files (`AGENTS.md`, `VISION.md`, `DASHBOARD.md`).
- `VERSION` file for centralized version management.
- Configurable songs directory via `songs_path` in `config.ini`.
- `Main.cpp` now reads version number from `VERSION` file dynamically.

### Changed
- None yet.

### Fixed
- None yet.

## [2.0.0-alpha28] - Fast/Slow Result Screen Details
### Added
- Added `fastCount` and `slowCount` parameter bindings to `ResultScene.cpp` to accurately pass inner FAST/SLOW deviation statistics from `ComboStats` directly into NocoUI.
- Modified `result.noco` layout schema to append explicit visible FAST and SLOW counters right under the Near count text field, increasing user visual feedback regarding chart timing offsets.

## [2.0.0-alpha29] - Input Gate Security Path Traversal Fix
### Fixed
- Added a security validation check inside `InputGateScene.cpp` to explicitly block directory traversal characters (`..`, `/`, `\`) in `song.id` payloads retrieved from a backend, preventing malicious server-injected path traversal zip extraction vulnerabilities.
