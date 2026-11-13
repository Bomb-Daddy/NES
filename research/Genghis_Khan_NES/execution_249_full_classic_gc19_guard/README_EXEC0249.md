# EXEC0249 full C11 integrated handoff and hostile QA — 2026-11-13

## Inheritance and reproduction

The complete inherited **EXEC0248** source ZIP was retrieved from verified Collaboration Drive (file ID `13x0L3P54VODShO9Axi7_J8Was9uQQNC5`), byte size `3591695`. Its three-action original-stock Classic controller tests, historical research, 1280×720 shell, original 14/27 PRE_SETUP and authentic owner-source checkpoint paths were all retained. This is not a replacement project.

Extract complete 0248 ZIP into a working folder, `cd source`, apply `patch -p1 < GK1_EXEC0249_Integration_From0248.patch`. SHA-check all five modified/new files against `GK1_EXEC0249_Changed_Source_SHA256.json`. Build real `execution_199` CMake game in Release and run `ctest -j12 --timeout 40 --output-on-failure`. Running this exact sequence on Linux resulted in 212/212 full pass, which includes the prior new 0248 integrated three-order CTest. The five changed/new hashes matched byte-identically in an independent clean patch replay.

## Actual code

`execution_199/gk189_classic_cold_save.c/.h`: new `gk249_classic_cold_file_guard` validates the *opened file descriptor*, not only a filename; calls existing `gk247_receipt_file_guard` for 14-country 1258B and World 27-country 2337B exact sizes; Windows `_sopen_s` now `_SH_DENYWR` rather than `_SH_DENYNO`; early errors close file and do not alter state. Legacy GC18 and original SHA validation are unchanged.

`execution_199/test_gk249_full_classic_cold_file_guard.c`: data-free real C11 CTest for exact 14/27 bytes, empty/intermediate/extra-byte sizes, invalid handles, and Linux perms/hardlink/owner hostile controls (26 assertions on current Linux runner). This does not pretend to prove original first Spring or Windows API runtime behavior.

`execution_199/test_gk189_classic_cold_save.c`: correct positive-test Win32 temp filename generation; guarded Win32 hard-link original private checkpoint test if the filesystem supports hard links. No original RAM embedded.

`execution_199/CMakeLists.txt`: registers linked test 249 on the existing app library, retaining original test248.

## Acceptance, boundaries and next

VERIFIED_STATIC 5/5 clean replay SHA, source ZIP copyright-clean. TESTED_PORT_LINUX Release 212/212, new guard test 26 checks. TESTED_PORT_WIN32 new code NOT RUN (historical 0243/0244 CI does not count). OBSERVED_RUNTIME_STOCK fresh WorkRAM historical only and archived SaveRAM historical only. Full campaigns and physical Xbox NOT RUN. The recovered original ROM is private and must never be published. Next implement original first Spring/ruler/difficulty/seed RNG and full strategic commands, not another reference-only shell.
