# GK1 Cumulative Master v4.50 — EXEC0301 additive handoff

Retain prior v4.49 EXEC0300 original NMI source finding, all original-stock private ROM/evidence, fresh-versus-archived first-Spring histories, enhanced-profile split, 1280x720 native game, save states, full 14/27-country templates, 220/220 successful EXEC0299 Windows CI, and every predecessor source and test.

**Actual new Windows defect:** GitHub Actions run 43698862057 of 2026-11-15 failed *before compilation* with `EXEC0300 REFUSED: incorrect native predecessor or duplicate stage: CMakeLists.txt`. The EXEC0300 staging script compared against an exact Linux archive CMake hash, but the accumulated Win32 CI already contains legitimate additional CMake tests. This does not challenge EXEC0300 NMI opcodes or require redoing earlier gameplay research.

**EXEC0301 actual implementation:** `stage301.py` is the production, SHA-gated Win32-variant integrator for EXEC0300's authentic source. It pins inherited `gk1_app_state.c/h` originals, exact reviewed 0300 resulting hashes, and three original new C11 files; preserves existing CMake by one-time narrow library insert/test append and updates only the app SHA expected by the real Win32 source-provenance gate. Deliberately rejects tamper/duplicate/missing test/malformed CR without mutation. `test_stage301.py` provides 13 independent hostile tests (no skipped fixture). Retains previously released modules.

**Local verified:** native Linux C11 compile, targeted 4/4 and eligible 236/236 CTests PASS, 13/13 hostile Python tests PASS. Excluded one stale archived Win32 workflow test; workflow under GitHub is authoritative for actual Windows, not cached zip. **Win32 native outcome EXEC0301 NOT YET VERIFIED** before its own workflow completes. First authentic playable Spring still unfinished. Separate historical original-stock Mesen fresh and archived evidence retained unchanged; physical Xbox and full campaigns NOT RUN.

This is an additive record only. Publication does not prove agent execution/acknowledgment. Continue future development until accepted playable release or Director stop.
