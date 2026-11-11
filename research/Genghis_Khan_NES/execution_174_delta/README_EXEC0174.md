# GK1 EXEC0174 — source-only real initialization boundary

C11 implementation based on **the complete EXEC0173 game**, not an isolated prototype. Adds `gk174_setup_boundary.c/h`, real native command `--source-boot-boundary OWNER_PRIVATE_ROM`, and negative/positive CTests.

For the **exact original stock NES ROM (SHA256 ada7066d...)** two source-bounded CPU interpretations execute: (1) unknown initial RAM fails closed at CPU PC $8060, first unknown RAM $00F5, after 70 completed instructions and 243 completed cycles; (2) the *archived fresh Mesen script's expressly selected* `RamPowerOnState=1` AllZeros setting advances to the actual source instruction at $C5BE that first reads PPUSTATUS $2002 after 142 completed instructions. This second option is **not universal power-on hardware**. This implementation does not cross the first PPU read, does not emulate Mesen, does not create first Spring, and does not grant Classic orders. No RNG transfer counted before these gates is evidence that RNG never occurs later.

The two traces are computed using the already integrated 6502 source interpreter. Authentication checks the complete private ROM. Both `Gk83Trace` results and their completed-instruction/cycle delta commit together, or output is unchanged. Bad path/source, invalid budget and mismatch refuse without a false result. All original ROM, original stock observations, and licensed assets remain private.

Build: `cmake -S execution_174 -B build174 -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build174 -j8 && ctest --test-dir build174 --output-on-failure -j8`.
For owner-private verification: `build174/gk1_native_shell --source-boot-boundary PATH_TO_AUTHORIZED_PRIVATE_ROM` and `build174/test_gk174_setup_boundary PATH_TO_AUTHORIZED_PRIVATE_ROM`.
