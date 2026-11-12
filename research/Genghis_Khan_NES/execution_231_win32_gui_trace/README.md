# Genghis Khan I — EXEC0231 (Windows GUI and NSIS acceptance)
2026-11-12. Complete inherited C11 source, not a separate game. The recovered original NES ROM and original gameplay assets are **private** and never uploaded to public GitHub.

## Real Windows result
[Windows 2022 GitHub Actions run 43303042054](https://github.com/Bomb-Daddy/NES/actions/runs/43303042054) completed **SUCCESS** on commit `56e2f53fc0c7dbf179189632d4f8899628fde540`. Exact 922-file predecessor hydrated; native MSVC WinMain/XInput compiled; 4/4 targeted Windows CTests passed; actual 1280×720 WinMain client opened, with 4 instrumented WM_PAINT/render completions during `--smoke-frames=3`, exit 0; NSIS installer generated, silently installed, and **installed executable ran the bounded GUI smoke with exit 0**. The installed binary SHA-256 was `9f88b7f41ecfb720fadb009415e6e2ffdc9ddfe3cbd4af145f87d479c0d3e3c2`. The installer was not uploaded as a downloadable Actions artifact due to storage pressure.

## Actual bugs fixed
Previous 20-second "GUI smoke hangs" were NOT real smoke tests: `git apply` from an inner directory silently skipped all source paths while returning exit code 0. The workflow now initializes a transient Git root within the hydrated source, checks exact precursor hashes, applies EXEC0230 with Windows CRLF handling, and verifies the patched paint variable exists before compilation. Once actually patched, the WinMain smoke returned 5 due to the hosted desktop geometry. The exact anchored `apply_trace.py` overlay uses a `WS_POPUP` **only for bounded smoke mode** (1280×720 client) and preserves the original decorated interactive window. Environment-gated `GK1_SMOKE_TRACE` logs only WinMain/paint/XInput stage names, never game state. `test_trace.py` exercises three independent valid/malformed/atomic negative regression conditions.

## Reproducibility and status
- Source scripts: [apply_trace.py](./apply_trace.py), [test_trace.py](./test_trace.py)
- Pipeline: [Windows workflow](../../../../.github/workflows/gk1-native-windows.yml)
- Predecessor: [EXEC0230 patch](../execution_230_win32_gui_smoke/GK1_EXEC0230_CopyrightClean_Integration.patch); [EXEC0229 complete source hydrator](../execution_229_windows_ci_bundle/hydrate_verified_bundle.py)
- Full canonical release and cumulative documents (Drive Collaboration folder): https://drive.google.com/drive/folders/1d0TA1lgqsVA3AWQQ9o0FZgmvao90oCeT

**Classification:** VERIFIED_STATIC source integrity + hostile controls; TESTED_PORT_WIN32 actual application graphical smoke and NSIS-installed smoke; TESTED_PORT_LINUX 200/200 *historic EXEC0230*, not new EXEC0231; OBSERVED_RUNTIME_STOCK older fresh WorkRAM and archived SaveRAM only, no new Mesen capture; NOT RUN physical Xbox, long interactive games, full campaigns. Original native first playable Spring with ruler/difficulty RNG, 14/27 countries and actual three orders remains OPEN. Trader RNG, autonomous AI, battle aftermath, dynasty, diplomacy, sound/media also open. No final game certification or agent acknowledgment inferred. Keep existing hourly development enabled.
