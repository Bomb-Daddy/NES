# Run006 Enhanced Q09 per-officer withdraw destinations — tested source candidate (2026-11-06)

Binding: SUP-OG-ENH-BPF-20261106-057. Separate Original-KOEI Enhanced game only. Preserve original strategic marketplace/economy/BP; battle must match NES observable gameplay. NES Classic Run014 source is untouched.

Verified immutable real Enhanced parent: 18-part Drive-published Run005 (original-game folder 1srLZtHslUMwClNXbgq3ZtLEAlErVzGbA), SHA256 fc747de9b10a7e14683acdde416197eb413b64c5abd3b044fb2fb130cf490fce (518 CRC-clean entries). Independently fetched 18/18 parts, reassembled exact SHA/size/CRC, and rebuilt from verified Run004 SHA 7b2891d3c7d7b6ef371b2e06926fce59bda1b0f480160ccb8fc32c9e286ec229 plus original Q03 diff. Earlier unpublished Run005 b550f12d... packaging is NOT the exact parent used here.

New tested Run006 complete local source archive: SHA256 5e169eb5e0d3f7ababc65b0696cd461a9f9fe7daafdb31fa3cb2b4a36e0a0281, 148706622 bytes, 519 CRC clean members, 517 unchanged; only third_branch/officer_battle_adapter.py modified and tests/test_bpf_q09_enhanced_per_officer_destinations.py added. This branch contains the exact source diff and test, not a claim of merged Windows release or Drive full-binary publication.

Original source fixture: hsovl2 $D89D loops officers and selects destination independently. Before: whole army forces same retreat destination even when different legal neighbor selected. After: optional complete officer_destinations mapping for all fielded officers validated *before any mutation*, commander's root selection stays consistent, each actual officer receives its individual home. Undeployed pending reserves return only to original verified home, independent of the chosen field retreat. Omitting mapping is backward-compatible legacy behavior, not the complete NES UI. Escape odds/capture and live peaceful/attacked insertion remain unimplemented. No Enhanced economy changes.

Test results, Linux + Xvfb only:
- Before on pristine Run005: 9 failed, 1 passed.
- Current Q09 + pending-home suites: 16 passed.
- Full Run006 fresh extraction: 229 passed, 2 skipped selected broad combat/army/GUI cases.
- Negative mutant ignored per-officer selection: expected failure observed, then restored.
- Actual game campaign save_bytes/load_bytes, byte-exact rejection rollback and army conservation tested.
- Broader full suite attempt timed out after 200 seconds (~78%); ROM-private module excluded; DO NOT CLAIM FULL TEST SUITE PASS.
- Original emulator fresh replay, native Windows, current-build physical Xbox, NES six-command battle UI and 30 event traces NOT RUN / OPEN.

Completion status: Enhanced Q03 resident defenders = TESTED_PORT PARTIAL; Run005 18 part Drive publication = VERIFIED_STATIC, source bytes delivered. Enhanced Run006 individual destination rule = VERIFIED_STATIC source fixture + TESTED_PORT PARTIAL. Q09 escape/capture, six-command interface, tactical AI, full battle parity, native Windows/Xbox = OPEN. Current NES Classic Run014 remains immutable (SHA ddb6ee5693e6dce140f4e216d14d241e8c1239aa8ddfb66fd1b7257d46ade398).

Publication blocker: local full Run006 ZIP and QA package are available to conversation; canonical Drive binary writing attempts returned container_session_unavailable and BLOCKED_FILE_REFERENCE. Google Drive Run006 project folder ID 1nrTlk-aGDH84lQX3taIerD0ygESAoIYw exists; NO FULL SOURCE CLAIM until 18-part upload and independent reassembly. Resume by uploading complete ZIP/18 parts and verifying Drive hash; then implement original per-officer successful/failed escape and capture-list semantics from hsovl2 D648-D6C8 in Enhanced Run006, without copying NES Classic source tree.
