# EXEC0142 — Source-bounded fixed-bank vector CFG

Predecessor: exact Drive EXEC0140 complete source, with the public EXEC0141 scanner reconstructed in this source tree from authenticated GitHub text. **Not** an exact byte-for-byte copy of the unavailable full EXEC0141 archive. All historical EXEC0140 source is preserved separately.

This source adds a documented-6502-opcode, vector-rooted *static overapproximation* of bank15 reachability, tracking candidate JSR $EF85 inline selectors; it rejects unknown opcodes, switchable bank targets, indirect jumps and truncated instructions without inventing bytes or controller events. The static result is **not proof that any call was executed by the original game**. Branches are both explored, and dynamic bank-switching remains unresolved. This does NOT unlock original first Spring, Trade or orders.

Build: cmake -S execution_142 -B build142 -DCMAKE_BUILD_TYPE=Release && cmake --build build142 -j6 && ctest --test-dir build142 --output-on-failure -j6.

The optional private-source check `build142/test_gk142_fixed_cfg --private-stock /private/original.nes` requires exact original ROM SHA and does not distribute any ROM bytes. Never copy original licensed assets into public source ZIP.
