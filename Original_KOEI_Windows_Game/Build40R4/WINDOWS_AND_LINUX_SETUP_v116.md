# Windows and Linux launch / v1.16

This is a **Python-source cross-platform candidate**, not a certified Windows binary. 

Windows: unzip the full package; double-click `PLAY_WINDOWS.bat`. This creates a local
`.venv`, installs Pillow + pygame-ce (network needed once) and verifies music/SFX,
writable user save path, Tk, and audio backend before launching. Python 3.11+ with
Tk is required. Use `BUILD_WINDOWS_EXE.bat` on a Windows machine with Python
and internet access to produce `dist/KOEI_Third_Branch/KOEI_Third_Branch.exe`.
Do NOT build a Windows exe on Linux and claim it was tested on Windows.

Linux: `python3 -m pip install -r requirements-audio.txt`, then `./PLAY_LINUX.sh`
(or `./Play_Third_Branch_Linux.sh`). A display server is required. 

Default save: Windows `%LOCALAPPDATA%/KOEI_Original_Third_Branch/saves/`, Linux
`~/.local/state/koei-original-third-branch/saves/`; explicit `--save` paths and
pre-existing saves in the working directory take priority. Compatible old save
formats are kept. Attack opens the same gamepad-driven preview/commit order UI
for mouse and Xbox input; no keyboard-only integer prompt.

**Acceptance not yet met:** Physical Windows launch, actual sound through speakers,
Xbox hardware, Windows executable on real PC and full controller-only UI paths
still require execution on Windows. This source does not certify those gates.
