# GK1 EXEC0284 — independently verified Win32 CI receipt

Date: 2026-11-14. Full inherited C11 1280×720 Windows WinMain app.
This extends the established EXEC0284 increment, not a duplicate execution number.

**Source implementation**:
- Original EXEC0284 `stage284.py` and `GK1_EXEC0284_Original_C289_AllHistory_Integration.patch` were zero bytes upon inspection; that was an actual publication/reproduction gap. The existing reviewed C/H and regression remained intact.
- Commit `4e5a6ed358bb4aa31a13556fce8e69a3c4063165` supplies executable stage284.py, authentic prior C/H SHA-256 preimage validation, three reviewed Git blob SHA checks, and fail-closed stage.
- Commit `5bfb487341f85dcbc55640ac184756de1eadcc80` adds four hostile Python staging and rollback checks, locally executed **4/4 PASS**.
- Commit `8a99df069af2988eb3a677bfa60481acf2479563` updates the **actual** `.github/workflows/gk1-native-windows.yml`: C289 all-history code and source-authenticated native CTest linked into complete game, release/installer and older hostile checks retained.

**Direct completed Windows execution evidence**:
- GitHub Actions https://github.com/Bomb-Daddy/NES/actions/runs/43624467156 head SHA `8a99df069af2988eb3a677bfa60481acf2479563`, conclusion **SUCCESS** on Windows Server 2022, November 14, 2026.
- Step 18 exact EXEC0284 C11 staging and 4/4 hostile source tests PASS; step 19 full native Windows compilation PASS.
- Step 20 CTest: **214/214 PASS**, 0 failures, exact `GK1_284_OriginalC289AllHistoryRateCandidateRollback` registered. Public CI tests missing-ROM refusal; NOT an observed authentic private Mesen trace.
- Step 21 WinMain rendering, synthetic-controller and malformed CLI checks PASS; step 22 real NSIS silent install and installed native WinMain launch PASS; artifact publication PASS.
- GitHub artifact ID `14125731860`, ZIP `578672` bytes, SHA-256 `2d9967b3d806cfeaad198d1df9d31c5f0f96f6347693e330620145ab056a59a5`; independently downloaded and ZIP CRC PASS.
- Installer inside ZIP `GK1-Native-Research-1.99.0-win64.exe`: `612269` bytes SHA-256 `d7a87f5b57f90725dadcda0f1369a01851407ba5330ac2e162cb93422c5240fa`.
- Manifest installed executable SHA-256 `fe7b5d997369f8e45705ff3bd5a0f4b44467a192920c1251acec4e4e2b61d582`. No private original NES ROM or licensed assets in installer.
- Independent Collaboration Google Doc with reproducibility: https://docs.google.com/document/d/1KL0oRRHQBI3NVfqf6BRcguOaixcO1ISZ1ZXnK5VZPew/edit, parent folder `1d0TA1lgqsVA3AWQQ9o0FZgmvao90oCeT`, fetched/read back.
- Direct binary upload to Drive **BLOCKED_FILE_REFERENCE** (connector egress restriction), so do NOT claim ZIP itself was uploaded to Drive; GitHub artifact and user-facing session copy exist.

**Classification**:
VERIFIED_STATIC: source-gated GitHub changes, local 4/4 hostile tests, Drive document readback PASS.
TESTED_PORT_WIN32_CI: 214/214 CTests, GUI/software-controller/NSIS PASS.
TESTED_PORT_LINUX: inherited independently verified EXEC0284 227/227 eligible; not rerun here.
OBSERVED_RUNTIME_STOCK_FRESH_RESET: earlier original WorkRAM Spring1175 only; no new emulator run.
OBSERVED_RUNTIME_STOCK_ARCHIVED_RESTORED: earlier original SaveRAM Spring1175 & World Spring1206; no new emulator run.
NOT RUN: physical Xbox, both complete campaigns, independently generated authentic first Spring.
Original stock ROM remains private; the history association is possible, not proven causal. First Spring, Trade, AI, diplomacy/dynasty, battle aftermath and full campaigns remain OPEN. No accepted release, no Director stop, and no implementer read acknowledgement claimed.
