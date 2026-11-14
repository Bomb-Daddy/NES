# GK1 EXEC0276 — Shared original stock setup-provenance admission
Date: 2026-11-14. Parent: EXEC0275 full native C11 app and cumulative master v4.24. This is Genghis Khan I English NES-to-Windows, not ROTK II or the new KOEI-inspired game.

## Executed production change
Previously, EXEC0275 refused to import the **one known authentic fresh 1175 WorkRAM** sample through `gk166_enter_original_fresh_json` following a mismatched native ruler/difficulty/ability wizard. However, other entry points (`gk162_enter_original_checkpoint`, `gk164_enter_original_mss`, and public verified-checkpoint commit) did not share that wizard-provenance check. A saved original archived 1175/1206 history could be presented as the result of an unrelated modern port setup, or direct original 83-byte import could bypass the fresh JSON admission gate.

EXEC0276 adds `gk162_original_setup_compatible()` in **production** gk162, reused by gk162 direct raw entry, gk164 archived Mesen entry, gk166 fresh JSON entry, and gk162 shared verified commit. Existing full application, default source CSV and prior C11 mechanics retained. For fresh original, the EXEC0275 observed setup choices remain unchanged. For archived original SaveRAM samples, which lack verified controller setup-choice provenance, an untouched title-screen wizard is allowed as checkpoint continuation, but altered/different wizard choices are refused. This is not original NES character/stat/RNG generation and not a new playable seasonal simulation. No private RAM/ROM included.

## Tests and hostile negative controls
- `cmake -S execution_199 -B build -DCMAKE_BUILD_TYPE=Release` then `cmake --build build -j 6` — complete native C11 game and tests compile/link, exit 0.
- `ctest --test-dir build -R 'GK1_276_|GK1_275_|GK1_164_|GK1_162_' --output-on-failure` — 4/4 PASS.
- New C11 binary `test_gk276_original_setup_admission` — **92 assertions**, two archived profiles + fresh, rejects mismatched setup and preserves entire `Gk1App` bit-for-bit on refused import.
- Full eligible suite `ctest --test-dir build -E 'GK1_257_CurrentWin32Workflow_RejectsStaleSourceAndFalsePass|GK1_226_.*' --output-on-failure --timeout 25 -j 6` — **222/222 PASS**. Three inherited test registrations excluded: obsolete 257 and 226 multiprocess X11 category. These are excluded, not passes.
- Predecessor EXEC0275 native C11 compiled with the same new regression harness (helper-admission checks removed only to make it link to old code): **FAIL exit 2**, previously unguarded archived `.mss` admission returned `GK164_SOURCE` rather than expected refusal `GK164_TEMPLATE`. `pred-negative.log` included.
- Exact patch replay `git apply --check && git apply` from the actual unmodified EXEC0275 archived source: 6/6 changed/new source files byte-identical to final files.
- Source-free test only: stock private Mesen `.mss` and live original ROM **NOT RUN** in EXEC0276; original fresh and archived stock observations from EXEC0076 remain historical, separate.

## Limitations and next implementation
Independent authentic first-Spring setup (NES ruler/difficulty/ability RNG, 14/27-country initialization, season, 3 live orders) remains OPEN. Controllers support source continuation with bounded VIEW/PASS; merchant RNG, AI, war ending, diplomacy, dynasty and complete campaigns remain OPEN. New native Win32 executable, installer and physical Xbox NOT RUN in EXEC0276. This patch is intended to be integrated into the live Windows workflow in the subsequent Windows-source acceptance stage; a Linux compile is not Win32 evidence.

## Publication gates
Preserve all inherited EXEC0275 files and existing verified private sources. Public source patch contains no ROM, screenshots, audio or archived raw country records. Publish complete source/tests/QA ZIP to correct Collaboration Drive when transfer supports it; publication is not an agent read/execute acknowledgement. Do not mark full release accepted. Keep recurring execution active absent Director stop or verified complete release.


## Post-publication native Windows CI acceptance (verified Nov 14, 2026)

- Live Windows workflow commit `e8707a3edc3c2d73fe7c746664a33f29e51dda0c`: https://github.com/Bomb-Daddy/NES/actions/runs/43579831781 — **completed/success**.
- Exact EXEC0276 source SHA-pinned stage, Visual Studio x64 C11 game compilation, linked `GK1_276_OriginalStockArchivedAndFreshWizardProvenance` CTest, all **209/209 Windows CTests**, WinMain software-controller/paint acceptance, NSIS package installation and installed-game launch all passed.
- The GitHub QA-only workflow ZIP (not a release) SHA256 `f084e214d47aaedcebbb91d1f21cc236d7e708cfa3902c1ac0b15e6aa3d711d1`, Windows installer SHA256 `4b64aeb1f8109cc5fca520e52c13c1542e095fd1682dfc522252c9685f78c483`. Both are uploaded to the verified Collaboration Google Drive, and independently downloaded byte-identical.
- Complete inherited public source ZIP (not ROM): https://drive.google.com/file/d/1x3BBeBElyu0_i4D3D-omXdRzG3eGkMJQ/view — 5,420,038 bytes SHA256 `2c78a73583f061fc8be2d4c567aa4b0fc68a785cce26d13acf9a3416f20e9c36`.
- Final acceptance receipt: https://drive.google.com/file/d/16FyZq4Nm9WLQQ58LFVe6EpLS8b40iB5h/view ; cumulative v4.25.1 https://drive.google.com/file/d/1MzB4kVlOlS_EzC0YSgYznc_up3clzTK_/view ; ledger v1.1 https://drive.google.com/file/d/1sbiaIx6d1biyLAWbRWfkzbeZ5gICu7Br/view .
- Still NOT RUN: physical Xbox, complete campaigns, original NES authentic independent first-Spring RNG/character/difficulty initialization. No completed-game release.
