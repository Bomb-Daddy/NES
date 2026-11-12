# EXEC0223: Enhanced finance practice controller Undo and replay

This is the **same inherited native C11 X11 / Win32-XInput source** from EXEC0222 and all historical releases, not a standalone replacement. Only the Enhanced *practice* screen is changed.

The existing 3-order practice allows LB to undo the most recent accepted PASS or Town-to-Food reassignment; all three orders can be undone and replayed, including after budget exhaustion. The executable validates receipt chronology, remaining count and the selected player country Town/Food conservation before each commit or undo. Corrupted receipt values, counters, or player resource counts are rejected without mutation. Real X11 controller synthetic LB smoke, 14+27 source-country test, rollback and 3-order replay passed.

Classic original-stock PRE_SETUP remains locked: this **does not** implement original dynamic first Spring, difficulty/ruler RNG, original 3 orders, merchant scheduler, AI, battle aftermath, diplomacy, dynasty, or native Windows/physical Xbox acceptance. No original ROM/media/private savestates in this code. Original stock ROM and evidence remain private.

Reproducibility: apply `GK1_EXEC0223_CopyrightClean_Integration.patch` with `patch -p1 --fuzz=0` against the exact complete EXEC0222 source (constructed from the 849-member EXEC0221 archive plus the published EXEC0222 patch) before building. Build commands: `cmake -S execution_199 -B build -DCMAKE_BUILD_TYPE=Release`, `cmake --build build -j8`, `ctest --test-dir build -j8 --output-on-failure`.
