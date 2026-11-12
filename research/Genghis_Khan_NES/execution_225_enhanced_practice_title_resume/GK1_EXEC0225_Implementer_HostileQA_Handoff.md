# GK1 EXEC0225 — Integrated Enhanced Practice Title Cold-Resume Implementer and Hostile QA

Date: 2026-11-12. Native **Genghis Khan I NES-to-Windows** standalone C11 project, not ROTK II or the separate KOEI-inspired strategy project. Inherits exact verified EXEC0224 full copyright-clean source archive SHA256 `78fd9188b2cfd36645583507f0dee45a8e5320a11332bf7c8216dd58f122a363` from Collaboration. This is a distinct additive EXEC0225, not a new prototype.

## Player-facing implementation
Controller **X** during the existing Enhanced Spring practice creates an 80-byte source-bound non-overwriting `.gk24` save via inherited EXEC0224. After genuinely quitting the native application and starting a *new process*, controller **Y from TITLE** opens the chosen receipt in the Enhanced practice screen, validating recorded source-country template identity for all **14 Mongol / 27 World** countries against the installed canonical CSV, SHA-256 of all 80 bytes, and three-order action replay. The player can spend a remaining practice order and press B to return to the country-selection screen. Original Classic rules, actual NES Spring initialized game state, and all original 83-byte country data remain untouched.

Resume is forbidden when the original game world phase is no longer `PRE_SETUP_TEMPLATE`, Classic profile is selected, a conflicting research stage is active, the receipt is malformed or corrupted, or any of the 41 source country templates disagree. A missing Enhanced receipt leaves the previous Classic title Y behavior unchanged. A present-but-tampered receipt refuses without redirecting to a different save family. On failure the C11 application state remains byte-identical; a successful cold load is limited to a practice model (NOT a full campaign or original-stock emulator checkpoint). Neither recovered original ROM nor raw licensed Mesen content included.

## Source patch
Seven changed/new source paths, all SHA256 in `GK1_EXEC0225_Changed_Source_SHA256.json`. From the exact EXEC0224 complete ZIP root apply:

```sh
patch -p1 --batch --fuzz=0 < GK1_EXEC0225_CopyrightClean_Integration.patch
cmake -S execution_199 -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j8
ctest --test-dir build --output-on-failure -j8
```

Then independently rerun Debug, and `python3 execution_199/packaging/test_gk225_two_process_resume.py build/gk1_native_shell` (requires `xvfb-run`). Debian packaging: `cpack --config build/CPackConfig.cmake -G DEB`. All seven paths strictly replayed with exact SHA and all inherited source files intact.

## Executed QA and categorization
**VERIFIED_STATIC:** seven-path strict patch replay byte-identical, authentic original country CSV source checks, C11 atomic rollback, negative cases wrong profile, phase, altered nonplayer province, modified disk SHA, missing receipt, both 14/27. No source ROM or original images in public packages.

**TESTED_PORT_LINUX:** Release CTest **194/194 PASS**, Debug **194/194 PASS**, independent patched-source clean rebuild **194/194 PASS**; fully-instrumented changed test target + entire gk1_app_state linked library **ASan+UBSan PASS**; actual native 1280x720 Xvfb path **three distinct application processes** (write, TITLE Y reenter, altered disk rejection), 120 frames each; Debian built. All controller presses in QA are software-injected native pad events, NOT a physical Xbox.

**OBSERVED_RUNTIME_STOCK_FRESH** and **OBSERVED_RUNTIME_STOCK_ARCHIVED:** no new original Mesen execution. Retain previous authenticated original WorkRAM (fresh) and SaveRAM (archived) observations in the private Genghis research folder separately. Exactly matched MesenCE executable and core still not restored to this execution environment; no claim new source RNG parity.

**NOT RUN:** Win32 native executable, NSIS installer, physical Xbox controller, true Classic full Spring initialization/difficulty/RNG, real merchant scheduler/AI/diplomacy/dynasty/battle endings and full campaigns.

## Hostile QA gates
Attempt source mismatch in only country 27, truncated or extended 80-byte receipt, character/phase mismatch, partial in-memory setup before load, first-run missing slot fallback, slot symlink and exclusive permissions, forged SHA with impossible action replay, non-existent CSV, and repeated reload under real X11; ensure previous valid save is never overwritten. Native Win32/XInput and device test remain OPEN.

## Next actual work
Do not use this Enhanced-only practice feature to claim original playable first Spring. Restore the exact original-stock headless Mesen runtime, follow inherited EXEC0218/0221 original source-write recorder, recover authentic initial difficulty/ruler/RNG and seasonal store chronology; then implement it inside the inherited full C11 app, with three real controller orders and separate original fresh/archived parity tests. Preserve Classic versus Enhanced. Next real player features: Trade merchant scheduling, autonomous AI, war terminal/aftermath, dynasty/diplomacy, original graphics/sound, Windows installer and hardware Xbox. Continuous scheduled implementation remains enabled; publication does NOT count as developer read acknowledgement.
