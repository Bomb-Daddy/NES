# Run016 — Enhanced Q14 sibling reconciliation completion register, 2026-11-06

**Project**: Original KOEI Enhanced, not canonical English NES Classic and not DOS. Binding behavior: one-to-one player-observable English NES battle mode; Enhanced marketplace/economy/BP untouched.

**Selected direct immutable parent**: Run015A `cec4363f1434d235ace279d990a1a7b461224ea5f63a5abee2c752238fc598b5`, full 530-member ZIP, reconstructed from exact 18 Drive-published parts, SHA/CRC independently verified during this run.
**Reference sibling donor, NOT ancestor**: Run015B `0a438906a3245e1fe56a66ac6ecb4ba303f51fca39d9297faec43c2addb33ea6`, local 530-member ZIP SHA/CRC independently verified.
**New source candidate**: Run016 `f2234784d6eb6746d24bee9f5fed6172077a925ec8f4fcfd7a4176ee321732fa`, 148,736,614 bytes, 533 ZIP entries, 528 of 530 parent members unchanged. Two actual production modules modified, new regression and cross-branch Tk test, plus README.

| ID | Behavior / requirement | Evidence class | Status |
| --- | --- | --- | --- |
| ENH016-SRC-PIN | Run015A vs B correct shared Run014 ancestry; selected A and B donor | VERIFIED_STATIC | PASS SHA/CRC, separate trees preserved |
| ENH016-PROVENANCE | Free officer `prior_faction`, `province_id`, `source_rule`, and source-flag uncertainty retained without inventing NES flag | TESTED_PORT | PASS focused on real Campaign |
| ENH016-FREE-HEAD | Actual provincial ordered list head remains player-visible in recruiting choices | TESTED_PORT; original implementation source-backed | PASS, mutation detected |
| ENH016-FREE-STATUS | Real historical availability changes to Free/recruitable, returns Serving on Enhanced recruitment | TESTED_PORT | PASS |
| ENH016-PLAYER-TK | Real six-command Tk controller via Xvfb, prisoner B2 Release / B1 Behead | TESTED_PORT | PASS via sibling hostile real UI |
| ENH016-SAVE-COLD | Actual `.ro2` two independent Python-process save/load with identical campaign fingerprint | TESTED_PORT | PASS |
| ENH016-LEGACY | Prior A saves without new optional metadata read unchanged | TESTED_PORT | PASS |
| ENH016-B-SAVE | Run015B `free_officers` schema lacks unambiguous original province link-order information | VERIFIED_STATIC | BLOCKED compatibility, explicit `RUN015B_SCHEMA_UNMIGRATED`, source archives preserved |
| ENH016-Q14-SPECIAL | Positively identified special flag original root $4ED8–$4EE8 | VERIFIED_STATIC partial | BLOCKED original branch mapping; fail closed |
| ENH016-REGRESSION | Fail-before (new target on unmodified A), green-after, mutant | TESTED_PORT | 5 FAIL/1 PASS -> 6 PASS; mutant 1 expected FAIL/5 PASS |
| ENH016-BROAD | Selected battle/UI/strategic Linux Xvfb suite and cold-extracted source | TESTED_PORT | 280 PASS, 2 SKIPPED both environments |
| ENH016-UNFILTERED | Full unfiltered collection | TESTED_PORT | BLOCKED 2 existing collection errors: ROM env missing, `.rollback.py` invalid import; 729 tests collected |
| ENH016-NES-GOLDENS | 30 event-by-event original emulator battle traces | REPORTED_UNREPRODUCED | NOT RUN |
| ENH016-WINDOWS-XBOX | Genuine native Windows executable and physical Xbox | REPORTED_UNREPRODUCED | NOT RUN |
| ENH016-ECONOMY | Original Enhanced marketplace/BP code untouched (member-hash comparison) | VERIFIED_STATIC | PASS no source change; exhaustive economic integration acceptance OPEN |
| ENH016-ACCEPTANCE | Director one-to-one battle/gameplay approval | INFERRED | NOT VERIFIED / PARTIAL |

**Platform executed:** Linux Python 3.13.5, pytest under Xvfb software-controller UI; ZIP extracted/repacked in Linux; two separate Python processes saved/loaded actual `.ro2`. No native Windows or physical controller or English NES ROM events executed.

**Open executable next action:** Recreate original special-flag Q14 return from source root $4ED8–$4EE8 and obtain source-linked historical province HEAD order; or attack the source-backed Q09 attacked-destination second tactical battle slot independently. Do not claim fully original player behavior without live traces. Preserve original pre-merge sibling sources and register exactly; no DOS/Classic contamination.
