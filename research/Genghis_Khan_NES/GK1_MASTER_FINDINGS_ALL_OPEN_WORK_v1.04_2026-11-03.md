# Genghis Khan I (USA NES) — Cumulative ROM Findings, Command System, and All Outstanding Proof

**Version 1.04 · 2026-11-03 · Research owner: Zhuge Liang / ChatGPT**  
**Completion: INCOMPLETE.** This is a research consolidation and GitHub publication, not a finished commercial ROM reconstruction, playable NES-to-Windows port, or completed economic equation. **Original-game acceptance register: 167 OPEN — 58 PARTIAL and 109 NOT RUN.** Nine rows are conditional future-port obligations, not proof requirements for interpreting original game behavior.

## 0. Scope, evidence language and publication boundary

- **Game:** KOEI *Genghis Khan*, original English US NES cartridge, not Genghis Khan II, ROTK II NES, ROTK II DOS, or the custom original KOEI Windows game. Keep all those projects' mechanics and source trees separate.
- **Input identity:** Director's authorized `Genghis Khan (USA).nes`, 262,160 bytes; SHA-256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`; iNES mapper 1/MMC1; 16 × 16 KiB PRG (256 KiB); CHR RAM; battery-backed cartridge RAM. The private ROM and cartridge SRAM captures are **not committed** to this public repository.
- `VERIFIED_STATIC`: inspected exact ROM bytes, hashes, pointer tables or bounded replay of original 6502 instructions in synthetic RAM. Bounded replay **is not** emulating the whole game.
- `OBSERVED_RUNTIME`: previous Mesen screenshots, original game execution, actual controller input and before/after SRAM captures. They establish only the specific states/actions observed.
- `MANUAL`/`GUIDE`: research reference, including contradictory or approximate source statements; not automatically original executable formulas.
- `INFERRED` / `UNKNOWN`: candidate equations, script reachability, undocumented boundary behavior and untested paths.
- `TESTED_PORT`: **NONE** for this Genghis Khan research. No evidence supports a playable original Genghis Khan Windows recreation.
- **Version rules:** No previous v1.00–v1.03 reports or Drive evidence files are overwritten. This v1.04 edition is a dated snapshot. New sources or contrary observations require a v1.05 revision rather than silently editing historical results.

## 1. Reproducible original ROM environment

Previously `Genghis_Khan_NES_Cloud_Mesen_AI_Setup_v1.01.zip` was installed/recovered on cloud Linux x86-64 with MesenCE native-AOT (upstream source commit `4b8a196e299b3de297b204fd1208d2981562c89b`) and `paulomanrique/mesen-for-ai` via `xvfb-run`. The bridge accepted `session.load_rom`, `run.step_frames`, `input.set`, `video.export_frame`, `cpu.registers` and `cpu.read_memory`. The Director's original ROM booted and AI pressed Start to reach scenario selection; both campaign starting maps were inspected. The restore archive and instructions persist in Drive; installations themselves are ephemeral across cloud containers.

The v1.00 warm-start Rate 123/Gold 428 state and v1.01–v1.02 standardized cold-start Rate 101/Gold 439 state **must not be combined as a controlled comparison**. Original-ROM human-facing testing of all commands and scenarios remains incomplete.

## 2. Original gameplay: verified economic transactions and state fields

### 2.1 Canonical observed Mongol baseline

Mongol Conquest, country 1, Spring 1175. Gold **439**, Food **208**, Troops **61**, Town **5**, Masons **4**, Food workers **27**, Artisans **6**, workforce total **103**, Morale **118**, Economy **185**, Defense **115**, Arms **116**, Skill **124**, market Rate **101**, orders **3**. The two original campaigns start at **1174** (Mongol Conquest) and **1206** (World Conquest); only startup was verified for World Conquest.

### 2.2 Completed actions (OBSERVED_RUNTIME in archived Mesen evidence)

| Command | Input / setup | Observed original-ROM result | Status/limit |
|---|---|---|---|
| `Assign → Labor` | Move four from Troops to Town | Troops 61→57, Town 5→9; total workforce 103 unchanged; orders 3→2; gold 439 unchanged | Completed in one province; all other limits unknown |
| `Tax → Change Rate` | Tax percentage 35→9 | Morale 118→222 (+104), orders 3→2, gold unchanged at moment of change | +4 per 1-point reduction fits only measured baseline; yearly income unknown |
| `Tax → Change Rate` | Tax percentage 35→8 | Morale 118→226 (+108), orders 3→2, gold unchanged | Minimum rate 0 vs 1 source discrepancy not resolved |
| `Tax → Change Rate` | Cancel | No tax/Morale/order change | Negative control |
| `Give → Gold` | Quantity 1/8/9 | Gold 439→438/431/430; Morale 118→119/121/121; orders 3→2 | Candidate stepped effect; large/high morale unknown |
| `Give → Food` | Quantity 1/4/5/8/9 | Food 208→207/204/203/200/199; Morale 118→119/121/121/123/123; orders 3→2 | Candidate `floor(food_gift/2)+1` fits these values only |
| `Train → Troops` | Baseline Skill 124 | Skill 124→143; orders 3→2; Gold/Troops unchanged | Training cap/other stats unknown |
| `Train → People` | Baseline draft eligibility 32% | Eligible fraction 32→35%, orders 3→2; candidate Charm 61→56 | Manual claims 50% cap; not tested at boundary |
| `Train → Self` | Leadership selected | Lord's Leadership 89→112, orders 3→2 | Other attribute training unknown |
| `Trade → Islam → Sell Fur` | Rate 123, quantity 1/7/9 | Whole gold received 0/4/6 at displayed 0.7 per unit; order cost 1 on positive completed transaction | `floor(q×0.7)` fits tested cases; not proven for all commodities/values |
| `Trade → China` | Different Market Rate byte values | Candidate `$7A8A` affected two displayed sale prices: at Rate 123, 3.6/1.9; at Rate 69, 2.7/1.4; no-op reentry stayed 3.6/1.9 | Strong causal link to displayed quotes, full formula/identity/merchant modifiers unknown |
| `Trade → China → Buy` | Rate 101 menu | Buyer’s market showed quotes including 6.4 and 2.2, but no completed purchase | **NOT A COMPLETED BUY** |
| `Trade → Uighur` | Sample state | Merchant unavailable | One-run precondition, not a global impossibility |
| `Tax → Extra Tax` | Extra 0 | No state changes or order cost | Negative control |
| `Tax → Extra Tax` | Extra 1/1 repeated/2 | 1: Gold 439→440; Economy 185→184; Morale 118→64; Troops 61→39; Town/Masons/Food workers/Artisans 5/4/27/6→3/2/17/3; order 3→2. Repeated 1 matched; 2 gave gold 441, Economy 183, identical listed labor/Morale | Disruptive effects real; formula/population cause unproven |
| `Tax → Extra Tax` | Extra 9 | Gold 439→448, Economy 185→176, Morale 118→61, Troops 61→37, Food workers 27→16, Town/Masons/Artisans 3/2/3; Food stock 208→209 | Don't suppress surprising Food +1; exact calculation unknown |
| `Pass` | 3 remaining orders | Orders 3→0; Spring→Summer 1175; next season 3 orders; Market Rate 101→99 | Other seasonal effects not causally isolated |
| `Move` | Single home country | “There is only a base country”, no order consumed | Not proof of two-country Move behavior |
| `Policy` | No vassal | “No vassal states”, no order consumed | Not proof of delegated AI behavior |

Related original-ROM field candidates: Troops `$6DE5`; Town `$6DE9`; Masons `$6DED`; Food workers `$6E0B`; Artisans `$6E07`; Skill `$6DE3`; Economy `$6DE7`; Food `$6E03`; Gold little-endian `$6E0D:$6E0E`; Morale `$6E0F`; tax % `$6E11`; emergency-tax-used flag `$6E13`; draft-eligibility % `$6E15`; orders `$7B36`; Market Rate `$7A8A`; Spring→Summer season marker `$65BD`; Lord Leadership `$65CD`. The field identities have different degrees of ROM correlation—none is a universal across-platform save schema.

### 2.3 Source-grounded gameplay still not numerically proven

- **People / Labor:** The original manual distinguishes drafted and not-drafted population. `Train→People` changes eligibility, `People→Draft` mobilizes; `Assign→Labor` redistributes five jobs. Reported maximum draft eligibility is 50%. Per-unit scaled population, conscription conversions and cross-province behavior remain open.
- **Year:** Spring taxes/army payroll/population food consumption, Fall Food and Artisan harvest, Summer and Winter development/weather/decay. Guides suggest the preceding four seasons' averaged labor; the exact accumulator, rounding and event order remain unproven.
- **Market:** Three networks (China/Islam/Uighur), ten country specialty commodities (Fur, Silk, Jewels, Metals, Medicines, Woodworks, Ceramics, Glass, Textiles, Perfumes), Food and six weapon types (daggers, swords, axes, lances, pikes, bombs). Merchant stock, availability, buy/sell spread, commodity identity by menu icon, absolute quotes and final gold rounding remain open. The original 500-unit marginal pricing and Charm modifiers are **NOT** part of the proven NES model; those belong to the separate custom Windows game's approved design.
- **Social:** Ordinary tax policy differs from displayed Market Rate; extra levies can sharply reduce Morale and workforce; gifts increase Morale nonlinearly; treatment of large populations, season boundaries, runaway morale and military desertion remains unknown.

## 3. Static ROM research findings (VERIFIED_STATIC where stated)

### 3.1 Cartridge interpreter and dispatch

The 256-entry VM dispatch reads script bytes at `$EFC9` and jumps through handler tables `$F788`/`$F888` to **163 distinct handler addresses**. The address-space mapping is conditional on MMC1 PRG mode 3: switchable PRG banks 0–14 in CPU `$8000-$BFFF`, fixed PRG bank 15 in `$C000-$FFFF`. This is **not** 256 in-game commands. ROM PC `$F422` points to a jump following an indirect memory store at `$F420`; `$F1CA` is immediately after a different indirect store at `$F1C8`. Those observed write PCs indicate an interpreter/memory-access layer, not tax arithmetic itself.

The `B7` opcode is a **separate** dispatcher at `$F9A8`: 48 16-bit helper entries begin at `$F9C3` (entry 0 is null, 47 non-null fixed-bank pointers). It does **not** mean 48 player commands.

### 3.2 Decoder and selected arithmetic semantics

| VM opcode(s) | Static / bounded-original-6502 instruction result |
|---|---|
| `A4 aa bb` / `A8 aa bb` | Read/write 16-bit absolute word at CPU `$bbaa`, when aligned and actually executed |
| `40–4F` / `50–5F` | Load small immediate constants into main/secondary VM register |
| `BB` / `BC` | 16-bit addition/subtraction; wraparound observed in bounded replay |
| `B5` | 16-bit multiplication (retain low word) |
| `BD` / `BE` | Left/right shifts; `BE` on 16-bit nonnegative word gives floor division by power of 2 |
| `B3` | Push current VM value onto internal stack |
| `8D` | Push sign-extended signed 8-bit immediate |
| `D7` | Conditional VM branch depending on value, replayed in selected setup |
| `E9` | VM call through target handling; full nested result not yet decoded |
| `B7` | Native-helper invocation through separate 48-pointer table |

### 3.3 Market Rate and related candidate scripts — not a final price formula

- Bank 2 script at mapped CPU `$91BA` branches via `D7 D0 91` to `$91D0` when selected working register is nonzero. Script `$91D0` has bytes `A4 8A 7A 51 BE B3 A4 88 7A 55 B5 B3 8D 15 E9 93 DB`. It reads Rate `$7A8A`, computes `floor(Rate/2)`, pushes that result; separately reads adjacent unclassified `$7A88`, multiplies by 5 and pushes; pushes constant 21; calls a nested VM target `$DB93`. These are **separate intermediate arguments**, not demonstrated to be combined into one displayed price.
- From archived original Mesen Spring and Summer RAM, Rate was **101→99** while `$7A88` remained **1**. Controlled bounded replay produces intermediate values **50→49** and **5**. This is not a real merchant quote across seasons.
- Three Rate+100 scripts have native starts bank2 `$9C92` (VM entry `$9C97`), bank3 `$8671` (VM `$8676`), and fixed bank `$DCE5` (VM `$DCEA`). Each begins with `JSR $EF85` then `A4 8A 7A 8F 64 B3`: read Rate, add 100, push. Bank2/bank3 share an 88-byte identical prefix but are not confirmed to run in the same game scenario.
- Fixed-bank VM `$DCEA` calls `$DA8B`, whose VM calls `$DA67`. In `$DA67`, observed source-byte B7 helper invocation indices are **26,14,26,14,26,15,01,15,02,27 (hex)** and targets **$FC51,$FD05,$FC51,$FD05,$FC51,$FD0E,$FA23,$FD0E,$FA52,$FC55**. Static helper call sequence only; exact returned values and arithmetic chain remain open.
- Bounded replay programs executed selected **original 6502 interpreter instructions** with synthetic RAM, not a complete Mesen original-game transaction. Rate reads were found as **five raw `A4 8A 7A` patterns** and **one raw `A8 8A 7A` pattern**; candidate byte patterns do not prove reachability.

### 3.4 Test results and their boundaries

| Test family / artifact | Recorded checks | What it actually validates |
|---|---:|---|
| Original Mesen saved-state regression (older evidence) | 18/18 | Re-reading certain recorded SRAM and negative controls, NOT 18 new gameplay features |
| v1.02 static ROM forensics | 39/39 | Header/banks, interpreter, handler table, memory patterns, negative controls |
| v1.03 VM entry/CPU replay | 21/21 | Bounded selected original 6502 instructions in synthetic RAM |
| v1.03 Rate helper decode | 25/25 | Source instruction/byte-pattern and arithmetic checks |
| v1.03 B7 native helper decode | 24/24 | Secondary dispatch map, source chain, negative controls |
| v1.03 revised VM arithmetic report | 47/47 across three recorded suites | Subset opcode semantics, earlier-source cross-checks, NOT complete economic code |

**Do not add all of these check counts and advertise the sum as independent mechanics tests**: some suites overlap and reuse prior archived captures.

## 4. Command-by-command coverage and unverified mechanics

This is the authoritative status summary for this GitHub publication. Each row contains the command/submenu names and major gaps; **all command families remain PARTIAL/NOT RUN as full mechanics**. The separate crosswalk supplies source context and the 167-row CSV supplies individual proof requirements.

| Main command | Known source/observed details | Missing essential proof |
|---|---|---|
| Tax | Rate % adjustment and Extra Tax observed in limited quantities | Complete spring tax revenue equation, minimum/maximum, yearly Extra Tax gate, desertion formula, cap/RNG |
| Assign | Labor conserved 103 in one state | Assign Army, max/zero/reassign/no-op costs, officer attribute costs, multi-country outcomes |
| Give | Gold and Food gifts measured | Item gifts, Morale/population-dependent complete formulas and caps |
| Train | Troops, People, Self completed in single sampled states | Prince, repeated training, 50% cap, ability exhaustion, attribute-gain distributions |
| Trade | 3 vendors/menu, Islam Fur sales and Rate-price causal check | Actual BUY, HIRE, buy/sell bid-ask spread, weapons, stock, arbitrage, all goods/rates/vendors |
| People | Menu and Draft input prompt observed | Complete nonzero DRAFT, Switch/Select/Demote/Promote/Marry, population/dynastic persistence |
| View | Own-state and submenus found | Foreign View cost/prerequisite, accuracy and all six views |
| Move | One-country rejection observed | Two-country moving Base, People, Gold, Food, Item, limits, adjacency |
| Policy | No-vassal rejection observed | Actual vassal policy, AI compliance, one-vs-three orders and autonomy |
| Treaty | Target selection reached | Ally/Pay Us/Become Vassal results, cost, expiry, diplomatic success |
| Spy | Submenu captured | View Report/Wreck Havoc/Kill/Search State, costs, chances, intelligence effects |
| War | No completed battle; one command probe timed out | Self tactical/Prince autoresolve, unit types, battle commands, terrain, losses, conquest and save |
| Pass | One Spring→Summer transition observed | Full four-season deterministic changes, Fall harvest, next Spring food/payroll/tax |
| Other | Menus captured | Save/quit/cold-load integrity, display/audio/UI/battle option switches and state persistence |

### Tactical battle commands (reference only, unexecuted)

Prior guide lists tactical **Move (Entire, Divide)**, **Attack (Normal, Duel, Arrows, Ambush)**, **Pass**, **Other (Demand, SOS, Raid, Hunt)**, and **Flee**. Actual battle state, stamina, tactical AI, retreat, siege, map/terrain/routing, win/lose and resource consequences remain untested. No argument from a guide is silently promoted to original ROM proof.

### Specific source discrepancies still unresolved

1. The tax percentage minimum is **1** in the original-manual citation used by the command crosswalk, versus **0** in the October 26 Master paraphrase; do not assume either without confirming both source reading and original ROM input enforcement.
2. The manual reports Assign's cost as **Leadership 5 + Planning 5**, while a FAQ abbreviates `L5,C5`. The latter could mean a different ruler attribute; reconcile by original-game execution and command-specific ability RAM writes.
3. A collection of raw byte patterns resembles Market Rate access but only aligned executed bytecode can establish the actual call path. Some source guides offer pricing examples but not exact ROM equations.

## 5. Everything still needed: complete 167-item proof register

**The accompanying CSV contains the full independent checklist, all 167 IDs, their current status, each requirement, test/proof acceptance conditions, static work suggested, and v1.03 source discoveries. All 167 are OPEN.**

### 5.1 Unfinished research grouped by subsystem

#### Scenario, ruler, country and game progression — 11 open (6 PARTIAL; 5 NOT RUN)

- **SC01 (PARTIAL):** Mongol Conquest: complete at least one full campaign path, not merely start screen — **proof:** Season-by-season captured playthrough, victory/end conditions and reproducible log
- **SC02 (PARTIAL):** World Conquest: complete at least one full campaign path, not merely start screen — **proof:** Same complete runtime evidence and end-state capture
- **SC03 (NOT RUN):** Validate every selectable ruler and starting country, including World Conquest 27-country and Mongol Conquest 14-country rosters as applicable — **proof:** Scenario/ruler/country matrix, starting resources/owners, missing/invalid selectable cases
- **SC04 (NOT RUN):** Validate all difficulty settings — **proof:** Matched starts and controlled AI/combat/economy comparisons per difficulty
- **SC05 (PARTIAL):** Ruler-stat generation and manual/automatic startup choices — **proof:** Record all settings, ranges, repeatability, starting effects and cancellation
- **SC06 (PARTIAL):** Map country IDs, capitals, names, rulers, neighbors, ownership and starting stock — **proof:** ROM table + in-game screen cross-check for both maps
- **SC07 (PARTIAL):** Extract every starting regional artisan specialty and starting item inventory — **proof:** Full data table validated on all regions in both campaigns
- **SC08 (NOT RUN):** Establish different province economic and military behavior — **proof:** Same command tested in at least two different provinces, leaders and resource levels
- **SC09 (NOT RUN):** Long-running AI turn progression and no hidden menu softlocks — **proof:** Recorded multi-year campaign, AI events, error logs, no-step timeout recovery
- **SC10 (NOT RUN):** Campaign conclusion, extinction, succession and game-over outcomes — **proof:** Original screenshots, persistent state and transitions for representative win/loss
- **SC11 (PARTIAL):** All scenario data and mechanics versioned without mixing Genghis Khan II, DOS, SNES or custom-game assumptions — **proof:** Per-finding provenance, cartridge hash and exact source version

#### Tax — 7 open (5 PARTIAL; 2 NOT RUN)

- **TX01 (PARTIAL):** Ordinary Tax Change Rate: full accepted numeric range and conflicting source lower bound 1 versus 0 — **proof:** Tests at 0, 1, 99, 100 and invalid entries with screenshot+RAM
- **TX02 (PARTIAL):** Ordinary tax morale equation including tax increases, high/low Morale, rounding, clamps — **proof:** Independent matched-state dose response and writer disassembly
- **TX03 (PARTIAL):** Ordinary tax ability costs, officer requirements, failure/no-op/cancel and order accounting — **proof:** Ability bytes, order count and failure screens for success and rejection
- **TX04 (NOT RUN):** Spring normal taxation gross-receipt formula, Economy/population/ruler/tax effects — **proof:** Two spring transitions with one-variable controls; mapped code and exact rounding
- **TX05 (PARTIAL):** Emergency Tax amount bounds, gold/economy/Morale/workforce losses and unexplained 9-gold Food +1 — **proof:** More amounts, different populations, mapped causal writes and negative controls
- **TX06 (NOT RUN):** Emergency Tax once-per-year permission, reset and second-use denial — **proof:** Execute twice in year then after next Spring; tax-use flag and messages
- **TX07 (PARTIAL):** Emergency-tax consequences including desertion, draft-pool restoration and long-term unrest — **proof:** Follow resident/drafted/troop fields across seasons and events

#### Assign — 6 open (3 PARTIAL; 3 NOT RUN)

- **AS01 (PARTIAL):** Assign Labor: zero and maximum allocation constraints for all five occupations — **proof:** Attempt 0, full workforce, over-allocation, cancel and reassign every class
- **AS02 (PARTIAL):** Labor edit buffer versus permanent SRAM commitment, no-op and ability cost — **proof:** Before/preview/confirm/cancel snapshots and command/ability counters
- **AS03 (PARTIAL):** Full workforce conservation across countries and between Draft, Hire, war, crisis, Move — **proof:** Total-population/drafted-ledger conservation with cross-command controls
- **AS04 (NOT RUN):** Assign Army: division among 1–10 field units and unit types — **proof:** Completed Army command, troop splits/types and battle deployment
- **AS05 (NOT RUN):** Assign Army rounding, troop/skill/arms changes, reallocation constraints — **proof:** Zero/maximum unit count, odd splits, changed and unchanged cases
- **AS06 (NOT RUN):** Assign cost source conflict: Leadership+Planning 5 versus guide L5,C5 — **proof:** Ability bytes and PC writer at successful Assign and insufficient-ability failure

#### Give — 5 open (3 PARTIAL; 2 NOT RUN)

- **GV01 (PARTIAL):** Give Gold quantitative Morale function across populations and caps — **proof:** Expanded amount sweep, zero, exact threshold, maximum, varied populations
- **GV02 (PARTIAL):** Give Food step function, rounding, stock constraints and high Morale cap — **proof:** Verify candidate floor(food/2)+1 outside observed 1/4/5/8/9 points
- **GV03 (NOT RUN):** Give Item: all item types, stock deduction, relative value and Morale — **proof:** Execute Item with named commodity at multiple values and rates
- **GV04 (PARTIAL):** Giving with insufficient stock, zero request, invalid quantity, cancel, ability/order costs — **proof:** Matched state negative controls for each Give branch
- **GV05 (NOT RUN):** Repeated gifts, saturation, population dependency and unrest prevention — **proof:** Sequential gifts and season continuation with controls

#### Train — 6 open (4 PARTIAL; 2 NOT RUN)

- **TR01 (PARTIAL):** Train Troops: Skill gain equation, quality caps and repeated training — **proof:** Vary starting Skill, leader stats, number of troops, difficulty
- **TR02 (PARTIAL):** Train People: eligibility gain, 50 percent cap and zero eligible headcount effect — **proof:** Repeat until cap and demonstrate Draft vs Train difference
- **TR03 (PARTIAL):** Train Self: which ability selected, gain formula, max and repeated use — **proof:** Cycle all ruler attributes and state boundaries
- **TR04 (NOT RUN):** Train Prince: select target, changed abilities, failure when no prince — **proof:** Confirmed Prince training on eligible and ineligible roster
- **TR05 (PARTIAL):** All Train branches: action/attribute costs, depleted attributes, no-ops/cancel — **proof:** Before/after ruler abilities, order counts and failure screens
- **TR06 (NOT RUN):** Seasonal Skill and Arms degradation and whether training gains persist — **proof:** Winter/Summer controlled passes with training/no-training comparator

#### Trade: rates, merchants, commodities, weapons, hire — 16 open (10 PARTIAL; 6 NOT RUN)

- **TD01 (NOT RUN):** Complete actual Trade Buy transaction for a positively identified item — **proof:** Purchase quantity, stock increase, gold decrease, order decrease; SRAM and screenshots
- **TD02 (NOT RUN):** Sale of same acquired item at a later Rate: realized arbitrage profit/loss — **proof:** Matched buy and later sell; all fees, action costs and signed net proceeds
- **TD03 (PARTIAL):** Differentiate sale and purchase quote calculations and merchant spread — **proof:** Same item and Rate buy-vs-sell screen/transaction comparisons
- **TD04 (PARTIAL):** Exact Market Rate `$7A8A` price function incl 0,69,101,123,190, limits and rounding — **proof:** Controlled dose response, source disassembly, same-state negative control
- **TD05 (PARTIAL):** Cross-season Rate reset and seasonal trajectories in both scenarios — **proof:** Spring→Summer→Fall→Winter→Spring capture with RNG controls
- **TD06 (PARTIAL):** Chinese merchant buy/sell inventory and any availability/stock limits — **proof:** Item-by-item screens and completed trades across states
- **TD07 (PARTIAL):** Islamic merchant buy/sell inventory and any availability/stock limits — **proof:** Item-by-item screens and completed trades across states
- **TD08 (PARTIAL):** Uighur merchant buy/sell inventory and unavailability cause — **proof:** Conditions for unavailable/available and completed trade
- **TD09 (PARTIAL):** Identify all ten regional goods, Food and their item icons precisely — **proof:** In-game item name/icon/price/stock crosswalk at each merchant
- **TD10 (PARTIAL):** Complete merchant price table for Food and ten goods — **proof:** Province, merchant, season, Rate, buyer/seller and units matrix
- **TD11 (NOT RUN):** Trade Hire: troop count/quality/morale/price, merchant and Rate effects — **proof:** Complete Hire, compare totals, Arms/Skill/Gold/Morale and failed hire
- **TD12 (NOT RUN):** Trade weapons: dagger, sword, axe, lance, pike, bomb availability and Arms effect — **proof:** Purchase each type; quantity, unit cost and aggregate Arms changes
- **TD13 (PARTIAL):** Transaction rounding, gold integer truncation, quantity limits, overflow, inventory caps — **proof:** Extend 1/7/9 Fur sales to varied amounts and currencies, buy control
- **TD14 (PARTIAL):** Trade costs: orders, Judgment/ability, browsing costs, failure/no-op and cancellation — **proof:** Start-to-end screenshots and all counters for both success/failure
- **TD15 (NOT RUN):** Commodity production and stock carryover, transfer and resale across provinces — **proof:** Multi-country inventory and trade replay; item conservation
- **TD16 (NOT RUN):** Economic profitability of artisan vs Food staffing vs direct purchases and hires — **proof:** Whole-year ROI simulation grounded in real action costs and prices

#### People / succession — 9 open (2 PARTIAL; 7 NOT RUN)

- **PE01 (NOT RUN):** People Draft: first committed nonzero draft, resident/drafted/troop ledger — **proof:** Reproduce numeric input, final confirmation, SRAM change, morale/quality effect
- **PE02 (NOT RUN):** Draft upper eligibility and current drafted count, eligibility 50-percent cap, insufficiency — **proof:** Boundary probes with prior Train People and different populations
- **PE03 (PARTIAL):** People Switch: reassign vassal prince and base roles — **proof:** Multi-country executed state; beginning one-country error already observed
- **PE04 (NOT RUN):** People Select: choose/appoint prince, eligibility and succession implications — **proof:** Successful and rejected selection with dynasty fields
- **PE05 (NOT RUN):** People Demote: remove or transfer prince, troop/country effects — **proof:** Two-prince multi-country experiment and undo/negative controls
- **PE06 (NOT RUN):** People Promote: heir designation, prerequisites and ruler-death interaction — **proof:** Promotion and succession tests under controlled leader death
- **PE07 (NOT RUN):** People Marry: spouse/child/heir, diplomacy effects, refusals — **proof:** Accepted/rejected marriages, children and family state
- **PE08 (PARTIAL):** All People commands order, personal ability and resource costs, canceled actions — **proof:** Counters and negative controls per subcommand
- **PE09 (NOT RUN):** Aging, heirs, ruler death, prince inheritance and dynastic/AI succession — **proof:** Multi-year and forced-state test with preserved save/replay

#### View / information — 6 open (1 PARTIAL; 5 NOT RUN)

- **VW01 (PARTIAL):** Own State view: exact fields, units, page navigation and no cost — **proof:** Screenshot complete panel and unmodified counters
- **VW02 (NOT RUN):** Own Ruler and foreign Ruler intelligence permissions — **proof:** Compare own/foreign before and after Spy View Report
- **VW03 (NOT RUN):** Allies view: treaty list, duration and vassal relationship visibility — **proof:** Compare before/after treaty and five-year expiry
- **VW04 (NOT RUN):** Prince and Child view: stats, family ties, selection, leader changes — **proof:** Known roster versus screens, after changes
- **VW05 (NOT RUN):** Stock view: item types, count, local commodity and potential display caps — **proof:** Record all stock pages against RAM known holdings
- **VW06 (NOT RUN):** Verify foreign-state intelligence costs, spy prerequisite, cancel and error — **proof:** Positive and negative target cases with orders/abilities

#### Move / multi-country logistics — 7 open (1 PARTIAL; 6 NOT RUN)

- **MV01 (NOT RUN):** Move Base after acquiring at least two countries — **proof:** Base selection, ruler location and subsequent order allotments
- **MV02 (NOT RUN):** Move People between neighbors: draft/troop accounting and morale/quality — **proof:** Before/after two-country ledger + invalid target control
- **MV03 (NOT RUN):** Move Gold: exact source/destination conservation, cost and bounds — **proof:** Positive, overstock, zero, nonadjacent and enemy target
- **MV04 (NOT RUN):** Move Food: exact stock conservation, cost and capacity — **proof:** Same independent controls for Food
- **MV05 (NOT RUN):** Move Item: each item stock and whether local-specialty constraints apply — **proof:** Transfer named item; preserve two-country item ledger
- **MV06 (NOT RUN):** Adjacency, ownership, direction, transport loss and command restrictions — **proof:** Two-country vs nonadjacent and cross-faction attempts
- **MV07 (PARTIAL):** Starting single-country Move denial consumes no order — **proof:** Already observed one-country control; confirm two-country success

#### Policy / delegated government — 6 open (1 PARTIAL; 5 NOT RUN)

- **PL01 (NOT RUN):** Acquire/appoint autonomous vassal and open functioning Policy menu — **proof:** Multi-country vassal state and completed policy change
- **PL02 (NOT RUN):** Domestic order controls: tax, labor, construction, relief, recruitment — **proof:** Record policy flags and next AI-driven resource changes
- **PL03 (NOT RUN):** Foreign policy controls: war, treaty, espionage and expansion — **proof:** Policy change followed by witnessed AI action
- **PL04 (NOT RUN):** Cost/prerequisites/limits of each policy and who can change it — **proof:** Negative control for nonvassal and unqualified ruler
- **PL05 (NOT RUN):** Vassal order counts, economic autonomy and province-state persistence — **proof:** Cross-season independent vassal state versus direct control
- **PL06 (PARTIAL):** Starting one-country No vassal states message is a precondition only — **proof:** Already observed no-op; confirm subsequent success

#### Treaty / diplomacy — 7 open (1 PARTIAL; 6 NOT RUN)

- **DT01 (NOT RUN):** Become Vassal: target selection, success and failure effects — **proof:** Accepted/refused outcomes, tribute/ownership and AI response
- **DT02 (NOT RUN):** Pay Us/tribute: amount, transfer timing, ability costs and refusal — **proof:** Before/after two countries and diplomacy counters
- **DT03 (NOT RUN):** Ally: duration, source-reported five-year term, renewal and expiry — **proof:** Successful alliance + multi-year countdown and boundary
- **DT04 (NOT RUN):** Diplomatic success probabilities and leader stats/military strength — **proof:** Matched repeated controlled tries and mapped RNG/code
- **DT05 (PARTIAL):** Geographic eligibility, neutral/nonadjacent targets and target validity — **proof:** Target prompt observed; completed accepted and rejected examples missing
- **DT06 (NOT RUN):** Diplomatic command order/ability/gold cost and cancellation — **proof:** Successful and rejected attempts with save/replay
- **DT07 (NOT RUN):** Diplomatic relations effects on warfare, spying, trade and vassal control — **proof:** Compare same commands under allies/enemies/vassals

#### Spy / intelligence — 6 open (0 PARTIAL; 6 NOT RUN)

- **SP01 (NOT RUN):** Spy View Report: paid intelligence, target validity and enabling foreign View — **proof:** Before/after access check, funds and orders
- **SP02 (NOT RUN):** Wreck Havoc: Economy/Defense damage, detection and failures — **proof:** Successful/refused tests, mapped source/target SRAM
- **SP03 (NOT RUN):** Kill: target ability/leader consequences, death and probability — **proof:** Controlled attack with success/failure/succession
- **SP04 (NOT RUN):** Search State: identify/remove enemy spies, cost and timing — **proof:** Positive/negative spy-present states
- **SP05 (NOT RUN):** Source-reported 10/50/100 gold prices, command/ability costs and affordability — **proof:** Cost matrix verified against real menu and accepted action
- **SP06 (NOT RUN):** Espionage AI response, retaliation, and cross-season persistence — **proof:** Spy action with turn advancement and adverse-response controls

#### War strategic — 8 open (0 PARTIAL; 8 NOT RUN)

- **WA01 (NOT RUN):** War Self: initiate invasion, target selection and starting force constraints — **proof:** Actual battle entrance, troop/food drawdown, valid/invalid target
- **WA02 (NOT RUN):** War Prince: delegated auto-resolve and prince stats/quality effect — **proof:** Complete AI-led battle, victory/failure and state conservation
- **WA03 (NOT RUN):** Declare war against ally, vassal, nonadjacent and neutral targets — **proof:** Validity matrix and original error messages
- **WA04 (NOT RUN):** Full war food, gold, troop/casualty and captured-stock accounting — **proof:** Prebattle/postbattle state of both countries
- **WA05 (NOT RUN):** Combat damage and victory/loss equations across troop types and commanders — **proof:** Repeated matched battle/ROM writer traces
- **WA06 (NOT RUN):** Postwar conquer/annex, ruler/prince capture and regional government — **proof:** Player conquest state and next-season operation
- **WA07 (NOT RUN):** AI-initiated invasion, defense, relief, and outcomes — **proof:** Observable independent AI actions and persisted combat result
- **WA08 (NOT RUN):** War command costs, canceled invasions and retreat/failure handling — **proof:** Counter snapshots on each end state

#### Tactical combat and battle UI — 12 open (0 PARTIAL; 12 NOT RUN)

- **BT01 (NOT RUN):** Deploy 1–10 units, cavalry/infantry/archer types and starting positions — **proof:** Original battle screenshot and unit stats
- **BT02 (NOT RUN):** Tactical Move Entire versus Divide — **proof:** Actual position changes, bounds, stamina and per-turn cost
- **BT03 (NOT RUN):** Attack Normal versus Duel — **proof:** Battle results, damage/casualties, commander effects and replay
- **BT04 (NOT RUN):** Attack Arrows — **proof:** Ammo, range, terrain, lethality, failure controls
- **BT05 (NOT RUN):** Attack Ambush — **proof:** Preconditions, surprise modifiers, skill and outcomes
- **BT06 (NOT RUN):** Battle Pass — **proof:** Turn/round progression and opposing AI response
- **BT07 (NOT RUN):** Other Demand / surrender — **proof:** Success/refusal and resulting province/army state
- **BT08 (NOT RUN):** Other SOS / reinforcements — **proof:** Reinforcement availability, travel, resource changes
- **BT09 (NOT RUN):** Other Raid / Hunt — **proof:** Food/supply changes, weather/terrain and success limits
- **BT10 (NOT RUN):** Flee / retreat — **proof:** Flee target, casualties, province state and game-over branches
- **BT11 (NOT RUN):** Terrain/siege/weather/fortification/archer paths and limits — **proof:** Battle map matrix and representative battles in each environment
- **BT12 (NOT RUN):** Battle duration (source mentions 30/60/90-day possibilities), battle food drain and terminal states — **proof:** Explicit game-time counters and exhaustion tests

#### Pass and seasonal simulation — 9 open (2 PARTIAL; 7 NOT RUN)

- **SE01 (PARTIAL):** Pass when all three orders vs one or zero; no-op and command cost — **proof:** Repeat for 3,2,1,0 orders in distinct states
- **SE02 (NOT RUN):** Complete four-season Summer→Fall→Winter→Spring cycle — **proof:** Continuous original ROM capture with season markers & resources
- **SE03 (NOT RUN):** Fall Food and Artisan harvest amounts and four-season averaging claim — **proof:** Multi-year assignment history and controlled Fall harvest
- **SE04 (NOT RUN):** Spring taxation credited, soldier payroll paid and food consumed — **proof:** Exact per-field before/after state across spring boundary
- **SE05 (NOT RUN):** Births, aging, population increase, sickness/starvation — **proof:** Distinct seasonal events with probability/replay and new population
- **SE06 (NOT RUN):** Summer storm and military Skill degradation — **proof:** Positive/negative seasonal observations in different territories
- **SE07 (NOT RUN):** Winter Economy/Defense damage and Arms degradation — **proof:** Before/after states and climate controls
- **SE08 (PARTIAL):** Market Rate spring reset and within-year decay incl province variation — **proof:** Full-year matched Province/Rate and stochastic controls
- **SE09 (NOT RUN):** Full seasonal operation order: AI actions, growth, tax, wages, food, harvest and market — **proof:** Observed event ordering with watchpoints and code trace

#### Other and cartridge services — 9 open (0 PARTIAL; 9 NOT RUN)

- **OT01 (NOT RUN):** Save native in-game game file — **proof:** Save menu and persistent battery-backed data
- **OT02 (NOT RUN):** Quit emulator/cold-load original save with equal game state — **proof:** Cold-run checksum, scenario/province/ruler/turn/stock comparison
- **OT03 (NOT RUN):** Wait text speed changes and persists — **proof:** Toggle screenshots, settings state and reboot behavior
- **OT04 (NOT RUN):** Music option toggle and volume/track behavior — **proof:** User-visible UI plus audio playback test
- **OT05 (NOT RUN):** Sound option toggle and effects behavior — **proof:** Audio event and off/on comparison
- **OT06 (NOT RUN):** Display toggle/setting effects — **proof:** Screenshot-level comparison and controller navigation
- **OT07 (NOT RUN):** Animation toggle/setting effects — **proof:** Equivalent battle/turn captures with mode on/off
- **OT08 (NOT RUN):** Battle option settings and consequence in combat — **proof:** Game option change and actual subsequent battle
- **OT09 (NOT RUN):** End/quit behavior, unsaved-state handling and reload — **proof:** Exit/restart negative and positive save cases

#### Cross-cutting numerical and content fidelity — 14 open (8 PARTIAL; 6 NOT RUN)

- **CF01 (PARTIAL):** NES integer widths, signedness, endian representation, clamps and overflows — **proof:** Boundary and mutation probes on every important economic field
- **CF02 (PARTIAL):** Total residents vs eligible pool vs mobilized/allocated workers distinction — **proof:** Population-ledger proof across Draft, deaths and transfers
- **CF03 (NOT RUN):** Multi-year and multi-country economic conservation for currency, Food and goods — **proof:** Independent ledger from original state transitions
- **CF04 (NOT RUN):** Artisan output by specialty, region, assigned labor and prior-season history — **proof:** Matched province/season tests and code extraction
- **CF05 (NOT RUN):** Economy and Defense development curves by Town/Masons, caps and decay — **proof:** Staffing dose response, season updates and adverse events
- **CF06 (PARTIAL):** Morale cap, rebellion/robbery outcomes, tax and relief effects — **proof:** Threshold sweep, negative controls and event traces
- **CF07 (NOT RUN):** Weapon quality Arms and Skill interaction with troop strength and losses — **proof:** Buy/Train/War intersecting state changes
- **CF08 (PARTIAL):** Different rulers' abilities and command feasibility/costs — **proof:** Stat-probe matrix and deliberate insufficient-ability failures
- **CF09 (NOT RUN):** AI economics: tax, hiring, allocation, trading, diplomacy and cheat/fairness rules — **proof:** Controlled AI province observation + code paths
- **CF10 (PARTIAL):** Exact menu input handling, confirm/cancel, numerical input and event timing — **proof:** Controller sequences that always reach same state; timeout recovery
- **CF11 (PARTIAL):** Every game text/status/error message, portraits/sprites/assets and source mapping — **proof:** Complete script/asset extraction and ROM-to-screen crosswalk
- **CF12 (NOT RUN):** Music and sound assets and when invoked — **proof:** Audio extraction or original output and event evidence
- **CF13 (PARTIAL):** Map topology and province/country numbering across both campaigns — **proof:** All mapped country/adjacency records with validated screen IDs
- **CF14 (PARTIAL):** Confidence classification separated across manual, guide, emulator, source and proposed designs — **proof:** Evidence-linked register covering every resolved and unresolved rule

#### ROM reverse engineering, repeatability and forensic QA — 14 open (9 PARTIAL; 5 NOT RUN)

- **RE01 (PARTIAL):** Confirm storage offsets/bank mapping for all known RAM candidates — **proof:** `$6Dxx`, `$6Exx`, `$7A8A`, `$7B36`, `$65xx` mapped across bank/region
- **RE02 (PARTIAL):** Resolve `$F422` and `$F1CA` current bank/PRG file offset and computation routines — **proof:** Mapper trace plus original-byte disassembly and branching controls
- **RE03 (NOT RUN):** Recover all command handler entry points and call graph — **proof:** Code/data coverage and bank-aware call table for 14 commands
- **RE04 (NOT RUN):** Reverse-engineer RNG, season timing and determinism limits — **proof:** Full CPU tracing and matched-state seed/time replay
- **RE05 (NOT RUN):** Reproduce every numeric formula from code, then validate in a second independent start — **proof:** ROM arithmetic, test vectors, boundary/rollback, no exceptions
- **RE06 (PARTIAL):** Verify candidate SRAM field sizes and copies/overlays (e.g. `$6E0D:$6E0E` Gold) — **proof:** Address mutation and writer traces, alias/duplicate controls
- **RE07 (PARTIAL):** Full graphics frame evidence: exact actual screenshot per menu/submenu — **proof:** Screenshot+input manifests and absent-menu gap coverage
- **RE08 (PARTIAL):** Run original-ROM evidence test suites after restore; do not overclaim archived test logs — **proof:** Exact commands, stdout/stderr/exit and artifact hashes in new clean host
- **RE09 (PARTIAL):** Preserve all timed-out/incomplete/negative test logs and failed controls — **proof:** Full raw logs and failure status, not curated proof
- **RE10 (PARTIAL):** Separate cold-start and warm-start Rate/Gold baselines — **proof:** Environment, setup script, RNG/time records for each benchmark
- **RE11 (NOT RUN):** Save/reload cartridge RAM faithfully and test corruption/recovery — **proof:** Cold-load state, battery save file format and negative corruption control
- **RE12 (PARTIAL):** Test all known manual/FAQ disagreements instead of silently resolving — **proof:** Tax 0/1 bound, Assign ability letters, averaged harvest, Rate ranges
- **RE13 (NOT RUN):** Verify SNES counterpart only if supplied and relevant; NES evidence remains NES-specific — **proof:** Separate ROM hash and execution evidence rather than platform assumption
- **RE14 (PARTIAL):** Refresh complete evidence/test register from raw originals after every batch — **proof:** Versioned, hashed manifest with latest authoritative source and unresolved tests

#### Reimplementation and human acceptance (separate from research) — 9 open (2 PARTIAL; 7 NOT RUN)

- **IM01 (NOT RUN):** Decide exact target: standalone faithful Genghis Windows reproduction versus research input to custom KOEI game — **proof:** Director scope/authority before modifying any separate game repository
- **IM02 (PARTIAL):** Implement Genghis economy independent deterministic reference functions when exact rules are known — **proof:** Current fixtures cover observed amounts only; no complete economy engine
- **IM03 (NOT RUN):** Enforce model conservation of population, workforce, Food, gold and items — **proof:** Unit, property, mutation and save/replay tests against original vectors
- **IM04 (NOT RUN):** Integrate merchant networks, Rate, weapons, hiring and seasonal economics in authorized codebase — **proof:** Actual UI-controlled gameplay and direct NES-to-port parity vectors
- **IM05 (NOT RUN):** Integrate 14 command families, AI autonomy, succession, war and battle only if faithful target authorized — **proof:** Real executable gameplay, not menu-only or fixture passes
- **IM06 (NOT RUN):** Build complete campaigns and static scenario data if a faithful port is in scope — **proof:** All source-defined starts and end states, cross-scenario regression tests
- **IM07 (NOT RUN):** Test native Windows launch, controller-only navigation, save/quit/cold-load and packaging if port approved — **proof:** Hardware-specific witnessed acceptance, not Linux emulator simulation
- **IM08 (PARTIAL):** Separate original NES rules from custom game's approved Body Points, unified merchant market and 500-unit pricing — **proof:** No unapproved cross-import; independent source and design labels
- **IM09 (NOT RUN):** Human playability and final director sign-off for any release, not for research alone — **proof:** Player run-through, regression log and explicit acceptance

### 5.2 Completion gates

- Every scenario/ruler/country startup and ending must be documented from the original game, not merely opening-screen evidence. A static scenario table may establish data, not a complete campaign.
- Every command/subcommand requires original behavior, costs, prerequisites, success/failure, cancel/no-op, at least two varied inputs, boundary and independent scenario comparisons, plus exact RAM/ROM dataflow where possible.
- Before accepting economics: full population-workforce accounting (Train People→Draft→Assign), all four seasonal phases, tax/payroll/food/harvest formulas, merchant buy/sell/hire and profit/loss with quantities/spreads, gold/stock rounding and edge cases.
- Before accepting diplomacy/war/AI: actual multi-country controlled game state, game-executed choices, tactical Self and autonomous Prince results, cross-season persistence, defeat/victory and cold-load.
- Before claiming any port: independently scoped codebase/Director requirements, parity test against original, desktop launch, controller usability and save/cold-load. **None of this is achieved by committing these documents.**
- The nine optional-port rows remain conditional and do not authorize mixing original Genghis mechanics into `Original_KOEI_Windows_Game` or the canonical ROTK II NES/DOS code trees.

## 6. Next technically decisive tasks (not completed here)

1. Decode B7 helper bodies `$FC51`, `$FD05`, `$FD0E`, `$FA23`, `$FA52`, `$FC55`; propagate returned values through `$DA67 → $DA8B → $DCEA` and the separate `$91D0 → $DB93` candidate.
2. Establish the reachable trade entry point and live mapper bank; differentiate purchase, sale, hire, availability, and per-good/vendor quote tables.
3. Resolve `$7A88` semantics, active Rate-update script and exact final price presentation/rounding; execute both buy and resale in controlled states before accepting arbitrage.
4. Decode `People→Draft` and the Spring/Fall accounting handlers, then test population units and seasonal averaging with earlier original Mesen snapshots and new independent traces.
5. Obtain a multi-country state for Move/Policy/People dynasty/diplomacy, a real War battle, and cold-save/load; resolve every remaining row in the 167-item matrix with evidence (not just static dispatcher hits).
6. Preserve original byte hashes, bank/CPU/file offsets, exact commands, raw stdout/stderr, negative controls and before/after screenshots; label each status conservatively.

## 7. Provenance and links

- [Original Genghis NES Master, researched reference (Drive)](https://docs.google.com/document/d/1MCWq6KnvkV-u0GlDWnA8Ct47BiFmfQtQmEECVKNljLA/edit)
- [Command-by-command source crosswalk v1.00 (Drive)](https://drive.google.com/file/d/1zpHkze9ZhHJiLGVYT92ORVFLC0PAI6Iu/view)
- [Open proof register 167 rows v1.00 (Drive)](https://drive.google.com/file/d/1r6UODVd97ws3fjCn1dcc1eiOe4i4qI4B/view)
- [Original-ROM live Mesen report v1.01](https://drive.google.com/file/d/1KtDHN3C3MzLCoWy15BtRSTf9BTsJEbtL/view)
- [Original-ROM live Mesen report v1.02](https://drive.google.com/file/d/1ltE8ST9LZKYMXc4uVX9nVKb4wCAPVkjb/view)
- [Command crosswalk + Mesen evidence v1.02](https://drive.google.com/file/d/1M4vObtuZrQ35fbJHn-53ffg4xETIo7LA/view)
- [AI offline source QA v1.01](https://drive.google.com/file/d/11oqOimIwqnikvnUZITiLTyNCUOa6vIDA/view)
- [Static ROM analysis v1.02](https://drive.google.com/file/d/1W1QJfMX9_c4EILz6eCxmWaa67PP_wTob/view)
- [167 item static matrix v1.02](https://drive.google.com/file/d/1nO-b4xWuJJ-gpCHr6rMFxdU1m2gsQcAL/view)
- [VM interpreter static validation v1.03](https://drive.google.com/file/d/1Z-qQjW1eBCxJuGA7UupSZBJ5l2CUIGGk/view)
- [VM handler evidence archive v1.03](https://drive.google.com/file/d/1EL5YbWuWPTg9rjDeQI3EbqehDarb7_xq/view)
- [Cloud Mesen recovery guide](https://drive.google.com/file/d/1iu5Vpd1A0p2n6zeY0VIqUYnivx_M1H5d/view)

**No game mechanics are newly marked complete by GitHub publication. Completion remains INCOMPLETE until the listed requirements are individually evidenced and accepted.**
