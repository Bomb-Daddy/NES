# Enhanced Run036 Completion Register v1.01 — Nov 7, 2026

Original-KOEI Enhanced only. Original-game root Drive ID `1i5kLog2o1tC9c2dsjdF2CLFnXYQHVk_l`; Canonical NES Classic SUP-0016 and DOS were not changed.

| Item | Evidence class | State | Verification |
|---|---|---|---|
| Immutable Run035 parent | VERIFIED_STATIC | PASS | SHA256 `48b4ed72b99141fe9672dcabbeb32cf417d498cb75461e5270e67bf0aceebf94` |
| Run036 original-game full source | VERIFIED_STATIC | PASS | 148785676 bytes, SHA256 `7b4e03d2f335a12fb5f5950e552b0fcd7e37f21abcfc425b8f68405211a92ecc`, 554 CRC-clean members; two deterministic builds; 551/553 unchanged parent ZIP members |
| Original-game independent Drive readback | VERIFIED_STATIC | PASS | Drive folder `1Jek1Uc8EhvwjqOBZy4JNevlfjy-0h0J3`, 18/18 parts separately downloaded with sizes/SHA256 identical to published manifest, full reconstructed ZIP SHA exact and CRC None, QA ZIP independently downloaded SHA `9350b3bba0ad46f7ceed679335e55ef423705228d9e2975364f706f76b843364`, CRC None |
| Q10 bribe prohibition for current live head | TESTED_PORT | PASS SCOPED | Original linked list head vs static `commander` flag; real Campaign and controller, no gold/RNG/turn when invalid |
| Q10 daily live-head switching exclusion | TESTED_PORT | PASS SCOPED | Live head protected even after commander flees; nonhead negative-control still eligible |
| Parent RED → green → mutant | TESTED_PORT | PASS SCOPED | RED 5 failed/2 passed (one GUI root geometry fixture setup error corrected before green), GREEN 7 passed, mutant 2 expected failures / 5 controls passed |
| Working, cold-extracted, independently Drive-retrieved suites | TESTED_PORT | PASS SCOPED | 200/200/200 passed, Linux Python3.13/Xvfb; unfiltered full pytest NOT run |
| Actual six-command Tk controller | OBSERVED_RUNTIME | PASS LINUX ONLY | 1280×720 software GUI, Q10 target selection/rejection, `Wait` `.ro2` disk save, separately executed Python cold load identical fingerprint |
| Original Q10 Source matrix | VERIFIED_STATIC | SOURCE REFERENCE | v1.4 hsovl1 `$D5DD`, `handle_unit_rout $7AC0`; source-derived, not an executed NES replay this run |
| Q09 simultaneous attacked-recipient second tactical battle / four-list scheduler | INFERRED | OPEN | Original event/slot scheduler source proof missing. Do not invent mechanism |
| Original 30 NES emulator event goldens | REPORTED_UNREPRODUCED | NOT RUN | Not run this pass |
| Native Windows and current-build physical Xbox | REPORTED_UNREPRODUCED | NOT RUN | Linux software GUI only; old Classic Xbox evidence not current build acceptance |
| 41 authentic maps / complete exact 1:1 UI | OBSERVED_RUNTIME | PARTIAL | Prior Run035 current UI screenshot showed provisional seven-province grid |
| Director final 1:1 acceptance | INFERRED | OPEN | Unverified remaining source/emulator/interface/integration gates |
| GitHub research patch and test publication | VERIFIED_STATIC | PUBLISHED / READBACK REQUIRED | Research branch `rotk2-battle-gap-research-20261105`, NOT canonical Classic source; inspect latest Git SHA in publication handoff |
| Numbered Collaboration handoff | VERIFIED_STATIC | PENDING | Publish 094 then read back correct folder and content |

No preserved source ancestor, strategic economy/BP, canonical Classic or DOS modified. Original game full release remains PARTIAL; don't claim Windows-native or controller parity.
