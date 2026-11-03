# Third-branch original KOEI game — v1.16 Windows/Linux portability repair

- Source v1.15 is retained unmodified in previous Drive checkpoint.
- Windows `PLAY_WINDOWS.bat` sets up a per-user virtual environment; source still requires Python 3.11+ (and internet for first dependency install). Native Windows executable is **not** supplied or claimed tested. `BUILD_WINDOWS_EXE.bat` uses PyInstaller on native Windows to build one.
- `PLAY_LINUX.sh` and existing Linux launcher remain usable.
- Default campaign saves now use writable per-user AppData/XDG locations, except pre-existing local saves are recognized; explicit `--save` overrides remain supported. Save/cold-load regression passes.
- Main Attack action now uses gamepad-operable Orders preview/commit (mouse and controller) instead of a keyboard-only Windows `simpledialog`.
- Adviser, family, and journal text panels now have controller scroll/close (A, B, Start) paths.
- System-DPI awareness requested on Windows to keep pointer positions aligned with fixed 1280x720 rendering.
- Pygame-ce preferred for audio; WinMM MCI is the Windows music fallback with `winsound` SFX; status/diagnostics explicit. Regression tests include mocked WinMM routing, not physical speakers.
- All 15 MP3 recordings, 14 WAV effects, 28 ruler portraits, 23 illustrations, Scenario 1 historical province/officer data and v1.15 monthly event rules are preserved.

## Honest acceptance boundary

`343 passed, 2 expected failures, 3 subtests passed` on Linux 3.13; actual Tk/Xvfb 1280x720 source run with controller-simulated Attack, advisor close, save/load PASS; strict local doctor PASS; Windows paths/audio fallback tested at logic level with mocks. **NOT RUN:** Windows 10/11 process launch, actual Windows `.exe` build, physical Xbox controller, speaker audio, 12x120 months with v1.15 event rates. The current source is a Windows-targeted release candidate, **not** a verified finished Windows distribution. PyInstaller cannot cross-compile Windows binaries from Linux.
