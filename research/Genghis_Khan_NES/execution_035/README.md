# Genghis Khan I NES → Windows: EXEC0035 native tactical SRAM read-only adapter

**Scope:** Separate Genghis Windows source analysis module, not ROTK II, not a full game or a release. This code reads prior-source verified tactical SRAM layout. It does **not** implement or infer collision legality, merging, Divide, battle terminal, or general pathfinding.

- `gk1_native_ram_view.h/.c`: decode 20 eleven-byte unit slots at $6060, 16x10 raw tile area at $615C, 16-bit scratch at $613E, and raw positional occupancy masks retaining zero-soldier unit records.
- `test_gk1_native_ram_view.c`: synthetic-only SRAM QA covering all 160 cells, 20 slots, raw high-bit tile bytes, endian, zero-soldier position, erroneous inputs and unchanged output / read-only state.
- `CMakeLists.txt`: Linux-compiled library/test target.

Verified commands:
```sh
gcc -std=c11 -O2 -Wall -Wextra -Werror -pedantic gk1_native_ram_view.c test_gk1_native_ram_view.c -o test_gk1 && ./test_gk1
# 601 assertions PASS
gcc -std=c11 -O1 -g -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined -fno-omit-frame-pointer gk1_native_ram_view.c test_gk1_native_ram_view.c -o test_gk1_sanitize && ASAN_OPTIONS=detect_leaks=1 ./test_gk1_sanitize
# 601 assertions PASS
cmake -S . -B build && cmake --build build -j2 && ctest --test-dir build --output-on-failure
# 1/1 PASS
```

**Proof classification:** existing native layout findings from EXEC0022/26/28/33 + NEW executable source tests for THIS module. No new native Mesen run, Windows executable, physical Xbox input, save, AI, graphics or audio integration was tested. Previous complete artifacts remain preserved; no licensed ROM/SRAM/checkpoint or other proprietary media is distributed.

Full [cumulative guide v2.17](https://docs.google.com/document/d/15Sk3bR35UBUYRW5Fk8LZLmEQqJIkifhDEuH3nSA1AQ4/edit) and [EXEC0035 handoff](https://docs.google.com/document/d/1hYfErs5ktGbo9454BGqUcQIMksVU5sgo01ilX7h1DtY/edit). Source ZIP local only SHA256 `ddd35e1159ee6ea0fd159ef6871ea3cedc43cf6725c6fb0adfff43f8bd3a179c`; no claim of Drive ZIP delivery.

Next: trace the actual native friendly-join/word2 VM route, then Entire/Divide and multistep; integrate into the standalone Windows game and run actual Windows/controller QA.
