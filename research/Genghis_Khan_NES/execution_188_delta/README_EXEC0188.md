# EXEC0188 — Authentic-source Classic ASSIGN receipt persistence

Continuation of the complete inherited EXEC0187 C11 native application. The original USA NES ROM is private and never included. This release does **not** reconstruct independent original first-Spring startup or claim original command ROM parity.

With a verified original Spring 1175 or 1206 checkpoint attached, Classic profile ASSIGN can transfer up to three native labor orders, with Pass consuming the remainder (inherited EXEC0187). NEW: controller **X** saves a 96-byte GC18 action receipt to a separate three-slot `.gc18` file, and controller **Y** loads/replays the receipt after the same authenticated private stock checkpoint is attached. The receipt contains source SHA-256, scenario/count/player, year/rate, action count and 0–3 explicit transfers, Pass flag, and a 16-byte SHA-256 verification truncation. It intentionally contains NO licensed original ROM or RAM bytes and **cannot resume a fresh process by itself**. After restarting, reattach the same original private checkpoint, open Classic ASSIGN, and use Y. No overwrite of an existing slot.

The Linux save path uses the existing user-data routing and exclusive-create mode0600 with file fsync. Linux cold-load validates regular file, owner-only permissions, single hardlink, fixed exact length, no-follow open, receipt integrity, original stock source identity, manual worker transfer bounds, order budget, and full replay, before atomically committing. Invalid files and source mismatches leave the derived session unchanged. The Windows branch is authored but **NOT RUN** on Win32 or physical Xbox.

Reproduce from the complete historical archive (extract all, not just execution_188):

```sh
cmake -S execution_188 -B build188 -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build188 -j8
ctest --test-dir build188 --output-on-failure
# Authenticated private input positive cases, do not distribute records:
./build188/test_gk188_classic_receipt execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv /private/fresh14.bin /private/archived_mongol14.bin /private/archived_world27.bin
xvfb-run -a ./build188/gk1_native_shell --smoke-frames 120
cd build188 && cpack -G DEB
```

**VERIFIED_STATIC:** source gating, replay, private SHA authentication, no licensed bytes in public receipt. **OBSERVED_RUNTIME_STOCK_FRESH_RESET/ARCHIVED_RESTORED:** inherited 14 fresh/14 archived Mongol/27 archived World source records, not new NES gameplay. **TESTED_PORT_LINUX:** actual builds/CTest/positive and negative tests where external QA receipt so states. **NOT RUN:** Win32 executable/NSIS, physical Xbox, independent NES startup, controller physical execution, full campaigns.
