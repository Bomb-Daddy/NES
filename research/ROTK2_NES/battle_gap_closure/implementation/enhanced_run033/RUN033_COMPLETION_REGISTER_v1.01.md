# RUN033 Completion Register v1.01 — Original KOEI Enhanced ONLY

| ID | Requirement | Evidence tier | Status | Actual evidence / blocker |
|---|---|---|---|---|
| RUN033-BASE | Exact Run032 Drive parent | VERIFIED_STATIC | PASS | Source 014ae2dcdc189395ee17f6d52d4568d526952180a7d4a4311583033741bd0e95, 550 CRC-clean members, 18/18 parts matched original Drive manifest |
| RUN033-Q14-UI | Captured officer and selected fate visibly match A action | TESTED_PORT | PASS scoped | Run032 RED 4 fail/2 pass; fixed 6 pass; mutant 4 fail/2 pass |
| RUN033-LINUX-TK | Real one-screen controller visible overlay | OBSERVED_RUNTIME | PASS Linux only | Actual Tk/Xvfb, View>Prisoner Fate, D-pad/SELECT/A/B, screenshot |
| RUN033-COLD | Scope and physical disk save / independent process | TESTED_PORT | PASS scoped | Working selected 70 pass; clean full ZIP 190 pass, separate-process .ro2 fingerprint |
| RUN033-SOURCE | New whole ZIP | VERIFIED_STATIC | PASS local | SHA256 76b9247f9d1f0e21d143352134e8eaf93fe21f3daf529b757460017f1684a95e; 551 CRC-clean members; 2 matching builds; old 549/550 members unchanged |
| RUN033-DRIVE | Full 18 parts, independent readback, correct folder | VERIFIED_STATIC | PASS | 18/18 provider-independent Drive raw source-part hashes/sizes; full rebuilt SHA, 551 CRCs, QA ZIP SHA+CRC, correct original-game parent; Drive-recovered cold 51 pass |
| RUN033-GITHUB | Patch / regression / manifests readback | VERIFIED_STATIC | IN PROGRESS | Research-only GitHub patch/test/evidence publication/readback separate from complete-source Drive archive |
| RUN033-HANDOFF | Numbered Collaboration 091 content/folder readback | VERIFIED_STATIC | IN PROGRESS | Numbered Collaboration 091 publication and correct parent/readback required |
| RUN033-NES-GOLDENS | Original English NES 30 event-by-event traces | REPORTED_UNREPRODUCED | NOT RUN | Source golden/emulator unavailable for this scoped correction |
| RUN033-SECOND-BATTLE | Q09 simultaneous second tactical battle/four-list scheduler | INFERRED | OPEN | Original scheduling not authenticated; no invented simulation |
| RUN033-WINDOWS-XBOX | Native Windows installer / physical Xbox | REPORTED_UNREPRODUCED | NOT RUN | Only Linux Xvfb software-controller exercised |
| RUN033-FULL | Complete 1:1 gameplay/interface and Director acceptance | INFERRED | OPEN | No unverified release claim; unrestricted suite not green |

Independent full Drive raw readback SHA `76b9247f9d1f0e21d143352134e8eaf93fe21f3daf529b757460017f1684a95e`; 51 Drive-cold regressions PASS. The initial v1.00 register preserved as pre-publication historic snapshot.
