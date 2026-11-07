# ORIGINAL-KOEI Enhanced — Run016 Q14 source-sibling consolidation

This is an **original-game Enhanced candidate**; **NOT** canonical NES Classic, DOS, Windows-native tested, or Director accepted. The distinct Run015 candidate source trees remain immutable.

## Exact ancestry and sibling disposition
- Selected **direct parent**: verified Run015A full ZIP SHA256 `cec4363f1434d235ace279d990a1a7b461224ea5f63a5abee2c752238fc598b5` (530 CRC-clean members), itself direct descendant of Run014 `cb61dbdd5ecd26fdadb7b1c700f75734e2457c5f74c4d94ac6621e2b0a56cf06`.
- Independently inspected **donor sibling**, NOT a parent: Run015B full ZIP SHA256 `0a438906a3245e1fe56a66ac6ecb4ba303f51fca39d9297faec43c2addb33ea6` (530 CRC-clean members).
- Unmodified A keeps `q14_free_officers_by_province`: the source-backed provincial list head insertion order and fail-closed identified Q14 special flag.
- Selectively retained B's `prior_faction`, `province_id`, and source-rule provenance, and B's historical free-officer availability annotation, as OPTIONAL `q14_free_officer_provenance` accompanying A list, without synthetic army or battle mechanic.
- Run015B's `free_officers` schema **cannot** be interpreted losslessly as an original province linked-list head order, especially multiple officers in one province. Such sibling saves explicitly fail with `RUN015B_SCHEMA_UNMIGRATED`; manually translating or using them as official player saves is NOT approved. Run015A saves without the optional new ledger remain loadable.
- Default recruitment BP/loyalty remains previously approved Enhanced strategic policy; **not** claimed original NES recruitment parity. `q14_source_special_free_flag=True` still fails closed as `Q14_SPECIAL_FREE_RETURN_UNMAPPED`.
- Original NES 30 traces, original special-flagged Q14 branch, attacked-destination second simultaneous battle slot, source-linked historical HEAD hydration, tactical AI and physical Windows/Xbox are OPEN.

## Production patch scope
- `third_branch/integrated.py`: add independent optional free-officer provenance with per-province identity/army conservation validation, new release/recruit transition, explicit sibling-save failure code.
- `third_branch/historical_s1.py`: preserve actual provincial free officer head order in player-recruitable selection, append separately sorted original hidden/arrived historical records.
- `third_branch/officer_battle_adapter.py` unchanged from A; B's different worded comments do not alter source behavior.
- All approved Enhanced marketplace, economy, BP, strategy untouched. Classic and DOS source never incorporated.

## Evidence boundaries
Actual Linux Python 3.13/Xvfb source-backed Campaign/Tk tests, mutation, process-separated `.ro2` cold load and checked ZIP. Original English NES emulator/runtime traces, native Windows builds and physical Xbox not executed. See QA ZIP for raw logs and completion register.
