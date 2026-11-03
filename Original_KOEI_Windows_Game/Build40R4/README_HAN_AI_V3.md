# Han-Named AI v3.2 — original KOEI third branch only

**Date:** 2026-10-30. **Baseline:** historical Scenario 1 source v1.10, Google Drive file `1BWV6pw_Jq6-IqkBoqSyoYSFkeyoaGlll`; untouched. **Scope:** 12 simulated human decision styles mapped onto 12 of the 15 historical ruler slots; the remaining three use the existing cautious control style. This package is a *tested source overlay*, not a claim of a new canonical Windows executable, a native game installation, or historical personality simulations.

## Files and integration

1. `third_branch/historical_ai_v3.py`: deterministic v3 chooser, legal command previews/execute, visible history-derived objectives, tactical/stock/seasonal specialist policies, bounded competence and risk.
2. `third_branch/historical_ai.py`: retains the old reference source under `take_turn_legacy`, changes `profile_data` to read v3 definitions, and routes the regular `take_turn` to v3. This preserves existing Python imports (`player_runtime.py`, historical ten-year runners).
3. `config/KOEI_12_Han_AI_Profiles_v3_2.json`: all 12 named agents, explicit playstyle, original weights reallocated away from prohibited naval mechanics (total 100), retention of v1.1 competence fields, no character/ruler identity substitution.
4. `tests/test_historical_ai_v3.py`: v3.2 no-opening-attack civilian regression plus 12-style execution, profile/name/weights, cold-load/replay, nonmutating preview and duplicate-action rejection. Baseline existing tests are also run.

**Caveats:** v1.10 game engine supports one AI action per faction-month. Therefore v1.1 voluntary attention budgets 2–12 cannot yet be truly exercised; the policy does not grant free actions. Improvements in commerce, food, arms, warfare and treasury decisions are constrained by real structural economy defects already identified in October 30 QA. Existing saves retain older embedded `ai_profiles` UI labels until an explicit schema-safe name-display migration is implemented; the v3 decision layer uses v3 profile data regardless. `rank_choices` reads only public province/order/journal information; no future RNG or alternate hidden-world state. Prototype `plan_attack` need not be issued as a prerequisite to `attack` in v1.10. Repeated first-order labels do not prove human competence.

## Historical test-agent names (not historical identity substitution)

P01 Liu Shan — Curiosity; P02 Liu Biao — Caution; P03 Sun Ce — Conquest; P04 Zhao Guo — Agrarian planning; P05 Liu Chong — Benevolent governance; P06 Mi Zhu — Markets; P07 Ma Jun — Industrial production; P08 Zhang Heng — Artisan invention; P09 Lu Su — Logistics; P10 Xun Yu — Court; P11 Chen Qun — Delegation; P12 Yang Xiu — Rules interpretation. These are evocative historical Han-period labels; they do not claim that the real persons behaved like the synthetic policies or were all present in 189 AD.

## Reproduction

From a clean copy of `og_sim` extracted from the above v1.10 source ZIP, overlay only the four files from this archive (preserve paths), then:

```bash
PYTHONPATH=. python -m pytest -q
PYTHONPATH=. python tools/historical_ten_year_resume.py --out /tmp/han_ai_v3_120 --seed 17 --stop 120
```

For independent strict comparison, also run original v1.10 unchanged on the same seed and 120-month historical scenario. To test the **final, refined** v3 competence-scoring edition, rerun the long simulation from month 1: don't reuse saves from the earlier pre-refinement run. Check real per-ruler action mixes (especially attack execution, logistics transports, profit, expert adversarial probes), save SHA-256, full cold reload, and that no synthetic/unapproved navy was exercised. Profile style may be varied without changing historical ruler identities. `Source overlay` is not a fully packaged Windows build.

## Hard acceptance exclusions

No installed Windows game verification, physical Xbox test, true human player skill validation, complete controlled profile rotations, correction of vacant province phantom taxes/metals, market-counterparty solvency, finite metal deposits or realistic food carrying costs. Do not classify any of these as PASS from these AI-only edits.

## Honest prior-run negative control

Initial v3.0 was rejected by producer QA: several nonmilitary profiles initiated overly frequent opening attacks. The baseline `historical_ten_year_resume.py` report incorrectly showed one unique action signature, because its parser expected the old request ID shape. The independent corrected forensic audit of that run found **12 distinct signatures**, but this is *not* sufficient to accept its behavior. v3.2 restricts offensive initiative by personality, compares risk with public military strength, and adds a targeted regression. **Do not mix v3.0 save evidence with final v3.2 source or label it final acceptance.**

## v3.2 correction beyond v3.1

Real 120-month v3.1 QA (all 120 cold saves) showed 60 `plan_attack` commands from P03 with just one real attack; 80 repeated governor assignments from P11; and zero food/arms transports from P09. These defects were not accepted. v3.2 drops unnecessary plan spam, stabilizes governor appointments, and allows genuine legal food rebalancing between linked allied provinces when a frontier province has significantly less than its neighboring source. The new targeted tests explicitly enforce the correction. Production AI remains a candidate until independently integrated and multiplayer/player-facing evidence exists.
