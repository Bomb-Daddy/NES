# ROTK II Battle Fidelity Completion Register — Run012 v1.00 (2026-11-06)

Source hierarchy: SUP-0016 canonical NES Drive; latest immutable parent Run011 SHA-256 b62f8c22fad8122dc412a8ef6181da5cbb80511bf3a031ae91a6f02becd5eb56. New child SHA-256 5214b118fd350544fa677394233d3a0c1d1db7e9c8d8eec5c64e55a7bd63ad9e. Previous parents unmodified. Separate Enhanced and DOS projects untouched. Tested Linux port candidate only.

| Q/item | Classification | Status / evidence / limitations |
|---|---|---|
| Q09 destination head liege / officer-by-officer | VERIFIED_STATIC + TESTED_PORT | Inherited Run011/Run010. No full source-linked head hydration in ordinary campaigns. |
| Q09 escape $D582 source formula | VERIFIED_STATIC + TESTED_PORT | Original source addendum from published Q09 research; red Run011 6 fails/1 pass; 9 green test cases with controlled RNG. Original runtime fixture NOT RUN. |
| Q09 officer byte +11 movement | VERIFIED_STATIC + TESTED_PORT | Source-movement field stored separately from remaining user action MP; a second explicit test failed before this correction, then passed. Arbitrary high-byte BFBB signed arithmetic still open. |
| Q09 immediate flag $40 | VERIFIED_STATIC + TESTED_PORT | Immediate exit without RNG draws. Source field missing in ordinary campaigns => explicitly provisional fallback. |
| Q09 occupied-neighbor side slot bit 1 | VERIFIED_STATIC + TESTED_PORT | Opposite-slot pairs counted by hex neighbors, not allied slot. |
| Q09 controlled failure -> no destination, capture | VERIFIED_STATIC + TESTED_PORT | Source $D6AC branch represented for terminal losing-side withdrawal. Mid-battle per-side capture linked-list/owner/timing OPEN; don't claim full parity. |
| Q09 controller/UI | TESTED_PORT | 29 Xvfb software Action UI tests; no Windows physical controller test this run. |
| Q09 atomic validation / disk cold load | TESTED_PORT | Illegal map destination/source byte causes no state/RNG changes; .ro2 disk cold-load state hash passes. |
| Entire Classic battle Q01-Q22 | REPORTED_UNREPRODUCED / TESTED_PORT PARTIAL | Applicable broad Linux 264 passed/37 skipped/1 deselected; not original game 1:1 accepted. |
| Original game or source fixtures | VERIFIED_STATIC source; OBSERVED_RUNTIME NOT RUN | No live Mesen/ROM replay this run. Static SNES byte provenance inherited, not newly observed original runtime. |
| Enhanced equal normalized battle result | REPORTED_UNREPRODUCED | NOT TESTED this run, no changes to Enhanced. |
| Native Windows / physical Xbox / Director acceptance | REPORTED_UNREPRODUCED | NOT TESTED this run, not a release. |
| Run011 canonical Drive delivery | VERIFIED_STATIC + checksum readback | Successful new upload of Run011 source and QA in canonical folder, materialized Drive bytes and SHA-256 matched exactly. |
| Run012 canonical Drive delivery | Pending this run | Child ZIP and QA ZIP are immutable output; folder/hash/readback must be confirmed before promoting. |
