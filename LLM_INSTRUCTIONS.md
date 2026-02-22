# LLM Instructions

This file consolidates instructions for all AI models (Claude, Gemini, GPT, Copilot, etc.) working on this project.

## Project Overview
**K-Shoot MANIA v2 (v1 Remastered)** is a C++20 reimplementation of the rhythm game K-Shoot MANIA, using the OpenSiv3D engine. The goal is to achieve feature parity with v1 while improving performance and maintainability.

## Core Directives
1.  **Language & Standard**: Use C++20. Follow the existing coding style (Google-like, but with specific NocoUI/Siv3D patterns).
2.  **Engine**: OpenSiv3D (v0.6.x).
3.  **Documentation**:
    *   Update `CHANGELOG.md` for every user-facing change.
    *   Update `VERSION` file on every build/release.
    *   Update `ROADMAP.md` and `TODO.md` as features are completed or identified.
    *   Maintain `DASHBOARD.md` with submodule status.
4.  **Testing**:
    *   Run `npm test` (which wraps `ctest`) to verify changes.
    *   Use `HEADLESS_BUILD=ON` for CI/unit testing without graphics.
5.  **Git Workflow**:
    *   Commit often.
    *   Update submodules recursively (`git submodule update --init --recursive`).
    *   Merge feature branches into `main` (or `master`) intelligently.

## Specific Instructions
*   **Version Control**: The version number is in the `VERSION` file. Read it dynamically in code.
*   **Submodules**: Ensure all submodules are documented in `DASHBOARD.md`.
*   **UI System**: Uses NocoUI (JSON-based layout). Textures are often loaded dynamically.
*   **Input/Output**:
    *   Input: `ksmaxis` (HID/DirectInput).
    *   Output: Currently missing (LED/HID lights). *Priority for implementation.*

## Handoff Protocol
When finishing a session:
1.  Run tests.
2.  Update documentation.
3.  Commit and push.
4.  Create `HANDOFF.md` summarizing the session and next steps.
