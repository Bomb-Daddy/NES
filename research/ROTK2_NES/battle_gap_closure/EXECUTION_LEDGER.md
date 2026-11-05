# Original battle research execution ledger — v1.00

**Date:** 2026-11-05. **Executor:** Zhuge Liang / ChatGPT with independent research and review workers.  
**Scope:** original NES emulator observations and original SNES static analysis.  
**Publication branch:** `rotk2-battle-gap-research-20261105` in `Bomb-Daddy/NES`.

## Delivery and source authority

The original implementation plan was committed before the experiments as `9d80c6482a11a1f774635f0466918e95f9b8eddf`; its file blob was read back as `8ed498559a55686c8e0f77aefd9339a9ee45b7a5`, exactly matching the authored text. The branch was rechecked before result publication and still pointed to that commit.

The live authority register was inspected and the November 1 shared NES/SNES battle ruling preserved. Before result publication, Drive metadata still identified matrix part1/part2 as November1 modifications, the final trace addendum as November2, and the authority register as last modified November2. The canonical NES project remained Drive folder `1ODnfdVL5R3bZBBUp4cXpocNqtTClrxIL`.

The private research evidence folder is [Research_Battle_Gaps_20261105](https://drive.google.com/drive/folders/1NlIRYS4k8sCpGgbMSapsmJcR7pmxVF64). Its actual parent was read back as the canonical NES folder, and its MIME was the native Drive folder type. Individual evidence files, readback hashes and final GitHub publication are indexed in [evidence/manifest.json](evidence/manifest.json).

## Executed research

| Stage / run family | Exact evidence location within private workspace/archive | Result and boundary |
|---|---|---|
| Runtime recovery and smoke | `runtime/RUNTIME_MANIFEST.json`, `runtime/sessions/smoke_direct_001/` | Hash-pinned original NES and historical Mesen; native headless launch with no X server |
| Cold checkpoint round trip | `runtime/evidence/checkpoint_memory_002/verification.json` and related save/load sessions | Native load true; complete RAM/state comparison and decoder-ready PNG equality |
| Invalid checkpoint control | `runtime/sessions/load_invalid_001/` | Native false return and nonzero exit; rejected as intended |
| Original invasion reconstruction | `runtime/experiments/cold_yuanshu_001/` | Scenario1, one human Yuan Shu, difficulty1; P19→P20;100 soldiers,0 gold,200 rice; ordinary input only |
| Daily fields and food | `wait_001/`, `wait_days_002/`, summary/weather cases | Day and writer traces, exact food changes, HUD arrows/pictures; physical bank1 evaluated |
| Food intervention/control | `food_resident_zero/`, `food_unrelated_zero/` | Same restored state; only declared troop-word edits; causal resident contribution |
| Human-player reconstruction | `twohuman_*/` | Difficulty2, two rulers, exact named forces and actual initial roster; failed menu assumptions and timeout attempts preserved |
| Successful whole withdrawal | `flee_prompt_001/`, `flee_confirm_001/002`, `flee_aftermath_001/` | Sole attacker successfully inserted into P29; persistent campaign continuation despite defeat text |
| Individual Flee branches | `individual_prep_001/`, `individual_flee_*/`, `individual_jiling_*/` | Ordinary-input multi-officer battle: Yuan Yin fails escape and is captured; cancellation preserves the army; Ji Ling successfully returns to friendly P19 with troops converted from 1,000 to 10 |
| Natural approach to combat | `move_south_001/`, `move_wait_001/002`, `move_forest_001/`, `forest_ready_001/` | Actual movement, mobility and intervening original turns; no RAM patches |
| Attack modes | `normal_forest_001/`, `simult_forest_001/`, `charge_forest_001/` | Identical starting state; modes1/2/4 and first-exchange troop words recorded; later AI changes excluded from first-exchange comparisons |
| Fire | `fire_prompt_001/`, `fire_execute_001/002`, `fire_delay_001/002/005`, `fire_day5_001/`, `fire_text_001/` | Failed attempts retain no-fire evidence; successful day5 ignition and no-wind clearing; actual NES text captured |
| Prepared prisoner/ruler UI | `prepared_*/`, including `prepared_ruler_death_commit_001/` | Original ROM executing declared RAM fixture descendants; eleven reviewed cases cover actual menu choices and committed records, including Cai Mao succession; no ordinary-play reachability claim |
| White cursor | `white_marker_noinput_001/`, `white_marker_writer_001/`, `white_marker_initializer_001/` | Original NES initializer, shadow-OAM writer and twenty-frame cycle; first invalid PNG preserved and clean repeat verified |
| Q09/Q14 source closure | `static_sources/q14_q09/` | Original byte identity, source branches, exact fallback and destination writer |
| Q19 source closure | `static_sources/q19/` | Five handlers, transfer semantics and bounded original-byte probes; not a complete original-console run |
| G09 expanded writer/map audit | `static_sources/q14_q09/g09_*` | Typed writer search, original map counts and source-verified pending-allied candidate; ordinary-play reachability remains PARTIAL |
| Independent review | `runtime/evidence/hostile_review_001/` and public `evidence/review/` | Scoped raw-artifact review, corrected claims, meaningful failed-before/passed-after gate checks |

The final per-run index is [evidence/runtime_runs.json](evidence/runtime_runs.json). It distinguishes process execution, retained-artifact integrity and checkpoint ancestry. Prepared-state descendants must retain their inherited classification even when their immediate schedule contains no new patch.

The frozen catalog contains **99 runs: 93 PASS_VERIFIED and six FAILED**. All 1,899 declared products match their retained hashes; 929 of 930 checked PNGs are valid; 129 saved checkpoint payloads were decoded. Four runs failed to complete, while two completed runs retain an integrity failure (the truncated `weather_day3` executable and the first invalid `white_marker_noinput_001` PNG). Product hash agreement does not turn an invalid product into a valid one. The three directly patched runs and eleven prepared descendants are distinguished from the 85 runs with no declared intervention ancestry. Catalog SHA-256: `d5f846032d794f3e570dd39bcc2f28aaee7dc9eca806f9e23195d74dfe894c42`.

## Commands and reproduction

The exact command, working directory, environment changes, complete stdout/stderr, elapsed time, native exit and input hashes are in each run's `process/receipt.json`. Its `probe_receipt.json` records expected outputs and failures. The schedule and generated Lua preserve the exact historical input sequence. These original logs are retained in full; the table above is a summary, not a replacement called “raw evidence.”

Typical native experiment:

```sh
python research/ROTK2_NES/battle_gap_closure/tools/battle_probe.py \
  --rom /path/to/authorized-original.nes \
  --mesen /path/to/pinned/Mesen \
  --schedule /path/to/evidence/runtime/experiments/wait_001/schedule.json \
  --state /path/to/evidence/runtime/experiments/cold_yuanshu_001/state_021800.b64 \
  --output /path/to/a-new-output-directory --timeout 60
```

The wrapper invokes `Mesen --testRunner --enableStdout --doNotSaveSettings --timeout=N SCRIPT ROM`. It removes DISPLAY, sets the tested locale/globalization/audio environment, and leaves HOME unchanged. New processes use independent portable copies and settings. No live GK1/Temujin process or profile was reused.

The source probes and exact Python versions are recorded in their own execution receipts. The Q19 probe executes a bounded set of original 65C816 instruction bytes against synthetic RAM; it is explicitly not a console or Windows run. The source generation used pinned decompiler commit `7c2bab7d634e22a3f5870fe548d6a0556760be98` and harness commit `9a436f493bf20f760c5eac49014163c2570e77bd`.

## Failed attempts and recovery

- The initial package-install route failed on the environment's privilege/group and mirror restrictions. Native Mesen direct mode subsequently worked without Xvfb. The install-attempt logs remain in the private evidence.
- The first save fixture permitted an extra terminal callback; a stopped guard corrected that behavior. The first immediately post-restore PNG was empty. Later probes schedule decoder-ready frames and reject invalid PNGs.
- The initial manifest parser for Q19 failed; its receipt/stdout/stderr were retained. The corrected source manifest matched original bytes. This was not described as a ROM mismatch.
- A reviewer demonstrated that the first PNG gate could accept a header-only image. The failing test cases and subsequent passing run are retained. The corrected gate performs structural and decompression validation.
- The retained `weather_day3` executable copy is an exact truncated prefix of the canonical binary. Its prelaunch receipt had the correct hash; cause and timing of the retained mismatch remain unknown. Its raw products/core were reviewed; a clean repeat reproduced the observation with the complete executable. The new runner checks copied-executable identity both before and after execution.
- Some two-human inputs initially used the wrong controller port; subsequent native input on port1 selected player2 correctly. Additional A pulses entered the following general's Move submenu. Those screens and unchanged roster/movement state were retained; B returned to the command menu before proceeding with single-acknowledgement Waits.
- Several longer two-human and prepared-menu attempts reached useful intermediate screens but timed out before their requested final checkpoint. Their receipts remain failed/incomplete. Recovery uses untouched earlier states and shorter bounded steps. No missing final state was fabricated from an earlier image.
- The first white-cursor no-input sampling requested frame1 immediately after restore and produced an invalid PNG. Its receipt remains INCOMPLETE_RUNTIME; the clean writer replay begins at frame2 and verifies the expected phases.
- Workspace memory pressure was observed during later runs. Completed identical runtime copies were deduplicated by verified hashes with a separate audit, preserving paths and bytes. The anomaly copy and active/new-process copies were excluded. This is a storage recovery, not proof of the precise cause of any timeout.
- Routine authoring attempts also included a failed relative-directory inventory and a malformed inline Python conditional before a follow-up schedule was created. Those were tooling/authoring failures, not original-game executions; the corrected commands subsequently produced the intended files.

## Publication verification

The private source ZIP was uploaded as Drive file `1TnLHU2n44qf7yvLanwX6IvT48xtqrnTD`; its downloaded copy is 4,913,084 bytes with SHA-256 `68ba2d1e9ffc2ea6e33e0dd60e517f6aeb85c28697e43b2845b8dd3c680bcb2c`, and all 667 ZIP member CRCs pass. The 112,649,376-byte runtime ZIP exceeded the connector's 104,857,600-byte transfer limit and was rejected before upload invocation; a folder read confirmed no partial original upload. Recovery preserved the frozen ZIP exactly and transferred two ordered byte fragments, 67,108,864 and 45,540,512 bytes. Both fragments and their companion JSON/Markdown were downloaded again, matched their local hashes, and were reassembled into the original ZIP: SHA-256 `b256ca4d62761ef345e3c0a8affa8e7fd5e7f34a0ab1af59f440af1c261c0010`, all 3,939 member CRCs passing. Actual file IDs, parents, sizes and MIME types were checked. Drive sniffs the first fragment as `application/x-zip`; it remains an incomplete fragment and must be joined to part02 before opening. [Assembly instructions](https://drive.google.com/file/d/19zOPCUzBN9A4REaUbn_KBjrSxV01KYrv/view) and [the evidence manifest](evidence/manifest.json) provide exact file links and hashes. The folder and uploaded files were verified as not publicly shared.

Fresh final validation parsed thirteen Python files, passed sixteen harness tests and twelve runtime-manifest tests, and verified 48 authored source/review copies against their retained originals. The public payload scan checked ninety UTF-8 files; all four generated Python caches were excluded. Eighty relative file links across nineteen Markdown documents resolved. The private archive retains the exact commands, complete stdout/stderr, source hashes and corrections in `runtime/evidence/final_validation_001/`. Its `FINAL_VALIDATION_MANIFEST.json` SHA-256 is `8df6d46dd3d1e14859032f161d17c200c2cc9338579a63cb2bef0a3eeb2bba1e`. These are evidence-tool and publication checks, not a claim that all game behaviors passed.

After the private runtime packet froze, the existing twelve manifest tests were copied into the public tools directory, changing only the test's tool-directory lookup from the temporary workspace path to `Path(__file__).resolve().parent`. The combined published suite then passed **28 tests** from `tools/` with `python -B -m unittest discover -v`. Its complete output, exact command and compared source hashes are in [the publication validation receipt](evidence/publication_validation/receipt.json). This portable test entry and its receipts are public GitHub additions; the frozen private runtime packet remains unchanged.

The original plan publication is verified above. Final source/runtime archive IDs, MIME types, actual parent checks, sizes, digest readbacks and GitHub file verification are recorded in the manifest and the final publication receipt. A branch commit records delivery of research only; it does not prove that another agent read it, implemented it, or completed a Windows acceptance test.
