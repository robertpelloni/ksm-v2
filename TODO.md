# Todo List

## High Priority
- [x] **Hardware I/O (Controller Lights)**
    - [x] Implement LED output for supported controllers (HID/Serial) - *Added HidLightingDriver, JSON Controller Profiles*.
    - [x] Add configuration for LED behavior (blinking, intensity) - *Added to Option Menu*.
- [x] **Build & Test Infrastructure**
    - [x] Resolve `kson` submodule case-sensitivity and merge conflict issues on Linux.
    - [x] Ensure automated unit tests pass 100% cleanly (`npm test`, 45 test cases, 979 assertions).
- [x] **Dashboard Architecture**
    - [x] Consolidate project dashboard into a unified single-page layout with high-value categorizations and tooltips (`DASHBOARD.md`).
- [ ] **Internet Ranking**
    - [x] Implement real score submission (Client side done).
    - [x] Implement "Ranking" tab in Input Gate (view rankings).
    - [x] Implement mock server score receiver (`POST /`) and dynamic leaderboard update.
- [ ] **Editor**
    - [x] Verify `kshooteditor` builds and runs (CMake target exists).
    - [ ] Check feature parity with v1 editor (Initial NocoUI layout created).

## Medium Priority
- [ ] **Input Gate Polish**
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

## High Priority
- [x] Result Screen: Surface `nearFast` and `nearSlow` details inside NocoUI.

## Security
- [x] Implemented directory traversal checks for `song.id` in `InputGateScene` zip extraction.
