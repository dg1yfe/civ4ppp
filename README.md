# civ4ppp
Civilization IV python path patcher

Patch Python Framework Path in Civ IV executable for macOS / OSX
to point to "/Library/Frameworks" instead of "/System/Library/Frameworks"

This enables deploying the required Python 2.3 framework without
having to disable SIP (in order to copy stuff to /System/Library/ ...)
