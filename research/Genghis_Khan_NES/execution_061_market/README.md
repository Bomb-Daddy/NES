# GK1 EXEC0061 — NES marketplace transaction engine (source-bounded)

**2026-11-07; Zhuge Liang; standalone Genghis Khan I English NES → Windows project.**

New executable C11 `gk61_market.c/.h` implements the original 3×17 merchant base table and source-derived integer quote/transaction arithmetic from the already-completed GK1 economic mapping v1.07. This is a **subsystem for integration into the existing native desktop game**, not another game and not a user-accepted Windows build. All 41 pre-setup country records and previous source are preserved.

## Provenance / authority

Original economic master: `research/Genghis_Khan_NES/v1.07/GK1_ECONOMIC_MAPPING_v1.07_2026-11-04.md` and original country schema `execution_049/gk1_scenario_factory.h` (Git blob `9605f7c08ed1b80ac4e82a6fee05879f48b58946`). The exact scenario country type is consumed directly; this source archive includes only the already-public header, no copyrighted ROM, SRAM, images or audio.

Merchant bases (17 items × China/Islam/Uighur) use original numeric values. With global `Rate`, buy quote tenths is floor(T×(Rate+100)/100), sell quote tenths max(1,floor(buy×90/100)), hire quote tenths floor(60×(Rate+100)/100). Positive goods buy gold debits ceil(q×buy/10), sell credits floor(q×sell/10). Weapons change Arms by floor(9×**China** base×(q+1)/drafted P), including purchases from other merchants, capped999. Hiring q increases Troops without decreasing Undrafted reserve, credits original weighted Arms/Skill100 and Morale50+rand11 using old drafted P, and charges ceil(q×hire quote/10) gold. Source limits and 999 caps preserved. Every successful action charges **one strategic order plus Judgment5** (Classic) or **shared Body BP5** (modified profile), not both. Accepted tiny sales may credit 0 gold. Browsing, rejected action, no offer, no stock, zero-quantity, or unaffordable actor leaves all caller records unmodified.

## Critical integration guard

`Gk61Market.playable` defaults to false and must be set by the caller **only after actual original source-backed setup/first-Spring logic has completed**. EXEC0059 and earlier native shell retains pre-setup drafts; those are NOT real first-turn game state. The `GK61_NOT_PLAYABLE` error prevents the engine from pretending to transact against source templates. Merchant presence, global Rate, buy/sell offers and the Hire morale roll are inputs from the **original scheduler/RNG**, not invented by this module. The transaction engine does not infer or create these values, render original UI, determine AI actions, update independent saves, or advance seasons. It must be wired into the existing game at its actual strategic turn boundary after those contracts close.

## Exact code/QA

Files: `gk61_market.c`, `gk61_market.h`, `test_gk61_market.c`, CMakeLists, this README, logs; companion `execution_049/gk1_scenario_factory.h` is exact from published source. Test command (from the archive root):

```
cmake -S execution_061 -B build61 -DCMAKE_BUILD_TYPE=Debug
cmake --build build61 -j2
ctest --test-dir build61 --output-on-failure
./build61/test_gk61_market
```

Linux GCC/CMake Debug, Release, AddressSanitizer+UndefinedBehaviorSanitizer passed in the originating isolated worktree; direct harness ran **466 assertions**, including all 3×17 ×2 rate fixtures, exact China Jewel buy/resale, Uighur Dagger Arms, Hire8 weighted quality, zero-credit sale, actor BP/ability/orders, capacity, corrupt inputs and rollback. Clean extracted source build was independently run. These are **TESTED_PORT_LINUX_C11**, not `OBSERVED_RUNTIME` of a new NES run or `TESTED_PORT_WINDOWS`. The original v1.07 NES economic evidence was reused, not repeated.

## Integration priority

Merge these same files into the current complete native C11 Genghis game tree (at EXEC0060 or newer verified source), preserving controller-only 1280×720 UI, 14/27 scenarios, source profiles, battle and research saves. On arrival at the original first playable Spring, wire Rate/presence/offer masks and approved actor command cost into the existing Trade menu, and connect updates to persistent country and actor state. Next tests must verify **real native game menu/controller/action/save/quit/cold-load**, not solely this math module. Maintain Classic vs approved shared-BP profiles and never replace already verified original economic formulas with invented balance assumptions.

**Status:** executable source contract built and tested; actual player-facing Trade and native Windows/Xbox acceptance remain OPEN.
