# EXEC0208 — authenticated archived original-stock RNG evidence in native C11

Development candidate built on exact inherited EXEC0207 native C11 game, **not** a first-Spring implementation.

## Actual source change
- `gk163_mesen_extract_with_proof` extends the native Mesen savestate importer without breaking its older API. The original archived country records still require the correct `nesSaveRam` bank, 83-byte country layout, 14/27-country authentic digest, and validated Mesen serialization before the CPU provenance is returned.
- `gk208_archived_source_rng` additionally authenticates the **entire** two approved private original-stock archived Mesen savestate files against their trusted SHA-256, and authenticates the original USA NES source ROM. It extracts the **archived CPU state** `$0084..$0089`, PC and CPU cycle counter, then independently compares one **hypothetical next RNG step**, source-derived 6502 vs native C11 arithmetic. It NEVER labels this next draw as having occurred.
- `--audit-stock-archived-seed ORIGINAL_ROM ARCHIVED_MSS mongol1175|world1206` invokes this in the existing executable, not a separate game stub. Reject paths return nonzero; the real controller application, stock and Enhanced profiles, all previous game code and historical tests remain intact.
- `test_gk208_archived_rng` hostile checks include bad arguments, nonexistent ROM, wrong profile, preserved output on failure, exact authentic private states when supplied as test args, and an altered archived screenshot (country records unchanged but whole-file hash mismatch) rejected. NO private ROM, Mesen state, screenshots, serialized save or seed bytes occur in the PUBLIC source or package.

## Reproduce
```
cmake -S execution_199 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j 8
ctest --test-dir build --output-on-failure -j 8
# PRIVATE local original files supplied separately: NOT in the public package.
build/test_gk208_archived_rng ORIGINAL_ROM ARCHIVED_MONGOL_MSS ARCHIVED_WORLD_MSS
build/gk1_native_shell --audit-stock-archived-seed ORIGINAL_ROM ARCHIVED_MONGOL_MSS mongol1175
build/gk1_native_shell --audit-stock-archived-seed ORIGINAL_ROM ARCHIVED_WORLD_MSS world1206
```

## Evidence boundaries
- VERIFIED_STATIC: original ROM exact SHA, complete private archive identity; independent original-derived RNG arithmetic parity; exact replayed source patch, CTest, sanitizers and hash manifests.
- OBSERVED_RUNTIME_STOCK_ARCHIVED_RESTORED: **previously captured** original NES Mesen saved checkpoints parsed in this execution. Distinguish archived 1175 Mongol SaveRAM vs archived 1206 World SaveRAM. This run DID NOT launch original Mesen and DID NOT observe the next draw occur.
- OBSERVED_RUNTIME_STOCK_FRESH: previous fresh-reset 1175 work RAM evidence remains separate, not resampled here.
- SOURCE_BOUNDED_PORT_CPU: inherited EXEC0207 original reset interpreter stops at hardware gate; archived seed != boot seed.  Neither captures RNG initialization or exact draw sequence.
- TESTED_PORT_LINUX: genuine Release/Debug/clean builds, native graphical execution, private original-parity tests, Debian package extraction. Exact actual command/test logs in QA. TESTED_PORT_WINDOWS: no. NOT RUN: Win32/NSIS, physical Xbox, full original 14/27-country campaigns.

## Unfinished hard gates
Exact authentic first-playable Spring setup (player/difficulty/character seed initialization for all 14/27), merchant scheduling, strategic AI, diplomacy, dynasty, battle aftermath, complete saved campaigns, original sound/art and Windows/Xbox acceptance. Do NOT copy archived/fresh Gold/Rate values into source as universal constants. Publication of this handoff is not an agent-read acknowledgement or Director release signoff. Stock ROM and licensed originals remain only in private Genghis Drive research.

Original Mesen format independently checked against SourMesen/Mesen2 Core/Shared/SaveStateManager.cpp and Utilities/Serializer.cpp; no code or assets copied into this source. Direct public references:
- https://github.com/SourMesen/Mesen2/blob/master/Core/Shared/SaveStateManager.cpp
- https://github.com/SourMesen/Mesen2/blob/master/Utilities/Serializer.cpp
