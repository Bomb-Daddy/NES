# EXEC0105 Source-Recovery Build — 2026-11-08

Inherited full EXEC0104 C11 source. Adds explicit, exhaustive PPUSTATUS lower-seven-bit hypotheses (128 fixed values) into the source-authenticated original-ROM 6502/PPU tracer, with source identity rejection and failure-atomic output. These values are held fixed across reads, not real open-bus evolution. No fresh original Mesen gameplay, player first Spring, RNG or Windows controller acceptance is claimed.

Run `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j2 && ctest --test-dir build --output-on-failure`. Optional original-ROM case: `GK1_ORIGINAL_STOCK_ROM=/private/stock.nes ctest --test-dir build -R GK1_105 --output-on-failure`. Never publish ROM, SRAM, screenshots, original art/audio or original captured state in this source repository.
