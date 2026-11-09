GK1 EXEC0137 HANDOFF — 2026-11-09

INHERITANCE: exact preexisting complete historical EXEC0136 game ZIP, no prior archive entries altered. New execution_137 C11 source is a successor, not a replacement of earlier versions.
CHANGE: source-derived 48-bit NES RNG + validated Rate expression integrated with existing Gk1App setup boundary; 95/95 Linux Release and Debug CTest, focused sanitizer, same native graphical smoke and Debian package.
RUN: `cmake -S execution_137 -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j4 && ctest --test-dir build --output-on-failure && xvfb-run -a build/gk1_native_shell --smoke-frames 120`
QA: original RNG big-endian $84..$89; original 48-bit multiply; independent 8-bit limb oracle; no draw on rand(0); boundaries for difficulty1..5; one-draw declining-Rate math is not whole original seasonal RNG sequence; application setup changes NONE, countries and original checkpoint NONE; no first Spring ready claim. All historical private ROM/stock RAM excluded from public artifact.
BLOCKER: real Mesen-for-AI setup seed event/source trace, character/difficulty/ability RNG and timing; original first Spring/merchant/AI/war/dynasty and two campaigns remain unimplemented. New authentic Mesen execution NOT RUN; Windows/NSIS/Xbox NOT RUN. Keep original recurring work running unless Director stop.
