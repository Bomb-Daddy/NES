# FORK FOUR — Enhanced Court Advisor AI batch-test branch

**Date:** 2026-10-31 · **Status:** isolated experimental fork; NOT canonical Enhanced/Classic/DOS production · **Code:** actual v1.19 full game source with carefully rebased compatible v1.20 UI/audio changes and a new autonomous advisor scoring hook.

## What this fork does

- Automatically selects each active AI faction's highest-Intelligence eligible non-ruler officer (stable tie: officer ID). If no non-ruler is eligible, can fall back to its ruler. Selection is recomputed each decision; **long-term tenure/political appointment is an open design feature**, not yet implemented.
- Uses actual, unaltered economic and military quantities from that faction's owned provinces. Historical trends come only from snapshots recorded while the faction owned the province. Enemy province holdings and hidden inventories are not passed to the advisor interpreter.
- Interprets genuine food stocks, changes in reserves, army demands, support and labor consequences. Below INT 50 the advisor trends toward a systematically mistaken interpretation; above 50 toward sensible interpretation. INT 50 approximately neutral. Deterministic small uncertainty represents the difficulty of interpreting data; **facts are never falsified**.
- Blends advisor and ruler evaluations at an experimental **20% / 80%** by default, with normalized 0–100 scores and identical legal-action execution. AI profiles and existing action candidate generation remain authoritative; advisor only re-ranks existing candidates. It cannot invent a command or pay resources for the player.
- **Human faction: 0% advisor decision influence**. The existing on-demand Domestic Advisor menu remains factual and read-only. Court Advisor appointment/UI/mentoring/diplomacy bonuses are NOT integrated.
- Separately stores ALL monthly full-game world snapshots and normalized province/officer/faction/event/decision tables in a per-campaign **SQLite `.f4save`** batch artifact, including turn zero, with full game-save bytes for cold reloading. There are **41 province rows, 352 officer rows and 15 faction rows per month** in the historical Scenario 1; every nested metric in the current game state is also retained in a compressed whole-world snapshot.

## Provenance and source preservation

- Parent: Google Drive `/Retro Gaming/Original_KOEI_Windows_Game` source v1.19, SHA-256 `59751851ecbc6acb37f0cfc216ab2d7b96bcecbd02fffec0ccb000a436373202` (148,463,200 bytes, 387 ZIP entries). Source acquired as two verified fragments with manifest.
- v1.20 Build012 patch source: exact match `third_branch/audio_system.py` baseline; candidate audio applied. UI patch's `third_branch/strategic_game.py` baseline hash DID NOT match v1.19; related read-only journal/turn text changes manually rebased onto the v1.19 file, and older strategic logic retained. Original patch is not treated as universally applicable or fully source-identical.
- NO files inside the parent Google Drive original-game folder were replaced. Fork is a separate folder and ZIP. Classic NES and DOS projects untouched.

## To run regression suite

Install `requirements-test.txt` and required Linux GUI support. From the game source root:

```bash
PYTHONPATH=. python -m pytest -q
```

## Batch tests (full historical 41-province game)

```bash
PYTHONPATH=. python -m fork4.batch_runner --out fork4_runs --months 120 --seeds 17,23,41 --weights 0,1000,2000,3000
```

Every `(seed, weight)` pair produces a complete independent `.f4save` database. The script resumes a partially completed run only if the stored seed/weight and game month match. It also writes `summary.csv` and `summary.json`. Run shorter smoke tests first:

```bash
PYTHONPATH=. python -m fork4.batch_runner --out fork4_smoke --months 12 --seeds 17 --weights 0,2000
```

Weights are in **basis points**: `0` means no influence, `2000` means 20%. For a real game campaign, the Fork Four default is 20% for AI-controlled factions, but can be switched off with `campaign.state['fork4_advisor_config']={'enabled':False,'weight_bps':0}`. The human ruler never receives automatic AI advisor influence.

## Decision logging / known QA limits

- Logs the chosen action, selected advisor ID/INT, candidate assessments, weighting and whether the top-ranked decision changed. The existing game command journal records actual committed actions and retains its standard legality/quote/BP checks.
- An advisor changing the TOP-RANKED candidate is not guaranteed to change the executed action: invalid commands can fail preview and the game can try a lower-ranked candidate. The audit `changed` counter measures top-rank changes, NOT guaranteed effective action differences.
- Runtime source currently uses a six/twelve-month `advisor_snapshots` cache for limited longitudinal risk inference; the batch archive retains all earlier complete states. The production game's `.ogsave` format has NOT been migrated to the new tables; use `.f4save` for batch persistence only.
- This is an initial *heuristic*, not a calibrated historical or probabilistic AI competence model. It does not guarantee every INT <50 adviser decreases every ruler's performance. It can double-count some ruler heuristic factors; require multi-seed comparative evaluation and revision before balance acceptance.
- Existing game AI limitations (candidate enumeration, low military activity, no complete officer appointment UI) remain. This fork does not claim to fix them or certify physical Windows/Xbox hardware.

## Evidence

- v1.19 source base: 386 passed / 2 xfailed / 3 subtests in earlier QA.
- Fork Four after decision hook (before v1.20 GUI rebase): 386 passed / 2 xfailed / 3 subtests; 6 newly added Fork Four tests passed independently.
- Two-month synthetic-real-game seed-17 run: 30 faction decisions per weight; advisor changed highest-ranked action in 9/30 at 20%.
- Twelve-month seed-17 pair: 180 faction decisions per weight, 44/180 highest-rank changes at 20%; both runs governed 29 provinces by month 12. State hashes diverged. Exact output archived separately.
- **Pending:** full test suite completion after latest v1.20 GUI rebase, 120-month/100-run batch, performance/long-horizon balance, fault injection, Windows GUI/Xbox verification. Do not call an unfinished or timed-out test a pass.

## Key modules

- `third_branch/fork4_advisor.py`: selection, visible data, interpretation and normalized 80/20 blending.
- `third_branch/historical_ai_v3.py`: single experimental AI candidate-ranking hook and decision metadata, ordinary Campaign execution intact.
- `fork4/batch_archive.py`: per-save SQLite full-state and normalized monthly history.
- `fork4/batch_runner.py`: matched seed/weight automated campaigns.
- `tests/test_fork4_advisor.py`: determinism, human 0%, data truth, bad interpretation, history persistence.

**Director control:** This experimental fork is for batch testing the model only; production game incorporation must be authorized separately after QA.
