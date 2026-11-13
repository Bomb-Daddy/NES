# GK1 EXEC0237 — Verified private original Spring entry on native Windows

**Status:** Research increment. Portable C11 implementation and Linux tests; no
new stock NES Mesen emulation or original first-Spring RNG reconstruction.
This is the complete inherited EXEC0236 app with an additional integration,
not a fork, new game or isolated visual mockup.

## Player-facing change

The **existing WinMain/XInput game** can now launch directly into the
SHA-authenticated **fresh Spring 1175 owner-private Mesen observation** when
an owner explicitly supplies its captured JSON path in the
`GK1_OWNER_STOCK_FRESH_JSON` environment variable. No private data, ROM,
licensed screenshots or assets ship with or are sought by the game. The
original 41-country CSV must be installed and match its pinned SHA. Without
the variable, startup is unchanged. Invalid, mismatched or missing private
source fails closed at the title screen, without changing either original
country template. `--smoke-*` Windows test modes deliberately ignore owner
source, and ordinary controller input still goes through XInput.

**This imports an authenticated historical checkpoint. It does NOT calculate
an independent original NES first Spring, generate RNG, or turn the source
three-order budget into three source-authentic executable game commands.**
Source country #1 Gold430/Rate121 is only the exact named fresh capture, not
a universal first-Spring constant. Archived Spring1175/World1206 observations
remain separate and are untouched. Enhanced Rest, saved campaigns and other
mechanics are not changed.

## Reproducible integration

1. Hydrate the latest full EXEC0236 predecessor, applying overlays 230–235.
2. Run `python apply_owner_fresh.py <hydrated-repository-root>` from this folder.
   This source-hash-gated script adds `gk237_owner_fresh_win.c/.h`, the test,
   and the WinMain/CMake hook. Exact source hash preconditions are embedded.
3. Configure and compile the **same inherited application**:

   `cmake -S <root>/execution_199 -B <build> -G Ninja -DCMAKE_BUILD_TYPE=Release`

   `cmake --build <build>`

   `ctest --test-dir <build> --output-on-failure`

4. Run source-hostile QA: `python test_owner_fresh_overlay.py <pre-237-root>`.
5. Optional **owner-private** independent acceptance (never ship the input):

   `<build>/test_gk237_owner_fresh_win <build>/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv <private-original-fresh-capture.json>`

CI uses the public negative test, with no private data. A Windows MSVC
compilation/WinMain test must pass independently before claiming TESTED_PORT_WIN32.
A physically connected Xbox must be tested before hardware certification.

## Evidence provenance and outstanding milestones

- VERIFIED_STATIC: complete prior source preserved; exact overlay prehashes;
  no copyrighted ROM bytes in published source; hostile mutated predecessor
  refuses atomically.
- OBSERVED_RUNTIME_STOCK_FRESH: historical user-authorized WorkRAM
  source-attested 14×83 record capture; *no new emulator run here*.
- OBSERVED_RUNTIME_STOCK_ARCHIVED: older SaveRAM 14/27 records remain distinct.
- TESTED_PORT_LINUX: actual integrated native C11 app, source tests and an
  owner-private positive fixture checked locally (no private input published).
- TESTED_PORT_WIN32: only after real Windows MSVC CI run including new source.
- NOT RUN: new original-Mesen setup/RNG experiments, physical Xbox controller,
  independent first Spring and end-to-end full campaigns.

The next gameplay requirement remains reconstruction of original stock
ruler/difficulty/season RNG from authentic source execution and real three
orders, then merchant presence/RNG, autonomous AI, diplomacy/dynasty, battles
and the complete two campaigns. Publication is not agent read acknowledgement.
