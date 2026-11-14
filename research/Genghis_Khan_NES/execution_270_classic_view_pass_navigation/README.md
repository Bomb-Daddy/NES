# Genghis Khan I — EXEC0270
Source and executable QA increment to the **existing** 1280×720 native C11 game; not an independent new game or a complete release.

## Linked C11 change
In the authenticated original-stock Classic VIEW, pressing A arms Pass. Previously browsing Up/Down/Left/Right or switching the source-record/view mode left the confirmation armed, so a later A unexpectedly ended the port's remaining Spring orders. Fix: clear only `stock_checkpoint.pending_pass` on successful VIEW navigation/mode change, preserving original stock records and the original three-order checkpoint.

The exact production C11 change, CMake registration and full 121-line executable regression are in [the reproducible integration patch](./GK1_EXEC0270_Integration_From_0269.patch). [The linked test is also published separately](./test_gk270_view_pass_navigation.c). Apply to the **complete EXEC0269** source extraction with `patch --forward -p1 < GK1_EXEC0270_Integration_From_0269.patch`; a repeat application must refuse. This directory is a source delta, not the complete native game.

## Actual testing
* Baseline unmodified EXEC0269 with private original-stock fresh-reset WorkRAM Mesen JSON: **FAIL** on stale confirmation after country UP, assertion 54.
* Patched complete C11 game: **645/645** authentic-source linked controller assertions pass across original fresh Mongol WorkRAM, archived Mongol SaveRAM and archived World SaveRAM captures, loaded via production source-verifying importer (NOT simulated source flags). Includes Classic Tax then VIEW movement, B cancellation, explicit A/A Pass, original country/raw SHA/three orders intact.
* Linux Release full C11 game: compiles and links.
* Linux bounded applicable CTest: **220/220 PASS**, explicitly excluding the inherited historical GK1_257 Windows-provenance test and GK1_226 four-process X11 case (hung during a separate unfiltered run). Seven other graphical X11 tests passed.
* EXEC0270 Windows/NSIS, physical Xbox and full campaigns: **NOT RUN**.
* New original NES/Mesen execution or independent original first-Spring RNG/setup: **NOT RUN**.

The original ROM and gameplay evidence remain private. Historic fresh reset and archived-restored source profiles remain separate; no universal first-Spring Gold/Rate is assumed.

Full successor source/QA ZIP: **5,369,801 bytes**, SHA-256 `523d0a0075b3243f6681e35ed2f4f541dec42e0691d1b4d5bd05f469de5d149b`, 1,092 members; 1,076 of 1,078 inherited members byte-identical.

[Implementer/hostile-QA handoff on Drive](https://docs.google.com/document/d/1XFRUhDldQdVeeRVcouEIFczYnYptiWqS5xTbkI7hpSM/edit) is in the verified Collaboration folder. The source ZIP is available as a conversation artifact, not claimed uploaded to Drive; Google Drive binary transfer was blocked by its file-reference egress restriction.

Next primary executable priority remains true independent stock first Spring, setup/difficulty/character RNG, 14/27 country parity, then source-generated merchant availability, AI, combat aftermath, diplomacy, dynasty and full campaigns. Preserve Classic/Enhanced distinction and source provenance.
