# GK1 EXEC0321 — bounded first-Spring finance-phase model (C11)

Inherits the **complete existing C11 app**, most recently Windows CI232 EXEC0320. This is an additive source-math candidate for original stock first-Spring sequencing, **NOT** the original first playable Spring initializer, not a screen, not a shortcut into `PLAYABLE`, and not a release.

Source original `gk75_spring_food_tax_pay` reproduces the bounded $8099 food and $8106 tax/payroll transformations. This code combines that engine with **distinct** earlier finance input and later personnel count instead of treating a final screenshot as an initial condition. For one previously observed original-stock fresh Spring1175 path, candidate prefinance troops60/town4/masons3/artisans6/food-workers27 and food258/gold425/economy186/morale118/tax35 predict **Food208, Gold430**. Later captured personnel shows troops61/town5/masons4. The precise earlier finance inputs are candidate assumptions drawn from source records and previous frames, not newly observed live RAM for every field. The formula candidate agrees with one case only. Independently archived stock Spring1175 Gold439/Rate101 does not match it, appropriately flagged as a different RNG/setup history. The separate rate121 from fresh is not computed. No values or order budgets are hardcoded into game state.

## Source and native integration

`gk321_first_spring_phase.c/h` and `test_gk321_first_spring_phase.c` are **new C11 source**. `stage321.py` SHA-256-authenticates the three source files on Windows LF/CRLF checkouts and atomically stages them inside the existing inherited `current-gk1/execution_199` game, adding `gk321_first_spring_phase.c` to the same `gk1_app_state` static native library and one CTest. It does NOT overwrite app state, cross-world data or save files. `test_stage321.py` is an executable hostile stage suite with rollback/duplication/corruption controls.

The previous EXEC0320 Windows complete source is staged before the new EXEC0321 source. The new native test links to the entire existing app library; the main controller game does not yet invoke these candidate-only APIs. Normal code cannot take this isolated math claim as source-authentic playable initialization.

## Exact local acceptance

Tested locally: GCC strict C11 Release/Werror test `31/31` assertions, ASan+UBSan same `31/31` assertions; Python hostile source-staging `17/17`. **Not** a new original stock Mesen run; original fresh/archived previously obtained and remain distinct. **No new full Linux CTest** on latest complete native game from this staging test; Windows compile awaits actual GitHub Actions. No physical Xbox or complete campaign execution.

The private original USA NES ROM, archived RAM/save states and all screenshots are excluded from GitHub and from this public handoff ZIP. Full stock initialization (setup options, RNG, ability, all 14/27 countries, orders3) remains **open**. After source and CI validation, the next engineer needs to trace true original stock CPU STORE/branch ordering for each finance/personnel update and replace candidate inputs with source-driven generated values from scenario seed and controller selections; then stateful real season and gameplay must be connected to full app.
