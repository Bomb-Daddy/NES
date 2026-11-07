# Run018 Completion Register — Enhanced Original KOEI ONLY — 2026-11-06

Authority: SUP-OG-ENH-BPF-20261106-057 and maintained KOEI source register; separate Classic NES Run021 and DOS unchanged.

Direct immutable SOURCE parent Enhanced Run017 `d052013bff0cad1a3fc9be9902647f9d087aba19f6c19609d9aaa9092ec115a6` (534 CRC entries). New locally tested source Run018 `95a9e1133684d7b18841d275bf8d32d0b45b814b30521836cbf5922dd56e3ba5` (148741521 bytes, 535 CRC entries; 532 old unchanged, 2 production files modified, one new regression file). Full QA ZIP SHA256 `6184ef11fdc7136927a0447a3c55f9f0055006d8a0b303248919e15f8672813b`.

## Labels and statuses
| ID | Evidence class | Status |
| --- | --- | --- |
| ENH018-PARENT | VERIFIED_STATIC | PASS immutable SHA/CRC local source, prior independent Drive Run017 published |
| ENH018-ATTACKED-Q09 | TESTED_PORT | PARTIAL/PASS scoped source-tagged officer live transfer |
| ENH018-BATTLE-SCALE | TESTED_PORT | PASS scoped pending recipient retains `troops100` without duplicating strategic army |
| ENH018-RECIPIENT-PLAYABILITY | INFERRED | OPEN, second simultaneous battle actor/controller/4-slot not implemented |
| ENH018-RED-GREEN | TESTED_PORT | PASS: 5 failed/2 passed pristine; 7 passed patched |
| ENH018-NEGATIVE-MUTANT | TESTED_PORT | PASS: 4 intended failures/3 passes |
| ENH018-BROAD | TESTED_PORT | PASS selected 223; full unrestricted collection 2 blockers and extended suite timeout |
| ENH018-EXTRACTED | TESTED_PORT | PASS 40 tests fresh extracted full-source ZIP |
| ENH018-CONTROLLER | OBSERVED_RUNTIME | PASS Linux Xvfb software input ONLY |
| ENH018-REAL-RO2 | TESTED_PORT | PASS process1 disk save/process2 cold-load exact fingerprint |
| ENH018-ORIGINAL-REPLAY | REPORTED_UNREPRODUCED | NOT RUN 30 original NES emulator golden event traces |
| ENH018-DRIVE-BINARIES | VERIFIED_STATIC | **BLOCKED**: Library container_session_unavailable; connector BLOCKED_FILE_REFERENCE; no SHA readback |
| ENH018-WINDOWS-XBOX | REPORTED_UNREPRODUCED | NOT RUN native Windows/physical Xbox |
| ENH018-DIRECTOR-ACCEPTANCE | INFERRED | OPEN; NOT A RELEASE |

Drive original-game correct child folder `1jfOKUY10stIyNpSbKEve_WGitXhIodAn`. Source and QA binary ZIP not in Drive. Full local source and QA exist at /mnt/data/rotk_enh_run018_work/output. Publication incomplete; preserve Run017 as latest independently verified full Drive source.

Next executable: restore Drive binary delivery and independent download 18-part SHA/CRC. Then implement actually playable recipient second battle for Q09 attacked destination and event-by-event emulator controls; preserve source-known flags, no unknown defaults. 
