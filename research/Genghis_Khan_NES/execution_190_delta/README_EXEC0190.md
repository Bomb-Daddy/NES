# EXEC0190 — port-only Classic ASSIGN correction in the inherited native game

Inherited complete EXEC0189, no code/data copied from original NES ROM into public source.

**Real native controller change:** when a user has attached one of the three private exact NES first-Spring source states, selected Classic ASSIGN and performed an allocation, controller Y reverses the most recent unpassed labor transfer. It verifies exact source and replays all earlier actions from that authenticated immutable source. Up to three budgeted actions; the refunded action returns one order. It refuses empty history, a passed/ended state, forged source or mutable state; no modification on refusal. In the ASSIGN screen X remains a private exclusive GC19 save; TITLE Y and other non-ASSIGN screens retain cold-load behavior. The on-screen guidance now reflects GC19 cold-saving added in EXEC0189 rather than falsely saying unavailable.

**This is a user-interface correction only, not an original NES command or proof of first-Spring setup, RNG, Tax, AI, merchants, marriage, terrain or battle endings.** The original 83-byte country records are never mutated. Any prior GC19 save still decodes via existing unmodified format; undo then new exclusive save requires a fresh slot.

Reproduce: `cmake -S execution_190 -B build190 -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build190 -j8 && ctest --test-dir build190 --output-on-failure`. Additional authenticated fixture test executable `test_gk187_classic_assign <CSV> <fresh14.bin> <archived14.bin> <world27.bin>`.
