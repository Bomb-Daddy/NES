# Enhanced Build38 — executable three-way source reconciliation

This candidate merges immutable Enhanced37 (human defender combat) and B36R1
(seven-job recruitment, defensive Move Army, player-readable text), both
verified as complete independent source archives. It does not promote either
branch wholesale and does not claim Director acceptance.

File-level resolutions:
- PRESERVED from Enhanced37: `third_branch/officer_battle_adapter.py`,
  `third_branch/tactical_view.py`, human-defender dispatch in `integrated.py`,
  Q09/Q10 army-ledger routing, Build37 controller tests.
- RECOVERED from B36R1: `third_branch/defensive_move.py`,
  `third_branch/player_text.py`, `third_branch/historical_ai_v3.py`,
  `third_branch/recruitment_labor.py`, `third_branch/orders_ui.py`, and
  source-backed tests for neutral garrison, seven-job recruiting, and UI command partition.
- MERGED in `third_branch/integrated.py`: B36R1's source commander-presence
  guard within the neutral Move Army path while retaining Enhanced37's human
  defender battle ownership logic.
- NEW: Q03 original defender second-list rice consumption in battle adapter;
  same correction independently applied to Classic37.
- PRESERVED historical B36R1 unique files, source snapshots, and rollback tests
  under `historical_reference_only/recovery_20261102`.
- TEST RECONCILIATION: four obsolete test expectations updated from B36R1
  for duel opening, prisoner choice, and tactical-only command dispatch.
  Original test bytes archived under `historical_reference_only/superseded_tests`.
  Build29's old CPU-attacker authorization expectation corrected and archived.

Source parents: Enhanced37 full SHA-256
`d80041c0234a6935a6d2be65803cef8c0b1e866402f5659eef1fc96b5a7fb470`;
B36R1 full SHA-256
`805d3fdfc8c67d6d6de6a6453e7f4efd57005c8a8c159df22a402bc404ce6491`.

QA status: TESTED_PORT only. Complete original combat parity, Windows native
binary and physical controller on this exact merged candidate NOT RUN.
No previous archive was overwritten or deleted.
