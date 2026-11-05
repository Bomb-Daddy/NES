# ZL-GK1-EXEC-0012 — First executed extraction: command data

**Version/date:** 1.00 — November 5, 2026. **Author:** Zhuge Liang. **Recipients:** Director; Nobunaga (Windows integration); Cao Cao (independent QA).

**Scope:** Executed static extraction from the two supplied, hash-pinned modified NES builds. This is usable implementation data, not another plan. Existing native modified-ROM passes remain valid. No new Mesen session or Windows build was executed in this batch.

## Deliverable

[Command-data package on Drive](https://drive.google.com/file/d/1kRiBYBfg8MIcq5_CzSUcvqBCf6sEhrxT/view?usp=drivesdk), in Retro Gaming / Collaboration.

Package: `GK1_EXEC_0012_Command_Data_v1.00_2026-11-05.zip`, 19,580 bytes. SHA-256: `0b886f53d4f55b25bc8ab73f47d2e820ed49414abb966251509e98aa0d014ed4`.

Contents:
- `command_costs_mod_pair_v1.json`: all 34 rows from the actual Demobilize-only and Shared-BP ROMs; 28 source-mapped command labels.
- `patch_site_index_v1.json`: all 80 manifest intervals with bank, CPU/file coordinates, lengths, changed-byte counts and before/after SHA-256. No ROM payload or patch bytes included.
- `ui_surface_index_v1.json`: 16 source-indexed UI surfaces, geometry, actor/subject information and existing evidence references. Prior runtime classifications are carried forward, not independently rerun.
- `extraction_receipt_v1.json`: exact input/output hashes and checks; two deliberate corrupted-byte controls.
- `README.md`: scope, completion register, limitations and consuming Windows modules.

## What ran

Python 3.13.5 on Linux read both ZIP archives and their actual NES binaries. Both archive CRC checks and recorded input SHA-256 matched. All 80 patch `before_hex` intervals matched the Demobilize input, all `after_hex` intervals matched the Shared-BP output, and all bank/CPU/file-offset conversions matched. Intervals do not overlap. Every one of 3,139 differing bytes is covered by the manifest; none lies outside it.

All 34 six-component cost rows were read at headered offset `0x1F1C8` (bank 7, CPU `$B1B8`, six bytes per row). For every row, the Shared-BP value equals the sum of the preceding six-component row and appears only in the Body column. These are `VERIFIED_STATIC_MOD` findings for the exact two binaries.

Two negative controls changed one target byte at `0x1F1C8` and `0x3CA67`. Both were rejected by their corresponding manifest interval checks. These checks establish extraction integrity, not new gameplay coverage.

## Extracted Shared-BP table

Row order is 0 through 33. The numeric array is directly reusable by the matching Windows profile:

```json
[10,20,10,10,5,5,5,0,5,5,20,20,20,20,20,40,15,20,20,15,5,20,20,15,15,40,25,10,5,20,20,10,75,25]
```

Source-mapped labels from the delivered `src/ui.py` and UI05 record:

| Row | Command | Shared BP |
|---|---|---:|
| 0 | Tax: Change rate | 10 |
| 1 | Tax: Extra tax | 20 |
| 2 | Assign: Labor | 10 |
| 3 | Assign: Army | 10 |
| 4 | Give | 5 |
| 5 | Train: Troops | 5 |
| 6 | Retired personal-training row; exact label unresolved in this export | 5 |
| 7 | Retired personal-training row; exact label unresolved in this export | 0 |
| 8 | Train: People | 5 |
| 9 | Trade: Buy/Sell/Hire | 5 |
| 10 | People: Switch | 20 |
| 11 | People: Select | 20 |
| 12 | People: Demote | 20 |
| 13 | People: Promote | 20 |
| 14 | People: Marry | 20 |
| 15 | People: Draft | 40 |
| 16 | Caller unresolved | 15 |
| 17 | Caller unresolved | 20 |
| 18 | Caller unresolved | 20 |
| 19 | Caller unresolved | 15 |
| 20 | Paid foreign State/Ruler View | 5 |
| 21 | Move: Base | 20 |
| 22 | Move: People | 20 |
| 23 | Move: Goods | 15 |
| 24 | Policy | 15 |
| 25 | Treaty: Vassal | 40 |
| 26 | Treaty: Tribute | 25 |
| 27 | Treaty: Alliance | 10 |
| 28 | Spy: View report | 5 |
| 29 | Spy: Havoc | 20 |
| 30 | Spy: Kill | 20 |
| 31 | Spy: Search | 10 |
| 32 | War: Self | 75 |
| 33 | War: Prince | 25 |

**Important:** Demobilize reuses row 2 (10 shared BP); it is not People Demote, which is row 12. A row's numeric value does not establish whether that command is currently reachable. Do not expose retired or unresolved entries as invented player commands. The 28 mapped labels are source mappings, not 28 newly exercised menu routes.

## Consuming Windows contract

Nobunaga: import `shared_bp_cost` for the Shared-BP feature and preserve the separate six-component table for the Demobilize-only profile. The numbers are extracted costs, not new balance choices. Resolve the acting character using the proven handler; affordability, cancellation, free paths and charge timing remain governed by that handler and its existing runtime evidence. Do not use the summed table for a six-stat profile.

Cao Cao: compare these tables to the exact named ROMs and then to the Windows-compiled table. Use the packaged patch/source hashes and original native fixtures. Do not rerun settled modified-ROM behavior solely because it is modified; newly exposed integration differences and unresolved paths are the useful targets.

## Completion register

| Item | Status | Limit |
|---|---|---|
| Paired 34-row table export | PASS VERIFIED | Static numeric values and sum conversion |
| 28 named command mappings | PASS VERIFIED | Source/UI mapping, not fresh gameplay |
| Remaining exact row labels 6/7/16/17/18/19 | PARTIAL | Values known; two retired and four caller mappings unclosed |
| 80 patch-site index | PASS VERIFIED | Exact bytes, addresses, complete diff coverage, no overlaps |
| 16 UI surface records | PASS VERIFIED | Inventory extracted; native statuses carried from source |
| New combat, audio or AI execution | NOT RUN | No new emulator session in this batch |
| Windows integration | NOT RUN | Data ready; no new Windows executable tested |

No unmodified-stock binary comparison was performed here. No original v1.07 requirement status was changed. The 61/40/66 historical count does not negate the later valid modified-ROM evidence. This batch organizes known binary behavior into importable data; it does not claim a newly solved combat or audio subsystem.

## Provenance

Demobilize ZIP SHA-256: `993d8bb04c9faa56ce628bc81be3e9445c70d3ebf49e11cfc6caf20e5af24e1e`; ROM SHA-256: `61ad28688d503231d12537b2cbebb073ae5f91ec1391f825c1d05312436393be`.

Shared-BP ZIP SHA-256: `9b2a72c2795bab63dd925bb0cd7dd73a7aa8dd80f0b7c711dfa376b8c802708e`; ROM SHA-256: `4764d46359581be1e7d12bcb85bd375c84cd4497d1084d9d306a6e884afbe7c5`.

Source paths in the Shared-BP developer ZIP: `build/manifest.json`; `source/mods/Genghis_Khan_NES/shared_bp/src/core.py`; `src/ui.py`; `docs/ui_inventory.json`. Exact machine-readable receipt is in the package.

**Next executable work:** resolve rows16–19 from their actual callers and formalize the acting-character handler, then capture genuinely missing combat and audio information. This is a queued next task, not a claim of background execution.

End of execution report.
