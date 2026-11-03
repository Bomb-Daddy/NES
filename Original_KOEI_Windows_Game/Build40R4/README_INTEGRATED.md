## Full source v1.4 current note
Run `python -m third_branch` for the real 41-province graphical game. Includes 164 in-game corrected-alignment seasonal battlefield images and native Xbox PC pad input mapped NES one-to-one. See `../CURRENT_SOURCE_README.md` for acceptance limits.

# Original KOEI-inspired strategy game — integrated test prototype v1.0

**Date:** 2026-10-29. **Project:** Third branch ORIGINAL WINDOWS GAME. NOT NES or DOS builds. **Status:** TESTED_PORT (headless Python/Linux) / Windows and Xbox NOT RUN.

This is an actual installable *Python command-line test game*, NOT a Windows executable or finished strategy game. It adapts the dated original-game core with a separate, unified in-transaction campaign layer. Game events change persistent ownership, troop totals, treasury, officer BP, loyalty, market stocks, treaties, workforce, arms and metal, and dated saves. It is a seven-province **synthetic test scenario** on an explicitly provisional 13x12 tactical terrain grid, **not** ROM-grounded 41-province map parity. The NES battle code and 164 enhanced seasonal image previews are NOT presented as native third-game integration. No original-game source was written into the canonical NES/DOS repositories.

## Quick start (Python 3.11+; no external packages)
From `og_sim` after extracting ZIP:

```bash
PYTHONPATH=. python -m third_branch.console --demo --seed 7 --months 3
PYTHONPATH=. python -m third_branch.console --interactive
```

Windows Command Prompt (with Python installed):

```bat
cd og_sim
python -m third_branch.console --demo --seed 7 --months 3
python -m third_branch.console --interactive
```

Interactive commands: `help`, `map`, `status`, `advice`, `officer`, `act A1 cultivate {"province_id":"p1"}`, `attack A3 p4 180`, `battle`, `battle-auto`, `month`, `ai A opportunistic`, `save campaign.json`, `load campaign.json`, `quit`. On Windows, use quotes / JSON as shown inside the running REPL; `act` takes raw JSON as a single trailing argument.

The `act` syntax can issue any implemented command: domestic `cultivate`, `allocate_workers`, `distribute_food`, `distribute_goods`, `recruit`; integrated `market`, `forge`, `train`, `transport`, `appoint_governor`, `plan_attack`, `attack`; six diplomacy commands `Ally`, `Joint`, `Marry`, `Give`, `Cancel`, `Threat`; and active battle actions `battle_move`, `battle_attack`, `battle_end_turn`, `battle_auto`. Commands reject illegal state instead of treating fake outcomes as success.

Example diplomatic action: `act A1 Give {"target":"B","gold_cents":500}`. A military command must originate from the officer's current province; `attack A3 p4 180` is legal at the opening fixture. An allied Joint commitment requires prior `plan_attack` for the target, an existing alliance, actual troops, and later independent deployment/consent checks.

## Reproducible QA

```bash
PYTHONPATH=. python -m unittest discover -s tests -t . -v
PYTHONPATH=. python -O -m unittest -v tests.test_integrated_branch
PYTHONPATH=. python tools/run_integrated_matrix.py --seeds 7,41 --months 12 --out out/integrated_personas.json
PYTHONPATH=. python -m third_branch.console --demo --seed 7 --months 3
```

`tools/run_integrated_matrix.py` runs 12 named play-style profiles for each seed, replays every same-seed campaign twice including cold save/load at month 6 and compares the full output. The new integration uses real campaign commands; the persona selector's detailed skill-weight score formula has **not yet** been transferred from `tools/persona_engine_v11.py` into the new AI. It presently maps personas to tested strategic styles (`opportunistic`, `cautious`, `balanced`, `diplomatic`, `delegation`, `logistics`), and each chosen order passes the same public campaign preview/commit interface as human commands. This is not equivalent to 12 representative human experiences or full 41-province victory testing.

## Reused working original-game systems

* Original source `og/core` (v2 economy and transaction bus): 13-phase scheduler, cultivation, recruitment, labor allocation, March tax, September production, population growth, disasters, public support, core SHA/atomic saves.
* Corrected v2.1 food/goods Distribute contract uses actor Charisma, 100-unit action limit, capped support, minimum stock consumed near cap. The existing standalone core tests still contain *two expected failures* against **legacy** domestic handlers; integrated game overrides them through `third_branch/approved_distribution.py` and has passing direct acceptance tests.
* 200 BP cap, initial BP=Politics, floor(Politics*30/100) recovery at next month rollover. Action BP prices for other skills are **explicitly provisional**.
* Unified market with five commodity types (food, artisan specialty, metal, arms, furs), seasonal prices, local supply/demand stock, 15% bid-ask spread, 500-unit marginal-tier + Charisma concessions (maximum 10%). Real treasury and inventory move atomically.
* Resources -> metal -> arms via monthly extraction/craftsmen; manual forging; officer training; adjacent friendly land transfers; monthly normal troop food upkeep; governor appointment/automated work; six-month longitudinal advisor summary.
* Cao Cao diplomacy architecture: six commands, directed hostility distinct from ruler trust, alliances, marriages, gift gold conservation, temporary Joint commitments with real war-plan/objective hooks, Threat transfer, guarded cooldowns, no-op-capable autonomous AI. **Cao's exact standalone source remains reference in `cao_diplomacy/` rather than being blindly substituted for original game state. All diplomatic odds/cooldowns are reversible experimental dials.**
* Active 13x12 tactical battle with movement, attacks, AI daily counterfire, capture/loss outcomes, persistent province ownership, retreat/captured-prisoner accounting, coalitions that actually deduct ally troops, save/cold-load, and real resource consequences. Tactical terrain is synthetic, not authentic ROTK2 NES battlefield; this is an early battle **vertical slice**, not full battle parity.

## Exclusions / high-priority limitations

* Not a native 1280x720 8-bit Windows graphical build; controller-only Xbox UI not connected or tested; no physically tested package. The playable **terminal** is a command test environment. A human can play commands and end months; no victory screen/release acceptance.
* Only seven synthetic provinces and seven officers; no complete official 41-province campaign, historical officer roster, controller-driven tactical sprite renderer or approved province-link dataset. The uploaded 164 NES enhanced previews remain separate and have known ROM-offset validation issues.
* Battle auto-resolver/grid projection, training increments, BP action costs, supply/demand price curves, dynasty/daughter setup, AI strategies, capitation, governor decisions, production coefficients, diplomacy success odds and actor/ruler IDs are SYNTHETIC-NOT-APPROVED. Tests prove invariants and integration on these fixtures, NOT historical accuracy, game balance or human enjoyment.
* Single attacker unit against one aggregate defender per battle; no multi-unit formations/special tactics, battle save inside individual battle days demonstrated by save/load of aggregate state, some prisoner fate is only ledgered, not interactively resolvable.
* The `og/core` original command bus and the combined campaign wrapper maintain separate but synchronized revisions. All third-game player commands enter via `Campaign.preview/execute` and stage the whole combined state copy; existing domestic handlers remain under the production-style `Bus`, but this wrapper is still a prototype needing production engine consolidation.
* AI 12 profiles receive provisional style mapping only; not complete skill calibration. No naval systems per Director's latest third-branch NO NAVAL decision.

## Authority and evidence
Source ancestry: `KOEI_BP200_NA2_Independent_Runtime_2026-10-29.zip` plus the published `CAO-DIPLO-0003` prototype. Third-branch code does not modify the NES canonical repository or claim any NES ROM-derived rules have been reproduced. Current source/version and tests identified in the package's hashes/QA artifacts. This package is testable on Python for Windows but **has not been run on Windows**.
