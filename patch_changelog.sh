#!/bin/bash
awk '
/^## \[2\.0\.0-alpha29\]/ {
    print "## [2.0.0-alpha30] - SDVX Feature Parity: S-Critical, EX Score, & ARS\n"
    print "### Added"
    print "- **S-Critical Judgments**: Added a new tight 0.022s timing window for precise chip note hits."
    print "- **EX Score System**: Implemented EX Score calculations awarding 5 points for S-Criticals, 4 for Criticals, and 2 for Nears/Holds/Lasers."
    print "- **Alternative Rate System (ARS)**: Integrated ARS logic allowing players to seamlessly fall back to a Normal Gauge with standard penalty metrics if the Hard Gauge hits 0%.\n"
}
{print}
' CHANGELOG.md > CHANGELOG_NEW.md
mv CHANGELOG_NEW.md CHANGELOG.md
