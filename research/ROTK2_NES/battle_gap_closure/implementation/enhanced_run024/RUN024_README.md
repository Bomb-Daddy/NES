# Original-KOEI Enhanced Run024 — Q09 modal Flee actor stays bound at commit

DATE: 2026-11-07. STATUS: **TESTED_PORT / PARTIAL**, NOT release acceptance.

## Source ancestry and product isolation
Direct immutable full-source parent is verified Enhanced Run023 SHA256 `776d20447d5d5f7820dbbca0ae2cb486e02a1c1edd33ccf902c3497937e9b157` from original-game Drive folder `16ycsr_en5zyLQ59fFFPFbWItI6VAMCUN`. New complete Run024 SHA256 `d48cdf9bf04999db31568cb303da92cdb2e402dd4cebbbcb5ba7e66e6491b0b8`, 148755565 bytes, 542 CRC-clean ZIP members; 540/541 existing source entries byte-identical. Changed production only `third_branch/tactical_view.py`; added only `tests/test_run024_q09_flee_modal_actor_integrity.py`. It does not use or modify canonical Classic NES SUP-0016, DOS, Build40R4, or Enhanced economy, marketplace or BP. The original Run023 source ZIP remains immutable for rollback.

## Actual source-backed correction
English NES source `hsovl2 D905` dispatches individual retreat against the selected officer. The Enhanced six-command Tk port previously validated selected identity when opening Flee but *not again when confirming a destination*. If selection changed or the officer departed while the destination modal was open, it could silently issue Flee for a different defender. A stale whole-army modal could retain a departed subordinate. Additionally low-level GUI `_issue` silently overwrote explicit selected `officer_id`.

Run024 verifies every frozen participating officer is still live and on the human side at confirmation, verifies selected officer/primary commander identity, refuses obsolete modal atomically, and guards explicitly named individual Flee at dispatch. A valid selected individual B9 and legitimate whole-army primary B2 retain prior mechanics. Exact original invalid-cursor text has not been authenticated; error message is PORT provisional. This is not source-code-structure imitation nor proof of all Q09 original scheduler behavior.

## Executed evidence on Linux Python 3.13 and Xvfb/Tk
- True pristine Run023 parent (test file copied in extracted ephemeral test tree; production unchanged): **5 failed / 1 passed** with real Tk case, expected RED. Log `RED_PARENT_XVFB.log`.
- Run024 focused inherited Q09 and controller tests: **43 passed**. Log `GREEN_FOCUSED_XVFB.log`.
- Expanded real GUI/controller tests: **66 passed**, log `APPLICABLE_GUI_GREEN.log`.
- Deliberate faulty single-condition mutant: **2 failed / 4 passed**, expected RED, restored exact source then **6 passed**, logs preserved.
- Brand-new cold extraction from complete Run024 ZIP: **66 passed** on Linux Xvfb (`COLD_EXTRACTED_GUI_GREEN.log`).
- Actual `.ro2` file written in process 1, loaded separately in process 2: same fingerprint, selected Flee remains bound/rejected on stale selection, validated conservation. Disk SHA in `COLD_WRITER.log`; `COLD_READER.log`.
- Two independent source ZIP rebuilds byte-identical SHA, checked all 542 CRCs. No archived parent source changed or overwritten.
- Applicable unfiltered broad suite **NOT green**: two inherited collection errors (private `ROTK2_TEST_ROM`, historical rollback import), historical Windows-port hang, and an extensive persona-engine matrix caused larger filtered runs to time out. Logs included. Do not count unfinished broad as passed.

## Completion
- RUN024-PARENT: VERIFIED_STATIC / PASS (immutable parent local exact SHA and Drive-published historical reference).
- RUN024-SOURCE-LOCAL: VERIFIED_STATIC / PASS (new SHA, CRC, deterministic two builds).
- RUN024-GAMEPLAY: TESTED_PORT / PASS scoped (no Flee retarget on modal confirmation/dispatch).
- RUN024-CONTROLLER: OBSERVED_RUNTIME / PASS Linux Tk Xvfb software-controller.
- RUN024-RED-GREEN-MUTANT: TESTED_PORT / PASS scoped.
- RUN024-COLD: TESTED_PORT / PASS 66 test cold extraction and 2-process .ro2.
- RUN024-BROAD: TESTED_PORT / INCOMPLETE timeout/known collection blockers.
- RUN024-ORIGINAL-GOLDENS: REPORTED_UNREPRODUCED / NOT RUN new 30 NES traces.
- RUN024-ORIGINAL-AUTO-BATTLE-SLOT: INFERRED / OPEN; original scheduler/second active tactical slot and four-list ordering not source/event authenticated end-to-end.
- RUN024-WINDOWS-XBOX: REPORTED_UNREPRODUCED / NOT RUN native Windows/current-build physical Xbox.
- RUN024-1TO1-ACCEPTANCE: INFERRED / OPEN (Director acceptance not granted).

## Next exact action
Recover authenticated original English NES event/source evidence for Q09 attacked-destination simultaneous second tactical slot and four-list turn ordering, then implement on *exact Run024 full SHA* and rerun parent RED, child GREEN, mutation, real controller and two-process cold, NES event goldens and genuine Windows/Xbox. Preserve the separate Classic NES Run021 and original-game economy/BP.
