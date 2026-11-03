# KOEI Classic / Enhanced — Build 08

**Only game choices:** Classic and Enhanced. Enhanced is the full 41-province expanded original-game source, not the retired former Enhanced version or a third title. In Classic the original NES game is executed in the MesenCE emulator (or a configured preexisting Classic build) with the player's own lawful ROM. The ROM is never distributed as part of this package.

## Linux
`./PLAY_LINUX.sh` starts the two-choice launcher. `./INSTALL_LINUX.sh` installs under `~/.local/share/koei-classic-enhanced` and creates a launcher desktop entry. Install `python3-tk`, Python 3.11+ and SDL2 via the distribution if missing; the installer automatically checks Pillow and installs it in an isolated runtime when needed. Full independent Linux executable: `./BUILD_LINUX_EXE.sh` (PyInstaller downloads its own build dependencies and verified MesenCE).

## Windows
`PLAY_WINDOWS.bat` starts a prebuilt full Windows package where one exists, otherwise the Python launcher. `BUILD_WINDOWS_EXE.bat` on Windows 10/11 builds the full `dist\KOEI_Classic_Enhanced` package, including the expanded game assets, native input and official verified MesenCE distribution. `INSTALL_WINDOWS.bat` installs this package for the current user and creates a desktop shortcut without writing campaign saves under Program Files.

## Source and permissions
Classic uses a user-provided NES ROM. Set `KOEI_CLASSIC_ROM` or choose it from the launcher's Classic configuration. Official MesenCE is GPL-3.0; each bundled build contains a release/source reference and license. The game source includes original retro art and an expanded non-NES ruleset; gameplay and controller fidelity still require actual Windows and physical-controller acceptance testing. Do not call a source package a precompiled Windows binary.

## Developer QA
`python -m pytest -q` runs regression checks. `python -m launcher --verify-modes` must print precisely Classic then Enhanced; `xvfb-run -a python ../build08_live_gui_qa.py` checks a real graphical session in Linux.
