# Genghis Khan I English NES → native C11 Windows/Linux research game — EXEC0071

**Source lineage and publication gate:** This is a genuine tested additive branch of the **full verified EXEC0069** native game source (Drive 1PyIdfTrqlaIHX5OVnisSjBdhm83Qal5U, SHA-256 57ffd2337f269e3c5bb10db6b5b482eb38f29aadb0b408c4a4bcf1fc39f4b4f8). The newer **EXEC0070** introduced an archived Spring 1175 WorkRAM viewer and is documented in the Drive cumulative master v2.51. Its complete updated ZIP is *not* available in the checked authoritative Drive folder or public GitHub. **Do not install or merge this branch over 0070.** Replay this source delta against actual 0070 after recovering exact tested source, retaining its Spring viewer and associated tests. No source for ROTK II or the separate KOEI-inspired Windows game is involved.

## Actual code behavior

- `gk71_order_budget.c/.h` encodes manual-audit-0003 strategic allowance for four seasons: base lord at most 3 orders, direct-government country at most 1, prince government 0 human-controlled orders (AI must operate separately); Pass forfeits remaining orders. All PRE_SETUP actions are **locked**, and rejected actions do not mutate the account/state.
- The *existing* `gk1_native_shell` now has a controller VIEW-driven budget screen from its manual rules display. D-pad Left/Right previews lord/direct/prince government, A requests Pass and receives explicit first-Spring lock, B returns. Both 14- and 27-country original worlds remain PRE_SETUP templates, not original Spring gameplay. View intentionally avoids the archived Spring viewer's documented A route in EXEC0070. Preserve 0.70 source route on actual rebase.
- A reserved synthetic phase-2 fixture exercises order consumption and season transition reducer in isolation; **it is not original NES gameplay**, a verified RNG formula, a player-ready campaign, a live controller Pass action or a game save.
- Original battle, Divide, Rest, Demobilize, three protected research saves, historic scenario templates, source identity and market lock remain inherited from EXEC0069. This branch does **not** claim EXEC0070’s later archived Spring viewer until rebased.

## Reproduce Linux-native tests

```
cmake -S execution_071 -B build071 -DCMAKE_BUILD_TYPE=Release
cmake --build build071 -j4
ctest --test-dir build071 --output-on-failure
./build071/test_gk71_order_budget ./build071/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv
./build071/gk1_native_shell --order-budget-pad-check
xvfb-run -a ./build071/gk1_native_shell --smoke-order-budget 120
```

Tests use software injected controller commands, **not** a physical Xbox controller. Executable GUI is TESTED_PORT_LINUX 1280x720 X11; Win32 implementation remains authored/static source only, no Win32/NSIS build or stock Mesen performed. Real Classic first playable Spring, ruler, difficulty, automatic stats, true trade offers, strategic AI, diplomacy, dynasty/marriage age6-vs8, complete campaign persistence and endings are still open.
