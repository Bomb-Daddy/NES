# Genghis Khan I Windows – EXEC0036 capture-to-port integration

This is a NEW copyright-clean C11 integration / hostile-QA component for the standalone Genghis Khan I English NES Windows port, NOT ROTK II. It consumes existing execution_034 movement projections and execution_035 SRAM layout, preserving earlier work.

The code provides gk1_capture_delta() for comparing two 8192-byte SRAM snapshots without treating them as completed-command states. It outputs separate change masks for unit records, soldiers, position, Mobility and opaque fields, raw terrain changes, scratch word $613E:$613F, and occupancy both including and excluding zero-soldier units.

gk1_capture_compare_fixture() requires an EXTERNAL controller trace and comparable-phase witness, combined modified-ROM SHA-256 4764d46359581be1e7d12bcb85bd375c84cd4497d1084d9d306a6e884afbe7c5, and in-battle checkpoint SHA-256 e1b6266c963a48d736627812d024910b6d600c2ad8571813eb3b0999e0f2938b. It checks ONLY the previously native-tested exact (7,7)->(8,7) fixture subset: six known combinations, including one controlled friendly co-location at 74/0 soldiers with scratch word 2. Other variants fail closed. Source bytes/hashes must be independently verified by caller; setting witness fields in tests is not gameplay proof.

Build from this directory (the sibling folders must exist):

    gcc -std=c11 -O2 -Wall -Wextra -Werror -pedantic ../execution_034/gk1_tactical_slice.c ../execution_035/gk1_native_ram_view.c gk1_capture_pair.c test_gk1_capture_pair.c -o test_gk1_pair
    ./test_gk1_pair
    cmake -S . -B build && cmake --build build -j2
    ctest --test-dir build --output-on-failure

Executed on Linux: 183/183 direct C11 checks PASS, 183/183 ASan/UBSan checks PASS, CTest 1/1 PASS. The tests are synthetic source integration tests. This run did not execute Mesen gameplay, any Windows executable, or an Xbox controller.

Scope limits: Neither a zero-soldier record nor physical co-location proves general merge rules, completion semantics, terminal battle logic, or occupancy-based movement legality. Unknown high-bit tiles remain raw. Existing 13 fixed-frame SRAM captures are valid point-in-time evidence, not necessarily completed actions. Next natural-source research remains the friendly join / two-count VM path, multi-step and Entire/Divide, then battle outcomes, AI and complete Windows build.

No licensed ROMs, checkpoints, SRAM, screenshots, artwork or audio are committed.
