#!/bin/sh
# Builds KOEI_Classic_Enhanced_Setup.exe on Linux (needs: nsis, python3, pip, network).
# usage: build_installer.sh WORKDIR     Output: WORKDIR/KOEI_Classic_Enhanced_Setup.exe
set -eu
SRC=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
W=${1:?workdir}
PBS_URL=https://github.com/astral-sh/python-build-standalone/releases/download/20241206/cpython-3.12.8+20241206-x86_64-pc-windows-msvc-install_only.tar.gz
PBS_SHA=767b4be3ddf6b99e5ade519789c1615c191d8cf99d5aff4685cc18b48931f1e6
mkdir -p "$W/dl" "$W/stage"; rm -rf "$W/stage/python" "$W/stage/app"
[ -f "$W/dl/py.tgz" ] || curl -sL -o "$W/dl/py.tgz" "$PBS_URL"
echo "$PBS_SHA  $W/dl/py.tgz" | sha256sum -c -
tar xzf "$W/dl/py.tgz" -C "$W/stage"
P="$W/stage/python"
rm -rf "$P/include" "$P/libs" "$P/Scripts" "$P/Lib/test" "$P/Lib/idlelib" "$P/Lib/ensurepip" "$P/Lib/turtledemo" "$P/Lib/lib2to3" "$P/Lib/site-packages/pip" "$P/Lib/site-packages/setuptools"
find "$P" -name '*.pdb' -delete
rm -rf "$P/tcl/tix8.4.3" "$P/tcl/nmake" "$P/tcl/tcl8.6/tzdata" 2>/dev/null || true
mkdir -p "$W/whl"
pip download --platform win_amd64 --python-version 3.12 --only-binary=:all: --no-deps -d "$W/whl" "Pillow>=10,<13" "pygame-ce>=2.5.8,<3" >/dev/null
for w in "$W"/whl/*.whl; do python3 -c "import zipfile,sys;zipfile.ZipFile(sys.argv[1]).extractall(sys.argv[2])" "$w" "$P/Lib/site-packages"; done
A="$W/stage/app"; mkdir -p "$A"
(cd "$SRC" && tar --exclude='__pycache__' --exclude='*.pyc' --exclude='./tests' --exclude='./qa_tools_20261102' --exclude='./historical_reference_only' --exclude='./.pytest_cache' -cf - launcher og config third_branch fork4 windows_entry.py requirements.txt 2>/dev/null || true) | tar -xf - -C "$A"
cp "$SRC/tools/windows_installer/run_game.pyw" "$A/"
mkdir -p "$A/tools" && cp "$SRC/tools/bundle_mesen.py" "$A/tools/"
[ -f "$W/dl/Mesen_2.2.1_Windows.zip" ] || curl -sL -o "$W/dl/Mesen_2.2.1_Windows.zip" https://github.com/nesdev-org/MesenCE/releases/download/2.2.1/Mesen_2.2.1_Windows.zip
(cd "$A" && python3 -c "
import sys; sys.path.insert(0,'tools')
import bundle_mesen
bundle_mesen.install('windows', download=lambda u: open('$W/dl/Mesen_2.2.1_Windows.zip','rb').read())
")
rm -rf "$A/tools"
cp "$SRC/tools/windows_installer/setup.nsi" "$W/setup.nsi"
(cd "$W" && makensis -V2 setup.nsi)
sha256sum "$W/KOEI_Classic_Enhanced_Setup.exe"
