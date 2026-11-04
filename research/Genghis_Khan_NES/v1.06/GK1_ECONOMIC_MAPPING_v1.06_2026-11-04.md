# Genghis Khan NES — Economic Mapping v1.06

**Date:** 2026-11-04 UTC (2026-11-03 evening in America/Detroit).  
**Author / research owner:** Zhuge Liang, ChatGPT/Codex.  
**Scope:** Original English NES Genghis Khan; continuation of the v1.04 GitHub handoff and v1.05 Drive source reconstruction.  
**Status:** PARTIAL / economic research INCOMPLETE. This version adds executed original-game evidence and reproducible source checks. It does not certify the full economy, a Windows port, or the 167-obligation register as complete.

## 1. What changed

The prior handoff left the connection between the reconstructed Rate routine and actual Trade prices open. This pass ran the original ROM in cloud Mesen and recorded the live VM arguments and results. China's price screens use table row 0; Islam uses row 1. Buyer quotes are integer tenths, seller quotes apply an additional integer 90% calculation, and buying rounds the final whole-gold debit upward in the tested cases.

Completed transactions now include purchases of 1 and 11 units of commodity index 2, resale of the single acquired unit, a one-unit purchase of commodity index 5, and an eight-troop hire. A natural Spring→Summer transition changed Rate 101→93 and renewed the three orders. Controlled RAM interventions separately tested price response at Rates 0, 69, 101, 123 and 190. These interventions are not claims about naturally reachable Rates.

The full register retains all 167 original IDs and requirements. Four previously NOT RUN obligations now have PARTIAL evidence (TD01, TD02, TD11 and TD15): **62 PARTIAL / 105 NOT RUN / 0 newly accepted complete obligations**. Specific bounded findings below are verified; their broader parent requirements remain open.

## 2. Authority and reproducible inputs

- [Previous GitHub handoff v1.04](https://github.com/Bomb-Daddy/NES/blob/main/research/Genghis_Khan_NES/GK1_EVIDENCE_PROVENANCE_AND_HANDOFF_v1.04_2026-11-03.md) and [master v1.04](https://github.com/Bomb-Daddy/NES/blob/main/research/Genghis_Khan_NES/GK1_MASTER_FINDINGS_ALL_OPEN_WORK_v1.04_2026-11-03.md). These remain preserved.
- [v1.05 source reconstruction](https://drive.google.com/file/d/1Z0WRR6c_lSyMYvOwFYh6sojjyCDAkBmz/view) and [v1.05 evidence ZIP](https://drive.google.com/file/d/11LX0buZDUJglLmu9G8QfL_GIpdhq_e9u/view). This later Drive evidence was not present in the v1.04 GitHub snapshot and is incorporated here.
- [New private runtime evidence ZIP v1.06](https://drive.google.com/file/d/1e_Prnv32dmszNZjhg_nYfF4bnlnFpT-J/view): original screenshots, SRAM, input/request/response logs, read-only VM traces, failed attempts, manifest, driver, bridge patch and replay tools. No ROM included. Archive SHA-256: `927028490b1d0bc26d87dadaffa8f43500d2ea8aa8f1e8c73d46da2a08761e2b`; 1,109,966 bytes; 416 entries, including 405 manifested evidence files.
- [Cloud Mesen restore bundle](https://drive.google.com/file/d/10a4CW22GYVbOOwAYx4MWPZ1ED496HzjD/view) and [restore guide](https://drive.google.com/file/d/1iu5Vpd1A0p2n6zeY0VIqUYnivx_M1H5d/view).
- ROM SHA-256: `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`; 262,160-byte iNES file. Private Director-supplied ROM.
- Mesen executable SHA-256: `f02685dafe559addf8dd8d769621a89777137d0c4592436aed65694a7e740676`. Restored Linux native-AOT build, commit `4b8a196e299b3de297b204fd1208d2981562c89b`.
- Repeated cold-start SRAM SHA-256: `dd22847140cc0397ab8f42e29f1832681628ed52bfa17e95041c288afc4ef773`. Mongol Conquest, Spring 1175, country 1, Gold 439, Food 208, Troops 61, Rate 101, three orders, frame 3615.
- GitHub base inspected: `6a89836c9c322c5ecd6e24b0ae6b328f52333ff6`. Current Drive authority was inspected; this is research for Genghis Khan, not authorization to change either ROTK II project or the separate original KOEI-inspired game.

Evidence labels: **VERIFIED_STATIC** means original instruction replay or inspected source; **OBSERVED_RUNTIME** means the actual ROM ran in Mesen and its output was captured; **INFERRED** is a candidate not accepted as fact; **UNKNOWN** remains unproved. No result here is TESTED_PORT or physical Xbox/Windows validation.

## 3. Price calculations established in this pass

`R` is the unsigned 16-bit value read at CPU address `$7A8A`. `m` and `i` are merchant-row and item-column arguments. The bounded checks use R in {0,1,69,101,123,190,255}, merchant rows 0–2 and columns 0–16. These input bounds do not establish natural generation limits or all 16-bit overflow behavior.

```text
T = BYTE[$7DE5 + 17*m + i]
buyTenths  = floor(T * (R + 100) / 100)
sellTenths = max(1, floor(buyTenths * 90 / 100))
hireTenths = floor(60 * (R + 100) / 100)
```

| Routine | Physical 16-KiB PRG bank | Native CPU entry / VM entry | Stop before VM return | Evidence |
|---|---:|---|---|---|
| Buyer table quote | 15, fixed | `$DCE5` / `$DCEA` | `$DD00` | v1.05 replay plus live quote arguments/results |
| Seller quote | 15, fixed | `$DD01` / `$DD06` | `$DD19` | Original-6502 replay and live Islam seller quotes |
| Hire quote | 3, switchable | `$8671` / `$8676` | `$8682` | Original-6502 replay; physical PRG execution watch and live hire |
| Purchase gold debit | 3, switchable | `$8644` / `$8649` | `$865C` | Original-6502 replay; 1-unit and 11-unit purchases |

For file offsets, add the 16-byte iNES header to the physical PRG offset. A CPU address alone does not establish a switchable bank. The hire watch at physical PRG offset `$C671` (decimal 50801) observed CPU `$8671`; fixed-bank quote traces do not require guessing a switchable bank.

For affordable, bounded purchases tested here, the original debit routine gives:

```text
newGold = floor((oldGold*10 - quantity*buyTenths) / 10)
        = oldGold - ceil(quantity*buyTenths / 10)
```

A quote of 6.4 therefore costs 7 whole gold for one unit and 71 gold for eleven units. Do not substitute `floor(quantity*6.4)` for the debit. Source tests cover gold {1,7,30,439,999}, quotes {1,5,18,22,64,120,181}, quantities {0,1,2,3,10,11,17}, filtered to affordable cases. This is not proof of transaction caps, underflow protection, inventory limits, or behavior outside those bounds.

The final sale-credit routine was not isolated in this pass. The current 5.0 sale credits 5 gold. Earlier v1.04 evidence recorded Islam Fur sales at displayed 0.7: quantities 1/7/9 credited 0/4/6 gold. `floor(quantity*sellTenths/10)` fits those observations, but remains a bounded runtime inference rather than a newly proved general credit routine.

**Merchant spread matters twice:** the 90% seller adjustment applies to each merchant's own table value. The China-buy / Islam-sell roundtrip also crosses different base values (32 versus 28). Its loss cannot be attributed only to the 10% adjustment.

## 4. Table values and item identity

The table bytes below come from the reproducible baseline SRAM. Row 0=China and row 1=Islam are tied to live merchant headings and VM calls. Row 2=Uighur remains an **INFERRED** row identity; the arithmetic checks on that row do not prove merchant availability or inventory. Columns are zero-based. This is a base-price table, not proof every merchant offers every item.

| Item index | China row 0 | Islam row 1 | Row 2, identity not runtime-confirmed |
|---:|---:|---:|---:|
| 0 | 10 | 4 | 8 |
| 1 | 16 | 37 | 25 |
| 2 | 32 | 28 | 25 |
| 3 | 18 | 22 | 10 |
| 4 | 14 | 8 | 15 |
| 5 | 11 | 14 | 7 |
| 6 | 13 | 15 | 18 |
| 7 | 8 | 5 | 20 |
| 8 | 20 | 15 | 31 |
| 9 | 18 | 29 | 38 |
| 10 | 10 | 11 | 11 |
| 11 | 20 | 15 | 25 |
| 12 | 80 | 75 | 82 |
| 13 | 100 | 110 | 90 |
| 14 | 40 | 40 | 35 |
| 15 | 60 | 70 | 65 |
| 16 | 150 | 200 | 250 |

Newly established inventory identity is deliberately narrower than a named-good crosswalk:

| Index / field | Country 1 address | What is verified |
|---|---|---|
| Commodity index 2 | `$6DF3` unsigned word | China purchase increases it; Islam resale decreases the same word |
| Commodity index 5 | `$6DF9` unsigned word | Second China purchase increases it; it is not Food |
| Food | `$6E03` unsigned word | Remains 208 throughout these transactions; earlier Give Food mapping retained |
| Gold | `$6E0D` unsigned word | Actual debits/credits below |
| Orders | `$7B36` byte | Each completed transaction shown below consumes one |

Item 2 resembles a jewelry icon, but a complete independent name/icon crosswalk remains open. Do not promote that visual guess into a verified name. Some exploratory filenames say `food_*` or `islam_*`; these are operator labels, not proof of item or merchant identity. The corrected interpretation is explicit in the evidence index and tests.

## 5. Completed original-game transactions

All first five entries below are at Spring 1175, country 1, Rate 101. Amounts are whole gold; quotes are displayed gold per unit.

| Experiment | Displayed quote | Stock / troops before→after | Gold before→after | Orders before→after | Evidence in ZIP |
|---|---:|---|---|---|---|
| China buy 1, index 2 | 6.4 | `$6DF3`: 26→27 | 439→432 | 3→2 | `roundtrip/baseline`, `buy_quantity`, `buy1_result` |
| Islam sell that 1, index 2 | 5.0 | `$6DF3`: 27→26 | 432→437 | 2→1 | `roundtrip/islam_sale_quotes`, `sale_quantity`, `sale1_result` |
| China buy 1, index 5 | 2.2 | `$6DF9`: 50→51; Food stays 208 | 437→434 | 1→0 | `roundtrip/food_quantity`, `food1_commit`, `food_settled` |
| China buy 11, index 2; separate cold start | 6.4 | `$6DF3`: 26→37 | 439→368 | 3→2 | `live_trade/baseline`, `return_menu` |
| Hire 8 after that purchase | 12.0 | Troops: 61→69 | 368→272 | 2→1 | `live_trade/return_menu`, `hire8_result` |

The one-unit roundtrip returned the commodity stock to its original count and lost **2 gold**, while consuming **two orders**. Food stayed 208. This is a same-season, same-Rate, cross-merchant loss; it is not a later-Rate arbitrage test or proof that no profitable arbitrage exists.

The eight-troop hire also reduced Morale 118→113, Arms 110→109 and Skill 124→122 in the observed country data. These observations do not establish the general dilution formulas, failed-hire behavior, maximum hire, or merchant effects. The hire was reached during exploration; it is reported as the action actually executed, not the originally intended navigation.

The natural transition after exhausting the three Spring orders reached Summer 1175 with Rate **93**, orders **3**, Gold **434**, Food **208**, index-2 stock **26**, index-5 stock **51**. Evidence: `roundtrip/food_settled`, `season_progress1`, `summer_progress2`. Only this transition is proved. A full year, World Conquest, harvest, payroll and workforce-history controls remain open.

## 6. Live Rate interventions and negative control

After the natural Summer transition, the researcher explicitly changed the word at `$7A8A` through `cpu.write_memory` and reopened China's Buyer’s Market. The on-screen main-panel Rate label was not repainted and continued to show 93; **the SRAM word and read-only VM trace establish the intervened Rate**, not the stale label. Gold, Food and orders did not change during these quote probes.

| RAM Rate | China item 3, base 18 | China item 5, base 11 | China item 7, base 8 | Context |
|---:|---:|---:|---:|---|
| 0 | 1.8 | 1.1 | 0.8 | Summer intervention |
| 69 | 3.0 | 1.8 | 1.3 | Summer intervention |
| 101 | 3.6 | 2.2 | 1.6 | Summer intervention |
| 123 | 4.0 | 2.4 | 1.7 | Summer intervention |
| 69 again | 3.0 | 1.8 | 1.3 | Return-to-input control after intervening values |
| 93 restored | 3.4 | 2.1 | 1.5 | Restored natural Rate |

A fresh cold-start trial then set Rate 190 and captured China's Spring Buyer’s Market: item 2 was **9.2** and item 5 was **3.1**, with trace returns **92** and **31**, Gold 439 and three orders. The main-panel Rate label remained the old 101. Evidence: `rate190/china_buy_rate190` and `rate190_vm_trace.jsonl`. This is a different seasonal inventory; it must not be inserted into the Summer table as though the same columns were offered.

Live source observations also connect Islam's seller display 5.0 to row 1/item 2: the inner buyer calculation returned 56 and the seller routine returned 50. For row 1/item 10 the corresponding values were 22→19, displayed 1.9.

The read-only callback observes the original VM dispatch at CPU `$EFC9` and records selected VM entries, frame, arguments and outputs. It does not rewrite emulated state. Explicit Rate interventions are separately logged as write-memory actions. Traces during AI turns are retained but excluded from player price-screen comparisons.

## 7. Verification and corrections

The packaged suite passed **72 test methods**: 55 rerun prior source methods, 8 new source methods and 9 captured-evidence methods. The new seller grid contains 357 Rate/row/column combinations; this is an arithmetic grid, not 357 completed trades. The CPU harness executes the ROM's instructions in synthetic memory, whereas the separate runtime evidence comes from Mesen running the game.

Negative controls include changed seller percentage and hire-base operands, rejection of a deliberately corrupted purchased-stock capture, table-byte versus adjacent-byte controls inherited from v1.05, and the live return to Rate 69. The source table SRAM test now uses the independently recorded baseline hash rather than computing its expected hash from the same bytes under test.

Retained failures and exclusions:

- Initial host package/setup and two smoke attempts failed; later real ROM execution succeeded. Their logs remain in the private archive.
- The initial hire replay stopped at an operand byte, `$8681`, rather than the actual VM return `$8682`; correcting the source boundary made the replay terminate correctly. The failure log is preserved.
- `food_*` labels in `roundtrip` refer to index 5, not Food. Tests assert that Food did not change.
- `controlled_islam_*` price-probe filenames actually show China after the menu transition. Merchant identity comes from the heading and row-0 trace. They are not Islam quote evidence.
- `live_trace/resell_acquired_*` reached Assign, not a resale. They are excluded from transaction claims.
- The first Rate 190 Summer probe did not refresh the quote screen. The next attempt reached Hire (17.4), not the buyer screen. Both are excluded from commodity-quote acceptance. The first packaged test run detected this and failed; the independent clean-start price trial corrected the evidence gap. Both the failing and passing test logs are retained.
- The trace whitelist includes `$8680`, which is inside the hire-call operand, rather than hire return `$8682`; therefore it does not establish the final hire quote by a return event. Hire quote proof here uses the correct source replay, physical entry watch and displayed quote/transaction. Do not claim a hire return trace was captured.

No failed trial was relabeled as a successful transaction. No Windows executable, physical controller or port implementation was tested.

## 8. Reproduction

Keep the private ROM and evidence outside the public repository. Extract the linked v1.06 evidence archive. With this version's `tools` directory and Python 3:

```bash
export GK_ROM="/private/Genghis Khan (USA).nes"
export GK_SRAM="/private/extracted/evidence/roundtrip/baseline.sram"
export GK_EVIDENCE="/private/extracted/evidence"
python3 tools/run_all.py > validation.stdout 2> validation.stderr
```

Expected: **72 tests, OK, exit 0**. Omitting `GK_EVIDENCE` deliberately runs only the 63 source methods and does not verify runtime captures. `GK_SRAM` must be the exact baseline whose hash is recorded above. The legacy archived-SRAM fallback is retained via `GK_ARCHIVE`, but that fallback is not a replacement for verifying the current baseline hash.

To re-execute the game, restore the linked Mesen environment under `recovery/mesen`, install/provide Xvfb, xauth and the native libraries recorded in the private setup logs, apply `bridge_readonly_trace_v1.06.patch` to the preserved bridge, and run the archived `research_session.py` from that directory layout. Set `GK_ROM` and `GK_TRACE_OUTPUT`; export `recovery/xvfb/usr/bin` on PATH and its `usr/lib/x86_64-linux-gnu` on LD_LIBRARY_PATH if using the locally extracted dependencies. For each `kind=action` record, send only its `value` object, serialized as one JSON line, in chronological order through the driver's stdin. Do not replay logged low-level requests as well as their parent actions. The deterministic `boot` prefix is in the driver. Allow enough frames for each actual menu; confirm the heading and capture before assigning a semantic label. The raw event files retain all navigation mistakes.

## 9. Remaining economic work and next executable task

[The v1.06 register](https://github.com/Bomb-Daddy/NES/blob/main/research/Genghis_Khan_NES/v1.06/GK1_COMPLETE_167_PROOF_MATRIX_v1.06_2026-11-04.csv) preserves the original requirements, proof criteria and earlier evidence columns, then adds current status, new evidence and next work. No obligation is silently deleted.

| IDs | Remaining proof |
|---|---|
| TD01, TD09 | Finish the name/icon/index crosswalk, including Food; attach a named-good purchase to that proof |
| TD02 | Buy a named item, carry the acquired amount to a later natural Rate, sell it, and measure gold plus orders/ability costs |
| TD03, TD04 | Complete merchant-specific bid/ask acceptance and full limits/rounding boundaries; derive natural Rate generation |
| TD05 | Controlled full-year trajectories in both scenarios |
| TD06–TD08, TD10 | Merchant inventories, stock limits, season/state-dependent offers, confirmed Uighur row and availability |
| TD11–TD14 | Hire dilution/failure, six weapons and Arms, credit routine, caps/overflow, ability costs, cancellation/failure controls |
| TD15, TD16 | Production, transfers, carryover/conservation across countries and full-year staffing ROI |
| Other economic IDs in the register | Tax, gifts, labor, harvest/payroll, disasters and their interactions; no closure inferred from price arithmetic |

**Next owner:** Zhuge Liang/Codex original-ROM research. **Next executable action:** map the current menu icons to named goods and their stock words, then run a cold-start named-item buy→natural season advance→resale with the existing driver and full captures. Follow with matched workforce-history harvest/payroll trials. **Acceptance:** positive quantities and named item identity; recorded price, gold, stock, Rate, season, orders and relevant ability values; source hash; matched controls; no unobserved formula substitutions. No Director decision is needed for these already authorized research steps.

This document is a verified research continuation and executable handoff. The economic mapping remains PARTIAL until the register's economic acceptance criteria are met.
