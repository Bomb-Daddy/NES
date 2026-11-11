# EXEC0180 — Original fixed-bank source seed writer paths (partial)

Complete existing standalone native C11 app inherited from exact EXEC0179
(constructed from authenticated EXEC0178 source+exact EXEC0179 patch). No
old project or source files discarded.

**New compiled source**: `gk180_seed_writer_scan.c/.h` walks opcode-aligned
static CFG from each ORIGINAL ROM RESET, NMI, IRQ vector in MMC1 fixed bank 15,
identifying unindexed direct zero-page/absolute writes and RMW to six RNG seed
bytes `$84..$89`, distinguishing internal RNG mutation from possible external
initialization sites. Static reachability deliberately includes both conditional
branches and JSR returns, and excludes unproven lower-bank transitions,
indirect jumps and unknown-index writes. It does **NOT** establish runtime
initialization, chronological seed, actual first Spring or playable Classic.

Original NES ROM path used only when exact 262160-byte SHA256 attested:
`ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`.
Only candidate PC/address labels, counts and flags emitted; ROM bytes and
licensed media stay owner-private. Never upload ROM or private Mesen evidence.

Tested integration to real `Gk1App` requires original PRE_SETUP, complete
setup intent, difficulty1..5 and refuses loaded source/enhanced histories.
`gk1_native_shell --source-seed-writers /PRIVATE/original.nes` runs it using
canonical native game state and real source dataset. It is source inspection,
not a game mechanic. `test_gk180_seed_writer_scan` exercises synthetic
RESET/NMI/IRQ graphs, indirect branch barrier, unreachable false-positive
bytes, source authentication, mutation rollback, and real-ROM boundary.

Compile:
`cmake -S execution_180 -B build180 -G Ninja -DCMAKE_BUILD_TYPE=Release`
`cmake --build build180 -j12 && ctest --test-dir build180 --output-on-failure`

Evidence labels: VERIFIED_STATIC = ROM identity/source graph; no new
OBSERVED_RUNTIME_STOCK (inherit distinct fresh WorkRAM vs archived SaveRAM).
TESTED_PORT_LINUX applies only to actually executed build/QA. NOT RUN:
Win32/NSIS, physical Xbox, original NES Mesen new playthrough, full campaigns.

Important: This does NOT satisfy the Director's top task of independent
14/27-country original Spring setup, or the three executable Classic orders.
Next original CPU + Mesen work must validate real branch/time sequence and
identify actual initial state writes before the game unlocks gameplay.
