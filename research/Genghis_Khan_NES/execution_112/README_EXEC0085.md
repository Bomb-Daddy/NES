# EXEC0085 — Preserve all 83 original stock country-record bytes and controller inspect mode

Version: **0.85.0-dev1**. This is a continuation of the complete EXEC0084 native standalone C11 app and all earlier source files, not a new prototype or a substitute for original game startup.

## Integrated implementation

The SHA-256-verified original stock `gk77_load` now preserves the *entire exact 83-byte private record* for each of all 14 Mongol or 27 World countries, rather than immediately discarding bytes not among the numeric field projections. The source-bound `Gk79Checkpoint` copies these bytes into *private native process memory* and computes a separate 32-byte SHA-256 of the 14x83 or 27x83 array. Both single-source converter and full checkpoint remain gated by the existing original profile and source-identity constraints. Any change in the stored raw bytes is detected before a Pass commit or full checkpoint save/load. The complete raw bytes are **not embedded in the public executable, public source, public test fixtures or published QA files**.

Within the existing source-bound stock Spring checkpoint, the physical controller **VIEW** button toggles two screens for the selected country: the existing decoded economic fields and a full read-only hexadecimal inspection of the 83 original source bytes. DPAD changes which country is selected; A continues to mean the existing two-confirmation Pass; B cancels Pass as before. VIEW and country selection do not advance a season, change a country or fabricate NPC actions. The hexadecimal view renders six bounded rows within the existing native **1280x720** surface. It uses the same Win32-XInput/Linux-X11 native source, no browser window and no scrolling.

All earlier original stock captures (fresh 1175 nesWorkRam, archived 1175/1206 nesSaveRam) remain distinct. **The original-stock ROM itself is not redistributed**. No new original Mesen run is claimed. The existing Gk80 save format remains byte-for-byte compatible: it stores the limited numeric snapshot and reattaches original source on cold load; it does not serialize the raw NES bytes into save files. This improvement is prerequisite preservation of original evidence, not an actual first-Spring setup/RNG implementation, real Trade, AI, diplomacy, dynasty, or completed game.

## Build and test

From the complete extracted ZIP root:

```sh
cmake -S execution_085 -B build85 -DCMAKE_BUILD_TYPE=Release
cmake --build build85 -j8
ctest --test-dir build85 --output-on-failure
```

Only if the owner supplies the original private ROM can the existing authenticated 6502 tests run genuine source checks:

```sh
GK1_ORIGINAL_STOCK_ROM=/private/exact_stock.nes ctest --test-dir build85 --output-on-failure
```

The new `GK1_085_FullOriginal83ByteSourcePreservedAndControllerViewRollback` test always verifies synthetic 14/27 records, all 83 bytes per country, source integrity, controller VIEW state, Pass, cancellation and corruption rollback. A *separate optional* private-original positive path requires `GK85_PRIVATE_FRESH`, `GK85_PRIVATE_MONGOL`, `GK85_PRIVATE_WORLD` pointing to independently SHA-verified raw exports. Without them it does **not** claim original RAM readback acceptance. Public code contains no raw stock RAM.

**Outstanding release gates:** source-authentic first playable Spring initialization and its original RNG; actual 3-order economic/war strategic scheduling; merchant presence/Rate RNG; autonomous AI; dynastic marriage age6/7/8 original-ROM test; battle endings and aftermath; original visuals/audio; both complete campaigns; permanent real-game save/cold load; native Windows package test and physical Xbox acceptance.
