# EXEC0251 — Classic original-stock cold-load returns to a usable command menu

Derived from the **complete EXEC0250 native C11 app**, not from an earlier standalone stub.

After a successful, source-authenticated owner-private GC19 restore, the actual native app
returns to `GK1_SCREEN_STOCK_COMMANDS` rather than an always-on stale Assign screen.
The command cursor selects TAX when at least one of the three first-Spring orders
remains, or VIEW (zero-cost, cursor 6) when the budget is exhausted / passed.
A canceled view/tax dialog leaves original country records, manual-derived mutable
state, source verification and remaining budget unchanged. Failed GC19 imports remain
transactional; they never change the UI, world or source. This is native UX
correctness, **not** authentic independently generated first Spring or NES parity.

The existing source-bound `test_gk189_classic_cold_save` now tests both a two-order
midseason cold restore and the passed-season cold restore. It accepts the old raw83
private fixture and, additionally, the authentic private Mesen fresh JSON and both
archived MSS profiles, with exact profile detection. The actual graphical app's
EXEC0250 three-controller-command native cold smoke also tests restored VIEW.

Build and public test:
`cmake -S source/execution_199 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DGK1_REQUIRE_ARCHIVED_WORLD_IMPORT=ON`
`cmake --build build -j12 && ctest --test-dir build -j12 --timeout 28 --output-on-failure`

Private authenticity tests require Director-owned source snapshots and canonical
41-country source CSV, and **must not** be uploaded to public GitHub:
`build/test_gk189_classic_cold_save CSV FRESH.json MONGOL.mss WORLD.mss`
`xvfb-run -a build/gk1_native_shell --smoke-classic-three-cold-original PRIVATE_SNAPSHOT 5`

No new original NES emulation was performed. Actual Win32 build, NSIS install,
physical Xbox input, independently generated first Spring, next seasons and two
complete campaigns remain untested / unfinished for EXEC0251.
