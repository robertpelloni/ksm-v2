# Roadmap

> **Status note (2026-11-19, Tier 1 honesty fix):** Checkboxes below are the project's own doc claims (Jules-authored), not Muse-verified results. In particular: the unit-test line covers the `kson` chart library only, not the full game; Internet Ranking and Input Gate are client/mock only — no production backend or live service is deployed. Items corrected on that basis are unchecked below.


## Project Goal
To achieve total feature parity with K-Shoot MANIA v1 and commercial rhythm games like Sound Voltex, ensuring a high-performance, C++20 compliant, and fully documented implementation.

## Feature Status

### 🎵 Music Select & Play
- [x] Title Screen
- [x] Song Select Screen
    - [x] Song Information Display
    - [x] Song Preview
    - [x] Folder Display (including subfolders)
    - [x] Level Sort
    - [x] Chart Select & Play Start
    - [x] Options while holding BT keys
    - [x] Play status while holding 3 BT keys
    - [x] Genre Icons
    - [x] All Folder
    - [x] Folder Close (Back/Esc/Backspace)
    - [x] Song Title/Artist Image Display (title_img, artist_img)
    - [x] Batch Folder Specification (`conf.ini`)
- [x] Course Mode

### 🎮 Gameplay
- [x] Note Judgment
    - [x] BT/FX (Keyboard & Gamepad)
    - [x] Laser (Keyboard, Mouse X/Y, Slider X/Y, Analog Stick)
    - [x] Dual FX button key
- [x] Game Logic (Score, Combo, Gauge - Normal/Easy/Hard)
- [x] Graphics
    - [x] Background (bg) & Animation (layer)
    - [x] HUD (Score, Gauge, Combo, Hi-Speed)
    - [x] Laser Effects (Wiggle, Overflow, Rotation, Bounce)
    - [x] Lane Zoom (Top, Bottom, Side, Split)
    - [x] Tilt (Normal/Bigger/Biggest, Keep, Manual)
    - [x] Chart Stop
- [x] Audio Effects
    - [x] Standard FX (Retrigger, Gate, Flanger, Bitcrusher, etc.)
    - [x] SwitchAudio
    - [x] f/fp sound source
    - [x] Peaking filter delay/gain
- [x] Sound Effects
    - [x] Laser Slam (Type change unimplemented)
    - [x] Assist Tick
    - [x] Chip FX sounds
- [x] User Defined Effects (`#define_fx`, `#define_filter`)

### ⚙️ Options & Config
- [x] Basic Settings
- [x] Key Config
- [x] Language Support
- [x] `config.ini` Load/Save
- [x] Score Load/Save
- [x] Player Profile Switch
- [x] AUTO SYNC
- [ ] Internet Ranking System (UI + client submission implemented; production backend NOT deployed — real integration NOT proven)
- [x] Twitter (X) Result Posting (Web Intent)
- [x] Advanced Timing Options (Global/Visual Offset, Audio FX Delay)
- [x] Audio Backend Configuration (Device Selection, Buffer Size)
- [x] Hardware I/O JSON Profiles

### 🌐 Input Gate / Online
- [ ] Download from Input Gate (Client logic implemented, mock active only — no live Input Gate service verified)
- [x] Auto Update (Version Check & Notification)
- [x] Ranking Tab UI (Client logic)
- [ ] Unlock Conditions & Popups

### 🛠️ Build & CI Pipeline
- [x] Headless Linux C++20 build support for `kson` (`npm test`)
- [x] Unit test suite passing 100% (45 test cases, 979 assertions)

## Upcoming Priorities
1. **Course Mode Refinement**: Verify and complete any missing course mode UI elements.
2. **Server Backend Development**: Create server-side API for Internet Ranking and Input Gate song lists.
3. **Editor Parity**: Ensure `kshooteditor` can edit KSON charts correctly.
4. **[x] Select Menu Level Filtering**: Extended `ISelectMenuItem` and the `SelectMenu` internal backend logic to filter arrays by `levelFilter` (currently only sorting `sortType` is natively hooked up to the filesystem read).
