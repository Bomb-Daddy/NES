# Fork Four — Hostile QA and Reward Governor integration
**Audit date:** 2026-10-31 · **Branch:** isolated `Fork4_Reward_Governor_Experimental_v2` · **Verdict:** **NOT RELEASE-READY**.

## Director decision: ruler-only Reward Governor
Added a standalone **Reward Governor** order in **All Orders / Personnel commands**. The dialog shows a scrollable **Governor / Province / Loyalty** list built from current legal appointments for the player's faction. The roster includes historical names when available and supports D-pad left/right selection. The selected governor receives a gold or horse loyalty reward, even if stationed outside the ruler's province. Only the faction ruler may execute it; it debits the **ruler's current province** treasury or owned horse inventory plus ruler BP, not the governor's treasury. All changes flow through preview, atomic commit, persistent log and save/load; the designated governor and province ownership remain unchanged. Revalidates appointments before committing. No sworn-brother-only province-control privilege is implemented or desired. **Gold/horse loyalty formulas are synthetic provisional rules, not original ROM parity.** Writings are not offered in the ruler-wide governor command (they do not increase loyalty).

Files changed over the officer-relationship experimental v1 baseline:
`third_branch/officer_relations.py`, `third_branch/orders_ui.py`, `third_branch/strategic_game.py`, `third_branch/context_help.py`, `tests/test_ui_integrity_v19.py`, `README_OFFICER_RELATIONS_EXPERIMENTAL.md`. New `tests/test_reward_governor_fork4.py` exercises remote treasury source, role permissions, missing/foreign appointments, horse inventory, rejection atomicity, controller-form parameters, cold save/reload, preserved province permissions and real strategic UI issue-from-a-different-province path.

## Test execution and positive evidence
- Full project suite from the copied standalone build: `python -m pytest -q -ra`: **409 passed, 2 expected failures, 3 subtests passed**. Actual stdout in `FULL_PYTEST.txt`.
- Added `tests/test_reward_governor_fork4.py`: **9 passed**.
- Original experimental v1 baseline before this command: **400 passed, 2 expected failures, 3 passing subtests**.
- Prior transaction fuzz run on officer-relationship v1: 100 seeds x 12 = 1,200 command attempts; 164 valid commits, 1,036 expected rejects, 0 unexpected errors, 0 rejected-state mutation, 0 save roundtrip mismatch. **This historical fuzz result is not a rerun on v2.**
- QA negative probes were rerun on this exact v2; results in `hostile_probes.json`.
- **Limit:** Tk graphical rendering and actual Xbox controller on Windows were not operated in this environment. Treeview displays/scrollbar and D-pad changes are code-inspected, not end-to-end device verified.

## Severity-ranked defects — reproduced, not corrected in this scoped branch

### P0 / release-blocking — combat deadlock when captured commander has distant army stations
**Reproduced in v2.** `new_campaign(77)`; add 10 troops to B2 army in p4, reduce B2's defending p5 station to 1, attack p5 from p3 with A3 (200 troops), then issue `battle_auto` with A3. The battle preview rejects `ENEMY_ARMY_STATION` because the captured officer changes faction while their stationed army remains in a different faction's territory; `advance_month` then rejects `BATTLE_ACTIVE`. This is the same class as month-48 seed 100005. **Fix**: commit original-verified battle outcome and reconcile prisoners, field commanders, distant garrisons and province/army ownership atomically; add replay regression for the exact seed and synthetic repro. Do not mask the exception or exclude a high-advisor-weight outcome from statistics.

### P1 — unauthorized governor appointment
**Reproduced in v2.** `new_campaign(98)` then `do('appoint_governor','A2',{'officer':'A2','focus':'personnel'},'hostile1')` succeeds while A2 is a subordinate, not the ruler. **Fix**: enforce the intended ruler authority at command bus, not only UI; define any authorized governor delegated-appointment scope explicitly.

### P1 — writings increase Intelligence without inventory/cost
**Reproduced in v2.** With high-INT appointed advisor, 30 `reward_officer(method='writings')` calls raise A2 Intelligence 1 → 31 while spending 0 gold and no writings inventory. BP is consumed, so this is **not** unlimited within one month; it is nevertheless an unaccounted item creation risk. **Fix**: track writings availability, price, consume one per grant, verify original ROM limits and formula.

### P1 — governor Personnel policy can reward the governor's own loyalty
**Reproduced on v1.** A2 appointed personnel governor in p2 spends 10 gold and 3 BP monthly and gains his own loyalty 65 → 68, despite self-reward being prohibited for manual commands. **Fix**: rule whether delegated Personnel policy can self-reward; if disallowed, exclude the governor from targets and handle no-eligible-target without resources spent. This code remains unchanged in v2.

### P1 — advisor AI does not use new loyalty/personnel actions
The actual candidate generators do not propose Reward Officer, Reward Governor, Feast, Brotherhood or Appoint Advisor, and advisor scoring does not use officer loyalty directly. **Fix**: add legal candidates and audit costs/benefits with separate high/low Intelligence and 0/20/40% weight trial; avoid confusing static menu availability with AI adoption.

### P1 — combat is not verified ROM 1:1
This game's battle resolver is a provisional native Python implementation. Source material exists in ROTK II NES ROM and SNES decompiler archive, but hex movement, battle RNG and event/commander settlement remain unproven one-to-one. **Fix**: finish separate SNES/ROM research and oracle fixtures per Fork 4 handoff; do not tag as exact parity until replay-equivalent.

### P2 — malformed governor focus accepted in state, crashes monthly progress
`validate` accepts `governors[p2]['focus']='invalid'`; next monthly simulation raises unhandled `KeyError('invalid')`. **Fix**: validate focus field and any appointment metadata before save/advance.

### P2 — sworn-brother difficulty cap not enforced by validator
At `bandit_difficulty=5` command flow caps oath members at 5, but a tampered state with 6 brothers (loyalty 100, same faction) passes `validate`. **Fix**: enforce effective cap in both command and persistent-state validation; leave brother-only province-control *excluded by director decision*.

### P2 — existing expected test failures
`tests/test_persona_engine_v11.py::TestProfileV11::test_distribution_above_100_must_reject` and `test_distribution_food_charisma_must_change_support_effect` remain **XFAIL**. They are not new failures and must not be reported as passes; investigate policy/design against accepted distribution requirements.

### P2 — UX and market parity limitations
Real Xbox/controller navigation and rendered Tk scrollbar have not undergone Windows human-play QA. The command uses real ruler-province gold/horse inventory but the Buy Horse path is still a provisional standalone purchase, not the approved unified marketplace. Ruler-wide transport is implicit instant reward; original movement/delivery cost unverified. Validate actual product behavior and original KOEI distances before claiming ROM parity.

## QA acceptance to release
1. Fix P0. Demonstrate synthetic repro and original seed 100005 advancing to month 60, with zero illegal army stations, correct prisoner ledger, stable save/load.
2. Lock governor appointment authority and ensure no self-reward loophole.
3. Introduce writings stock/cost and enforce all state invariants.
4. Add reward/loyalty actions to advisor decision generation and retest paired seeds from a frozen version.
5. Verify Windows Tk/Xbox with a 10+ governor scrollable roster and independent ruler location; inspect labels, confirmations, and resource debit on save/reload.
6. Keep exact ROM parity as a separate acceptance gate. Cross-version mechanics must be tagged `VERIFIED_NES`, `VERIFIED_SNES`, `PORT_REFERENCE`, or `PROVISIONAL`.

## Developer execution
Run from build root: `python -m pytest -q -ra` then `python -m pytest -q tests/test_reward_governor_fork4.py`. The old 20% histories were produced by an earlier code revision and **must not** be silently treated as version-matched results after combat/personnel engine changes. Full v2 is a standalone fork; the source overlay is an incremental application onto the validated v1 officer-relations baseline only.
