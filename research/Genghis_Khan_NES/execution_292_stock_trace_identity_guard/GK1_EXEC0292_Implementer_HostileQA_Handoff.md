# GK1 EXEC0292 — Stock power-on transcript identity and chronology guard

2026-11-14 — non-release source and quality-assurance increment.

## Authority and baseline

Prior numbered execution **EXEC0291** was found on GitHub and in Collaboration Drive; this is incremental work, **not** an EXEC0076 restart. The original-stock NES ROM is already recovered, authenticated, and PRIVATE. Older v2.56 and EXEC0076 documents are retained but superseded by newer numbered implementations. The existing native C11 game remains unchanged by EXEC0292.

## Source changes

- `GK1_EXEC0292_Private_Mesen_PowerOn_Recorder_V2.lua`: checks the actually loaded Mesen ROM's `fileSha1Hash` against the independently authenticated stock-original SHA-1 **before opening** a private output file, captures CPU source writes at `$0084–$0089`, exact `$C289` entry/return seed transitions and mapped PRG bank, and the fourteen specific fresh-reset controller-stage frames. The log is *private original gameplay evidence*, not for public distribution. Fix: inspect WorkRAM backing via CPU address `$0DCD` rather than SRAM address `$6DCD`.
- `gk292_verify_stock_trace_v2.py`: independently validates provided V2 chronology, actual 48-bit C289 arithmetic and 15-bit output, six witnessed writes, C289 bank, 14 uniquely ordered controller stages, and explicit original ROM SHA-256 and SHA-1 optionally verified from the private original NES file. Stops on errors without changing any native save or campaign.
- `test_gk292_trace_v2.py`: 4 Python tests; 38 deliberate hostile mutation cases rejected.
- `test_gk292_lua_mock.py`: 3 tests actually loading source Lua through installed liblua5.4 with a *mocked* Mesen API. Checks wire format, original-ROM identity gating and compiling source Lua; **this is not an authentic Mesen run**.

## Reproduction on Linux host

```bash
cd GK1_EXEC0292
python3 test_gk292_trace_v2.py
python3 test_gk292_lua_mock.py
python3 -m py_compile gk292_verify_stock_trace_v2.py
# Only on private authorized research host; DO NOT publish this ROM:
python3 -c "from gk292_verify_stock_trace_v2 import *; import hashlib,sys; b=open(sys.argv[1],'rb').read(); assert len(b)==262160 and hashlib.sha256(b).hexdigest()==ROM_SHA256 and hashlib.sha1(b).hexdigest()==ROM_SHA1" /path/to/private_stock.nes
```

7 Python tests pass; 38 hostile mutation cases pass; real Lua is compiled/executed by the mock harness; original-stock ROM hash checked independently. The host does not currently have the previously matched headless Mesen runtime: no original stock execution was performed in this increment. No new native C11 build, Win32 CI, NSIS, Xbox controller, or complete campaign was run here. The Lua recorder is **NOT** claimed to be verified against native Mesen API/runtime; keep its output PRIVATE and audit before using as evidence.

## Provenance and known limitations

`VERIFIED_STATIC`: file hashes, tested source/archive integrity, source identity hash comparison, deterministic synthetic test fixtures.

`TESTED_HOST_LINUX_PYTHON_LUA_MOCK`: Python tests and Lua 5.4 source execution under mocked emulator API. This is NOT `TESTED_PORT_LINUX` native C11 game testing.

`OBSERVED_RUNTIME_STOCK_FRESH` and `OBSERVED_RUNTIME_STOCK_ARCHIVED_RESTORED`: historical EXEC0076+ verified gameplay evidence remains intact; no new Mesen capture.

`NOT RUN`: new V2 Lua under genuine Mesen, Windows executable/installer for EXEC0292, physical Xbox controller, both full campaigns.

The V2 verifier deliberately sets `mesen_execution_independently_proven=False` and `controller_command_menu_visual_confirmed=False`. A valid transcript can show a candidate year/orders/memory-bank state but **cannot prove screen-visible playable first Spring**, independent Mesen provenance, or port parity. Distinguish fresh WorkRAM from archived SaveRAM. Stock first-Spring world generation, true start RNG/character setup and controller command entry remain central blockers.

## Next executable priority

Run this source instrumented against the exact privately held headless Mesen emulator+core on actual stock SHA `ada7066...`; use an independent screenshot/CPU trace and fresh/archived first-Spring comparison with the exact input sequence; trace startup seeding, selectors, NMI/controller timing, and full 14/27 source country transformations. Implement verified behavior **in latest complete inherited C11 native app**, with three actual orders and persistent cold-load. Avoid hardcoding archived or fresh screenshot results as universal constants. Preserve Classic vs Enhanced separation and all prior gameplay features.

**No private ROM bytes, raw gameplay trace, screenshots, SRAM, or copyrighted audio/images may be included in public GitHub or Collaboration source packages. Private original data stays in Genghis research folder `1mJ72ezJb9kzIGUHHXPTh3lXtiVxWjEL7`.** Publishing code is not recipient acknowledgment. Hourly execution should continue unless Director explicitly stops or a complete release is accepted.
