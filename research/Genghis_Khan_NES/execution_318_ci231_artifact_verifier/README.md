# Genghis Khan I EXEC0318 - verified Windows CI231 artifact and hostile source QA

This is an additional executable artifact-verification increment, **not** the game implementation or a release.
The inherited native 1280x720 C11 application is preserved. EXEC0317's cross-campaign CPU-claim provenance guard was actually built/tested at GitHub Actions.

- [Actual successful Windows workflow](https://github.com/Bomb-Daddy/NES/actions/runs/43777224990) at commit `4f5818dd31546384774db2ff75ca99d2b654aa80`.
- Native Win32 CTest **231/231 PASS**; WinMain, simulated software-controller and installed NSIS application PASS.
- CI artifact ZIP SHA256: `8c8fcc44c42ea5a69ca3ea0aff06c048095b623d9df7bfa6835fac7d3c3b548c` (579155 bytes).
- Embedded QA-only NSIS installer SHA256: `fa4df44aa0c11b326173403492526666e2b86ffe9f405da5b0e4cea78d06fee6` (612748 bytes).
- `verify_ci231.py` binds the archive, installer, manifest, commit, run and false release/device/campaign flags. `test_verify_ci231.py` is 18 host-side executable tests including negative controls.
- To reproduce: put the exact CI ZIP in this source directory and run `python3 verify_ci231.py GK1_EXEC0317_WIN64_CI231_QA_ONLY_NOT_RELEASE.zip`; then `python3 test_verify_ci231.py`.

No new original-stock ROM replay or Linux C11 run here. Prior authentic fresh/archived stock observations stay distinct. Original first playable Spring, 3 genuine controller orders and complete campaigns remain OPEN. Physical Xbox NOT RUN. QA-only, NOT RELEASE. No licensed ROM, RAM, audio, sprites, checkpoints or user saves are in this directory.
