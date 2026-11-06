# Classic ROTK II Run017 completion register — 2026-11-06 (additive)

**TESTED PORT CANDIDATE, not Director-accepted release.** Canonical Classic NES source is SUP-0016 Google Drive / Retro Gaming / ROTK2_NES_Windows, not GitHub Build40R4. Original-KOEI Enhanced and DOS untouched.

| Work item | Evidence | Result |
|---|---|---|
| Q09 attacked-destination live officer insertion | VERIFIED_STATIC + TESTED_PORT | PARTIAL: hsovl2 $D688–$D6A5 original rule; 7455 troops retained in attacked neighbor; second simultaneous tactical battle not implemented |
| Q09 peaceful Flee and source-unknown fallback | TESTED_PORT | PASS targeted: 7455 -> 74 once, fallback unchanged |
| Save/cold-load, post-battle settlement | TESTED_PORT | PASS actual .ro2 disk save/cold-load, no double division or origin reversion |
| Controller BattleScreen/ListPrompt | OBSERVED_RUNTIME | PASS Linux Xvfb software navigation, not Windows physical Xbox |
| Red-before and negative mutant | TESTED_PORT | Run016 2 FAIL/2 PASS; Run017 5 PASS; two mutants yielded 3 + 1 expected failures |
| Applicable suite, cold extracted source | TESTED_PORT | 292 PASS, 37 SKIP, 1 DESELECT; original-ROM test missing private ROM, obsolete Classic008 inventory excluded; unfiltered not green |
| Run017 binary | VERIFIED_STATIC LOCAL ONLY | Full ZIP SHA256 30c5dc01da6fe2a5f827d023fb926712d743dc5d1bc1d8ce826f632b484d1f8e, 162 CRC clean entries; immutable parent Run016 SHA a0e218c87734b833c92821fff3dd1afd67e51f0d5494da9bee8ce8bcca1fb0cd, 160 of 161 parent files byte-identical |
| Run017 Drive source delivery | BLOCKED | Folder 1yrh_xOb1jHtrTo4kIdEk_1YdA395A_NS under NES root 1ODnfdVL5R3bZBBUp4cXpocNqtTClrxIL verified, full source/QA upload attempt failed container_session_unavailable; Run016 remains latest full Drive source |
| Enhanced Run007 source | REPORTED_UNREPRODUCED | BLOCKED exact SHA 15ed58d6a0bba0f824bccfbc864a55cc3267bef004071d336f8df0c76b6b6a92; Enhanced Run009 is parallel 18-part donor SHA e546d49dfe1f12f290445907997a5dc64f48d67afea8907705cb04abdae971d8 NOT merged |
| Fresh original emulator, 30 event goldens, complete Q01-Q22, Enhanced controlled RNG, Windows native + physical Xbox, Director acceptance | INFERRED gaps / REPORTED_UNREPRODUCED | NOT VERIFIED/OPEN |

Local QA ZIP SHA256 7ab229c9796c27eaaf92ee1853958a993382e0275a75abe7eaa4d437b0a3fc0e (48,435 bytes, 13 CRC clean). Hash-guarded rebuild and negative/green/parent logs in evidence. Research branch contains reversible production diff and 5 focused regression tests; GitHub publication is not canonical Drive binary publication.

Original source: https://github.com/chrisaacson69/rot3k2-snes-decompiler/blob/7c2bab7d634e22a3f5870fe548d6a0556760be98/src/hsovl2.src
Numbered Collaboration 064: https://docs.google.com/document/d/1C2ryAFDS2bZKLw8IEJY3XXMrGsynauLVNpNtD8Riy_c/edit (parent/content readback verified).
Next: recover Drive binary upload and SHA/CRC readback for Run017; then implement attacked neighbor second tactical battle slot. Enhanced: recover exact Run007 before selective Run009 UI/Run006 destination forward merge; preserve Enhanced marketplace/economy/BP.
