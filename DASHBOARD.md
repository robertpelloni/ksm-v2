# Dashboard

**Project Status**: Active Development
**Version**: 2.0.0-alpha32

## Submodules

| Submodule | Path | Branch/Tag | Description |
|-----------|------|------------|-------------|
| **kson** | `kson/` | `master` | Chart format library (KSH/KSON). Linux C++20 build verified. |
| **ksmaudio** | `ksmaudio/` | `master` | Audio engine wrapper (BASS/Miniaudio). |
| **ksmaxis** | `ksmaxis/` | `master` | Analog input library (HID/DirectInput). |
| **NocoUI** | `kshootmania/ThirdParty/NocoUI/` | `master` | UI layout engine. |
| **CoTaskLib** | `kshootmania/ThirdParty/CoTaskLib/` | `master` | Coroutine task library. |

## Feature Status

*   **Audio**: Backend configuration (Device/Buffer) implemented via Option Menu (WASAPI Shared).
*   **Internet Ranking**: Backend integrated (Async HTTP POST), real submission enabled, mock server score receiver active.
*   **Input Gate**: Download manager active, Auto Update check implemented (Client/Song List).
*   **Hardware**: LED Lighting (HID) support integrated.
*   **Editor**: Verified stub (requires GUI build).
*   **Unit Tests & Automated CI**: All 45 test cases and 979 assertions passing via `npm test`.

## Directory Structure
*   `kshootmania/src`: Main game source code.
    *   `Scenes`: Game scenes (Title, Select, Play, Result, etc.).
    *   `MusicGame`: Core gameplay logic.
    *   `Input`: Input handling.
    *   `Network`: Internet Ranking & API clients.
*   `kshootmania/App`: Assets and runtime resources.
    *   `ui`: NocoUI layout files (`.noco`).
    *   `lang`: Localization files.
*   `tests`: Unit tests.
