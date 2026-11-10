# GK1 EXEC0154 — explicit one-edge source interpreter diagnostic

**2026-11-10 | 1.54.0-dev1 | Copyright-clean C11 source**

This increment builds on the **complete inherited EXEC0153 native C11 game**; it does not replace the game, suppress PRE_SETUP, or manufacture an original first Spring. Classic stock NES and approved Enhanced profile remain separate.

## Actual implementation

`gk154_ppu_status_race_branch()` provides an **explicitly counterfactual** before/after selection for a *single* ambiguous PPUSTATUS read within the existing +/- two PPU-dot VBlank boundary. Its candidate-copy write is atomic: failed selections cannot modify timing state or output. The ordinary `gk103_ppu_status()` continues to fail closed.

`gk154_trace_original_one_race_branch()` and guarded `gk1_app_source_one_race_branch()` extend the inherited authentic-original 6502 source-interpreter with a **one-time** opt-in choice, then return to fail-closed operation on the next unresolved race. A successful selection records the source PC and branch; original country write-watch counters are retained. The opt-in command is:

```
gk1_native_shell --source-one-vblank-race PRIVATE_STOCK_ROM_PATH 0 1
gk1_native_shell --source-one-vblank-race PRIVATE_STOCK_ROM_PATH 0 2
```

Original ROM and Mesen samples are **private**. This is not Mesen execution and cannot determine the real NES edge timing or character/difficulty/RNG setup.

## Reproduction

From the complete historical ZIP (all inherited execution_* sibling folders retained):

```
cmake -S execution_154 -B build154 -DCMAKE_BUILD_TYPE=Release
cmake --build build154 -j8
ctest --test-dir build154 -j8 --output-on-failure
GK1_ORIGINAL_STOCK_ROM=/private/authenticated-stock.nes ctest --test-dir build154 -R 'GK1_154' -V
```

Tested Linux Release 112/112 PASS; Debug 112/112 PASS; focused ASan/UBSan public/private test 38/38 PASS; 1280x720 Xvfb 120-frame compiled program and extracted Debian native program each PASS. Independent clean ZIP 112/112 must be separately checked after packaging. Full ASan/UBSan suite NOT certified.

## Genuine bounded results

Original USA stock 262160-byte private ROM SHA256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`: inherited even-pre-render origin-0 source hypothesis normally halts at PC `$C5BE` with 16,323 executed instructions. When explicitly choosing single-race outcome 1, its interpreter progresses to **25,731 instructions** then hits another unresolved PPUSTATUS hardware gate at `$C5BE`; choosing outcome 2 progresses **17,221 instructions** before another gate. Both have zero source-executed country stores prior to the respective gate. These figures are **VERIFIED_STATIC under an UNVERIFIED timing hypothesis**, not actual stock Mesen/controller observations, not proof the game does no country updates.

Inherited **OBSERVED_RUNTIME_STOCK_FRESH_RESET**: original Mongol Spring1175 Gold430, Food208, Troops61, Rate121, three orders, WorkRAM physical `0x0DCD`. Distinct prior **OBSERVED_RUNTIME_STOCK_ARCHIVED_RESTORED**: Mongol1175 Gold439/Rate101, World1206 Gold193/Rate114, SaveRAM physical `0x0DCD`. Goods begin at country+`0x22`. Never hardcode one original run as universal RNG initial state.

## Acceptance remains open

Original source-authentic 14-/27-country first playable Spring, three genuine controller Classic orders, accurate setup/difficulty/character generation and RNG, merchant scheduling, AI, diplomacy, dynasty daughter ages 6/7/8, battle terminal/aftermath, original media, saved full campaigns and Win32 NSIS/physical Xbox-only testing all remain incomplete or NOT RUN. No original stock ROM, SRAM, images, audio or gameplay states may be published in copyright-clean archives.
