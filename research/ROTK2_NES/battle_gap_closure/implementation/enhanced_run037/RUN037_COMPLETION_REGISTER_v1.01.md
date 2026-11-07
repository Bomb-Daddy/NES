# Enhanced Run037 Completion Register v1.01

Canonical Original-KOEI Enhanced only. Exact source parent Run036 `7b4e03d2f335a12fb5f5950e552b0fcd7e37f21abcfc425b8f68405211a92ecc` (published Original-game folder `1Jek1Uc8EhvwjqOBZy4JNevlfjy-0h0J3`). Run037 `f85a12daadeb7b9557f633ad5b0661f4eff9b91d082e2aafd7054d518a682762`, 555 CRC-clean ZIP members, 552/554 inherited unchanged, full source 148788833 bytes.

| Item | Evidence class | State | Evidence |
|---|---|---|---|
| Source parent and ancestry | VERIFIED_STATIC | PASS | Exact Run035→Run036 published patch+builder reconstruction and Run036 SHA; untouched immutable parents |
| Run037 full ZIP and 18 split parts | VERIFIED_STATIC | DRIVE VERIFIED PASS | Deterministic source SHA; manifest; 555 ZIP CRC; independently Drive-downloaded 18/18 parts, per-part SHA/size; reconstructed full SHA and 555 ZIP CRC passed |
| Q08 empty adjacent cell Fire Trick | TESTED_PORT | PASS SCOPED | Original Q08 `$818A`/`$C99C` unoccupied target chance; real engine and GUI |
| Guard against stale target ID redirection | TESTED_PORT | PASS SCOPED | Invalid inputs fail before RNG or state change |
| Q08 rain, occupied enemy, non-Fire negative controls | TESTED_PORT | PASS SCOPED | 11 focused pass; 7 expected failures/4 controls with mutant |
| Fresh-source working/cold broad | TESTED_PORT | PASS SCOPED | 200/200/200 passes working, local cold and Drive-retrieved cold Linux Python 3.13/Xvfb |
| Actual controller and save/cold-load | OBSERVED_RUNTIME | PASS LINUX SOFTWARE | 1280×720 Tk, NES six commands, `.ro2` disk write/second independent process read fingerprint |
| Original Q08 source reference | VERIFIED_STATIC | SOURCE REFERENCE | Answer Matrix v1.4, not original NES emulator execution this run |
| Original Fire targeting friendly occupied cell | INFERRED | OPEN | Fail closed, no original source-targeting proof |
| Q09 automatic second tactical battle and original four-slot turn order | INFERRED | OPEN | Authenticated original scheduler proof missing |
| Exact 41-map and UI acceptance | OBSERVED_RUNTIME | PARTIAL | Provisional grid in earlier real GUI screenshot; exact pixel/text not verified |
| 30 original NES event goldens | REPORTED_UNREPRODUCED | NOT RUN | No original emulator compared this pass |
| Native Windows and current physical Xbox | REPORTED_UNREPRODUCED | NOT RUN | Linux controller only |
| Unrestricted full suite and Director acceptance | INFERRED | OPEN | Selected scoped green is not full parity |

Original-game Drive folder ID `1FkUfTDPoJlfYPjmw4MTcG1SWxavgD_9Y` holds 18 independently retrieved source parts; exact reconstructed SHA `f85a12daadeb7b9557f633ad5b0661f4eff9b91d082e2aafd7054d518a682762`, 555 ZIP CRC passed. QA archive and small evidence, GitHub research patch, number 095 Collaboration handoff are separately pending verified publication until readback. No Director acceptance. Classic, DOS and Enhanced strategic economy/BP source remain untouched.