# EXEC0286 — Original-source C289 trace campaign provenance gate

This contains a reproducible patch and regression for the EXISTING complete native Genghis Khan I C11 game. Do not mistake it for a playable first Spring, a full Windows acceptance or actual new Mesen game event.

1. Obtain predecessor `GK1_EXEC0285_Complete_Inherited_C11_Source_And_QA_FINAL.zip` (Collaboration Drive `1r0Njm1NzMlCsDTyKOwweLMV6QmQ3Iqk1`), SHA256 `5606c9a7f4943347b11749dff6129e1f5f4085b1d2827ffe1904b17b2a535b1c`.
2. Extract it, then `git apply --check GK1_EXEC0286_Source_And_Integration.patch && git apply GK1_EXEC0286_Source_And_Integration.patch` from its extracted root.
3. `cmake -S execution_199 -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build --target gk1_native_shell test_gk286_c289_scenario_binding test_gk285_source_difficulty test_gk282_stock_rng_trace test_gk284_rate_history`.
4. Set `GK1_PRIVATE_ORIGINAL_ROM` to the independently SHA-verified privately held original NES ROM path for arithmetic-based tests, never add it to the project directory or public archive; run `ctest --test-dir build -R 'GK1_28[456]|gk282_original_c289_trace' --output-on-failure`.

All original game ROMs, raw RAM evidence and copyrighted audiovisual assets are excluded from this source package. `OBSERVED_RUNTIME_STOCK` classifications refer only to earlier independent original NES capture and are not established by these synthetic-source arithmetic tests.
