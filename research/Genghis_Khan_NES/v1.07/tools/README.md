# Genghis Khan NES economic source-test package

This package reproduces the research team's bounded original-instruction tests and explicitly included archived-evidence checks. It contains authored Python, numeric research data and input fingerprints. Supply your private ROM, SRAM captures and manual externally. No ROM, SRAM capture, PDF, image or game asset is bundled.

Use Python3.12 or another compatible standard-library Python. No pip packages or network access are required. Validation here used `/usr/bin/python3`3.12.3. Every suite runs in a separate process to prevent module-name collisions.

## Inputs

Set absolute paths:

```bash
export GK_ROM='/private/Genghis Khan (USA).nes'
export GK_SRAM='/private/v106/evidence/roundtrip/baseline.sram'
export GK_EVIDENCE='/private/v106/evidence'
export GK_ARCHIVED_CAPTURES='/private/v102/Genghis_Khan_NES_Research_v1.02/captures'
export GK_MANUAL='/private/Genghis_Khan_NES_USA_manual_archive.pdf'
export GK_ROI_EVIDENCE='/private/current-year-evidence/live'
python3 run_package.py --output ./results
```

`PRIVATE_INPUT_REQUIREMENTS.json` gives each expected relative filename, byte size and SHA-256. All requested inputs must exist and match; missing or mismatched inputs cause exit2 before tests start. There is no archive fallback, fabricated baseline, substituted expected value, or automatic skipped-test success.

`GK_SRAM` must be the v1.06 baseline with SHA-256 `dd22847140cc0397ab8f42e29f1832681628ed52bfa17e95041c288afc4ef773`. The ROM hash is `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`. The manual hash is `ecc2150b0ef43861b3590fa8d1671b99fe5f486894b1289836eb77265f49d0ae`.

`GK_ARCHIVED_CAPTURES` supplies10 prior capture SRAM files used by domestic comparisons. `GK_ROI_EVIDENCE` supplies baseline, summer_step3, fall1175_ready, winter_step2 and spring1176_step1 SRAM files. `GK_EVIDENCE` supplies the v1.06 capture/tracing files listed in the fingerprint manifest. These are external required inputs, not package omissions silently ignored by tests.

## Suites and counts

| Suite | Unittest methods | Additional cases | Nature |
|---|---:|---:|---|
|v106_full|72|—|63 prior source methods plus9 archived-capture methods|
|domestic|27|—|Domestic source and prior capture comparisons|
|trade|38|—|37 original methods plus1 integration regression|
|seasonal_core|26|—|Original instruction seasonal/economic paths|
|seasonal_extension|24|—|Original instruction AI, unrest, robbery and related dependencies|
|logistics|37|—|Transfers, ownership, adjacency, quantities, attributes and costs|
|commodity|12|—|Index/layout source, manual fingerprint, captured stock comparisons|
|scenario|13|—|Original loaders, all41 country records, limits and mutations|
|scenario_setup|—|15|Original condition-labelled setup replays|
|roi|13|—|Conditional policy model checked against source and captured year|
|Default total|262|15|No duplicated v106_source run|

To run a selected suite, use `python3 run_package.py --suite trade`. Repeat `--suite` for several. `--list` lists required inputs and expected counts. An explicitly selected `v106_source` suite runs63 methods with only GK_ROM/GK_SRAM; it excludes the9 archived-capture methods and is not the72-method validation. It is not run alongside v106_full by default.

A suite passes only with exit0, the expected completed count, and the unittest `OK` line (or the explicit15-case setup completion line). A truncated log or exit0 without the completion record fails. Raw stdout/stderr and command/count/hash records are written to the requested results directory. Tests may also write derived JSON beside their copied script. Those files contain test outputs and do not become included source inputs for another suite by default.

## Attribution, preservation and adaptations

Authors: Zhuge Liang/Codex original-ROM research team, with batches by domestic_source_completion, seasonal_source_completion, commodity_identity and trade_source_completion. Earlier v1.06 source tools are preserved from the prior Zhuge Liang/Codex research handoff. Package integration: trade_source_completion,2026-11-04.

`originals/` preserves the supplied authored files unchanged. `SOURCE_PROVENANCE.json` records original and adapted hashes. `adaptations.patch` records every change to executable copies. Edits replace workspace-specific or fallback private-input paths with explicit environment paths; imports retain their authored relative layout. Original research directories were not edited during packaging.

One substantive correction is explicit: the trade test model used50000 for merchant presence, but the original immediate$4E20 equals20000. The original CPU replay had always executed20000. The package changes the test model to20000 and adds seed1408 with economy333/morale118, where first rand100=26: the correct threshold26 rejects merchant0 while the incorrect threshold25 accepts it. This prevents the earlier weak sample grid from masking the transcription error.

Another corrected documentation claim is recorded here: the original ability actor country index is word$7B34, not$7B66. The logistics suite independently verifies both bank0 and bank3 helpers and temporary source-picker restoration. The source CPU instructions were unchanged.

These results remain bounded research evidence. They do not certify a Windows port, Xbox control, uninterrupted whole-game execution, or broad economic acceptance from a green method count. Controlled RNG substitutions in the domestic suite and exogenous assumptions in the ROI model remain as authored and identified in those scripts; packaging does not relabel them as natural gameplay.
