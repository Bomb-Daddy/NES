# ROTK II NES Classic Run014 Q09 — live successful individual Flee insertion (2026-11-06)

**Scope:** SUP-0016, canonical Google Drive `Retro Gaming/ROTK2_NES_Windows` only. Classic original observable gameplay parity, NOT source structure identity; Enhanced and DOS remain separate.

## Source ancestry and publication
- Immutable complete Run013 parent SHA-256: `28e4d7c0296621941b274ffcbc5d41651a1a76b0053ee45d35086fddb4b51c68` (158 CRC-clean ZIP members); Run013 itself is exact Run012 descendant.
- Full Run014 child SHA-256: `ddb6ee5693e6dce140f4e216d14d241e8c1239aa8ddfb66fd1b7257d46ade398` (4,323,594 bytes; **159 CRC-clean ZIP members**). Of 158 parent payloads, 157 unchanged; one production module changed; one regression module added. A second deterministic build gave the identical SHA.
- **Canonical Drive Run013 recovery:** previously blocked complete Run013 source + QA ZIP were uploaded and independently downloaded, SHA-256 and CRC passed. Folder: https://drive.google.com/drive/folders/1jFtB5U3mQPfiSRtSVxbwxAb8w-KpyBDj
- **Canonical Drive Run014 publication:** complete source and QA ZIP uploaded, independent raw-materialization SHA-256 and CRC passed. Folder: https://drive.google.com/drive/folders/1jvzzv6aczCSUjQIV5iVx8oO1ag-vcCnI
- Run014 QA ZIP SHA-256: `42d0e62b99c25b4f54df143bf19705e494edf9719372ab386ba2d47fdeb5a094`. Contains all 159-file manifest, red/green logs, source diff, test, build script, report, completion register. Original parents unchanged, rollback preserved.

## Original facts and changed observable behavior
VERIFIED_STATIC source: `hsovl2 $D648/$D679/$D69A/$D6A5`; `root $5154/$507C` in original SNES mechanics. Upon successful **individual** Flee, the officer is immediately inserted into the peaceful destination province while the tactical battle continues. Battle troop units divide by 100 for a peaceful province, and moving to an unowned empty province establishes owner/officer head. Non-ruler does not displace an existing garrison head. The source-metadata-gated Classic engine now executes these real-time effects rather than postponing them all to battle finalization.

## Executed tests
- Original untouched Run013 **RED 2 failures / 2 passes** on focused fixtures.
- Run014 **GREEN 7/7**, including actual disk `.ro2` save/cold-load, deterministic 7455-to-74 conversion, captured failed flee no province mutation, unknown field legacy fallback, cancellation atomicity, owned-head behavior, controller Action.DOWN/CONFIRM real command.
- Linux applicable broad **275 passed, 37 skipped, one deselected**, and *same outcome* from independently fresh extracted source ZIP.
- Xvfb **31 software-controller tests passed**; not a physical Xbox acceptance.
- Mutant that removed live source insertion **2 expected failures**; restoration recovered green.
- ROM-gated test excluded (original ROM env unset); historical Classic008 frozen inventory test deselected. No other full parity inferred.

## Evidence and outstanding work
- **VERIFIED_STATIC:** original successful individual Flee writer fact.
- **TESTED_PORT:** source-gated new live state behavior, negatives, controller-visible, cold-load.
- **OBSERVED_RUNTIME:** **NOT RUN** against original game in this patch.
- **INFERRED / OPEN:** source movement byte reachability and hydration into ordinary scenarios.
- **REPORTED_UNREPRODUCED / OPEN:** mid-battle failed-Flee opponent capture list, under-attack destination battle-scaled troops/list, whole-army serial commits, Enhanced normalized state/controlled RNG parity, original replay for this patch, native Windows, current physical Xbox, Q01–Q22 completeness and Director acceptance.

Next executable correction: source-derive `hsovl2 $D6AC–$D6C8` opponent prisoner-list and live hex unlink, add fail-before/green-after tests from exact Run014 parent (hash above), controlled UI/cancel and cold-load, and compare Enhanced normalized RNG. Do not promote Github working research as canonical source.
