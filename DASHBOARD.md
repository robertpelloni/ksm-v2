# K-Shoot MANIA v2 — Unified Executive Dashboard

**Project Status**: 🟢 Active Development | C++20 / OpenSiv3D v0.6 Architecture
**Release Version**: `2.0.0-alpha33`
**Test Suite Health**: 100% Passed (45/45 Test Cases, 979 Assertions) via `npm test`

---

## 1. High-Value Priority Features

| Priority | Feature Domain | Status | Operational Details & Intuitive Workflow | Tooltip Guidance / Notes |
|:---|:---|:---:|:---|:---|
| **P0** | **Gameplay & Judgments** | ✅ Functional | S-Critical (0.022s window), Critical, Near (Fast/Slow tracking), Error, EX Score (5/4/2/0), ARS Gauge fallback. | ℹ️ *Timing: Strict 22ms S-Critical. ARS automatically transitions Hard Gauge (0%) to Normal Gauge with penalty logic to prevent instant stage abort.* |
| **P0** | **Chart Engine & Parsing** | ✅ Functional | Dual-engine loader for `.ksh` and `.kson`. Quadratic Bézier curve expansion, stop-into-scroll-speed baking. | ℹ️ *Compatibility: Dual format loaded via `kson::LoadKshChartData` and `LoadKsonChartData`. Full laser slam and tilt normalization.* |
| **P0** | **Automated CI/CD** | ✅ Functional | Headless compilation (`HEADLESS_BUILD=ON`), Linux GCC 13 C++20 compatibility, Catch2 test runner. | ℹ️ *CI Validation: Run `./scripts/test_wrapper.sh` or `npm test`. Excludes GUI dependencies (OpenSiv3D) for automated runner environments.* |
| **P1** | **Input Gate & Downloads** | ✅ Functional | In-game song browser, ZIP extraction, HTTP version checking (`/version.json`), and download progress dialog. | ℹ️ *Security: Strict path sanitization on `song.id` blocks `../`, `/`, and `\` to prevent archive path traversal attacks.* |
| **P1** | **Internet Ranking** | ✅ Functional | Client asynchronous score posting (`InternetRanking::SubmitScore`), server mock receiver (`POST /`), live ranking (`/ranking.json`). | ℹ️ *Configuration: Configured via `internet_ranking_url` in `config.ini`. Mock server provides dynamic leaderboard ranking.* |
| **P1** | **Hardware I/O & Lighting** | ✅ Functional | HID lighting controller interface, dynamic JSON profiles (`hardware/controller_profiles.json`), option menu toggles. | ℹ️ *Profiles: Enables RGB button/laser lighting on standard and custom arcade controllers (SVSE, FAUCET, Arduinos).* |
| **P2** | **Audio Engine (ksmaudio)** | ✅ Functional | BASS / Miniaudio backend, configurable device enumeration, buffer latency (50ms–200ms), WASAPI Shared mode. | ℹ️ *Latency Tuning: Adjust `audio_buffer_ms` and `audio_device_id` in Option Menu. WASAPI Exclusive mode flag pending DLL backend.* |
| **P2** | **Player Profiles & Unlocks** | ✅ Functional | Persistent statistics (`save/unlocks.json`), total plays/clears tracking, secret unlocks, Profile screen (`FX-L + FX-R`). | ℹ️ *Persistence: `Unlock::UnlockManager` automatically persists player progression across gameplay sessions.* |

---

## 2. Consolidated Architecture & Submodules

| Submodule | Target Directory | Upstream Origin | Branch / Tag | Build Integration | Primary Role |
|:---|:---|:---|:---|:---|:---|
| **kson** | `kson/` | `m4saka/libkson` | `master` | CMake `add_subdirectory` | Core rhythm chart data structures, parser, and serializer for KSH and KSON specifications. |
| **ksmaudio** | `ksmaudio/` | `kshootmania/ksmaudio` | `master` | CMake `add_subdirectory` | Cross-platform audio output, streaming, and real-time DSP sound effect processor wrapping BASS. |
| **ksmaxis** | `ksmaxis/` | `kshootmania/ksmaxis` | `master` | CMake `add_subdirectory` | Analog rotary knob input handling, mouse/HID/gamepad axis calibration and normalization. |
| **NocoUI** | `kshootmania/ThirdParty/NocoUI/` | `m4saka/NocoUI` | `master` | Embedded submodule | Declarative JSON-based UI layout framework (`.noco`) for declarative scenes and components. |
| **CoTaskLib** | `kshootmania/ThirdParty/CoTaskLib/` | `m4saka/CoTaskLib` | `0.3.3` | Header-only lib | C++20 coroutine scheduling framework (`Co::Task`, `Co::NextFrame`, `Co::ScreenFadeIn/Out`). |

---

## 3. Game Scenes & NocoUI Layout Catalog

Consolidated single overview of all scene workflows and interface layout definitions:

```
kshootmania/App/ui/
├── scene/
│   ├── title.noco           → Title Screen: Mode selection (Start, Option, Input Gate, Exit), Profile shortcut (FX-L+R)
│   ├── select.noco          → Song Select Wheel: Folder navigation, level filtering, difficulty tabs, search modal, stats panel
│   ├── play_hud.noco        → In-Game Gameplay: Highway 3D, score counters, laser lanes, judgment animations, lane tilt/zoom
│   ├── play_prepare.noco    → Stage Intro: Track jacket display, BPM, song title/artist preview, stage loading overlay
│   ├── result.noco          → Results Screen: Score, Grade, S-Crit/Crit/Near/Error breakdown, Fast/Slow counters, Twitter share
│   ├── option.noco          → Options Menu: Audio device, buffer latency, key bindings, offsets, LED lighting configurations
│   ├── input_gate.noco      → Input Gate: Online song catalog, chart download manager, auto-updater, ranking leaderboard
│   └── course_result.noco   → Course Results: Multi-stage aggregated score, course clear status, cumulative performance
├── parts/
│   ├── select_item_*.noco   → Top, center, and bottom song wheel card elements
│   ├── option_*_item.noco   → Option menu key config and setting entries
│   ├── gate_item.noco       → Input Gate song entry card
│   └── ir_*.noco            → Internet Ranking notification and waiting overlays
└── dialog/
    ├── gate_downloading.noco→ Download progress bar modal
    ├── gate_unlocked.noco   → Unlock notification dialog
    ├── search_input_dialog.noco → Live text search filter modal
    └── favorite_*_dialog.noco → Favorite folder add/remove dialogs
```

---

## 4. Network Services & Mock Server Topology

```
+-----------------------------------------------------------------------------------------+
|                                    K-Shoot MANIA v2                                     |
|                       Client Network Modules (OpenSiv3D SimpleHTTP)                     |
+-----------------------------------------------------------------------------------------+
                     │                                         │
        HTTP GET /list.json, /version.json, /ranking.json      │ HTTP POST / (Score Payload)
                     ▼                                         ▼
+-----------------------------------------------------------------------------------------+
|                          Local Mock Server (Port 8080)                                  |
|                         `scripts/mock_input_gate_server.py`                             |
+-----------------------------------------------------------------------------------------+
| • GET /list.json     -> Returns downloadable song catalog and archive assets            |
| • GET /version.json  -> Serves version compatibility metadata for client auto-update    |
| • GET /ranking.json  -> Serves top 20 player rankings including live submitted scores   |
| • POST /             -> Receives ScoreDataJson (score, maxCombo, critical, near, etc.)  |
|                         Persists scores to `scores.json` and updates dynamic leaderboard|
+-----------------------------------------------------------------------------------------+
```

---

## 5. System Tray & Background Process Management

*   **System Tray Icon**:
    *   Assets configured: `kshootmania/App/icon.ico` (Windows), `kshootmania/App/icon.icns` (macOS), `icon_editor.ico`.
    *   Operating state: Runs as a full foreground OpenSiv3D interactive graphical window.
*   **Background Services (Mock Servers)**:
    *   To start background test server: `python3 scripts/mock_input_gate_server.py &`
    *   To inspect active processes: `lsof -i :8080` or `pgrep -af mock_input_gate_server`
    *   To cleanly terminate servers: `kill $(lsof -t -i :8080) 2>/dev/null || true`

---

## 6. Configuration Key Registry (`config.ini`)

| Key Name | Default Value | Value Range / Options | Subsystem Role |
|:---|:---|:---|:---|
| `songs_path` | `songs` | Absolute or relative directory path | Root folder path for scanning charts and song packages. |
| `autosync` | `0` (Off) | `0` (Off), `1` (Low), `2` (Mid), `3` (High) | Auto Sync timing adjustment sensitivity on results screen. |
| `audio_device_id` | `0` (Default) | System audio device index | Output audio device selected in Option Menu. |
| `audio_buffer_ms` | `100` | `50` – `200` (ms) | BASS audio stream buffer length (latency vs stability). |
| `audio_wasapi_exclusive` | `0` (Off) | `0` (Shared), `1` (Exclusive) | Low-latency direct hardware audio output mode toggle. |
| `show_song_title_images` | `1` (On) | `0` (Off), `1` (On) | Toggles title/artist raster images over vector text fonts. |
| `internet_ranking_url` | `""` | Valid HTTP/HTTPS URL endpoint | Remote server destination for IR score submissions. |
| `input_gate_url` | `""` | Valid HTTP/HTTPS URL endpoint | Remote server destination for Input Gate song packages. |
| `hardware_lighting_enable` | `1` (On) | `0` (Off), `1` (On) | Enables USB/HID controller LED output updates. |
| `hardware_lighting_blink` | `1` (On) | `0` (Off), `1` (On) | Enables reactive pulsing and flashing for laser slamming. |

---

## 7. Build Pipeline & Verification Commands

```bash
# 1. Run headless unit tests (Catch2 test suite - 45 test cases, 979 assertions)
npm test

# 2. Build complete main game executable (requires OpenSiv3D installation)
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target kshootmania -j$(nproc)

# 3. Build standalone chart editor target
cmake --build build --target kshooteditor -j$(nproc)

# 4. Start mock network server for Input Gate & Internet Ranking testing
python3 scripts/mock_input_gate_server.py
```
