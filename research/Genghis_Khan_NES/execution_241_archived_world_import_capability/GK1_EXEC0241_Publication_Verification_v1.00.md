# GK1 EXEC0241 — post-publication verification receipt — 2026-11-13

Execution built on the current full EXEC0240 native C11 source, 1280×720 WinMain/X11 game. The previous original-stock-ROM-missing blocker remains FALSE: exact ROM private, authenticated historical evidence preserved separately. This increment does not independently generate first playable Spring, and is not a complete game.

## Implemented and actually tested

- Native application `gk1_native_shell.c` now reports a truthful archived-original World checkpoint import refusal when this specific game executable lacks zlib, instead of suggesting valid private Mesen evidence itself is invalid; no source records are mutated on rejection.
- Native `gk241_archived_import_capability.c/.h` capability enum and diagnostic integrated into `gk1_app_state` through actual CMake source, not a standalone mock.
- Explicit `GK1_REQUIRE_ARCHIVED_WORLD_IMPORT=ON` fails at configure when zlib is disabled and configures with zlib enabled; old no-zlib research build remains possible with source import explicitly unavailable.
- New C11 regression guards 14/27 country identities, conflicting archive/fresh source imports, valid static source handling and side-effect-free refusal.
- TESTED_PORT_LINUX: strict Release zlib 207/207 CTests passed; no-zlib 204/204 applicable CTests passed, three existing DEFLATE-dependent tests deliberately disabled. Independent exact patch replay of 5 modified/new source files succeeded and 3/3 focused rebuilt CTests passed.
- Direct actual Linux X11 1280×720 application launched under Xvfb `--smoke-frames 15`, exit 0 (first wrong CLI usage `--smoke-frames=15` exited 4 and was corrected; not a gameplay success). The application explicitly reported `stock_spring=no stock_checkpoint_pass=no order_budget=no battle_pass=no`.

## Publication verification

- Downloadable complete copyright-clean inherited source+QA archive: `GK1_EXEC0241_Complete_Inherited_C11_Source_Windows_Capability_QA.zip`, 16,624,997 bytes, SHA256 `14099a1bfa8456afcc41080e801663df3aa85760f3afe46594a4997e04339a20`, Drive ID `1fyvdLV14zlC0-zlhnCT9a-9jk4xkZ695`. Complete ZIP and 4 other principal files independently materialized from Collaboration Drive and compared byte-identically with originals: 5/5 PASS.
- Public GitHub source+patch+QA manifest+README: `research/Genghis_Khan_NES/execution_241_archived_world_import_capability/`, 6/6 independent Git blob SHA-1 matches local exact files; latest commit `afd7e77016d2bf7508eb3f6fab412546862abfac`. No original ROM, `.mss`, SRAM or screenshots committed.
- Cumulative Master v3.95 verified to contain all 504,125 original bytes of v3.94 intact at its end; no historical facts overwritten.
- Private original ROM separately verified in Drive MIME/application/octet-stream, 262,160 bytes in correct Genghis original research folder; not included in public archive.

## Status gates

VERIFIED_STATIC PASS for exact inherited source, source regression, GitHub object hashes and Drive readback. TESTED_PORT_LINUX PASS as above. OBSERVED_RUNTIME_STOCK fresh WorkRAM and archived SaveRAM refer only to historical earlier authenticated original Mesen records; NO NEW NES EMULATION THIS EXECUTION. TESTED_PORT_WIN32 for EXEC0241 NOT RUN; historic EXEC0236 CI cannot be applied to this newer build. Physical Xbox and full campaigns NOT RUN. Independent authentic Spring initialization/ruler/difficulty RNG and original three executable orders remain OPEN. Do not stop hourly until Director explicit stop or complete accepted release; publication is not agent acknowledgement.
