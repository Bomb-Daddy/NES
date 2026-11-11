# GK1 EXEC0196 — Implementer / Hostile QA Handoff — 2026-11-11

**Project**: EXISTING standalone Genghis Khan I English original NES-to-native Windows/Linux C11 strategy game. NOT ROTK II nor the separate original KOEI-inspired project.

## 1. Inheritance, implementation and actual player-visible change

Verified the Collaboration folder's current last complete EXEC0195 package and GitHub execution_195_delta; used original complete 45,105-member archive, not stale EXEC0076 or a new prototype. The full 1280×720 native game source persists with both scenarios, Classic/Enhanced profiles, original checkpoint import, private exclusive stores and previous 158 regressions.

**Bug**: Classic ASSIGN actions spend replay-verified Classic budget, but the adjacent TRADE price-reference screen was still displaying source BASE 3 orders. A player spending 1 labor order saw 3 apparent available orders in Trade.

**Implemented** `gk196_classic_trade_status.c/.h`, linked into existing `gk1_app_state` and `gk1_native_shell`. The native Trade reference now displays CLASSIC DERIVED orders and spent Assign actions and Pass status. Source-authenticated original first-Spring country records, original rate and immutable base source budget are preserved; no merchant offer or actual trade is synthesized. Stock Trade entry and controller navigation verify the Classic mutable source replay, and corrupt action receipts fail closed without screen/cursor/order mutation. Enhanced source/profile behavior remains separate.

New original source flow: player selects authenticated original stock Spring from previously private snapshots, runs controller Classic ASSIGN to spend 1 of 3 manual-derived actions, returns to 14-command menu and opens Trade quote. Header now says CLASSIC DERIVED ORDERS 2, not BASE ORDERS 3. Read-only Buy/Sell quote selection changes only the screen selection, not Gold, goods or remaining orders. This is **PORT-ONLY consistency**, not a proven NES Trade action.

## 2. Verified tests

- **VERIFIED_STATIC** predecessor all 45,105 archive entries preserved byte-identical; current exact 621/621 source members independently replayed from text patch and SHA256-matched; 15 added/changed text source/packaging files, no predecessor removed; complete new archive 45,726 members, CRC PASS, no original ROM/Mesen images/save-state files.
- **TESTED_PORT_LINUX** Release **159/159** CTest, Debug **159/159**, independent complete-source clean replay Release **159/159**. Focused new C test **115** active assertions across each exact PRIVATE fresh-1175 WorkRAM/archived-1175 SaveRAM/archived-1206 World SaveRAM; inherited Classic View source test **150** assertions. Focused AddressSanitizer+UndefinedBehaviorSanitizer **115** assertions PASS. Deliberate corrupt mutable world or raw original bytes fails closed and output remains unchanged.
- Native Xvfb **120 frames each** with three independent source conditions and simulated pad input: Classic ASSIGN → Trade quote live 2 remaining; source original remains 3, no trade executed. Debian extracted app native 120-frame baseline PASS and private fresh-source trade simulation PASS.
- **OBSERVED_RUNTIME_STOCK** historical source evidence preserved separately: fresh Gold430/Rate121, archived Mongol Gold439/Rate101 and World Gold193/Rate114; **no new Mesen emulation in EXEC0196**, do not conflate different RNG/choice histories.
- **NOT RUN** Win32, NSIS, physical Xbox, genuine full campaign, newly booted original stock NES.

## 3. Reproducible run

Start with exact complete EXEC0195 archive; extract all previous execution_NNN siblings, then copy `execution_195` to `execution_196` and run `patch --batch -p1 -i GK1_EXEC0196_Complete_Integration_From0195.patch` in parent. (Or use complete historical source ZIP's `execution_196`.)

```bash
cmake -S execution_196 -B build196 -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build196 -j8
ctest --test-dir build196 --output-on-failure -j8
build196/test_gk196_classic_trade_status build196/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv /PRIVATE/fresh_14_after_wait.json /PRIVATE/stock_spring1175.mss /PRIVATE/stock_world_spring1206.mss
xvfb-run -a build196/gk1_native_shell --smoke-classic-trade-original /PRIVATE/fresh_14_after_wait.json 120
(cd build196 && cpack -G DEB)
```

Do not substitute a random one-time Spring numeric fixture for original program RNG and setup. Source original authorized ROM SHA `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`, private ZIP SHA `8b757e11b12aeb3efe93e36c10628599ca5f12e91b15c02354cd3ebbd11bf52a`. Original fresh records are WorkRAM physical 0x0DCD; archived recovered SaveRAM physical 0x0DCD; goods begin +0x22. User-provided private original source never included in output packages.

## 4. Release blockers and next executable priorities

Core original first-Spring initialization/difficulty/character generation/RNG remains OPEN; full authentic 14/27 country playable campaign, genuine merchant offers, AI/diplomacy/dynasty, battles and war aftermath, audio/art assets, Win32/NSIS and physical Xbox acceptance still OPEN. Manual audit v2.01 settled; avoid repeating older errors. Future changes must be based on this complete source (or verified later), build actual player actions and test source-authentic responses rather than producing reference-only stubs. Existing enhanced profile still contains independent port-only mechanics; do not mislabel them as stock.

**Publishing contract:** copyright-clean GitHub source delta and complete new source/installer/test/handoff in exact verified Collaboration folder parent `1d0TA1lgqsVA3AWQQ9o0FZgmvao90oCeT`. Raw original NES evidence in private Genghis folder `1mJ72ezJb9kzIGUHHXPTh3lXtiVxWjEL7`. Publication alone is not agent read/execute acknowledgement; recurring development remains enabled.
