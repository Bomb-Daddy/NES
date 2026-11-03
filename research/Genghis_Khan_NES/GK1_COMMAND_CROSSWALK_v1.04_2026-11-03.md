# Genghis Khan I NES — Command-by-command crosswalk, GitHub edition v1.04

**Date:** 2026-11-03. **Status:** 14 main commands inventoried, full game execution **INCOMPLETE**. This edition retains the complete original crosswalk v1.00 below and adds the post-v1.00 technical findings. It does not reclassify untested operations as complete.

## New evidence since source crosswalk v1.00

- The latest v1.02 ROM investigation also completed `Give → Food` at 1/4/5/8/9 units; `Tax → Extra Tax` at 0/1/1 repeated/2/9 units; and `Train → Troops` at the captured baseline. Reports and exact post-command RAM values are linked in the master.
- The v1.03 static work recovered an embedded 256-entry interpreter dispatch and a separate 48-entry `B7` native-helper dispatch. It decoded select memory, arithmetic, stack and call operations; these are **not** 256 gameplay commands or 48 gameplay commands.
- A bank-2 candidate stream at CPU `$91D0` reads market Rate `$7A8A`, shifts right by 1, independently reads `$7A88` and multiplies by 5, pushes 21 and calls `$DB93`. A separate family of three scripts reads Rate and adds 100. **Neither family is confirmed as the Trade buy/sell price path.**
- The original command list and all unresolved tests below remain binding research work. This document is research-only, not an implementation order for the separate KOEI-inspired Windows game or the ROTK II projects.
- The complete 167-row status/proof/next-step matrix is `GK1_COMPLETE_167_OPEN_PROOF_MATRIX_v1.04_2026-11-03.csv` alongside this document.

## Original cumulative command crosswalk (source edition v1.00 preserved verbatim)

# Genghis Khan (US NES) — Command-by-Command Source and Runtime Crosswalk

**Version 1.00 | 2026-11-03 | Owner: Zhuge Liang / ChatGPT | Status: IN PROGRESS**

## Purpose and authority

The Director instructed us to USE the existing Koei reference corpus before additional ROM experimentation and to understand the original Genghis Khan's commands individually. This is a research-only crosswalk, **not** the original Windows game's approved mechanic spec and not a completed reconstruction. Inputs: original NES manual as summarized in the project Master and independently located on FrameAdvance, DarkstarRipclaw's NES guide, the October 26 economic research supplement, original-ROM evidence v1.00/v1.01, and new replayed Mesen captures in this package. The original game is distinct from Genghis Khan II, ROTK2 NES and the separate custom KOEI-inspired Windows game.

## The research correction

- The prior apparent puzzle of emergency-tax workforce collapse is anticipated in the guide: soldiers may desert after an emergency levy, while the manual warns of morale and economic damage. The **actual mechanism and allocation distribution** still need ROM tracing; the guide does not establish a numeric formula.
- Tax → Change Rate is the **national tax percentage**; the lower-left HUD Rate is the **market index**. They have different SRAM addresses and effects.
- Train → People changes the **eligible fraction of population**; People → Draft actually changes troops. Assign → Labor partitions the **existing drafted pool** and does not create population. The manual states a 50% eligibility cap; this is not yet runtime boundary-verified.
- Some references disagree or appear imprecise: original tax lower bound **1** in the manual versus **0** in a master-document paraphrase; Assign cost **Leadership+Planning 5** in the manual versus FAQ shorthand **L5,C5**. Preserve the source mismatch and test it; do not silently choose a version.
- A guide's rule-of-thumb about four-season averaged Food/Artisan production is not a proven code formula. Test Fall against controlled assignment history.

## Command source map (14 commands)

### 01. Tax

**Command/subcommands:** Change Rate; Extra Tax.

**From references (MANUAL/GUIDE, not port-verified):** Tax percentage changes future spring income and current Morale; Extra Tax can be used once per year and damages morale/economy. Original manual gives 1–100 for ordinary tax (older master paraphrase says 0–100; keep discrepancy open).

**From live Mesen (OBSERVED_RUNTIME; scope as stated):** Change Rate 35→9 (+104 Morale), 35→8 (+108), cancellation unchanged; Extra Tax 1/2/9 gold tested with workforce losses and morale damage.

**Remaining tests:** Exact thresholds and tax-income equation; annual one-use gate; different province and high/low morale.

### 02. Assign

**Command/subcommands:** Labor; Army.

**From references (MANUAL/GUIDE, not port-verified):** Labor partitions drafted pool across Troops/Town/Masons/Food/Artisans; Army divides active troops into 1–10 field units and selects cavalry/infantry/archers. Manual says Leadership+Planning 5; one FAQ abbreviates L5,C5; flag for verification.

**From live Mesen (OBSERVED_RUNTIME; scope as stated):** Labor Town 5→9, Troops 61→57, conserved workforce 103; order 3→2. Preview alone does not commit.

**Remaining tests:** Army outcomes, bounds, 0 workers, change vs no-op costs, unit rounding.

### 03. Give

**Command/subcommands:** Gold; Food; Item.

**From references (MANUAL/GUIDE, not port-verified):** Gifts improve Morale, with manual indicating smaller populations gain more; different stocks and goods may provide different gains.

**From live Mesen (OBSERVED_RUNTIME; scope as stated):** Gold 1/8/9: +1/+3/+3 Morale; Food 1/8/9: +1/+5/+5; each spent one order and exact stock.

**Remaining tests:** Item gifts and value dependence, formula/rounding, Morale cap, different populations.

### 04. Train

**Command/subcommands:** Troops; Prince; Self; People.

**From references (MANUAL/GUIDE, not port-verified):** Troops raises Skill; Prince raises a prince ability; Self raises a lord ability; People raises eligible mobilization fraction, manual cap 50%. Each command has its own ability requirements.

**From live Mesen (OBSERVED_RUNTIME; scope as stated):** Troops Skill 124→143, Leadership 89→84, order 3→2. People mobilization 32→35, Charm 61→56, order 3→2. Self Leadership 89→112, order 3→2.

**Remaining tests:** Repeatability of gains, training amount dependence, maxima, Prince, depleted-ability behavior.

### 05. Trade

**Command/subcommands:** Sell; Buy; Hire, each through China / Islam / Uighur availability.

**From references (MANUAL/GUIDE, not port-verified):** Market Rate is distinct from tax rate; buy higher/sell higher when Rate higher; merchants differ in inventory/availability. Buying weapons improves Arms; Hire adds troops but depresses military averages and Morale.

**From live Mesen (OBSERVED_RUNTIME; scope as stated):** Completed Islam Fur sales at Rate 123: 1→0 gold, 7→4, 9→6; several Rate memory perturbations changed displayed prices; merchant-specific availability and inventories.

**Remaining tests:** Buy completion, hiring completion, spread, cross-season arbitrage, exact vendor price and rounding formulas.

### 06. People

**Command/subcommands:** Switch; Select; Demote; Promote; Marry; Draft.

**From references (MANUAL/GUIDE, not port-verified):** Switch exchanges/assigns princes; Select recruits new princely leader; Demote removes; Promote heir; Marry dynasty marriage; Draft mobilizes eligible residents (distinct from Train→People).

**From live Mesen (OBSERVED_RUNTIME; scope as stated):** Menu opened; Switch reported only one base country. Draft numeric prompt reached but failed to complete a nonzero transaction.

**Remaining tests:** Draft amount, losses and population conservation; all succession/promotion outcomes; multi-country cases.

### 07. View

**Command/subcommands:** State; Ruler; Allies; Prince; Child; Stock.

**From references (MANUAL/GUIDE, not port-verified):** Displays intelligence and assets. Guide claims viewing own holdings free, enemy rulers/states requires active spy and spends an order.

**From live Mesen (OBSERVED_RUNTIME; scope as stated):** Menu labels captured, but no cross-target or order-cost verification.

**Remaining tests:** Own-vs-foreign free/cost boundary, spy prerequisite, result display, data validation.

### 08. Move

**Command/subcommands:** Base; People; Gold; Food; Item.

**From references (MANUAL/GUIDE, not port-verified):** Transfers resources or people to neighboring controlled states; shifting base changes direct control and may consume remaining orders.

**From live Mesen (OBSERVED_RUNTIME; scope as stated):** Starting one-country state returned "There is only a base country" and spent no order.

**Remaining tests:** Two-country resource-conservation and adjacency test; transfer limits; quality/morale effects.

### 09. Policy

**Command/subcommands:** Vassal-prince domestic and foreign rules.

**From references (MANUAL/GUIDE, not port-verified):** Instructs autonomous princes on tax, labor, army, welfare, recruitment, diplomacy and military priorities; no vassal means command unavailable.

**From live Mesen (OBSERVED_RUNTIME; scope as stated):** Starting one-country state returned "No vassal states", with no order spent.

**Remaining tests:** Conquer/assign a vassal, change policy, watch AI execute on next seasons.

### 10. Treaty

**Command/subcommands:** Become Vassal; Pay Us; Ally.

**From references (MANUAL/GUIDE, not port-verified):** Diplomacy with neighbors. Manual says alliances run five years; attempts depend on military/leader strength. Guide lists tribute and vassal options, with odds provisional.

**From live Mesen (OBSERVED_RUNTIME; scope as stated):** Target-country prompt reached, no completed treaty.

**Remaining tests:** Cost/adjacency/AI response, tribute transfers, alliance expiry, rollback.

### 11. Spy

**Command/subcommands:** View Report; Wreck Havoc; Kill; Search State.

**From references (MANUAL/GUIDE, not port-verified):** Scouting unlocks enemy View; sabotage targets Economy and Defense; assassination attacks ability values; counterespionage searches home. Manual documents gold costs for main options.

**From live Mesen (OBSERVED_RUNTIME; scope as stated):** Submenu displayed only, no executed spy attempt.

**Remaining tests:** 10/50/100-gold cost gates, success chance, known negative controls, view permission.

### 12. War

**Command/subcommands:** Self; Prince; tactical battle commands.

**From references (MANUAL/GUIDE, not port-verified):** War pays troops and food costs; manual battle is unit-based and involves terrain, phases and possibly 30/60/90-day limit; guide has separate self/prince execution paths.

**From live Mesen (OBSERVED_RUNTIME; scope as stated):** Previous sweep timed out; no proven attack or battle.

**Remaining tests:** Actual start in each scenario; Self tactical and Prince autoresolve; victory/failure and persistent losses.

### 13. Pass

**Command/subcommands:** Finish orders.

**From references (MANUAL/GUIDE, not port-verified):** Ends all remaining orders; advances through opponents to next season when appropriate.

**From live Mesen (OBSERVED_RUNTIME; scope as stated):** 3→0 orders; Spring 1175→Summer 1175; restored 3 orders next season; Rate 101→99.

**Remaining tests:** Fall production and next Spring fiscal/food cycle, deterministic phase order.

### 14. Other

**Command/subcommands:** Save; Wait; Music; Sound; Display; Animation; Battle; End.

**From references (MANUAL/GUIDE, not port-verified):** Game services and settings; Save must support cold reload, Wait text speed, display/sound/animation options, battle-view options.

**From live Mesen (OBSERVED_RUNTIME; scope as stated):** Submenu captured, no save/reload validation.

**Remaining tests:** Save/quit/cold-load integrity, UI toggles, reload of edited game state.


## Battle commands (reference inventory; NOT ROM-verified)

Manual-war entry is not the same as all tactical battle commands. The available NES guide describes tactical **Move (Entire, Divide)**, **Attack (Normal, Duel, Arrows, Ambush)**, **Pass**, **Other (Demand, SOS, Raid, Hunt)**, and **Flee**. None were executed in the current investigation. Tactical stamina, movement points, archer inventory, siege, terrain, retreats, surrender and victory persistence require actual battle captures and memory evidence. Record this as GUIDE ONLY until an original NES battle is executed.

## Live original-ROM evidence and tests

ROM `Genghis Khan (USA).nes`, SHA256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`, mapper 1; Linux cloud Mesen native-AOT + original `mesen-for-ai` via `xvfb-run`, `session.load_rom`, `run.step_frames`, virtual `input.set`, `video.export_frame`, and `cpu.read_memory`. Cold start: Mongol campaign Spring 1175, country 1, Gold 439, Food 208, Market Rate 101, Morale 118, 3 orders. Distinct previous v1.00 warm-start Rate 123/G428 comparisons are not pooled.

**New v1.02 captures re-evaluated from original-ROM artifacts:**

- `Train→Troops` Skill in `$6DE3`: 124→143 (+19); lord Leadership candidate `$65CD`: 89→84 (-5); orders `$7B36`: 3→2. Captured result text confirms Skill 143.
- `Train→People` draft eligibility in `$6E15`: 32→35 (+3 percentage points); lord Charm candidate `$65D3`: 61→56 (-5); orders 3→2. Captured result text confirms 35% mobilization eligibility.
- `Train→Self`, selected Leadership: candidate lord Leadership `$65CD`: 89→112 (+23); orders 3→2; Gold 439 and other province resource fields stable.
- `People→Draft` numeric input reached, but the nonzero-draft probe hung. Its last capture is a prompt/preview; no committed draft can be claimed. Preserve partial screenshots and log.
- `Tax→Extra Tax` in earlier v1.02 runs: 1 and 2 gold exact outcomes and 9 gold variation show tax receipts and disruptive morale/workforce changes. These are consistent with the manual/guide warnings about discontent and desertion, **not** enough to prove causation or formulas for every lost worker.
- `Give→Food` in earlier v1.02 runs: Food 208→207/200/199 for gifts of 1/8/9, Morale 118→119/123/123, orders 3→2. Do not infer a per-unit continuous formula.

**Evidence level:** `OBSERVED_RUNTIME` for this ROM and these captured states; `VERIFIED_STATIC` for hash and byte comparisons; `GUIDE`/`MANUAL` for inherited mechanics; `INFERRED` for any untested functional dependence. No `TESTED_PORT` or full scenario acceptance. Previous v1.01 archive/report remain immutable. This v1.00 crosswalk is a cumulative documentation supplement, not a renaming of the underlying v1.01 research run.

## Required exact experiment protocol, per command

1. Read this crosswalk and the linked manual/guide section **before** issuing the next command; state an expected change and distinguish true unknowns from already-described behavior.
2. Boot from a ROM-SHA-pinned isolated session, record scenario/ruler/province/season/difficulty, and capture the exact menu state, screenshots, frame counts, full RAM snapshot and relevant CPU state.
3. Use one command with exact virtual controller input. Capture preview, confirmation and final acknowledged result separately. Do not call preview 'committed.'
4. Compare before/after fields against a no-op or cancel control from **the same** starting state. Retain unchanged control evidence and failures.
5. Test boundaries and different amounts/attributes/provinces; in multi-season tests retain history of assignment, gifts, market Rate, tax, morale and RNG conditions.
6. Watch RAM writes, record writer PC, mapper bank and PRG file offset; disassemble formulas, verify behavior by repeating with changed inputs and a second independent scenario.
7. Use evidence labels. Only promote specific inputs/formulas to independently verified after ROM run plus negative control plus bank-aware code/independent-state confirmation.

## Research prioritized from reference descriptions

**A. Recruit/worker loop:** Train→People (eligibility) → People→Draft (headcount) → Assign→Labor (allocation) → Pass to Fall (production) → Pass to Spring (tax/wages/food). Test 50% cap, total-population conservation, troop/labor earnings and rounding.

**B. Trader loop:** view all merchants → Trade→Buy commodity at low Rate → advance seasons → Trade→Sell same stock at higher Rate and reverse. Record unit price, spreads, purchase cost, quantity caps, merchant availability, command/ability cost, and integer rounding. Do not use the custom game's 500-unit marginal pricing or charisma rules as NES fact.

**C. Succession/control loop:** after second country acquired, test Move, People→Switch, Policy, diplomatic Treaty and spying. This avoids repeatedly interpreting one-country precondition errors as failures of the mechanics.

**D. Combat loop:** War→Self versus War→Prince; tactical command inventory, unit types, food/gold losses, retreat and conquest outcome; save and cold-load.

**Completion status:** 14/14 root names inventoried in guides. NOT all 14 commands executed to final effect. Neither full scenario has been completed. Exact economic formulas and one-to-one game recreation remain INCOMPLETE.

## Sources and provenance

- **Primary NES manual, linked/provenance:** https://frameadvance.net/sources/koei-genghis-khan-nes-manual/
- **DarkstarRipclaw, Genghis Khan NES Guide and Walkthrough (orders and battle):** https://gamefaqs.gamespot.com/nes/587298-genghis-khan/faqs/44071
- **KOEI/Documentation Genghis Khan NES Master (October 26 source synthesis):** https://docs.google.com/document/d/1MCWq6KnvkV-u0GlDWnA8Ct47BiFmfQtQmEECVKNljLA/edit
- **Economic/Labor research supplement (same evidence status):** https://drive.google.com/file/d/1lqN8xR9VbYMms1UBFTwf74ct3Hg6nbum/view
- **Previous live Mesen report v1.01 (immutable original evidence):** https://drive.google.com/file/d/1KtDHN3C3MzLCoWy15BtRSTf9BTsJEbtL/view
- **Prior raw captures v1.01:** https://drive.google.com/file/d/15Jaeo6bKCAaY6IuxdmGZ1HTRNJLywgyI/view

**Director-facing answer:** Read references first, then execute only to close the measurable gap. Never replace source-supported facts with speculation, and never claim 'full understanding' while new scenarios, commands, prices, or formulas remain untested.

