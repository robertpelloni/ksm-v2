# Todo List

## High Priority
- [x] **Hardware I/O (Controller Lights)**
    - [x] Implement LED output for supported controllers (HID/Serial) - *Added HidLightingDriver, JSON Controller Profiles*.
    - [x] Add configuration for LED behavior (blinking, intensity) - *Added to Option Menu*.
- [x] **Internet Ranking**
    - [x] Implement real score submission (Client side done).
    - [x] Implement "Ranking" tab in Input Gate (view rankings).
- [ ] **Editor**
    - [x] Verify `kshooteditor` builds and runs (CMake target exists).
    - [x] Check feature parity with v1 editor (Initial NocoUI layout created). -> **Deferred/Blocker**: Requires `kshooteditor` dedicated target setup or extraction of `kson` tools first.

## Medium Priority
- [x] **Input Gate Polish**
    - [x] Add "Auto Update" check.
    - [x] Handle download errors more gracefully (UI feedback).
- [ ] **Audio**
    - [x] Implement Audio Backend Configuration (Device/Buffer).
    - [x] Add WASAPI Exclusive Mode support toggle (Backend DLL pending).

## Low Priority
- [ ] **Localization**
    - [x] Add more languages (Chinese, Korean).
- [ ] **Skinning**
    - [x] Document NocoUI structure for skinners.
