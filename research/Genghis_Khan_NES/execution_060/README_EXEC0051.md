# GK1 EXEC0051 — dual-campaign PRE_SETUP snapshot persistence

**Scope:** An additive native C11 implementation in the same inherited app (`execution_050` → `execution_051`). This is **not** a second game or a complete Windows release.

## Implemented

- Both original Mongol (14-country) and World (27-country) mutable, pre-setup world snapshots persist together, including 31 source-defined numeric fields per country and both navigation cursors.
- Original country/lord/source-location identities are rebuilt from the exact v1.07 CSV; no private ROM, graphics, audio or SRAM data is serialized. Format is 2,594 bytes: fixed 48-byte header with identity/version/source SHA-256 and stage, 41×62-byte countries in little endian, CRC-32 (IEEE).
- Strict stage `PRE_SETUP_TEMPLATE` only. Original playable Spring setup and character/difficulty are not inferred; saving them is not implemented.
- C11 `gk51_pre_setup_save_new` refuses existing files (POSIX `O_EXCL` or Win32 `CREATE_NEW`), flushes new files, does not overwrite any existing game/user campaigns. `gk51_pre_setup_load` rejects incorrect bytes/length/CRC/version/source identity/stage/cursors/economic invariants and commits world state atomically on success; previous battle/replay remains intact.
- Same native shell binds X to **pre-setup save** and Y to **pre-setup load** on the campaign roster UI; separate existing replay paths remain unmodified.
- Native CLI switches `--pre-setup-export path 1|2`, `--pre-setup-import path`, and `--pre-setup-pad-smoke path` provide real executable/process checks. Pad smoke injects synthetic pad edges through the actual `pad_actions` route; **not physical Xbox**.

## QA reproducibility

```bash
cmake -S execution_051 -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j4
ctest --test-dir build --output-on-failure
build/gk1_native_shell --pre-setup-export /tmp/gk1_w.gkp 2
build/gk1_native_shell --pre-setup-import /tmp/gk1_w.gkp
build/gk1_native_shell --pre-setup-pad-smoke /tmp/gk1_pad.gkp
xvfb-run -a build/gk1_native_shell --smoke-scenario 30
```

Exact 11/11 pass on Linux Debug, Release and ASan/UBSan; hostile 5,231 direct checks; real separate-process create/load; duplicate slot exit6; corrupted byte exit7; native 1280×720 X11 scenario+Divide smoke. Compilation uses strict `-Wall -Wextra -Wpedantic -Werror`. Build logs under `qa_exec0051/`. Exact stdout/exit and SHA manifest in archive.

## Evidence level and next action

`TESTED_PORT_LINUX` for code/CLI/X11 and synthetic pad injection. `VERIFIED_STATIC` v1.07 source via prior work. **NOT RUN** Windows native build, physical controller, new Mesen captures, original first playable seasonal setup, unrestricted strategic command economy/AI, or full game saves. Next implement the original pre-setup player/ruler/difficulty/stat/season transform from checked NES evidence, then real turn economy and campaign persistence; do not treat this pre-setup snapshot as a campaign save.
