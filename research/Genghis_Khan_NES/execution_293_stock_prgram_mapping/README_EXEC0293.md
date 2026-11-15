# GK1 EXEC0293: first-Spring PRG-RAM CPU mapping guard

This **code increment** corrects the EXEC0292 Lua probe's incorrect CPU memory mapping. CPU `$6DCD` is the physical `nesWorkRam`/`nesSaveRam` bank offset `$0DCD` used by the original stock game; CPU `$0DCD` is internal NES RAM (a mirror of `$05CD`) and must not be used to identify country records in cartridge RAM.

Evidence: exact Director-authorized original stock ROM SHA-256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`; private original EXEC0076 evidence archive SHA-256 `8b757e11b12aeb3efe93e36c10628599ca5f12e91b15c02354cd3ebbd11bf52a`; historical fresh original NES stages 09 and 14 independently compared `nesMemory` and `nesWorkRam` for year/gold/food/troops/rate/orders/countries. `nesSaveRam` was zero for fresh stock, not missing data. DO NOT PUBLISH THOSE PRIVATE BINARIES.

## Reproduce host/source tests

```sh
cd EXEC0293_PUBLIC
python3 test_gk293_trace_v3.py
python3 test_gk293_lua_mock.py
# On a secure, authorized research host only:
python3 test_gk293_stock_capture_hqa.py /private/original_stock.nes /private/EX0076_capture.zip
# run the Lua V3 in an actually matched Mesen research runtime with IO permitted,
# GK293_PRIVATE_TRACE_PATH pointing to a private output; verify with:
python3 gk293_verify_stock_trace_v3.py /private/trace_v3.csv --rom /private/original_stock.nes
```

The new Lua recorder has **NOT** run inside real Mesen in this execution, because the matched headless Mesen binary/core was not available in the current host; Lua mock execution is not emulator execution. No native original-stock first Spring or entire campaigns were generated. Existing C11 code is unchanged; targeted inherited Linux native C11 GK1_288 CTest passed.

The V3 validator intentionally rejects older V2 logs that could falsely attribute cartridge country RAM to NES internal RAM. Stage WorkRAM claims remain **reported by instrumentation**, not independently proven. Never promote self-reported `work=1` or a terminal state to source-authentic original gameplay without separate live emulator evidence.
