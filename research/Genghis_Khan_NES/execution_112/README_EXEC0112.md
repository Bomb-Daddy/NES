# GK1 EXEC0112 — authenticated all-field first-Spring setup-delta crosswalk

Same complete inherited native C11 application as EXEC0111; **research build, not completed game**.

`gk112_stock_setup_deltas.c/.h` compares the 24 source-backed numeric original-stock Spring fields (13 country stats/workforce values plus 10 goods and Food) against the **exact SHA-verified all-41-country pre-setup baseline** for every country in either campaign. Historical captures are supplied privately by the owner. An explicit second function compares the original **fresh-reset Mongol 1175** and **archived-restored Mongol 1175** paths field by field, without conflating the histories or fabricating a common RNG formula.

The native controller checkpoint viewer now cycles DECODED → RAW → verified FINANCE residual → 24-FIELD SETUP/SEASON residual. Each profile and every original country is displayed without scrolling. The viewer uses the current authenticated `Gk79Checkpoint`, never the reconstructed scratch copy or a synthetic snapshot. Every underlying numeric source remains unmodified. No first-Spring generation, autonomous player orders, merchant presence or AI transition is claimed.

Test native source with `cmake -S execution_112 -B build112 -DCMAKE_BUILD_TYPE=Release && cmake --build build112 -j8 && ctest --test-dir build112 --output-on-failure` from the extracted complete historical archive. To run additional **private** authentic 3-case positive tests, call `test_gk112_stock_setup_deltas VERIFIED_CSV FRESH_14x83 ARCHIVED_MONGOL_14x83 ARCHIVED_WORLD_27x83`. Do not publish those three original proprietary binary records, ROM, screenshots, Mesen states or source RAM. Test includes corruption of a nonselected country, wrong-profile rejection, false phase/order prevention, original app integration and byte-identical rollback on failure.

The deltas are combined setup/difficulty/player/RNG/season effects: **not independently generated first Spring or ROM formulas**. Both genuine histories and each of 14/27 source country identities must remain distinct. Exact manual audit v2.01 and all older verified gameplay contracts are unchanged.
