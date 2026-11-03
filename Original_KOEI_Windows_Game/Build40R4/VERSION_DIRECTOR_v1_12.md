# Original KOEI Third Branch — v1.12 Director Mechanics and Strategic Occupation Candidate

2026-10-30. Version `OG_1000_DIRECTOR_V2` implements the Oct 30 Director economic corrections in original game ONLY.

The source builds on the verified v1.11 package in original-game Drive. No synthetic launch factions: 15 source S1 rulers, 41 provinces, 352 officer catalog and 23 curated family links preserved. 1,000 point cultivation/wealth/flood progress maintained.

Implemented commands: `occupy_vacant`, bound preview/commit, revision, 8 BP proposed cost, 1 ration per 60 traveling soldiers, real commander/army move; player UI Orders and controller form, weighted AI decisions; old hostile battle route separate.

Implemented economic policy: civilians do not consume government food, existing soldier food remains; no added government cash sinks; no finite mineral deposits; per-province evidence flag gates new metal extraction. Unknown deposits suppress extraction, and the mine-location map is not proven. Marketplace buys no longer depend on merchant stock for V2; existing stocks are now price-pressure indicators, buyer gold and selling goods still required.

AI: twelve documented profiles, profile-weighted priorities, food-surplus suppresses cultivation, neutral occupation scored, historical player turn orchestrator uses this chooser for V2. 3 additional historical rulers use cautious comparison profile. NOTE: no 12 human concurrent player test.

Tests: 312 pytest pass, 2 expected failures, 3 subtests; GUI Xvfb 1280x720 real UI occupation + cold save; 120-month seed17 monthly saves and 1800 ruler records; independent Month1/61 replay and tamper rejection pass. 12 distinct AI action signatures. 21 neutrals occupied and 47 hostile attacks.

KNOWN LIMITATIONS: All 41 metal site designations are `None` (unknown). Site evidence must be researched and installed; current source does not simulate metal extraction except test fixtures. No actual Windows launch or physical controller, no multi-seed/ruler rotation. Gold and food totals per governed province changed modestly, not balanced beyond this test.

The full numerical report is an independent artifact in Drive, not part of this ZIP.
