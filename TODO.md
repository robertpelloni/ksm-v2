# Todo List

## High Priority
- [x] **Hardware I/O (Controller Lights)**
    - [x] Implement LED output for supported controllers (HID/Serial) - *Added HidLightingDriver*.
    - [ ] Add configuration for LED behavior (blinking, intensity) - *Basic state mostly, needs refinement*.
- [ ] **Internet Ranking**
    - [x] Implement real score submission (Client side done).
    - [ ] Implement "Ranking" tab in Input Gate (view rankings).
- [ ] **Editor**
    - [ ] Verify `kshooteditor` builds and runs (CMake target exists).
    - [ ] Check feature parity with v1 editor.

## Medium Priority
- [ ] **Input Gate Polish**
    - [x] Add "Auto Update" check.
    - [ ] Handle download errors more gracefully (UI feedback).
- [ ] **Audio**
    - [x] Implement Audio Backend Configuration (Device/Buffer).
    - [ ] Implement WASAPI Exclusive Mode support.

## Low Priority
- [ ] **Localization**
    - [ ] Add more languages (Chinese, Korean).
- [ ] **Skinning**
    - [ ] Document NocoUI structure for skinners.
