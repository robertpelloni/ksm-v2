import sys

def patch_changelog():
    filepath = 'CHANGELOG.md'
    with open(filepath, 'r') as f:
        content = f.read()

    new_entry = """## [2.0.0-alpha31] - 2026-06-23

### Added
- Created foundational NocoUI layout and build target for `kshooteditor`.

### Fixed
- Reverted S-Critical judgments triggering on long notes and laser ticks to prevent score inflation.
- Fully implemented missing Alternative Rate System (ARS) logic in gauge calculation and Result Screen parsing.
- Fixed `find_package(Siv3D REQUIRED)` failing on Linux CI by isolating it under `#ifndef HEADLESS_BUILD`.

"""
    # Insert new entry right after the main header
    parts = content.split('# Changelog\n', 1)
    if len(parts) > 1:
        content = parts[0] + '# Changelog\n\n' + new_entry + parts[1]
    else:
        content = '# Changelog\n\n' + new_entry + content

    with open(filepath, 'w') as f:
        f.write(content)

patch_changelog()
