# Roadmap

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
- [x] Internet Ranking System (UI/Stub)
- [x] Twitter (X) Result Posting (Web Intent)
- [x] Advanced Timing Options (Global/Visual Offset, Audio FX Delay)

### 🌐 Input Gate / Online
- [x] Download from Input Gate (Client logic implemented, mock active)
- [ ] Auto Update
- [ ] Unlock Conditions & Popups

## Upcoming Priorities
1. **Course Mode Refinement**: Verify and complete any missing course mode UI elements.
2. **Internet Ranking**: Implement actual score submission API (backend required).
3. **Input Gate**: Connect to real server API (backend required).
