> This is the preserved source-batch explanation. Its original runtime targets and stage-specific status are resolved or explicitly bounded by the v1.07 master and final acceptance register; these source tests are not relabelled as Mesen playthroughs.

# Original Genghis Khan USA NES — scenario economic data

Author: Cao Cao / commodity_identity research task, 2026-11-04. Scope: SC07 country/economy data and CF04 regional specialty crosswalk. Classification: **VERIFIED_STATIC**, including execution of original CPU instructions in a transparent source harness. This task did not use Mesen or the Windows port. It additionally performs an independent byte-level comparison of the parent's OBSERVED_RUNTIME World first-turn capture; capture ownership and comparison provenance are distinguished below.

## Result and exact stage

All **41 country records** are resolved: 14 Mongol Conquest records and 27 World Conquest records, each with original country and ruler spelling, regional specialty, all ten specialty inventory words, Food, Gold, workforce and drafting fields. `scenario_economy_data.csv` and `.json` contain the complete data and per-record ROM/SRAM addresses.

These are the **original loaded templates before player/difficulty setup and seasonal processing**. The source dates are Winter 1174 and Winter 1205. They must not be labelled universal first-player-turn snapshots. Original country setup was separately replayed for Mongol Conquest and all four World player choices at difficulties 1, 3 and 5, with an explicit seed: all ten specialty inventory words remained unchanged in all 15 runs; Food, Gold, workforce and other values can change. Subsequent game activity may change inventories too. See `country_setup_replays.json`.

The existing Spring 1175 capture is already different: all 14 regional specialty bytes match the source, but three of the 140 specialty stock words differ (country 5 Woodworks 80→0; country 12 Woodworks 79→0; country 13 Fur 63→0), and all Food words differ. It is useful corroboration, not proof of universal starting inventories. See `existing_baseline_comparison.json`. The parent subsequently supplied a native World first-turn capture; its distinct values are preserved separately and compared below.

## Primary provenance and replay

ROM: private `Genghis Khan (USA).nes`, SHA256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`; 262160 bytes. The iNES header is 16 bytes; PRG has sixteen 16 KiB banks. All offsets below are CPU addresses unless labelled file offsets.

Original bank 0 native entry `$97D8`, VM body `$97DD` through boundary `$9851` selects bank 7 source `$8004` for scenario selector 1 or `$96B4` for selector 2. Original bank-copy syscall 16 copies `$15EA` bytes to `$63EA`, then copies common bank 7 `$AD74` to `$79D4` for `$0512` bytes (through `$7EE5`). The loader patches each country lord pointer at `$6E1E+83*i` to `$65BF+31*i`. The source CPU executed these original mapper calls and byte-copy loops, rather than substituting a high-level scenario decoder. The only hardware adapter is MMC1 five-write serial PRG-bank selection at `$FFF0`, using the startup 16 KiB bank-switch mode; it does not replace the original calculation or copy instructions.

`scenario_loader_replay.json` preserves VM trace, changed SRAM writes, mapper bank sequence and resulting hashes. Scenario 1 executed 81,291 CPU instructions; scenario 2 executed 91,181. Both bank sequences are 7→0→7→0. The tests independently compare every copied byte against the ROM, allowing only the original lord-pointer relocations, and check destination canaries.

| Data | SRAM layout | Independent original helper |
|---|---|---|
| Country name | `$63EE+17*i`, 17-byte record | Read from the copied source; `^` renders as a space |
| Country record | `$6DCD+83*i` | `$DBA7` native / `$DBAC..DBB4` VM |
| Country index | `(country_pointer−$6DCD)/83` | `$DBCD` native / `$DBD2..DBDA` VM |
| Regional specialty index | byte at `$7692+12*i` | `$DEF3` native / `$DEF8..DF04` VM |
| Inventory 0..10 | word at `country_pointer+$22+2*item_index` | `$DF05` native / `$DF0A..DF10` VM |
| Ruler | pointer at country `+$51`; initial ruler records `$65BF+31*i` | Original loader relocation |

For a scenario source `S`, a copied SRAM address `A` corresponds to file offset `16 + 7*16384 + (S + A − $63EA − $8000)`. Both JSON and CSV include the country, name and specialty offsets explicitly. Stock words are little endian. A zero is an actual original zero, not missing data. Regional specialty is its own byte: do not infer it from whichever stock happens to be largest. World Baghdad is **Textiles** although its initial Perfume stock is 77 and Textiles stock is 37.

## Original manual name crosswalk

The USA manual cover identifies **NES-GX USA**. Printed page 21 / archive PDF page 24 supplies the labelled merchandise icons. The crosswalk established by that chart and original item-layout replay is:

`0 Fur; 1 Silk; 2 Jewels; 3 Metals; 4 Medicines; 5 Woodworks; 6 Ceramics; 7 Glass; 8 Textiles; 9 Perfume; 10 Food; 11 Daggers; 12 Swords; 13 Axes; 14 Lances; 15 Pikes; 16 Bombs.`

Only 0..10 have these country inventory slots. Weapons use the arms purchase path. **Perfume is singular** in the original USA manual. Full evidence is in the sibling `commodity_identity/COMMODITY_IDENTITY_REPORT.md`, `commodity_crosswalk.csv`, and `test_identity.py` (12 passed tests).

Primary manual scan: https://archive.org/download/nes-manuals-no-header-help-screens/Genghis%20Khan%20%28USA%29%20%5B2225C20F%5D.pdf ; SHA256 `ecc2150b0ef43861b3590fa8d1671b99fe5f486894b1289836eb77265f49d0ae`, 43 PDF pages. Private source file: `batches/commodity_identity/Genghis_Khan_NES_USA_manual_archive.pdf`. Independent mirror: https://www.digitpress.com/library/manuals/nes/Genghis%20Khan.pdf ; SHA256 `f425f8cc008954c75bc7bebc7da4124c93e06bad84c0340478ebbc8b52d7b30d`.

## Country and regional specialty crosswalk

Country IDs here are the player's displayed 1-based IDs; machine indices are ID minus one. Ruler and country spelling follows original ROM text, with its embedded space markers decoded; modern geographical or historical spellings have not been substituted.

### Mongol Conquest

| ID | Original country | Initial lord | Specialty index | Specialty |
|---:|---|---|---:|---|
| 1 | the Mongols | Temujin | 0 | Fur |
| 2 | the Tatars | Tug Temur | 0 | Fur |
| 3 | the Ongirats | Daisetin | 0 | Fur |
| 4 | the Onguts | Alagush | 0 | Fur |
| 5 | the Taichiud | Targutai | 5 | Woodworks |
| 6 | the Jadarans | Jamukha | 0 | Fur |
| 7 | the Keraits | Tooril | 0 | Fur |
| 8 | the Merkits | Toktogha | 3 | Metals |
| 9 | the Zurkins | Sachu Beki | 0 | Fur |
| 10 | the Tumeds | Bistam | 2 | Jewels |
| 11 | the Oirats | Qudoga Beki | 0 | Fur |
| 12 | the Naimans | Tayang Qan | 5 | Woodworks |
| 13 | the Kirghiz | Chimsuyi | 0 | Fur |
| 14 | the Qarlugs | Arslan | 0 | Fur |

### World Conquest

| ID | Original country | Initial lord | Specialty index | Specialty |
|---:|---|---|---:|---|
| 1 | the Mongols | Genghis Khan | 0 | Fur |
| 2 | the Chin Empire | Hsuan Tsung | 1 | Silk |
| 3 | the Cumans | Basch Khan | 0 | Fur |
| 4 | Kiev Russia | Fuseboro | 0 | Fur |
| 5 | Teutonic Knights | Sachsen | 6 | Ceramics |
| 6 | Poland | Resska | 3 | Metals |
| 7 | England | Richard | 8 | Textiles |
| 8 | France | Phillip | 3 | Metals |
| 9 | the Holy Empire | Heinrich | 7 | Glass |
| 10 | Hungary | Endore | 8 | Textiles |
| 11 | Byzantin | Alexious | 3 | Metals |
| 12 | the Rum Seljuks | Qelkubud | 2 | Jewels |
| 13 | Khorazm | Muhammad | 7 | Glass |
| 14 | the Kara Khitai | Cherkes | 2 | Jewels |
| 15 | the Uighurs | Barjag | 9 | Perfume |
| 16 | Hsi-Hsia | Heng Tsung | 0 | Fur |
| 17 | To Fan | Sakiaba | 5 | Woodworks |
| 18 | Korea | Shen Tsung | 4 | Medicines |
| 19 | Japan | Yoritomo | 1 | Silk |
| 20 | Al Muwahhid | Sultan | 2 | Jewels |
| 21 | Ayyub | Saladin | 7 | Glass |
| 22 | Baghdad | Caliph | 8 | Textiles |
| 23 | Delhi | Balulala | 4 | Medicines |
| 24 | Ghore | Qori | 9 | Perfume |
| 25 | Pagan | Naraba Testu | 8 | Textiles |
| 26 | Nan-Sung | Ning Tsung | 6 | Ceramics |
| 27 | Dai-Viet & Ta-Li | Duan Tsung | 5 | Woodworks |

## World first-turn runtime capture comparison

The parent operated the original game through native setup and captured `batches/runtime/live/world_first_turn_step2.sram`, `.json` and `.png`: Spring 1206, World Conquest, Genghis Khan, difficulty 1, frame 8727. I independently inspected the screenshot and decoded every country record from the 8192-byte SRAM. SRAM SHA256 is `daf1ce18cd6fae4d0679c8afb2356f840e3cd6baf858a02de862cca077592e5e`, matching the capture metadata. Country count, year, season, player ID and difficulty were also read directly from SRAM. This is a comparison of parent-captured OBSERVED_RUNTIME evidence, not a new emulator run by this task.

All **27/27 country names**, **27/27 lord names** and **27/27 regional specialty IDs** match the original World template. **264/270 specialty stock words** match. The six differences are:

| Country ID | Country | Item | Winter 1205 loaded template | Spring 1206 first turn |
|---:|---|---|---:|---:|
| 8 | France | Metals | 93 | 0 |
| 10 | Hungary | Textiles | 62 | 0 |
| 12 | the Rum Seljuks | Jewels | 87 | 0 |
| 19 | Japan | Silk | 80 | 0 |
| 25 | Pagan | Textiles | 93 | 0 |
| 27 | Dai-Viet & Ta-Li | Woodworks | 150 | 0 |

All **27 Food words differ**. Genghis has Gold 193, Food 332, Troops 116, undrafted residents 106 and eligibility 32%; these independently decoded values agree with the parent metadata and visible main-screen quantities where displayed. No changed value is silently substituted into the original template. `world_first_turn_economy_data.csv` and `.json` preserve all 27 current country records, including all 11 stock words, economic fields, drafting fields, capture stage and hash. `world_first_turn_comparison.json` records every stock change and all other decoded field changes relative to the template.

`compare_world_capture.py` reproduced the comparison and passed its 27-country specialty/name and metadata crosschecks (complete `world_comparison.stdout`, empty stderr). This snapshot is specific to the selected player, difficulty and prior RNG/game history. Without intermediate state captures, the six zeroed stocks and other differences are not attributed individually to setup, seasonal processing, AI actions or events. Original templates and the captured first turn remain separate evidence stages.

## Drafted citizens, undrafted residents and eligibility

The original resident fields are distinct. Country `+$48` is an **unsigned byte percentage**; country `+$49` is an **unsigned 16-bit undrafted resident count**. For country 1 these are `$6E15` and `$6E16..6E17`. A former label treating `$6E15..6E16` as one word is wrong: it combines the percentage with the low byte of a different field.

Original helper `$DECD` / VM `$DED2..DEF2` adds exactly Troops `+$18`, Town `+$1C`, Masons `+$20`, Artisans `+$3A`, and Food workers `+$3E`. It excludes the undrafted word. This is the drafted workforce subtotal; the CSV `Combined_population` is the arithmetic sum of drafted and undrafted, included as an explicit computed quantity, not a claim that every UI or seasonal calculation uses it.

Draft entry bank 3 `$909D` / VM `$90A2..90C2` computes:

`eligible = min(floor(byte[country+$48] * word[country+$49] / 100), 999 − drafted_total)`.

The original instruction replay covers all 41 countries, the 255/256 resident boundary, the drafted total 998/999 boundary and a negative control proving that changing undrafted residents does not change `$DECD`. Loaded Mongol country 1 has 30%, 32 undrafted and 100 drafted; loaded World country 1 has 30%, 100 undrafted and 176 drafted. World Japan alone has initial eligibility 35%; the other 40 records have 30%.

Manual corroboration: printed page 17 / PDF page 20 describes Draft as recruiting from people not already drafted and says the influx dilutes Arms, Skill and Morale. Printed page 18 / PDF page 21 defines Move People as movement of drafted citizens among the five labor groups. Printed page 15 / PDF page 18 describes Train People increasing the percentage available for mobilization, up to 50%. These distinguish undrafted residents from the already-assigned workforce; they do not provide the exact integer formula, which comes from original code. Valid private page renders are `verified_manual_page-20.png` and `verified_manual_page-21.png`.

## Setup qualification

Original bank 0 VM `$9C52..9CF9` runs player/AI country adjustment. AI quantities use a difficulty factor `(79+7*difficulty)/100`, with integer flooring/caps; selected AI countries receive a further strength adjustment. Player Arms, Skill and Economy take separate difficulty/random reductions. The referenced setup body does not rewrite any of the ten specialty inventory words. Its Food and other state results are condition-labelled in `country_setup_replays.json`; they do not include all later initialization, seasonal transitions, AI actions or random events.

`replay_setup.py` tests difficulty 1/3/5 for Mongol player country 1 and World player countries 1/7/11/19, seed `$123456789ABC`. All 15 full original country-setup replays passed the inventory invariant. The seed and final RNG state, instruction counts, country Food/Gold/Arms/drafted/undrafted before and after, and SRAM hash are preserved for each run.

## Verification and limitations

Commands executed from the workspace:

```sh
PYTHONDONTWRITEBYTECODE=1 /usr/bin/python3 batches/scenario_economy_data/extract_scenarios.py
PYTHONDONTWRITEBYTECODE=1 /usr/bin/python3 batches/scenario_economy_data/test_scenarios.py
PYTHONDONTWRITEBYTECODE=1 /usr/bin/python3 batches/scenario_economy_data/replay_setup.py
```

Results: **13/13 tests passed**; **15/15 setup replay cases passed**. Source-ROM mutations prove the copied stock and regional-specialty lookups depend on the intended bytes. The entire loader copy is checked independently against source ROM offsets. `tests.system_python.stderr` contains the complete test run and OK conclusion; `setup.stdout` contains every setup condition and its final PASS count. The loader uses the sibling `seasonal_source/source_cpu.py` and `inspect_source.py` read-only; private ROM bytes are required to reproduce it. The baseline comparison additionally requires `evidence/roundtrip/baseline.sram`.

Manual printed page 9 / PDF page 12 verifies setup order: Start, New Game, scenario, player count, lord, abilities, difficulty. World allows up to four players. The verified private render is `world_setup_verified.png`. Exact default cursor positions are not documented there.

Failed/interrupted intermediate evidence is retained: the PATH `python` invocation returned success while `tests.stderr` ended midway through the run; it is not the accepted test result. Some PDF-render invocations left truncated images (`manual_page-20.png`, `manual_page-21.png`, `setup_manual-08.png`, `setup_manual-12.png`), including one multi-page system-renderer invocation. The exact cause is undetermined. Individual-page rerenders succeeded; only inspected, complete images support conclusions. The accepted test result is the complete system-Python log. Verified drafting page renders are `verified_manual_page-*`; verified setup render is `world_setup_verified.png`. No failed intermediate is silently treated as passing.

SC07 static data extraction, CF04 specialty identity and comparison against the supplied World first-turn capture are complete. Captured values from both scenario baselines are stage-qualified. Full harvest parity and attribution of each intervening setup/season/AI change belongs to the parent runtime/seasonal work and remains outside this task's executed evidence. This report does not promote source tests to emulator or Windows acceptance.
