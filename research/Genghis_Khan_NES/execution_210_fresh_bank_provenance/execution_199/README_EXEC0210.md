# GK1 EXEC0210 — Original fresh Mesen WorkRAM bank provenance admission

Predecessor: complete and unchanged EXEC0209 C11 game. This changes the existing
`gk165_extract_fresh_json` admission path invoked by the real native game via
`gk166_enter_original_fresh_json`, not a separate game. Existing original stock
Classic and approved Enhanced states, 14/27-country templates, researched
controller orders, persistence, prices, battle and sound modules remain intact.

## Actual identified bug and correction

The original archived fresh NES reset was captured with real first Spring 1175
country state in physical `nesWorkRam` offset `0x0DCD`. The original helper's
root `year/gold/rate/orders/countries` aliases read **nesSaveRam**, which was
blank on that fresh reset, displaying zeros. The CPU-visible `nesMemory` view
and named `nesWorkRam` view correctly showed 1175, Gold430, Rate121, Food208,
Troops61, three orders and 14 countries. The valid original archived restored
checkpoints instead have live data in `nesSaveRam`, not `nesWorkRam`. The wrong
bank's zeros do not mean no real first Spring existed.

Before EXEC0210, `gk165` pinned a verified 14×83 WorkRAM SHA but did not verify
that other representations of the same original first Spring remained coherent.
A forged `nesMemory` Gold or false nonempty `nesSaveRam` could accompany an
unchanged signed country record and still be admitted. EXEC0210 now compares
seven fields between the CPU-visible `nesMemory` and authentic `nesWorkRam`,
and requires all seven SaveRAM fields zero, for this **one pinned fresh-reset
capture**. It accepts the legacy capture's *all-zero* five root aliases **only
as a known capture-script defect**, or a corrected five-field root matching
WorkRAM. Mixed aliases are refused. All refusal paths preserve caller output,
and the exact authenticated 14×83 WorkRAM digest remains mandatory.

## Reproducibility

```sh
cmake -S execution_199 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j8
ctest --test-dir build -j8 --output-on-failure
# Private original source receipt must be provided separately:
./build/test_gk210_fresh_bank_provenance PRIVATE_fresh_14_after_wait.json PRIVATE_corrected_fresh_14.json
./build/test_gk165_fresh_json PRIVATE_fresh_14_after_wait.json
```

The private `fresh_14_after_wait.json` and corrected private copy are NOT
public artifacts. Standalone public CTest tests only argument/failure gates,
while the private data assertion suite proves historical evidence admits and
rejects deliberately mismatched bank views. Rest of CTest remains unchanged.

## Evidence classifications and remaining work

- **VERIFIED_STATIC:** Exact original USA ROM SHA256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3` and private first-Spring evidence ZIP SHA256 `8b757e11b12aeb3efe93e36c10628599ca5f12e91b15c02354cd3ebbd11bf52a`. The new code itself contains NO ROM, SRAM, raw original 83-byte country records, screenshots, states or licensed assets.
- **OBSERVED_RUNTIME_STOCK_FRESH_RESET:** Previously captured actual Spring1175 WorkRAM stage14 from original Mesen; not a new emulator execution.
- **OBSERVED_RUNTIME_STOCK_ARCHIVED_RESTORED:** Prior SaveRAM Mongol1175/World1206 distinct history preserved; not retested here.
- **TESTED_PORT_LINUX:** Existing full native C11 app successfully built and new negative bank checks exercised against original private receipt. Linux software/controller tests are not physical Xbox acceptance.
- **NOT RUN:** New Mesen runtime, native Win32/NSIS, physical Xbox controller, complete campaign.

**Critical limit:** EXEC0210 makes one original first-Spring source import
harder to misrepresent, but does NOT generate arbitrary authentic Spring RNG,
character/difficulty setup, merchant presence, AI, diplomacy, dynasty, battle
aftermath, original sounds/graphics or both complete campaigns. The original
Gold430/Rate121 observed result is one history, not a universal constant.
