#!/bin/sh
# Installs the full two-mode KOEI source game for the current Linux user.
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
PREFIX=${KOEI_INSTALL_DIR:-"${XDG_DATA_HOME:-$HOME/.local/share}/koei-classic-enhanced"}
PYTHON=${PYTHON:-python3}
"$PYTHON" - <<'PY'
import sys
if sys.version_info < (3,11):raise SystemExit('Python 3.11+ required')
try:import tkinter
except ImportError:raise SystemExit('Install distribution python3-tk then rerun installer')
PY
mkdir -p "$PREFIX" "${XDG_DATA_HOME:-$HOME/.local/share}/applications"
# Never recursively copy our installation directory if installer is run in-place.
if [ "$HERE" != "$PREFIX" ]; then
  cp -a "$HERE"/. "$PREFIX"/
fi
# Use the functioning local Python with Tk and Pillow if both are installed.
# An outer environment may already provide these; never build a nested venv
# which loses its site-packages and requires network for no reason.
if "$PYTHON" -c 'from PIL import Image; import tkinter' >/dev/null 2>&1; then
    PY_RUN=$(command -v "$PYTHON")
else
    if [ ! -x "$PREFIX/.venv/bin/python" ]; then
        "$PYTHON" -m venv "$PREFIX/.venv"
    fi
    if ! "$PREFIX/.venv/bin/python" -c 'from PIL import Image' >/dev/null 2>&1; then
        "$PREFIX/.venv/bin/python" -m pip install -r "$PREFIX/requirements.txt"
    fi
    PY_RUN="$PREFIX/.venv/bin/python"
fi
if ! "$PY_RUN" -c 'from PIL import Image; import tkinter' >/dev/null 2>&1; then
    echo 'Tk/Pillow unavailable; installation incomplete' >&2; exit 1
fi
cat > "$PREFIX/LAUNCH_INSTALLED_LINUX.sh" <<EOF
#!/bin/sh
set -eu
cd "$PREFIX"
exec "$PY_RUN" -m launcher "\$@"
EOF
chmod +x "$PREFIX/LAUNCH_INSTALLED_LINUX.sh"
# The native controller backend and bundled Classic Mesen use SDL2.
if ! ldconfig -p 2>/dev/null | grep -q 'libSDL2-2.0.so'; then
    echo 'SDL2 runtime is required for Xbox controller/Classic on Linux.' >&2
    echo 'Install libsdl2-2.0-0 using your distribution package manager.' >&2
fi
cat > "${XDG_DATA_HOME:-$HOME/.local/share}/applications/koei-classic-enhanced.desktop" <<EOF
[Desktop Entry]
Type=Application
Name=KOEI — Classic / Enhanced
Comment=Exactly two games: Classic NES and expanded Enhanced
Exec=$PREFIX/LAUNCH_INSTALLED_LINUX.sh
Path=$PREFIX
Terminal=false
Categories=Game;StrategyGame;
EOF
chmod +x "$PREFIX/PLAY_LINUX.sh"
echo "Installed KOEI Classic / Enhanced at $PREFIX"
echo "Run with: \"$PREFIX/LAUNCH_INSTALLED_LINUX.sh\""
