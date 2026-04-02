# K-Shoot MANIA v2 Vision

The ultimate goal and design of this project is to create an open-source, modernized, high-performance, and feature-complete C++20/OpenSiv3D rewrite of the popular "K-Shoot MANIA" v1 rhythm game.

## Core Pillars

1. **Total Feature Parity:**
   Recreate every mechanic, laser overflow behavior, background tilt system, chart stop, and audio effect found in v1, along with newer commercial game conventions (like Sound Voltex EXCEED GEAR/Valkyrie Model features), while remaining lightweight.

2. **Extensibility & Configuration:**
   The engine uses `kson` (JSON-based) as a primary chart format. The UI must be fully skinnable via the `NocoUI` engine. Features like hardware LED control (via `hidapi`), low-latency audio (WASAPI/BASS), and global/visual offsets are first-class citizens.

3. **Connectivity:**
   Internet Ranking, Online Song Downloading (Input Gate), and Social Sharing (Twitter Web Intents). The client is fully autonomous but securely designed to hook into a generalized backend server API (`list.json`, `ranking.json`).

4. **Editor Parity:**
   Provide a fully functional chart editor (`kshooteditor`) built on the exact same logic base as the game, avoiding the drift common between v1's separate editor and game clients.

5. **AI-Assisted Robustness:**
   This project is actively maintained, refactored, and expanded via autonomous AI engineering (Jules, Claude, Gemini, GPT). All instructions, configurations, and plans must be documented thoroughly across `.md` files to ensure consistent context handoff between models over time. Bugs must be non-existent; all missing features must be implemented without ever stopping.
