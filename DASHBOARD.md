# Dashboard

**Project Status**: Active Development
**Version**: 2.0.0-alpha19

## Submodules

| Submodule | Path | Branch/Tag | Description |
|-----------|------|------------|-------------|
| **kson** | `kson/` | `master` | Chart format library (KSH/KSON). |
| **ksmaudio** | `ksmaudio/` | `master` | Audio engine wrapper (BASS/Miniaudio). |
| **ksmaxis** | `ksmaxis/` | `master` | Analog input library (HID/DirectInput). |
| **NocoUI** | `kshootmania/ThirdParty/NocoUI/` | `master` | UI layout engine. |
| **CoTaskLib** | `kshootmania/ThirdParty/CoTaskLib/` | `master` | Coroutine task library. |

## Directory Structure
*   `kshootmania/src`: Main game source code.
    *   `Scenes`: Game scenes (Title, Select, Play, Result, etc.).
    *   `MusicGame`: Core gameplay logic.
    *   `Input`: Input handling.
*   `kshootmania/App`: Assets and runtime resources.
    *   `ui`: NocoUI layout files (`.noco`).
    *   `lang`: Localization files.
*   `tests`: Unit tests.
