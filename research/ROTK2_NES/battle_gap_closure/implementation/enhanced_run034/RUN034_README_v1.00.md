# RUN034 source-backed Q10 manual bribe after former switching — 2026-11-07

## Binding and ancestry
Director: same original English NES battle observable behavior for Classic and separate Enhanced; no source identity requirement. Original-game Enhanced root `1i5kLog2o1tC9c2dsjdF2CLFnXYQHVk_l`; canonical NES Classic `ROTK2_NES_Windows` SUP-0016 and DOS untouched. Latest immutable Run033 full 18-part reconstructed Drive source SHA256 `76b9247f9d1f0e21d143352134e8eaf93fe21f3daf529b757460017f1684a95e`, 551 CRC-clean members. New Run034 SHA256 `79c5bb7d98ef6376a34b7ea56d6d670c9c5d452439a8602d2c7b8c555d3d0d8c`, 552 CRC-clean members, 549 of 551 original entries unchanged. Two deterministic ZIP builds match.

## Original behavior and fixed observable defect
Original source Q10 hsovl1 `$D5DD`: player selects a NON-COMMANDER opposing officer and bribe gold; a unit that formerly switched is NOT exempt from manual bribery, and returning a subject to the original ruler automatically succeeds. `+2 switched` excludes repeat daily *spontaneous* turncoat trials (`hex:$7AC0`), not deliberate enemy bribes. Run033 wrongly rejected `u.get('switched')` in real `battle_bribe` and `Tactics > Switch sides` controller, preventing player-visible former-officer return.
Run034 removes only those restrictions in `third_branch/officer_battle_adapter.py` and `third_branch/tactical_view.py`; retains enemy/noncommander, original liege, pool, loyal officer handling, gold spent on refusal, switched spontaneous exclusion, no troop duplication, and approved Enhanced economy/marketplace/BP.

## Executed Linux QA
Unmodified parent + new regression six cases: **4 FAIL / 2 PASS expected RED**. Corrected focused and inherited 38 PASS. Selected broad game/Q10/army/renderer: **183 PASS** on working tree and **183 PASS** on cold-extracted complete ZIP (Python 3.13, Tk/Xvfb), 31 cold focused PASS. Mutant 1 re-add bribe ban: 3 expected FAIL/3 control PASS. Mutant 2 re-add switch transfer ban: 2 expected FAIL/4 control PASS. Mutant 3 re-add GUI ban: 2 expected FAIL/4 control PASS. Real Tk one-screen software controller visible gold/target and actual `battle_bribe` saves .ro2; separate new Python process cold-load matches saved campaign fingerprint; troop totals and source gold behavior tested. Screenshot included.

## Important limitations
This is original source-backed behavior, NOT 30 original NES emulator event replays or exact text/pixel equivalence. Unrestricted full suite NOT GREEN/NOT RUN (prior private original ROM fixture and legacy import blockers). No native Windows execution, physical Xbox input, authentic concurrent Q09 automatic second battle, four-list turn scheduling or Director acceptance. Exact NES text still unverified. Only Linux software controller was exercised.

## Next execution
Do not overwrite this source. Recover original Q09 already-attacked recipient automatic second-battle scheduling, officer list/phase order from authentic source or emulator event fixtures; implement against this SHA or later verified direct descendant, RED/GREEN/mutants/conservation/Tk/real `.ro2` two-process. Keep Classic NES in SUP-0016; never mix DOS or Original_KOEI GitHub Build40R4 source.
