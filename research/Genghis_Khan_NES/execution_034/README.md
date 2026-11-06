# GK1 EXEC0034 — C11 tactical source slice

**This is executed, compiled portable C11 integration code, NOT the finished Windows game.** Do not use this restricted fixture as a generalized movement/combining/pathfinding algorithm. No ROM/SRAM/sounds/screenshots/checkpoints or copyrighted game bytes are included.

Uses the approved combined modified NES ROM source profile (SHA256 `4764d46359581be1e7d12bcb85bd375c84cd4497d1084d9d306a6e884afbe7c5`), source reports [EXEC0030](https://drive.google.com/file/d/1Lz8a4_f5wA3jnQxH9umqIrJBP5mZLT0L/view), [EXEC0032](https://drive.google.com/file/d/18PpZ0CK0OVxqrLPXLX_zhq4_jkSi68tp/view), [EXEC0033](https://drive.google.com/file/d/17XZ_PGRYmots0MPOaUWhY6IEccKUcBuw/view).

Implements typed terrain cost and hazard tables, exact floor-and-48-bit-RNG casualty math, and **only** the previous controller-observed input cases: forest 10-Mobility pending step + B completion, terrain0 automatic completion, forest Mobility2 rejection, Mobility4 equality accept/auto-complete, friendly-occupied controlled case (mover74/friend0/completion word2), and enemy-occupied Move rejection. Unknown states fail closed without mutating their output. The other-units value is a test sentinel, not a source memory hash.

Compile and verify:

```sh
gcc -std=c11 -O2 -Wall -Wextra -Werror -pedantic gk1_tactical_slice.c test_gk1_tactical_slice.c -o test_gk1
./test_gk1
# 100 assertions PASS
gcc -std=c11 -O1 -g -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined -fno-omit-frame-pointer gk1_tactical_slice.c test_gk1_tactical_slice.c -o test_gk1_sanitize
ASAN_OPTIONS=detect_leaks=1 ./test_gk1_sanitize
# 100 assertions PASS, no reported sanitizer error
cmake -S . -B build && cmake --build build -j2 && ctest --test-dir build --output-on-failure
# 1/1 CTest PASS
```

These commands were **actually executed on Linux** in EXEC0034; no Mesen replay this execution and **no Windows executable, Xbox controller, installer, campaigns, graphics, audio or game save QA** is claimed. Native controlled outcomes were executed in earlier numbered batches, not this one.

Exact source Git blob SHA1 values for readback:
- Header `f2945835347591bea54df8566508b63f5b84a3d2`
- Implementation `71fc26432c3d451cdf8232975390c1071915e2a4`
- Test `6fb1ec43eef48b43a51213a82f768f2e5981b456`
- CMake `50249913349385efcbd9012dc49e523a89984a91`

[Master v2.16](https://docs.google.com/document/d/13r2_-6tvGZO-niX7TGSkgwHfsQJw7cfu4SoNm4AHDH8/edit) · [Numbered handoff](https://docs.google.com/document/d/1NFO-GrfoEEdwsU_lKFviUaS9C_8E89MziUQ8sG9yMSk/edit) · [Completion ledger](https://docs.google.com/document/d/1MPJCmGa9_cdM9PU7KAtVmUquK1bzZhRAN-c8ngp5BSs/edit)

Remaining: native source branch tracing for the friendly unit join/completion word2, Entire versus Divide, multi-step, zero-unit terminal states, and actual isolated Genghis Windows integration/QA.
