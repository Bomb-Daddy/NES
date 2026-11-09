# EXEC0112 — All-field ORIGINAL-stock setup/season delta crosswalk; complete native C11 app

**2026-11-08 America/Detroit.** Complete inherited `execution_111` carried forward intact as `execution_112`, version `1.12.0-dev1`; same 1280x720 X11/Win32 target, controller interface, two campaigns, Classic/Enhanced, country source identity, turn/Pass, Trade reference, research modules and existing save isolation. This is NOT the completed Windows game.

## Actual source change

`gk112_stock_setup_deltas.c/.h` extracts and compares all **24 known numeric** source-bound country fields for 14 Mongol/27 World countries: Arms, Skill, Troops, Economy, Town, Defense, Masons, Artisans, Food Workers, Gold, Morale, Tax, Undrafted, ten specialty goods and Food. It requires exact SHA of original 41-country CSV, unmodified PRE_SETUP templates, and one of THREE original-stock SHA-bound 14x83/27x83 private captures, checked using the established full-world `gk111` gates. Returns per-country signed deltas and per-field signed/absolute totals, changes/increases/decreases; all operations read-only, no partial output on error. An explicit second API compares the two genuine **distinct** Mongol Spring 1175 histories (fresh WorkRAM vs archived SaveRAM) without treating either as a universal RNG constant. It rejects the wrong profile, tampered nonselected country, altered template, and unverifiable evidence.

The integrated native X11 controller **View** screen now cycles decoded original data, 83-byte original raw record, finance residual, and full 24-field setup/season delta panel. Every country in each scenario remains browseable via the controller; no scrolling, no arbitrary merchant availability, no automatic Spring initialization. This is source evidence visualization, not game mechanic completion.

## Verified results

- `VERIFIED_STATIC`: exact source digest and known three original stock record SHAs preserved; paired genuine original Spring 1175 cases exhibit **93 nonzero country/field comparisons across the 14x24 matrix**. These are differences from player choice/RNG/season history and not a solved formula.
- `TESTED_PORT_LINUX`: full Release 71/71 CTest, Debug 71/71, ASan+UBSan 71/71; clean independent historical source rebuild required for publication; original three private capture test 3/3 in Release/sanitizer; real Xvfb 1280x720 controller screen 120/120 frames for each of three original profiles.
- `OBSERVED_RUNTIME_STOCK`: archived source evidence recorded previously during exact original Mesen runs, fresh 1175 and archived 1175/1206 remain separately identified. **Mesen was NOT rerun in this execution**; Xvfb is native Linux port only.
- `NOT RUN`: native Win32 executable, NSIS installer, physical Xbox, first-Spring RNG source parity, 12 strategic commands, full Trade merchant scheduler, autonomous AI/diplomacy/dynasty/battle end, whole campaigns.

Source positive test (PRIVATE ORIGINAL RECORDS NEVER PUBLIC):
`test_gk112_stock_setup_deltas VERIFIED_ORIGINAL_CSV FRESH_14x83 ARCHIVE_MONGOL_14x83 ARCHIVE_WORLD_27x83`

Public test:
`cmake -S execution_112 -B build112 -DCMAKE_BUILD_TYPE=Release && cmake --build build112 -j8 && ctest --test-dir build112 -j8 --output-on-failure`

No original ROM, raw 83-byte captures, screenshots, RAM, old save-states, original art or music in GitHub or public archives. Original evidence is exclusively in user-private research Drive folder `1mJ72ezJb9kzIGUHHXPTh3lXtiVxWjEL7`.

## Next real release gate

Derive exact original stock initial player/difficulty/automatic attributes and country RNG transformation with ROM code tracing and controlled original Mesen inputs; implement independent authentic source-generated first Spring and three playable orders in EXISTING native C11 game. Only then connect player command Trade to real merchant presence/offers and consume orders, then AI, diplomacy, dynasty, battle, campaigns, and Windows/Xbox full-run acceptance. Never promote archived research View/Pass or partial finance/field delta into authentic independently generated Spring status.
