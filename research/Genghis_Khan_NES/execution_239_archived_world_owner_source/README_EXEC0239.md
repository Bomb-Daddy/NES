# EXEC0239 — same inherited Genghis Khan I native C11 application: archived original World Mesen SaveRAM startup

This is new **executable C11 code integrated into the same WinMain/XInput app** inherited from EXEC0238, not an alternative simulator or universal Spring values. Original USA NES ROM and original evidence are kept in owner-private Drive only; none is redistributed. It adds an explicit opt-in archived original World Spring1206 27-country checkpoint to complement the previously integrated fresh original Mongol Spring1175 WorkRAM JSON checkpoint. This is checkpoint replay, **NOT independent original RNG/first-Spring generation**.

The owner may set `GK1_OWNER_STOCK_WORLD_MSS` to the matching private *World* original Mesen `.mss` file before launching the normal Windows game. It refuses missing or invalid source, wrong RAM bank/Mongol snapshot, noncanonical 41-country CSV, pre-mutated game state, or simultaneous `GK1_OWNER_STOCK_FRESH_JSON`. CI/graphical smoke mode never loads private files. Unset vars preserve normal title. Stock original checkpoint uses existing controller UI with three remaining orders, but original 14-command mechanical parity is incomplete.

## Reproduce from exact complete predecessor

1. Extract private-free `GK1_EXEC0238_Complete_Integrated_C11_Source_QA.zip` (SHA256 `9f7131078ff3f87e03c8ab707df4f9506a2e607577138114e0f8087cc311e06b`) to `<root>`.
2. Run `python3 apply239.py <root>` with this published integration patch + SHA file. `python3 test_overlay239.py <EXEC0238_zip>` checks 9 hostile controls.
3. Build: `cmake -S <root>/source/execution_199 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build -j12 && ctest --test-dir build -j12 --output-on-failure`. 205/205 tests passed twice, including clean reconstruction.
4. Optional OWNER-PRIVATE only (not publicly distributable): `build/test_gk239_owner_world_mss build/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv <private World original MSS> <private Mongol original MSS>`: 30/30 assertions PASS.
5. Actual Linux graphical smoke: `xvfb-run -a build/gk1_native_shell --smoke-frames 15` exit0, 1280x720 window. All new Win32 code still requires real Windows CI, NSIS installation and physical Xbox acceptance.

There are five exact new/modified source paths: `gk239_owner_world_mss.c/.h`, `test_gk239_owner_world_mss.c`, `CMakeLists.txt`, and `gk1_native_shell.c`. All other 935 inherited source archive members are exact copied bytes.

Next priority: original Mesen CPU source trace for first Spring setup/ruler/difficulty/RNG; independent authentic full 14/27-country season and original executable three orders; original Trade RNG and complete campaign mechanics. No historical stock evidence is falsely elevated to newly run original emulator status.
