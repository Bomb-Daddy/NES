# GK1 EXEC0264 — native application source change, reproducible handoff

**Date:** 2026-11-13. **Predecessor:** verified EXEC0263, whose complete inherited C11 source is the 1,026-member `GK1_EXEC0262_Complete_Inherited_C11_Source_QA.zip` nested in EXEC0263's complete publication ZIP. No EXEC0264+ was observed in the checked current Collaboration listing/GitHub commits before this work. This is the same 1280×720 Linux X11 / Windows WinMain-XInput native game, not a new prototype or an emulated ROM product.

## Implementation — directly linked to the existing app

`gk1_app_install_templates` previously replaced both 14/27-country loaded templates but retained the existing game's stock checkpoint, previously expended Classic/Enhanced orders, controller screen/cursors and pending owner-private file request. That created an impossible combination: a new scenario's canonical pre-setup templates paired with an old authenticated checkpoint and its partially committed port state. The new behavior validates *both* source scenarios first, creates a clean `Gk1App` via its existing initializer, preserves the selected Classic vs Enhanced profile, installs both canonical templates and commits the entire state atomically. Invalid source installations leave every byte of the prior state intact.

Changed existing files: `execution_199/gk1_app_state.c` and `execution_199/CMakeLists.txt`. New linked source test: `execution_199/test_gk264_scenario_restart.c`. Nothing touches the original stock/private ROM, source data, gameplay patches, or previously saved campaigns; only an explicit new template installation resets the in-memory game. Existing private save files are never deleted or overwritten.

## Reproduction and independent hostile controls

1. Unzip original EXEC0263 full publication and extract its embedded EXEC0262 complete source ZIP, preserving the original directory structure.
2. `git apply --check GK1_EXEC0264_CopyrightClean_Integration.patch` then `git apply ...` from that source root. Applying again fails, as intended. The full EXEC0264 archive contains the complete inherited source plus these changed/new C files, not a component-only stub.
3. `cmake -S execution_199 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release`, `cmake --build build -j8`, and `ctest --test-dir build --output-on-failure --timeout 35 -j4 -E '^GK1_257_CurrentWin32Workflow_RejectsStaleSourceAndFalsePass$'`.
4. `GK1_264_AtomicScenarioReinstallClearsSourceAndPortState` failed against the *unmodified inherited source* at `s->screen==GK1_SCREEN_TITLE`, proving the regression detects the old bug. After the source patch, it passed for both Classic and Enhanced profiles. The full applicable Linux suite passed **219/219**, including original prior tests and X11 graphics, with one separately documented inherited Windows-workflow gate excluded. Independently replayed exactly both changed CMake/C files and one added test file, and the second patch application was refused.

### Inherited CI failure that MUST NOT be concealed

The current `GK1_257_CurrentWin32Workflow_RejectsStaleSourceAndFalsePass` static guard FAILS against the current public `.github/workflows/gk1-native-windows.yml`: it demands `git apply --directory=current-gk1` and explicit patches `patch259`, `patch260`, while the current workflow uses `git -C current-gk1 apply` for *only the EXEC0257 patch* after extracting the SHA-pinned EXEC0256 lean source. This is a real version-provenance gap: that workflow does **not** prove it built EXEC0262/0263/0264 latest complete source. Do not relax the guard to obtain a cosmetic full green run. Fix the workflow to build a SHA-pinned *current* complete source or replay every verified later patch, then update the hostile gate. This does not retroactively invalidate the recorded 202/202 Win32 CI tests for the exact binary they actually compiled, but it limits their applicability.

## Acceptance and open work

- **VERIFIED_STATIC:** 1026 inherited members checked, only two intended existing-file changes and one added linked C regression; independent exact patch replay/repeat refusal and source hash manifest PASS.
- **TESTED_PORT_LINUX:** new native-linked test PASS, full applicable CTest 219/219, inherited 1280x720 X11 tests PASS. Original Win32 workflow static gate fails (excluded from applicable count), not hidden.
- **OBSERVED_RUNTIME_STOCK_FRESH_RESET** and **OBSERVED_RUNTIME_STOCK_ARCHIVED_RESTORED:** historical source evidence reused, no new Mesen execution or parity test.
- **NOT RUN:** EXEC0264 native Windows CI executable or NSIS, physical Xbox, full campaign.

**CRITICAL:** Independent, original-stock Spring setup/difficulty/character/RNG is still not implemented; authenticated-stock checkpoint import is not original setup generation. Original merchant presence/RNG, AI, battle aftermath, diplomacy/dynasty and whole-campaign acceptance remain open. Do not promote this source-integrity correction to finished-game status. Keep hourly work enabled until Director stop or genuine accepted complete release. ROM and licensed evidence stay privately in Genghis research, never public GitHub.
