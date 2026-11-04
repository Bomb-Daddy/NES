> This is the preserved source-batch explanation. Its original runtime targets and stage-specific status are resolved or explicitly bounded by the v1.07 master and final acceptance register; these source tests are not relabelled as Mesen playthroughs.

# Genghis Khan USA NES commodity identity crosswalk — v1.07 research batch

Date: 2026-11-04 UTC. Researcher: commodity_identity agent for Zhuge Liang. Scope: original USA NES only. No ROM, manual, or game art is authorized for public redistribution by this report.

**Result: PASS VERIFIED for the bounded name/icon/index/stock crosswalk.** Names come from a visually inspected original NES manual; indices and stock addressing are established by original-6502 instruction replay and matched to archived original-game screenshots. This does not complete every merchant transaction, weapon effect, availability rule, or broader economic obligation.

## Inputs and source authority

- Original English NES manual, cover code **NES-GX USA**, visually inspected at archive PDF page 2. Original title is Genghis Khan; no sequel or DOS substitution.
- Primary scan: https://archive.org/download/nes-manuals-no-header-help-screens/Genghis%20Khan%20%28USA%29%20%5B2225C20F%5D.pdf
- Primary scan mirror: https://www.digitpress.com/library/manuals/nes/Genghis%20Khan.pdf
- Archive PDF SHA-256 `ecc2150b0ef43861b3590fa8d1671b99fe5f486894b1289836eb77265f49d0ae`, 18,062,394 bytes, 43 PDF pages. Printed p21 is PDF page24, the Merchant’s List. File `manual_page-24.png` is the inspected chart. Printed page numbers equal PDF page number minus3 in this body section.
- Mirror PDF SHA-256 `f425f8cc008954c75bc7bebc7da4124c93e06bad84c0340478ebbc8b52d7b30d`, 1,700,525 bytes, 22 PDF pages. Archive copy used for visual findings because it has clearer single-page frames.
- Source ROM SHA-256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`. Source hashes, sizes, and exact input paths are in `INPUT_HASHES.json`.
- Existing v1.06 report and prior_master_v1.04 were read. Earlier icon guesses and filenames are superseded only where explicitly resolved below. No repository authority or runtime environment was changed.

## Crosswalk

Coordinates are quote-text tile coordinates in the 256×240 original screenshot; icon rectangles are immediately to their left. Stock words are little-endian 16-bit fields for country1 in the documented baseline. The chart supplies labels in column-major order.

| Index | Manual label | Menu column,row | Quote tile x,y | Country1 stock / Arms | Baseline stock | China / Islam / row2 base tenths |
|---:|---|---|---|---|---:|---|
| 0 | Fur | 1,1 | 14,9 | $6DEF | 73 | 10 / 4 / 8 |
| 1 | Silk | 1,2 | 14,11 | $6DF1 | 29 | 16 / 37 / 25 |
| 2 | Jewels | 1,3 | 14,13 | $6DF3 | 26 | 32 / 28 / 25 |
| 3 | Metals | 1,4 | 14,15 | $6DF5 | 16 | 18 / 22 / 10 |
| 4 | Medicines | 1,5 | 14,17 | $6DF7 | 20 | 14 / 8 / 15 |
| 5 | Woodworks | 1,6 | 14,19 | $6DF9 | 50 | 11 / 14 / 7 |
| 6 | Ceramics | 2,1 | 20,9 | $6DFB | 21 | 13 / 15 / 18 |
| 7 | Glass | 2,2 | 20,11 | $6DFD | 30 | 8 / 5 / 20 |
| 8 | Textiles | 2,3 | 20,13 | $6DFF | 41 | 20 / 15 / 31 |
| 9 | Perfume | 2,4 | 20,15 | $6E01 | 18 | 18 / 29 / 38 |
| 10 | Food | 2,5 | 20,17 | $6E03 | 208 | 10 / 11 / 11 |
| 11 | Daggers | 3,1 | 26,9 | N/A: shared Arms $6DE1 | — | 20 / 15 / 25 |
| 12 | Swords | 3,2 | 26,11 | N/A: shared Arms $6DE1 | — | 80 / 75 / 82 |
| 13 | Axes | 3,3 | 26,13 | N/A: shared Arms $6DE1 | — | 100 / 110 / 90 |
| 14 | Lances | 3,4 | 26,15 | N/A: shared Arms $6DE1 | — | 40 / 40 / 35 |
| 15 | Pikes | 3,5 | 26,17 | N/A: shared Arms $6DE1 | — | 60 / 70 / 65 |
| 16 | Bombs | 3,6 | 26,19 | N/A: shared Arms $6DE1 | — | 150 / 200 / 250 |

The manual label is **Perfume**, singular; the older notes’ “Perfumes” refers to the same item. Weapons share the aggregate Arms destination; there are no corresponding six sequential stock words in this purchase branch. Do not extend the stock formula to weapon indices: doing so would mislabel workforce and other country fields. Table row2 is intentionally left unnamed here; this work does not newly prove Uighur row identity.

## Proof chain and exact source locations

1. **MANUAL / visually inspected:** printed p21 labels each icon. The 17 icon shapes and positions match `evidence/roundtrip/buy_quotes.png`; the first11 also match `islam_sale_quotes.png`. Names are not inferred from visual resemblance alone and are not claimed to be stored as English strings in ROM.
2. **VERIFIED_STATIC, original instruction replay:** physical PRG bank3, buyer local-index→coordinate code `$816E` through boundary `$8192` produces all17 coordinates above. The same local index is pushed at `$819D` before the buyer quote call at `$819F` (`$DCE5` native / `$DCEA` VM). All17 inputs were replayed. The coordinate helper `$CC6F` stores the two arguments at `$7EEA/$7EEC`; v1.06 captured index2 and5 quote frames independently contain the corresponding13,14 and19,14 positions. This joins quote-table column, screen position, and manual icon label.
3. **VERIFIED_STATIC:** seller `$80FB` through boundary `$8111` produces the identical first11 positions. Seller quote call is bank3 `$811C` to `$DD01`. The layout computation is independent of merchant; merchant is passed separately to the quote routine. This proves shared item positions, not inventory availability at every merchant/season.
4. **VERIFIED_STATIC:** fixed-bank native `$DF05`, VM `$DF0A` through boundary `$DF10`, computes `countryBase + $22 + 2*itemIndex`. Replayed for all11 stock indices and three synthetic bases. Source bytes are checked against the ROM. Country1 base `$6DCD` maps index2 to the independently observed `$6DF3` and index5 to `$6DF9`, and maps Food/index10 to `$6E03`.
5. **VERIFIED_STATIC:** bank3 `$8CFB` branches at index11: indices0–10 reach stock path `$8D01`, while11–16 reach Arms path `$8D06`. The original address calculation `$8D1D` through boundary `$8D20` produces `countryBase+$14`, country1 `$6DE1`. This establishes routing/address identity, not the weapon quantity/Arms formula.
6. **OBSERVED_RUNTIME, archived evidence inspected; no new emulator run by this agent:** v1.06 China index2 purchase26→27 at `$6DF3` is now named **Jewels**. Islam resale27→26 is also Jewels. China index5 purchase50→51 at `$6DF9` is **Woodworks**. `food_quantity.png` actually selects the Woodworks icon; Food stock stays208. Existing `food_*` filenames remain unchanged for provenance. Islam seller row1/index10, quote1.9, is Food.

## Verification

Command from workspace root: `python batches/commodity_identity/test_identity.py`. Final result: **12 test methods, OK, exit0**. Raw stdout/stderr remain in this batch.

Coverage: all17 buyer positions; all11 seller positions; all11 stock fields; stock base dependence; the11/17 branch boundary; shared Arms destination; known archived purchases; exact ROM/manual hashes. Negative controls mutate the stock-offset operand and weapon-column coordinate operand and detect changed results. JSON records preserve bounded replay steps/VM states. This is synthetic CPU replay of authentic 6502 bytes, **not Mesen execution, TESTED_PORT, Windows validation, or physical-controller testing**.

## Original-manual economic references (MANUAL, not executed formulas)

- Printed p14: tax selection1–100; extra tax once/year. Ability costs: change tax Leadership5/Charm5; extra tax10/10; Assign Leadership5/Planning5.
- P15: Give costs Planning5; smaller populations gain more morale. Trade costs Judgment5.
- P12–13: town workers affect Economy, masons Defense, farmers food, artisans specialty output. Recruitment reduces morale; Arms decreases in winter and upon recruiting.
- P16: weapons raise Arms and cannot be resold; hiring lowers morale, Arms and Skill.
- P22: spring distributes food equal to half total population, collects gold taxes, and pays one gold per soldier. Shortfalls reduce population or troops. Fall food/specialty receipts depend on their labor groups. The page does **not** specify four-season averaging.

Inspected page PNGs: printed12–16=`manual_page-15.png` through`manual_page-19.png`; printed21=`manual_page-24.png`; printed22=`manual_page-25.png`. OCR files are search aids and contain recognition errors; visual inspection, not OCR output, supports the references. No exact economy formula or runtime bound is promoted solely from the manual.

## Batch outputs

- `commodity_crosswalk.csv` / `.json`: all17 names, quote coordinates, icon rectangles, stock address, baseline value, three existing base-price rows, source labels.
- `replay_identity.py`: original CPU replay using existing read-only v1.06 harness.
- `stock_address_replay.json`, `menu_layout_replay.json`, `seller_layout_replay.json`, `buyer_routing_replay.json`: machine-readable outputs.
- `test_identity.py`, `identity_tests.stdout`, `identity_tests.stderr`, `INPUT_HASHES.json`: reproducibility and checks.
- Original manual PDFs/page PNGs remain private reference evidence. No public release, shared Drive publication, or Mesen action was performed.

**Integration implication:** TD01’s existing v1.06 completed purchases can now be positively named Jewels and Woodworks. TD09’s static/manual name/icon/index mapping is closed at this bounded scope; merchant-by-merchant live stock/price acceptance remains a separate register criterion. All broader economic tests remain with the parent research owner.
