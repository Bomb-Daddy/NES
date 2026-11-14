# Genghis Khan I — EXEC0271: exact EXEC0270 native Win32 game integration

This is a reproducible source/CI increment for the **existing complete inherited** C11 native Windows/Linux Genghis Khan I game. It does not recreate the game in a prototype. The Classic stock-NES evidence remains distinct from Enhanced Shared-BP/Rest.

## Bug and provenance

The formerly successful Windows CI compiled the **EXEC0269** game while the newer **EXEC0270** Linux app had fixed Classic controller behavior: navigate to another country or change read-only VIEW after initially arming Pass, and the pending Pass is canceled; deliberately pressing A again to arm followed by A to confirm still operates. A stale first confirmation must not unexpectedly consume the three original Spring orders.

The Windows workflow now requires the old app source SHA256 `a0edd0a5dc4974a17485320368c6613240785a2c813a941dcaef527fd632c05c`, exactly applies the three reviewed source anchors with `apply_gk271_win32_270.py`, and requires the new complete-app SHA256 `ee00ef0fc03676d09edfef9ca630acb9e605f128e5af5a66b1eafb2e86c2dc68`. It copies the published `execution_270_classic_view_pass_navigation/test_gk270_view_pass_navigation.c` with SHA256 `490c9d476ffd272b5e0dcd507c19c241556df1b7d5aa4d417ae10ffb6e450f4c`, links against `gk1_app_state`, and registers native CTest `GK1_270_ControllerViewNavigationDisarmsPendingPass`.

A Git-for-Windows `git apply --include` initially returned success without actually changing the C11 app; the postimage hash gate caught it (run [43554403732](https://github.com/Bomb-Daddy/NES/actions/runs/43554403732)). The new exact SHA-bounded source transform fixes that false positive, and its hostile self-test tests corrupted preimage, anchor drift, altered source and duplicate replay.

The newer game's SHA necessarily breaks the historical EXEC0265 source-provenance CTest, which pinned the old app SHA even though the 0265 restart behavior remains intact. The live workflow therefore upgrades the **staged copy only** of its precise expected SHA while retaining all its ten negative controls, the historical test264 and source checks. It does not disable or exclude that gate.

## Exact source and execution

- **Live workflow**: [`.github/workflows/gk1-native-windows.yml`](../../../..//.github/workflows/gk1-native-windows.yml); the ZIP's historical workflow snapshot is not the authoritative live version.
- **Source transformation**: [`apply_gk271_win32_270.py`](apply_gk271_win32_270.py).
- **Hostile workflow test**: [`test_gk271_win32_0270_provenance.py`](test_gk271_win32_0270_provenance.py).
- **Inherited gameplay diff/test**: [EXEC0270](../execution_270_classic_view_pass_navigation).

Windows GitHub Actions must complete compilation, all CTests (including 264, 265, 266, 270), WinMain software controller and paint tests, NSIS packaging, and installed-app launch. The unmodified base ARCHIVE has 1,092 inherited entries preserved byte-identically; local copyright-clean EXEC0271 source/QA artifact additionally includes these scripts and QA metadata, not the private original ROM.

Linux native Release was built, and **220/220 bounded applicable CTests** passed (excluding only historical stale Win32 workflow test GK1_257 and known four-process X11 hang GK1_226). Software-generated controller testing is **not** physical Xbox acceptance.

## Boundaries

No new original NES/Mesen execution here. Previously authenticated fresh Spring 1175 and archived Spring1175/World1206 are distinct histories; no single screenshot is a universal initializer. Independent authentic first Spring character/difficulty/RNG and full 14/27-country campaigns, autonomous AI, original Trade merchant RNG, diplomacy/dynasty, full war aftermath, audio/visuals, physical Xbox hardware and accepted release are still open. Public GitHub, this README and QA artifact contain **no original ROM, Mesen state, private RAM, saves, licensed assets or audio**.

[Numbered implementer/QA handoff](https://docs.google.com/document/d/1MqnaHFcTdICwaJBLTbpt2SWbe807OByQHfHR8OXXaDM/edit) · [Cumulative addendum v4.20](https://docs.google.com/document/d/1xyvKXo-vcnP4Q8QwydHbK2HbqOD0DTiJ3tv9ZIw_n-U/edit) · [Completion ledger](https://docs.google.com/document/d/1B1ucHjxnsgLeHpzXMtu8PmfFeNIYf3uyivyU17sXubU/edit).
