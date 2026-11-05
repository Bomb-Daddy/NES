# GK1 original NES headless emulator smoke tests

**Read first:** [ChatGPT Headless Mesen Operations](../../../reference/MESEN_HEADLESS_CHATGPT_OPERATIONS_v1.00.md).

Files:

- [`headless_mesen_campaign_test.py`](headless_mesen_campaign_test.py): executable Python 3.11+ smoke and private `.mss` checkpoint verifier. Verifies ROM/emulator/bridge SHA, Mesen bridge activation, real NES SRAM, ruler/country state, real PPM frame and optional noncommitting D-pad input. Can produce and cold-reload a **new** state, but does not play entire campaigns. Preserves original state; no game RAM writing.
- [`test_headless_mesen_campaign_test.py`](test_headless_mesen_campaign_test.py): 14 `unittest` cases (synthetic RAM parsing, Year1/12/19 army targets, expected-state negative controls, pending/saved/loaded callback states, screenshots, CLI validation, output no-overwrite).

```sh
python3 research/Genghis_Khan_NES/tools/headless_mesen_campaign_test.py --mode doctor
python3 -m unittest -v research/Genghis_Khan_NES/tools/test_headless_mesen_campaign_test.py
python3 research/Genghis_Khan_NES/tools/headless_mesen_campaign_test.py --help
```

Run the live checkpoint test only with a user-authorized private ROM, exact Mesen executable, compatible **GK1 campaign-patched** mesen-for-ai bridge and source `.mss`; see the guide for a complete, verified sample command, hashes and expected receipt. A stock upstream `mesen-for-ai` without `state.restore` cannot load our research `.mss` using this path.

**Observed runtime on Linux 2026-11-05:** PASS restoring original Spring 1186 save (Gold 832/Orders 3), PASS 256x240 frame capture, PASS 5-frame D-pad Up with release, PASS creating and cold-reloading a new private `.mss`. The initial wizard and pending-restore failures are documented, not hidden. No Windows/physical Xbox test is implied.
