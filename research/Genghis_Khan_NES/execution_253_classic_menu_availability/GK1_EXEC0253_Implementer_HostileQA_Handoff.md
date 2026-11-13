# GK1 EXEC0253 — Source-integrated Classic controller command availability

**Director/implementer/hostile QA handoff · 2026-11-13**

Baseline: exact complete inherited EXEC0252 copyright-clean C11 source ZIP SHA256 `093e51087a80b26b880c5c22e22ce5394a8a95c9fbd1c5e1a964151aa077b340`. Latest baseline checked against Drive Collaboration and GitHub before implementation. Preserve all earlier history, work and private authenticated original-stock captures.

**Player-facing defect:** in the live 14-command original-source Spring menu Classic TAX was rendered LOCKED while its existing (manual-derived, NOT NES-parity-certified) command route really executes and costs one order. PASS displayed AVAILABLE after order exhaustion, and there was no discoverable instruction that a completed third action could still be undone before irreversible Pass.

**Real code:** `gk134_stock_command_route.c/h` now exports `gk134_menu_capability` as a pure 14-slot classifier; native 1280x720 shell calls it with the controller's source-authenticated remaining order count, and source-replayed `gk187_intact` actions/pass when valid. Source failure is represented as REFUSED. With three available Classic orders TAX displays SET RATE, ASSIGN displays MANUAL, and PASS displays PASS. With zero orders and three unpassed actions TAX/ASSIGN display UNDO ONLY (Y remains accessible), PASS ENDED. After Pass these show ENDED. Trade is explicitly PRICE VIEW, not a real merchant event. Every original-country RAM record remains immutable. This does NOT implement new economy, original first Spring RNG, seasons or gameplay beyond the existing commands.

**Exact reproduction:** Extract source ZIP; apply `patch -p1 < GK1_EXEC0253_Integration_From0252.patch` from extraction root; verify six source SHA256 against JSON; `cmake -S source/execution_199 -B build -DCMAKE_BUILD_TYPE=Release -DGK1_REQUIRE_ARCHIVED_WORLD_IMPORT=ON`; `cmake --build build -j8`; `ctest --test-dir build --output-on-failure -j8 --timeout 35`. Before applying patch, the new test target/function is absent; a deliberately corrupted baseline must reject patch.

**Verified:** native Linux 215/215 tests PASS; non-private 15-frame X11 smoke exit0; independent baseline-to-patch replay required before publication. The general smoke does NOT exercise the private-source command menu, so visual original checkpoint acceptance remains NOT RUN this increment. No original ROM or licensed assets are uploaded to GitHub.

**Still open:** independently generating authentic first Spring with all 14/27 original nations, correct original setup/ruler/difficulty/RNG and three genuine NES order effects, real merchant scheduling, AI, diplomacy, dynasty, battle end, sound/graphics, Win32 release, physical Xbox, full campaigns. DO NOT substitute a saved original checkpoint for independently generated gameplay. Keep recurrent execution enabled until Director stop/accepted complete release.
