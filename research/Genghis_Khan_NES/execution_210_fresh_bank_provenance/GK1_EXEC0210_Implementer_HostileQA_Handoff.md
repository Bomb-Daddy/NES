# GK1 EXEC0210 — Implementer and hostile QA handoff

2026-11-11 America/Detroit. Exact predecessor: **EXEC0209**, not EXEC0076 or EXEC0062. Existing full native C11 application retained. Change set is limited to `execution_199/gk165_fresh_json.c`, `execution_199/test_gk210_fresh_bank_provenance.c`, `execution_199/CMakeLists.txt`, and `execution_199/README_EXEC0210.md`.

## Source finding

The archived *fresh reset* original-USA-Mesen capture contains 14×83 real first Spring1175 countries at `nesWorkRam` physical `0x0DCD`. Its `nesMemory` CPU-address view agrees, while its `nesSaveRam` view is empty. The legacy capture script incorrectly placed SaveRAM-derived zeros in root `year/gold/rate/orders/countries`, despite correct named WorkRAM fields (1175, Gold430, Food208, Troops61, Rate121, orders3). The **other two archived-restored** original Mesen conditions use SaveRAM. They are not interchangeable. The fresh source sample is a single observed RNG/controller history.

## Implemented and integration

- Actual native game fresh source importer `gk165_extract_fresh_json()` now checks seven fields in all THREE memory views, requiring CPU `nesMemory` equals physical `nesWorkRam`, and the distinct blank SaveRAM seven fields all zero.
- Mixed or forged root metadata rejected while the known all-zero legacy capture and independently corrected WorkRAM-derived root are accepted.
- Caller output remains byte-identical on refusal, and the 14×83 source SHA identity gate is preserved; controller-based stock import into `gk166_enter_original_fresh_json` still exercises the same source-provenance check.
- Exact original ROM and complete owner-generated evidence stayed private. No public ROM, savestate, original country bytes, screenshots, audio or other licensed assets.

## Adversarial QA

Original archived fresh JSON input verified, then altered in memory during tests without publishing. The unmodified EXEC0209 importer **fails the new negative regression** (accepts a forged `nesMemory` Gold value with original country SHA intact); EXEC0210 rejects it, plus swapped/nonempty SaveRAM, forged original root year/gold/countries and WorkRAM rate. Corrected root aliases still import, and country digest remains valid. ASan/UBSan, Debug, Release and integrated native graphical smoke are tracked as distinct evidence. Earlier 14/27 stock imports and user country records remain unmodified.

**VERIFIED_STATIC** original source SHA and separate WorkRAM/SaveRAM provenance. **OBSERVED_RUNTIME_STOCK_FRESH_RESET** is the prior actual source capture, not newly executed original Mesen. **TESTED_PORT_LINUX** compiler/tests and native graphical frames. **NOT RUN** Win32, NSIS, physical Xbox, full campaigns. New original RNG chronological trace and independently created first playable Spring for all 14/27 countries remain open. Do not interpret this bank fix as those milestones.

## Reproduce

1. Extract exact EXEC0209 complete integrated source archive (public, copyright-clean).
2. `git apply GK1_EXEC0210_CopyrightClean_Integration.patch` from extracted archive root.
3. `cmake -S execution_199 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build -j8 && ctest --test-dir build -j8 --output-on-failure`.
4. Obtain the private original fresh evidence JSON from the owner's private Drive, verify original archive SHA and source ROM SHA. Run `build/test_gk210_fresh_bank_provenance` with original legacy and corrected JSON paths. Do not publicly redistribute private evidence.
5. Run the standalone existing native `test_gk166_fresh_entry` with verified original 41-country CSV and private fresh JSON; this confirms native world/source checkpoint import still succeeds and the source original 14-country data remains unchanged.

The existing recurring task remains enabled. Publication is not external agent read or execution acknowledgment.
