# Private sound and music integration — v1.13

**Separate original KOEI-inspired third branch only.** Install both companion ZIP archives into the SAME parent directory so all files share the `KOEI_Original_Third_Branch_v1_13/` project root. The CODE ZIP includes all 14 original generated WAV sound effects and code; the PRIVATE MUSIC ZIP contains 15 unchanged Director-provided 1989 Soundware MP3 recordings. Music archive is **PRIVATE-USE**, not cleared for public distribution.

## Start

* Windows: `py -3 -m pip install -r requirements-audio.txt` (installs Pillow + pygame-ce), then `Play_Third_Branch_PC.bat`. On supported Windows platforms, WinMM/MCI may also play MP3 without pygame; **not tested on Windows hardware**.
* Linux: `python3 -m pip install -r requirements-audio.txt`, then `./Play_Third_Branch_Linux.sh`. With no pygame, FFplay is a fallback if installed.
* If album or playback backend is missing, gameplay continues and **NES START > Audio Status** explains why music cannot play. No gameplay save state depends on a sound device.

## Controls

NES START > Music: Arranged / Off, Sound Effects: On / Off, Audio Status, Soundtrack Library (15 tracks). All options are controller-accessible via D-pad, A and B. Library pages 1-8 and 9-15 can play all supplied recordings. Arranged map music changes across five experimental geographic region groups. Battles use track 14; diplomacy/marriage have distinct cues when the real event is committed; game-map BGM restores after a battle closes. Short **newly-generated original** game sound effects occur for navigation, confirmation, cancel, invalid orders, trade, occupation, battle, save/load, and turn change. They are not original NES SFX, and the arranged orchestral region placement is not NES parity.

## Test and rights

Run `python -m pytest -q` from the project root for the full regression suite. Run `python -c "from third_branch.audio_system import verify_private_assets; print(verify_private_assets())"`; missing/changed MP3s or generated WAV files are reported. No installed audio device is inferred from code/static checks. Linux FFplay decoding with dummy audio, real Tk/Xvfb GUI controls, missing file/backend negative controls, and hash verification were tested. Windows audio hardware, controller physical sound output, public Soundware distribution rights, event timing for every narrative ending, and original NES sound effect fidelity were **NOT** tested/cleared.

Audio remains outside deterministic saved-world state. Old saves load without data mutation; changes to music settings are session-local.
