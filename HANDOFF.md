# Session Handoff (2.0.0-alpha20)

## Summary of Changes
*   **Course Mode UI Refinement**: Implemented display of Course Title Image (`title_img`) in both the Course Selection screen (`SelectMenuCourseItem`) and Course Result screen (`CourseResultScene`). Modified NocoUI files (`select_item_center.noco`, `course_result.noco`) to include a `TitleImage` node.
*   **Documentation**: Consolidated instructions into `LLM_INSTRUCTIONS.md`. Created `VISION.md`, `MEMORY.md`, `DEPLOY.md`, `TODO.md`, `DASHBOARD.md`.
*   **CI/Test**: Verified tests run via `npm test` (kson unit tests pass). Note that `kshootmania` target requires Siv3D and cannot be built in headless CI environment yet.

## Current State
*   **Version**: 2.0.0-alpha20
*   **Submodules**: All updated.
*   **Repo**: Clean.

## Next Steps
1.  **Input Gate**: Connect to a real server. Currently uses mock data.
2.  **Controller LED Support**: High priority for hardware parity. Currently missing entirely.
3.  **Editor**: Verify `kshooteditor` functionality.
4.  **Internet Ranking**: Implement real submission backend.

## Notes for Next Agent
*   **NocoUI**: Layouts are JSON files (`.noco`). You can edit them textually.
*   **Testing**: Use `npm test`. It runs a headless build of `kson`.
*   **Headless Build**: `cmake -DHEADLESS_BUILD=ON` skips graphical targets.
