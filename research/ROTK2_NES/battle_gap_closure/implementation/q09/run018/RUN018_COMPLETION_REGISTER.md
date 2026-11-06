# Classic Run018 / Q09 whole-army immediate insertion

Canonical source: Retro Gaming/ROTK2_NES_Windows (SUP-0016), not Original-KOEI Enhanced or DOS.
Source authority: Director English NES observable mechanics; original approved SNES source same mechanics. hsovl2 $D89D iterates officer withdrawals through the $D648 live officer insertion path. VERIFIED_STATIC: https://github.com/chrisaacson69/rot3k2-snes-decompiler/blob/7c2bab7d634e22a3f5870fe548d6a0556760be98/src/hsovl2.src

IMMUTABLE PARENT: Run017 complete source 162 entries SHA256 30c5dc01da6fe2a5f827d023fb926712d743dc5d1bc1d8ce826f632b484d1f8e; Drive ID 1p39vtROBns7UH5nhx6336B1EBnuvlz1P. Parent and Run017 QA independently re-downloaded and SHA/CRC verified.

NEW SOURCE: Run018 full ZIP SHA256 f343fbdbd13126bcda2b7ecc26d53cdff098fa767e23e3cf702c38d28186247b, 163 CRC-clean entries, 161 identical to parent. Actual engine withdraw() now calls original-source-known successful field-officer immediate province placement for whole-army withdrawals; preserves officer-specific destinations, excludes reserve/queued troops and unknown original fields. No Enhanced changes.

TESTED_PORT: RED unmodified parent 4 fail/1 pass; GREEN 6 focused pass (peaceful/attacked placement, split destinations, unknown fallback, illegal destination atomicity, Xvfb software controller two prompts, true .ro2 cold-load). Applicable broad Linux/Xvfb: 293 PASS, 37 SKIP, 1 DESELECT; freshly cold-extracted ZIP same. Negative mutation 4 intended fail/2 pass. Full unfiltered suite NOT green: private ROM tests and obsolete Classic008 inventory excluded. OBSERVED_RUNTIME only software-controller input under Linux Xvfb, not Xbox. VERIFIED_STATIC source fixture above. REPORTED_UNREPRODUCED emulator replay and 30 goldens. INFERRED/OPEN natural-campaign source-field hydration, second concurrent tactical battle and Q01-Q22 complete parity.

PUBLICATION: canonical Run018 Drive folder https://drive.google.com/drive/folders/1FG7fo-D0gmqHIRHe8IeSsAl-_OHQUJ2O; QA ZIP SHA256 095c0e334258251ee1ef8bdb16978eccb8d7bca6a6f5c53b64d0b61a2c48ecf3. Full source and QA and register independently read back from Drive, exact SHA, CRC and parent folder verified. No prior source overwritten. Diff, test and hash-guarded builder in this GitHub folder; QA ZIP contains member hash inventory/raw test logs/clean-source retest.

NEXT executable correction: original attacked-neighbor second battle slot / simultaneous tactical list consistency from source and emulator fixture against immutable Run018; red-before/green-after/negative/controller/long-term save. Enhanced independent Run010 lineage remains separate. Native Windows and physical Xbox not tested, Director release acceptance OPEN. Status: TESTED_PORT source candidate, NOT Windows release.
