# EXEC0056 — Genghis Khan I English NES-to-Windows native research increment

This increment extends **the existing standalone Genghis Khan I C11 native shell**, not ROTK II or the separate original KOEI-inspired game. Immutable parent is the complete 368-entry EXEC0055 ZIP on private Collaboration Drive (ID `1UloQyjPYPhstaFhyOcYe68dqVE_84tUI`, SHA-256 `69d879d726dde6345bf0e6aa8283f69efb83116e8b9d36c2cfdbfe2ba453f523`).

## Implemented and actually executed

In the existing 1280×720 controller-only manual bonus *draft* editor, X removes one point and Y removes up to ten (clamped at zero) from the selected five-character / six-ability field. Other fields and campaigns remain unchanged. A+1 and B+10 remain. **This correction is native UI functionality, not a claimed original NES mechanic**. Outside editing X/Y retain existing save/load mapping. New `--bonus-correction-export` and `--bonus-correction-import` exercise pad routing and the inherited source-gated exclusive-create 2,734-byte setup save in actual Linux processes.

Debug CTest **16/16**, Release **16/16**, AddressSanitizer+UBSan **16/16**, and clean extracted full-source ZIP Release **16/16** passed. Prior RED14/16 due to new-test pointer typo and old packaging-version expectation was fixed with logs retained. Native Xvfb 1280×720 setup graphical smoke passed, and Debian `0.56.0-dev1` extracted package passed version/roster/data checks. Synthetic pad inputs, **not physical Xbox**. No original NES runtime or native Windows acceptance.

New complete inherited+delta, 450-entry source+QA ZIP: `GK1_EXEC_0056_Manual_Draft_Correction_Full_Native_Source_v1.00.zip`, SHA-256 `89097b2f849caa337f66b45f799117666d4363e080e37397295f0e9dac73d1c9` (833737 bytes). New Linux research DEB SHA-256 `910166e52f908fb4d19c91b27fa334f54f9bdbf56608cb26bc42d4e4b27ff754`. These were built and verified **locally**, and were not Drive-uploaded due to `container_session_unavailable`. The full build tree is in the conversation ZIP, NOT claimed to be entirely mirrored in this GitHub directory.

Authoritative incremented [master v2.38](https://docs.google.com/document/d/1LXmkjzstbIMKTnKZ3ed0tvEXfX2MI7IJMDyTYpojUOA/edit), [handoff](https://docs.google.com/document/d/1oAVlmHmaVSFtgvpwhqBkeAW5mj_51n2K5qroOGKf5zs/edit), [completion ledger](https://docs.google.com/document/d/1ML4Qdi9F_-8ZkY1b-V0xgSZ85gE3Ul0e0yU9RM7FQiM/edit).

**Still incomplete:** original character-stat generation and first playable Spring, both full campaigns/strategic AI/diplomacy/dynasty, complete battle and end outcomes, genuine graphics/audio, full campaign saves, Windows installer and actual Xbox acceptance. Classic and approved modified Enhanced remain separate evidence profiles. No licensed ROM, audio, graphics or raw private machine state in this public text.
