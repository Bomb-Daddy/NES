# GK1 Cumulative Master v4.39 / EXEC0290 additive original source writer chronology

# GK1 EXEC0290 — source-written original CPU RNG chronology integrated into complete native app
2026-11-14 (America/Detroit) / 2026-11-15 UTC. **REAL C11 SOURCE AND TESTS, NOT A MEMO-ONLY EXECUTION.**

## Authority / duplicate check
Drive Collaboration 1d0TA1lgqsVA3AWQQ9o0FZgmvao90oCeT and GitHub Bomb-Daddy/NES were checked first; most recent completed source execution was EXEC0289 (Windows staging for EXEC0288). Inherited complete EXEC0288 copyright-clean archive was used unchanged except exactly six additive/current source changes. Earlier complete archive remains immutable.

## Changed game code
`gk290_seed_chronology.c/h`: new bounded 8192-event CPU timeline validator for original NES seed bytes $0084–$0089. It requires six explicitly logged CPU seed writes before first $C289 draw, actual PC/bank/CPU address attribution, monotonically increasing CPU cycles, original-derived 48-bit C289 draw recurrence, exact before/after seed parity and all interleaved seed writes accounted for. Earlier source history tools counted gaps without explaining them; this fails closed for unexplained gaps and does not silently infer the first seed from a prior savestate. Every rejection leaves caller result byte-identical.
`gk1_app_state.c/h`: new read-only `gk1_app_original_seed_chronology()` wired into the same full C11 native Gk1App source state. It requires classic profile, correct untouched original 14/27-country scenario, setup boundary, selected difficulty and no foreign checkpoint/enhanced active state, then validates the supplied timeline without changing the player world.
`CMakeLists.txt`: new executable C11 regression and CTest registration. `test_gk290_seed_chronology.c`: source-written six-byte positive, interleaved writer, repeated C289, hidden mutation, forged draw, wrong mapped bank/PC/address, missing writer, bad ordering, fail-atomic output, app Classic/Enhanced and template mutation refusal.

## Tests actually executed
- Native Linux `cmake -S execution_199 -B build -DCMAKE_BUILD_TYPE=Debug`, `cmake --build build -j8`: PASS full installed game source targets, graphical native shell included.
- `ctest -R GK1_290_ --output-on-failure`: 1/1 PASS; native executable prints 43 assertions PASS.
- CTest full registered `234` results: `233 PASS, 1 FAIL` (known inherited legacy `GK1_257_CurrentWin32Workflow_RejectsStaleSourceAndFalsePass` gate checks obsolete 257 workflow expressions that no longer exist in the modern Win32 GitHub workflow); do **not** misreport 234/234. Targeted eligible run excludes exactly that stale static workflow check: **233/233 PASS**. No genuine app regression was ignored.
- Independent `-fsanitize=address,undefined` compile of actual production library and new test 1/1 PASS.
- New EXEC0290 Win32 native binary and physical Xbox NOT RUN. Actual predecessor EXEC0289 Win32 GitHub Actions run 43656672177 completed **217/217** CTests, WinMain/software controller and NSIS installer/installed launch PASS, at 2026-11-15T00:11Z. Previous 288 Linux 228/228 remains historical evidence.

## Strict evidence boundaries
VERIFIED_STATIC: tests, SHA, integrated source, no private ROM/checkpoint distributed. TESTED_PORT_LINUX: actual native Debug/ASan and CTests as above. OBSERVED_RUNTIME_STOCK_ARCHIVED_RESTORED / FRESH_RESET: only inherited EXEC0076 stock evidence, NO NEW Mesen run. TESTED_PORT_WIN32_CI: EXEC0289 prior Windows 217/217 verified; EXEC0290 NOT RUN. Physical Xbox NOT RUN, full campaigns NOT RUN. Synthetic test event records are **not original Mesen footage**. Passing C289 math proves arithmetic on claimed trace only, not that a CPU instruction actually ran. `real_mesen_proven`, `stock_first_spring_generated`, `orders_granted` remain zero.

## Next real gameplay requirement
Read authenticated power-on Mesen CPU writer event sequences with independently verified evidence, source-trace original seed creation, NMI/controller/difficulty and ability transformations, then implement **real** first Spring and 3 usable orders in the same native game for 14/27 countries, with independent fresh/archived parity under differing RNG histories. Merchant presence, AI, battle aftermath, diplomacy, dynasty, full campaigns, and physical controller remain open. Never hardcode Gold430/439 or Rate121/101. Original ROM private research folder only. Publication does not imply implementer read acknowledgement. Hourly development stays enabled.
