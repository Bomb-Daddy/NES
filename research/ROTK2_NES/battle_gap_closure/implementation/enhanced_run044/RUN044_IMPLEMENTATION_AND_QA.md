# Run044 Enhanced Q04 — pending preview cannot occupy a live battlefield hex

**Separate Original-KOEI INSPIRED ENHANCED only; not canonical NES Classic/DOS.**

Exact immutable source parent Run043, independently Drive-verified: `84c59e2448311eaa075f5fe3932674d84d87306d4ce484ab60cee16d46ae36be` (565 members). Run044 local full candidate: `20fe5a3ed964d4cd4854419ba887dadca2df9f7cfee420ae3041cbd206eb14e1`, 148815052 bytes and 567 CRC-clean members (same hash across two independent package builds). Only `third_branch/q04_manual_deployment.py` changes in production.

**Original-source evidence:** 41 English NES region tables at `0x28934`, neighbors at `0x28014`, terrain at `0x26FF8`; source `hsovl6:$D0CF/$CE5C` human interactive deployment to a legitimate zone cell; AI uses free zone candidates. `ROTK2_Battle_Answer_Matrix_LIVING_v1_4_part1_of_2.md` is the published research. Run043 displays pending player units as *not deployed* but the old occupancy check still erroneously treated their preview hex as fielded. Run044 excludes uncommitted members of `q04_pending_deployment` when calculating occupied coordinates. Already confirmed officers and CPU units still block; no attack or battle mechanics changed.

**Actual Linux/Xvfb tests:** untouched Run043 3 RED/1 control PASS; patched 4 GREEN; deliberately reverted mutant 3 RED/1 control PASS. Broad 343 PASS, fresh source extraction 44 PASS. Real Tk controller D-pad/A included. Four separate processes saved/loaded a real .ro2 with the first officer placed on a second pending officer's provisional hex and then the second placed elsewhere. Cold fingerprint `d43b8d026526dc132728e69ac9e51adebc5a7b07e7c65d34f29f3283f6d5a5a8`, disk SHA `d59a8aaba08d5558c4296a27f7fa7c94088ea8bc1c65c3fecfabd514689e4ecd`. Selected tests pass, unrestricted suite not claimed green.

**Inherited 41-map static audit:** 227 defender/invader source-region zone pairs among 41 maps, all 227 have 2 or more legal candidate cells (minimum 5, maximum 20). Derived from previously hash-pinned ROM map bundle, NOT a new NES emulator run.

**Publication gate:** Correct original-game Drive Run044 child folder `158Sfjh_9nvGEutRisW8eyvJj2GDeLn76` created and source split into 18 locally SHA-verified parts. Direct container egress `container_session_unavailable`, connector raw file `BLOCKED_FILE_REFERENCE`. **Run044 NOT independently Drive-binary verified and NOT promoted. Run043 remains latest fully Drive-verified Enhanced source.** The Run044 source ZIP/QA and parts are in the conversation sandbox, awaiting distinct exported file references.

Completion Register: https://docs.google.com/document/d/1BYT5N0sp14UlXpNH8A4pA4ph-qaTIxzyWw6PQ/edit

Numbered Collaboration handoff 103: https://docs.google.com/document/d/13eeYmJ85w3cPfmj-Ly1hloqjBi7MUUuvVrJi3s96r2I/edit

Full 30 original NES goldens, native current Windows, physical Xbox, and Director 1:1 parity remain OPEN. No fictitious second-battle scheduler.
