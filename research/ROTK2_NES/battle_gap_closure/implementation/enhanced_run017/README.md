# Original-KOEI Enhanced ROTK II battle parity — Run017 Completion Register

Date: 2026-11-06 (America/Detroit). Director scope: one-to-one *observable* English NES ROTK II default battle, not identical code. This run is an original-KOEI Enhanced partial source/port test. **DO NOT promote to Director-accepted release.**

## Exact source

- Direct immutable Run016 full-source parent: `f2234784d6eb6746d24bee9f5fed6172077a925ec8f4fcfd7a4176ee321732fa` (533 members). Run016 explicitly descended from Run015A; Run015B behavior donor only. Both originals retained.
- Run017 full-source ZIP SHA-256: `d052013bff0cad1a3fc9be9902647f9d087aba19f6c19609d9aaa9092ec115a6`, 148738638 bytes, 534 CRC-clean entries. 532/533 parent members are byte-identical. Only `third_branch/integrated.py` modified; one new test file added. Independent rebuild exact SHA match.
- Original code fixture source: chrisaacson69/rot3k2-snes-decompiler Git commit `7c2bab7d634e22a3f5870fe548d6a0556760be98`, `src/sengo.src` prisoner disposal `$82A4`, menu `$8749`, `src/root.src` related officer state `$4EB0`; these verify where Q14 fate processing lives but **do not yet establish actual natural scenario successor identity or ordering**.

## Exact correction / limits

Before Run017, beheading a captured enemy ruler always cleared the faction's `ruler_officer` even if that faction survived with controlled territory and an eligible second officer. Run017 allows an *explicitly supplied* `q14_source_ruler_successor` mapping to name a living same-faction officer located in a remaining friendly province. On the actual `battle_prisoner` behead choice, it sets that actual dynamic ruler pointer. A captured, unknown, wrong-faction, dead, or nonresident candidate is rejected atomically (`Q14_SOURCE_SUCCESSOR_INVALID`). With missing historical source mapping the ruler pointer remains `None` and a disposition records `ORIGINAL_SUCCESSOR_UNRESOLVED`. This is **not** a reconstructed English NES succession-selection algorithm and does not manufacture missing bytes. The original `ruler_slots` initial snapshot is preserved unchanged. Enhanced economy, trade, BP code unchanged. Classic and DOS trees unchanged.

## Executed real QA on Linux Python 3.13 / Xvfb software-controller

- Pristine Run016 parent + new real Campaign regressions: RED 4 FAIL, 1 PASS.
- Run017 actual production after patch focused: 5 PASS.
- Run017 mutation (`successor=None` deliberately discards input): 4 expected FAIL, 1 PASS. Restored production: 5 PASS.
- Scoped Enhanced `test_build*`, `test_run*`, Q03, cohorts, historical S1, integrated battle suite, excluding the ROM-private file: 299 PASS / 2 SKIPPED (working tree).
- Completely cold-extracted Run017 full source ZIP, same scoped suite: 299 PASS / 2 SKIPPED.
- Real NES-style six-command Prisoner fate controller action under Tk/Xvfb: PASS, tested through Campaign transition; no physical Xbox controller involved.
- Disk .ro2 saved process #1, cold-loaded and resolved process #2, saved and independently verified process #3: PASS; final fingerprint `f0a9302212b1d2ad43109d3d3a3a9912651b352c6d516481333d32bc6461846c`.
- Independent deterministic source rebuild reproduced full ZIP SHA, ZIP 534 member CRC PASS. Actual parent full SHA pinned before building.
- Unfiltered full test collection produced two known errors: private `ROTK2_TEST_ROM` absent, historical `test_ui_integrity_v19_BUILD13.rollback.py` import; broad unfiltered execution halted around 70% and **is not claimed green**.

## Classified completion entries

| ID | Classification | Status |
|---|---|---|
| ENH017-PARENT | VERIFIED_STATIC | PASS exact Run016 SHA / verified source ancestry |
| ENH017-Q14-CODE | TESTED_PORT | PASS controlled source-supplied successor only; complete original algorithm PARTIAL |
| ENH017-INVALID-NEG | TESTED_PORT | PASS atomic rollback, malicious candidate checks |
| ENH017-REDGREEN | TESTED_PORT | PASS 4 RED/1 PASS, 5 GREEN, mutant 4 RED/1 PASS |
| ENH017-CONTROLLER | OBSERVED_RUNTIME | PASS Linux Xvfb software controller only |
| ENH017-COLD | TESTED_PORT | PASS 3-process real `.ro2` persistence |
| ENH017-BROAD | TESTED_PORT | PASS 299/2 scoped, unfiltered blocked by 2 known errors |
| ENH017-SOURCE-FIXTURE | VERIFIED_STATIC | PASS source routine addresses; ruler identity mapping original not supplied |
| ENH017-NATURAL-SUCCESSION | INFERRED | OPEN: original successor priority/identity historical scenario |
| ENH017-ATTACKED-SLOT | INFERRED | OPEN: Q09 second tactical battle recipient slot |
| ENH017-NES30 | REPORTED_UNREPRODUCED | NOT RUN: 30 original event traces |
| ENH017-HARDWARE | REPORTED_UNREPRODUCED | NOT RUN: native Windows & physical current-build Xbox |
| ENH017-DIRECTOR | REPORTED_UNREPRODUCED | NOT ACCEPTED |

## Next executable

Recover real English NES scenario successor order/linked province head or run original emulator Q14 controlled cases. Bind authenticated historical source data to ordinary scenario `q14_source_ruler_successor` and compare complete original event traces. In parallel Q09 attacked-neighbor second active battle topology. Do not promote missing-data behavior to verified original parity. Preserve Classic Run021 exact full SHA `93a9acb7dee1819bd6f04a761b5b0aacbc315b4d8b92472269135b204b6e9eb3` and DOS separately.
