# Changelog

All notable changes to this project will be documented in this file.

## [Unreleased]

### Added

## [2.0.0-alpha20] - 2026-02-20

### Added
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
