# EXEC0054 — Native Linux installer for inherited GK1 research build

This increments the **existing** Genghis Khan I English NES-to-Windows C11 app from EXEC0053. It does not create a second game and is **NOT** a finished two-campaign game. Classic and Enhanced research profiles remain distinct. The pre-setup economic CSV stays byte-exact; no ROM, audio or image assets included.

Actual deliverable: version-labelled, controller-only Debian package with a desktop menu entry. It installs the native application and verified country CSV **adjacent** under `/opt/gk1-native-research`. A launcher at `/usr/bin/gk1-native-research` selects `~/.local/share/gk1-native-research` (or `$XDG_DATA_HOME/gk1-native-research`) for the working directory so exclusive-create research drafts and replays are not attempted in a read-only install folder. Offline game startup does not need a GitHub or Drive connection.

Run via Linux application menu (Genghis Khan I Research 0.55.0-dev1), or `gk1-native-research`. Only joystick controller input works in the game; Linux joystick device `/dev/input/js0` or configured device is expected. `gk1-native-research --version` is diagnostic only, not a keyboard gameplay path. `gk1-native-research --scenario-report 1|2` dumps bounded PRE_SETUP roster data for QA.

This is `TESTED_PORT_LINUX` **only**. No Windows executable compiled, no installed Windows game or physical Xbox controller tested, no native NES trace executed. Campaign setup, first Spring, combat AI, authentic audio/video, and complete saving remain open. Do not install this as a claim the Director's full game is finished.

Build: `cmake -S execution_054 -B build54 -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build54 && ctest --test-dir build54 --output-on-failure && cpack --config build54/CPackConfig.cmake -B build54`.

Hostile QA: extract .deb with `dpkg-deb -x`; verify executable + original CSV hash; verify the `.desktop` Exec and visible version; launch from an unrelated working directory with a temporary XDG data directory and `GK1_RESEARCH_BINARY_OVERRIDE` set to the extracted executable; confirm save directory and nonoverwrite error, and corrupt CSV early failure. This does not use a real Xbox device or Windows installation.
