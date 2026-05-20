# IDEAS & Deep Analysis

## Current Codebase Observations (v2.0.0-alpha24)

K-Shoot MANIA v2 is an OpenSiv3D/C++20 based rhythm game. It replaces the old v1 backend with modern architecture, leveraging NocoUI for layout, ksmaudio (BASS wrapper) for sound, and ksmaxis for HID input.

### 1. Unfinished / Unhooked Backend Features
*   **Audio Backend**: WASAPI Exclusive mode is toggled in the UI (`ConfigIni::Key::kAudioWasapiExclusive`), but the actual implementation in `ksmaudio::Init` is just an empty `if (exclusive)` block because the `basswasapi` library is missing. **Idea:** Integrate `basswasapi.dll` into the `ThirdParty_Windows` folder and implement the callback.
*   **Hardware I/O**: `HidLightingDriver` sends hardcoded standard reports (YuanCon format) based on the `LightingState`. **Idea:** Different controllers (FAUCET, SVSE5, Virar) use different HID Report structures. We need a `ControllerProfile` JSON to define exactly which bytes/bits map to which LED.
*   **Network (Input Gate / IR)**: The client side uses OpenSiv3D `SimpleHTTP`, but it points to dummy endpoints or uses mock data if `ConfigIni` URLs are blank. **Idea:** Build a simple lightweight Node.js/Express or Python/FastAPI companion backend repo to serve `ranking.json`, `list.json`, and handle ZIP distribution.

### 2. Underrepresented UI Features
*   **Result Screen Score Details**: The UI shows Critical/Near/Error, but misses deeper analytics like Fast/Slow split (which exists in `ComboStats::nearFast` / `nearSlow`). **Idea:** Update `result.noco` to include Fast/Slow breakdown graphs.
*   ~~**Song Select Sorting & Filtering**: The backend supports folders, but advanced filtering (e.g., Level 15+, specific effector) isn't prominently accessible via controller inputs. **Idea:** Add a dedicated "Filter" modal (perhaps toggled by FX-L + FX-R in select scene).~~ -> **DONE**. A dedicated `SelectFilterModal` has been fully wired up via `FX-L + FX-R` to physically update the internal search list filtering logic in `SelectMenu::reloadCurrentDirectory`.
*   **Profile / Player Data**: The game saves high scores locally via `KscIO`, but there is no overarching "Player Profile" view showing total play count, global grade, or Dan/Course titles. **Idea:** Create a `ProfileScene` accessible from the Title.

### 3. Refactoring & Technical Debt
*   **NocoUI Rigidity**: Many scenes still manually position logic. NocoUI doesn't support complex dynamic lists well yet (hence manual `drawSongList` loops in `InputGateScene`). **Idea:** Expand NocoUI with a `ListView` or `DataGrid` component.
*   **CMake & Target Separation**: `EditorMain.cpp` is built alongside `Main.cpp`. The editor requires Siv3D GUI tools, making headless CI testing hard for any shared components not cleanly separated into `kson`. **Idea:** Extract all pure logic into a `ksmcore` static library that both the Game and Editor link against, allowing GUI-free unit testing of game logic.

### 4. Radical Pivots & Structural Ideas
*   **WebAssembly (Wasm) Port**: OpenSiv3D supports Emscripten. **Idea:** Port K-Shoot MANIA v2 to the web. The UI and logic are mostly self-contained. The primary hurdle would be replacing `ksmaudio` (BASS) with Web Audio API or Miniaudio.
*   **Custom Chart Format API**: `kson` is a great JSON format, but writing raw JSON is tedious. **Idea:** Create a Python/JS SDK for generating KSON charts programmatically, useful for procedural generation or external tooling.
*   **Multiplayer / Battle Mode**: **Idea:** Implement peer-to-peer or server-mediated live battle mode where players see opponent's gauge and send "attacks" (e.g., temporary screen tilt, laser wigglng).
