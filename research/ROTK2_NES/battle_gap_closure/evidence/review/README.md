# Independent evidence review

This directory contains the authored hostile review, its derived metadata, and the complete red/green test receipts and logs for the two evidence-gate repairs. The copied files are byte-identical to the original review artifacts; [PUBLIC_REVIEW_MANIFEST.json](PUBLIC_REVIEW_MANIFEST.json) records their hashes. Original command paths inside receipts and stack traces are preserved for provenance.

Start with [HOSTILE_REVIEW_v1.00.md](HOSTILE_REVIEW_v1.00.md). Its bounded review covers 18 original-emulator run artifact sets, 579 products checked against receipts and stdout payloads, and 325 fully decoded PNGs. Those counts describe inspection of the executor's native evidence. The reviewer did not execute the original emulator. The 16 passing harness tests use synthetic fixtures and are a separate evidence class.

The report predates later two-human reconstruction, daily-boundary, and prepared capture/menu extensions. It does not claim to review those additions or to close their outstanding criteria. Current coverage belongs in the [Completion Register](../../COMPLETION_REGISTER.md) and [findings](../../FINDINGS_v1.00.md).

## Gate repairs and preserved failures

| Gate | Before the repair | After the repair |
|---|---|---|
| PNG completeness | [Receipt](png_gate_red_receipt.json), [stdout](png_gate_red.stdout.txt), [stderr](png_gate_red.stderr.txt): 13 methods, 16 negative-case failures, exit 1 | [Receipt](png_gate_green_receipt.json), [stdout](png_gate_green.stdout.txt), [stderr](png_gate_green.stderr.txt): 13 methods pass, exit 0 |
| Copied executable before/after identity | [Receipt](runner_identity_red_receipt.json), [stdout](runner_identity_red.stdout.txt), [stderr](runner_identity_red.stderr.txt): 16 methods, two identity failures, exit 1 | [Receipt](runner_identity_green_receipt.json), [stdout](runner_identity_green.stdout.txt), [stderr](runner_identity_green.stderr.txt): all 16 methods pass, exit 0 |

An empty stdout file is intentional: `unittest` writes this output to stderr. These are complete test outputs, not excerpts. The authored gates and tests are in [battle_probe.py](../../tools/battle_probe.py), [run_mesen.py](../../tools/run_mesen.py), and [test_battle_probe.py](../../tools/test_battle_probe.py).

## Derived review records

| Record | Scope |
|---|---|
| [reviewed_runtime_artifacts.json](reviewed_runtime_artifacts.json) | Initial 11 native cases plus weather-day-3 recheck; 209 products and 88 PNGs |
| [fresh_combat_review.json](fresh_combat_review.json) | Six Fire/Normal/Simultaneous/Charge cases; 370 products and 237 PNGs, bounded memory values and trace observations |
| [weather_day3_integrity.json](weather_day3_integrity.json) | Preserved truncated executable-copy mismatch, hashes and timestamps; cause and exact time remain unknown |
| [weather_day3_recheck_review.json](weather_day3_recheck_review.json) | Fresh replay with complete retained canonical executable and matching products |
| [png_existing_artifacts_validation.json](png_existing_artifacts_validation.json) | Earlier scan of 317 existing PNGs; this overlaps other review counts and must not be added to them |
| [weather_recheck_review_attempt01.json](weather_recheck_review_attempt01.json) | Reviewer import-path failure before results were generated |
| [fresh_review_offset_correction.json](fresh_review_offset_correction.json) | Correction of a temporary reviewer metadata label; original evidence was unchanged |

Only authored reports, derived values, hashes, test logs, and receipts are included here. Original ROMs, emulator binaries, complete save states, full RAM dumps, game screenshots, and complete decoded commercial listings are not included.
