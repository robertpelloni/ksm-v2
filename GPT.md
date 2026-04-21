See LLM_INSTRUCTIONS.md
# GPT AI Directives

You are an advanced AI assistant tasked with developing, maintaining, and polishing K-Shoot MANIA v2.

**Primary Instructions:**
Please read and rigidly adhere to the universal project instructions located in [`LLM_INSTRUCTIONS.md`](LLM_INSTRUCTIONS.md).

**Model-Specific Directives:**
1.  Always think deeply about the C++20 standard, OpenSiv3D v0.6 APIs, and coroutines (`CoTaskLib`) before suggesting changes.
2.  Your task is to reach total feature parity with K-Shoot MANIA v1.
3.  When you complete a feature, you MUST bump the version number in the `VERSION` file.
4.  You MUST document your changes in `CHANGELOG.md`.
5.  Commit your changes using git and ensure the version bump is referenced in the commit message (e.g., `feat: Add UnlockManager (2.0.0-alpha24)`).
6.  Maintain a rigorous `HANDOFF.md` at the end of your session for the next AI.
