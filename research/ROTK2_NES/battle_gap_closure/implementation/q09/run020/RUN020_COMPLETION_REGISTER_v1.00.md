# SUP-NES-BPF-20261106-068 — Classic Run020 Q09 source successor links

Date: November 6, 2026. Status: **TESTED_PORT source candidate**; NOT an accepted native Windows release. Director wants **1:1 observable gameplay** with English NES ROTK II, not identical 6502/SNES implementation. Canonical Classic root: Google Drive / Retro Gaming / ROTK2_NES_Windows (SUP-0016). Separate Original-KOEI Enhanced Run011 and DOS unchanged.

## Source ancestry and change
- IMMUTABLE actual parent: Classic Run019 complete source SHA256 `59dec33f717481c93ab7791e3ddc47458acdd56c95c3c8392aa6ade0d841a056`, 164 entries; retrieved local and independently re-downloaded from canonical Run019 Drive folder `1cfFcZisnquW_MpDbvYYV_dr4PaIyap6R` after recovering previously blocked uploads. Run019 Drive source ID `1OGgAEUVqPycvD2OMW8h-vocUrwW9r0QP`; QA ID `1M8_k62ZLwjkt-Ry8TsxJkGf_C1yLHLrm`; bytes, SHA256 and ZIP CRC verified.
- New Run020 complete source candidate SHA256 `964d9ad55c50eaab944bf8f70ff257a230494b1ff6a799e152d88561b21ea345`, 165 entries. Exactly two old source files modified, one new regression test, 162 parent members byte-identical. Deterministic patch builder using the immutable Run019 zip (also a separately fetched copy) produces identical source archive SHA.
- Source oracle: public decompiled English-mechanics SNES `root.src`, `select_officer_5154` `$5159-$517D`, `$518E-$5195`, source pinned commit `7c2bab7d634e22a3f5870fe548d6a0556760be98` at https://github.com/chrisaacson69/rot3k2-snes-decompiler/blob/7c2bab7d634e22a3f5870fe548d6a0556760be98/src/root.src . Native logic loads existing province head, writes escaping officer as head, and stores former head as link successor; this is distinct from copying its code structure. Director authorizes SNES mechanics as NES system. Evidence is VERIFIED_STATIC, NOT original NES emulator execution.
- Actual implementation: `Officer.q09_officer_next: Optional[int]` persists in `.ro2` saves; immediate source-known escapes now preserve the destination's prior known resident head as the new officer's successor. An officer exiting as old source head advances to its explicitly known, still resident successor rather than discarding the known linkage. Missing/foreign/stale pointers do not fabricate a new head. Existing unknown-metadata fallback is not falsely upgraded to parity.

## Executed evidence (Linux Python 3.13 / Xvfb software-controller)
- Fail-before pristine Run019 new regression: 5 FAILED, 1 PASSED. New controller test was corrected from an erroneous test-only `.options` assumption before the final baseline; no production fix was implied by that test correction.
- Corrected focused regression: **6 PASSED**.
- Applicable Linux/Xvfb cross-tree tests on working source: **310 PASSED, 37 SKIPPED, 1 DESELECTED**; cold-extracted source: **310 PASSED, 37 SKIPPED, 1 DESELECTED**.
- Deliberate mutant disables successor assignment: **3 EXPECTED FAILED, 3 PASSED**; restored focused suite **6 PASSED**.
- Actual BattleScreen/YesNo/ListPrompt software-controller withdrawal route, two selected destinations, and immediate successor links: PASS under Linux Xvfb. Actual `.ro2` save/cold-load, state hashes and invariants: PASS.
- Exclusions and gaps: one original-ROM test not collected because `ROTK2_TEST_ROM` unavailable. Historical CLASSIC008 exact-file-inventory test deselected because new source additions inevitably differ from legacy release manifest. **Not a claim that every test in repository passed**. Native Windows EXE, current-build physical Xbox, original NES 30 event traces, Enhanced equality, and Director release acceptance NOT RUN/NOT VERIFIED.

## Completion Register
| Item | Evidence class | Status |
|---|---|---|
| Q09-020A original head and successor write source | VERIFIED_STATIC | PASS source excerpt |
| Q09-020B consecutive whole-army successor order | TESTED_PORT | PASS |
| Q09-020C known source head removal to resident successor | TESTED_PORT | PASS |
| Q09-020D malformed cross-province link rejected as guessed head | TESTED_PORT | PASS |
| Q09-020E `.ro2` save/cold-load and state invariants | TESTED_PORT | PASS |
| Q09-020F real Linux Xvfb software-controller UI | OBSERVED_RUNTIME | PASS |
| Q09-020G private ROM/30 original NES event traces | REPORTED_UNREPRODUCED | OPEN |
| Q09-020H natural campaign original byte hydration | INFERRED | OPEN |
| Q09-020I simultaneous second tactical battle slot | INFERRED | OPEN |
| Q09-020J full Classic/Enhanced 1:1 and Windows/Xbox | REPORTED_UNREPRODUCED | OPEN / NOT RUN |

## Next executable step
From EXACT verified Run020 SHA, derive original attacked-neighbor second-battle-slot/linked order evidence; implement smallest source-backed current battle transfer correction with RED/GREEN, negative, controller, `.ro2` cold-load and event trace. In parallel, separately Enhanced Run011 requires naturally sourced campaign Q09 bytes and Q14 fates, retaining economy/BP unchanged. Do not merge DOS or Original KOEI source trees, or promote any TESTED_PORT candidate to release without Director acceptance.