# ROTK II battle evidence — independent hostile review v1.00

**Reviewer:** Cao Cao research/QA subagent, coordinated by Zhuge Liang.  
**Date:** 2026-11-05 UTC.  
**Disposition:** The bounded runtime claims below are supported by inspected original-emulator artifacts. Two concrete evidence-gate defects were reproduced, fixed within the parent's explicit authorization, and verified with preserved red/green tests. Historical and reachability limits remain open as specified below.  
**Execution distinction:** This reviewer did not run the original game or emulator. Runtime observations were independently inspected in the parent's native captures, logs, states, and memory. Reviewer-executed tests used explicit synthetic fixtures. No Windows port or physical-controller acceptance was performed.  
**Delivery:** Local authored review for parent integration; this file does not claim remote publication or agent acknowledgment.

## 1. Review scope and counts

The primary review covered `wait_001`, `wait_days_002`, `food_resident_zero`, `food_unrelated_zero`, `flee_prompt_001`, `flee_confirm_001`, `flee_confirm_002`, `weather_day3`, `weather_day4`, `weather_day5`, and `summary_b`. The fresh `weather_day3_recheck` was then inspected separately. The final limited extension covered `fire_day5_001`, `fire_text_001`, `fire_execute_002`, `normal_forest_001`, `simult_forest_001`, and `charge_forest_001`.

| Scope | Original-emulator run artifact sets inspected | Products checked against receipt and stdout payload | PNGs fully decoded |
|---|---:|---:|---:|
| Initial review plus weather recheck | 12 | 209 | 88 |
| Limited Fire and attack-command extension | 6 | 370 | 237 |
| Combined inspected set | 18 | 579 | 325 |

All 579 listed products matched their receipt size/hash and original stdout payload. All 325 PNGs passed full Pillow decoding and the strengthened bounded PNG check. These are artifact counts, not 579 independent gameplay tests. The initial 12-run audit also checked script/settings/core/log identities, exit and timeout receipts, and the captured bank-1 window pairs. It found one retained executable mismatch, detailed below. The six added runs' copied executables all matched their prelaunch digests; four of those runs also contain the new explicit post-run identity gate.

Receipts: `reviewed_runtime_artifacts.json`, `fresh_combat_review.json`, and `weather_day3_recheck_review.json`. A separate earlier scan validated 317 existing experiment PNGs at its recorded time; that scan overlaps this review and must not be added to these counts.

## 2. Concrete defects and repairs

### 2.1 A header-only PNG could pass as a screenshot — fixed

The original `check_png` accepted a 33-byte PNG signature/IHDR prefix with no image data or end chunk. This was a real false-PASS risk even though the native screenshots inspected here were complete.

The authorized replacement now checks a bounded file size, complete chunk structure, chunk names, every CRC, first/unique IHDR, native 256×240 dimensions, supported 8-bit noninterlaced RGB/RGBA layout, contiguous image chunks, nonempty IDAT, final IEND without trailing bytes, complete bounded zlib decompression, exact scanline lengths, and legal row-filter bytes. Unknown critical chunks are rejected. This intentionally validates the pinned native encoder's screenshot formats; it is not a universal image-format library.

The test suite was run before the implementation change. Thirteen test methods produced **16 negative-case failures** under the old gate, including header-only, truncated chunks, bad CRC, trailing bytes, missing image/end chunks, malformed compressed streams, over/under-sized image data, and invalid filters. After the fix, all 13 methods passed. Positive complete RGB and split-IDAT fixtures remain accepted.

Evidence: `png_gate_red.*`, `png_gate_green.*`, and `png_existing_artifacts_validation.json`. The existing-native-image scan passed 317 PNGs without an emulator run.

### 2.2 Copied executable identity was not enforced before and after execution — fixed

The original runner checked the canonical executable, recorded a prelaunch digest of the copied executable, and gated completion on the extracted core and unchanged ROM. It did not reject a bad copied digest before launch or require the retained copied executable to remain unchanged after the process.

Both authorized copies of `run_mesen.py` now reject a copied executable whose digest differs from the pinned digest before launching. The receipt also records `mesen_after` and `copied_executable_unchanged`, and `runtime_identity_passed` requires that post-run match.

Two meaningful negative tests failed before the runner change: a copy operation that corrupts the copied program still launched it, and a successful subprocess that replaced its own copied file still received a successful identity result. Both fail closed after the repair. An unchanged positive fixture still passes. These tests use a small Python executable and synthetic ROM/core byte strings, with fixture-only expected digests; they are **not emulator tests**.

The final full harness suite ran **16 tests, exit 0, OK**. Evidence: `runner_identity_red.*` records exit 1 and two failures; `runner_identity_green.*` records exit 0. The complete stdout/stderr logs and source hashes are retained.

Verified current authored file identities:

| File | SHA-256 |
|---|---|
| `battle_probe.py` | `b21a626935a6b45c6918925856632d43a97e70cc5dcd15660a05b7ac086d1cbc` |
| Both `run_mesen.py` copies | `886da014cfb13e79e54da4e441f8d31ab862abec2c4ac43b1b15d9ac6b8baf58` |
| `test_battle_probe.py` | `faa0402bcb23272ab4d034f7343c315149054c01acd05393d082595e162a84d6` |

Only the authorized PNG function/import, tests, and runner identity checks were edited by this reviewer. Remote publication and integration remain the parent's responsibility.

### 2.3 Retained executable anomaly in the original weather-day-3 run — preserved limitation

The canonical executable is 69,697,976 bytes, SHA-256 `f02685dafe559addf8dd8d769621a89777137d0c4592436aed65694a7e740676`. The retained `weather_day3/process/portable/Mesen` is 49,545,216 bytes, SHA-256 `132ba0f7ced50d94c8e165cf55b0998ad5526a8421d8c8ba4a3159c56c05a599`. It exactly matches a prefix of the canonical file and is missing 20,152,760 trailing bytes. Its mode is 0644 rather than 0755. The receipt nevertheless records the canonical prelaunch digest, exit 0, and the old successful identity gate.

The extracted core still matches the pinned core digest. Screenshots, memory, receipt hashes, and stdout payloads from that run match each other. This is an unresolved retained-artifact integrity limitation, not evidence that the native outputs were fabricated. The parent reports no intentional cleanup or truncation. The review did not alter the file.

The retained file's mtime is 18:48:49.395121 UTC and ctime 18:48:52.274980; the process receipt was written at 18:48:53.090514. These timestamps do **not** establish that truncation happened after execution. Cause and exact timing remain UNKNOWN. Full stat and hash evidence is in `weather_day3_integrity.json`.

The fresh `weather_day3_recheck` has the complete canonical copied executable. All nine products match receipts and stdout; all three PNGs decode; it reports one `LOADED`, `DONE 310`, exit 0, no timeout, and no probe errors. At frames 10 and 300, full SRAM shows day 3, weather 3, and wind 4. Its receipt predates the new post-run fields, so its retained-executable identity was checked separately by this reviewer. The old anomaly remains preserved.

## 3. Food population and intervention evidence

**Classification:** OBSERVED_RUNTIME, independently inspected. Controlled state interventions are explicit.

The current defender field list contains 19,000 raw battle troops (190 displayed units). Its province resident list contains another 1,000 raw troops (10 displayed units). Both lists are present in the pre-transition full SRAM.

| Case | Defender counted population established from lists | Defender rice transition | Decrement |
|---|---:|---:|---:|
| Baseline day 1→2 | 190 fielded + 10 resident | 45,000→44,334 | 666 |
| Baseline day 2→3 | 190 fielded + 10 resident | 44,334→43,668 | 666 |
| Resident troop word zeroed | 190 fielded + 0 resident | 44,334→43,701 | 633 |
| Unrelated officer troop word zeroed | 190 fielded + 10 resident | 44,334→43,668 | 666 |

The day-2 baseline and both interventions use checkpoint SHA-256 `7ea302d1797cf59349253ce6add5e458d6426fe955a54bd1a81487c7fbdc9faf`. The resident intervention changes CPU `$68C5/$68C6`, corresponding to SRAM `$28C5/$28C6`, from the resident officer's 1,000 troop word to zero. The unrelated control changes `$6067/$6068`, an officer outside either defender list, from 100 to zero. At the first captured full-SRAM comparison, only those declared bytes differ from baseline. The intervening log includes old value, requested value, and readback. Attacker consumption remains 333 in every compared transition.

This establishes the second-list contribution in the current fixture. It does **not** identify the historical additional 60 units in the older guide's 130-fielded/190-counted case. That earlier officer identity remains unreproduced unless separately recovered. Do not label the current 190+10 case as a recreation of the earlier 130+60 case.

## 4. Native restoration, bank identity, and time/weather traces

The saved-state payloads embedded in the reviewed Lua scripts were decoded and inspected with the MSS parser. The day-2 and flee prompt states matched their recorded input hashes; their SRAM matched the first restored captures exactly except for the explicit food patches. `LOADED` is printed only after the native `emu.loadSavestate` call returns true.

The cold-checkpoint evidence independently matches eight saved/reloaded products: CPU RAM, CPU `$6000` window, full SRAM, mapper bytes, memory mapping, CPU fields, full state fields, and PNG. The MSS-derived SRAM equals the captured 32 KiB native SRAM. An earlier invalid checkpoint negative is preserved with native exit 2/wrapper failure and unchanged CPU RAM; an earlier empty screenshot is also preserved. The MSS compression-envelope size is not proof of a historical FCS-container extraction bias.

The ten `wait_days_002` snapshot rows agree with the authored day ledger. The native trace contains nine day increments, two weather writes, and three wind writes. Day is `$7D1C`; the old candidate `$7D25` remains 3 without an observed write. Day advances 2→11. Wind `$7C26` changes 4→6 on day 4, 6→2 on day 7, and 2→4 on day 10. Weather `$7C25` changes 2→3 before the day-3 increment and 3→1 before the day-5 increment. The trace's recorded byte-write ordering matters: the weather callback can still report the old day while executing that day's transition.

Day writes use native PC `$E7C4`, VM IP `$8294`, mapped PRG `$10294`; the observed wind writer uses VM IP `$BA9F`, mapped `$1DA9F`; weather uses `$B5AF`, mapped `$1D5AF`. Rice writes use native PC `$F20D`, VM IP `$B7F2`, mapped `$1D7F2`.

The inspected HUDs show day 3 with a down-arrow wind graphic at value 4 and days 4/5 with `No Wind` at value 6. This does not map every weather or wind code. No same-value wind write was observed in this particular trace; the logging mechanism does preserve same-value writes in other watched fields.

The full SRAM is the reliable comparison surface across temporary mapper-bank changes. All initial reviewed window pairs map to bank 1, but the added runs include temporary different CPU windows: `fire_execute_002` frame 95, `fire_day5_001` frame 570, Normal frames 620/650, Simultaneous frame 650, and Charge frame 610. Their full SRAM preserves the battle records. Reading those CPU windows as bank-1 data would invent false zeroes or discontinuities.

## 5. Fresh Flee confirms relocation despite generic defeat text

Both `flee_confirm_001` and `flee_confirm_002` restore the same unedited prompt checkpoint, SHA-256 `d1c724346ceb9e9f8755420dc4b3e45accf0bbc9fe017595da85bb7d80de2e23`. The direction menu offers Province 29 and Province 31 as open territory. The selected officer record is Yuan Shu at `$672A`, the only fielded attacker in this fixture.

Before confirmation, Province 29 has no resident head and owner `FF`; the attacker field head points to `$672A`, with troop word 10,000. After confirmation, the attacker field head is zero, Province 29's head is `$672A`, its owner is 4, the officer troop word is 100, and the ruler's province chain starts at Province 29 followed by Province 19. The officer's liege remains 4 and its battle position becomes `FF`. Thus the original records show successful relocation with the normal battle-to-strategic troop scaling.

The dense second replay records clearing the attacker head at frame 89, result `$7D0B=0` and reason `$7BF4=6` at frame 90, and troop/list/ownership updates at frame 91. The result/reason bytes are written again unchanged at frame 92. The first replay reaches the same pair.

The screenshots nevertheless say `Yuan Shu succumbed to a greater power` in the first replay and `Yuan Shu was crushed` in the second. Those messages describe the battle result and cannot by themselves establish capture, death, or failed relocation. The current evidence covers one fielded attacking officer and this successful destination. It does not establish multi-officer Flee, cancellation, every fallback, failed escape/capture, or the outcome of the older G08 case without that case's persistent state.

## 6. Limited Fire and attack-command review

### Fire

`fire_day5_001` starts with day 5/weather 1/wind 6, selects command mode 3, and records cell `$7E27` changing `00→20` hex at frame 552 and `20→00` at frame 593. This cell is index 51 of the 13-column field, coordinates (12,3). The text replay from its unedited pre-confirmation state records the same transition at frames 52/93. Its frame-66 PNG visibly says `It worked!`.

`fire_execute_002` selects mode 3 but its frame-93 PNG says `Oops!`. No fire-bit-set event appears anywhere in its watched 156-cell field during the captured interval. Its initial conditions are day 2/weather 2/wind 4; the weather changes afterward. These observations show one successful ignition and one failed attempt. They do not isolate weather as the causal difference or validate a probability formula, spread rule, or every terrain interaction.

### Normal, Simultaneous, and Charge

All three forest runs restore the identical native checkpoint SHA-256 `2eee61249fb6759508c0d12c28dd0567ad01b5373290ddadc3288a9cf82c9d73`. Their frame-10 full SRAM also has identical SHA-256 `9bcba89eeb2fceb427b2346e6b69a7ccd12cd3079619174dfe09d600ca277e12`. No RAM patch is recorded. Their baseline is day 5/weather 3/wind 6, distinct from the successful Fire fixture's weather 1.

Actor `$672A` begins with 9,847 troops at packed position `C4`; the selected target `$687A` begins with 1,000 at `B4`. The menu dispatch writes mode 1, 2, or 4 at frame 391. The troop writes establish:

| Command | First completed exchange | Actor troops | Target troops |
|---|---|---:|---:|
| Normal, mode 1 | Frames 615–616 | 9,608 | 692 |
| Simultaneous, mode 2 | Frame 622 | 9,608 | 692 |
| Charge, mode 4 | Frame 651 | 9,641 | 856 |

The frame-650 Charge snapshot precedes its troop writes and still contains the starting words; use the frame-651 trace or later full SRAM for its completed result. Normal and Simultaneous undergo a later AI exchange around frames 1345–1348, reaching 9,527/613. Those later totals must not be described as the original player exchange.

These are bounded command-path observations. A shared starting checkpoint does not make the different commands' entire execution paths or RNG consumption identical. This review does not claim a coefficient isolation, all Simultaneous targets, multiple Charge exchanges, or full formula parity.

## 7. Independent static report review

This follows the Director's unified NES/SNES mechanics authority. The inspected SNES image is SHA-256 `815bfcf4fd6eb23a20c2e50dde023c210b273ffb6cd86a93909d803c3643ce46`; symbol/source repository commit is `7c2bab7d634e22a3f5870fe548d6a0556760be98`; corrected generator commit is `9a436f493bf20f760c5eac49014163c2570e77bd`. The source investigator's all-module receipt covers 177,982 listed bytes across 29 modules. This review inspected the relevant decoded instruction paths; it did not independently rerun that entire all-module byte-verification job. The original NES runtime image is pinned separately to SHA-256 `11e312691b516e5e29dade3c8ce5434df3561cc69e1aecad9c948acdfddc517a`.

The regenerated source was used, not the older misleading pseudo-C. The Q09/Q14 report's critical paths were independently checked: `sengo:$83A4`, `$8436`, `$84A7`, `$85FE`; `root:$4EB0`, `$507C`, `$5154`; `hsovl2:$D582`, `$D648`, `$D78C`, `$D877`, `$D89D`, `$D905`; and the relevant `hex` list unlink/capture callees. No substantive semantic error was found in the reviewed Q09/Q14 conclusions. Ordinary release, defeated-ruler release availability, escape validation, per-officer movement, capture failure, and destination scaling remain distinct paths. Source closure is not a replay of every historical runtime case.

The G09 pending-ally candidate was also reviewed narrowly. `hsovl6:$CDF6/$CF94` can retain unplaced pending nodes; `hsovl1:$D2A4/$D14D` serializes them on continuation; `root:$2066/$23B1/$4820` permits the continued-battle route; `hsovl3:$DA43/$D53A` reloads saved state and then calls `$D900`. In the reviewed same-liege source-list branch, `$D900` appends the pending list and clears its pending pointer without reading or dividing troop words. The `FC95` bit-2/bit-8 skip and alliance check are explicit gates, not demonstrated universal blockers.

No hidden normalization was found in that reviewed chain. The candidate therefore remains **PARTIAL/INFERRED for legitimate-play reachability**, with conditional instructions VERIFIED_STATIC. Actual legal play must still preserve a pending ally through continuation and change the alliance before resumption. Neither a constructed RAM fixture nor the source branch alone proves that full campaign sequence. No new troop cap or correction is authorized by this review.

## 8. Remaining limits and disposition

The two identified evidence gates are PASS VERIFIED within the authored harness tests and captured native artifacts. The fresh weather replay provides clean retained-executable evidence, while the old mismatch is still recorded as UNKNOWN cause/time. The food, current Flee, time/weather, Fire, and first-exchange claims are supported at the bounded scopes above.

Still open: historical extra-60 officer identity; historical G08 outcome without persistent state; full wind/weather code coverage; multi-officer and failed Flee/capture paths; wider combat formula/Charge/Simultaneous coverage; legitimate-play reachability of the pending-ally scale candidate; and Windows/controller acceptance. None of the artifact counts or synthetic harness tests closes those questions.

The raw native run directories remain untouched. Review-script mistakes are recorded separately: the first weather review import omitted the sibling module path and exited before generating results; a temporary fresh-review metadata extraction incorrectly labelled record offset +2 as an officer ID and was removed immediately. Neither error changed an original artifact or the validated troop, position, or fire findings.
