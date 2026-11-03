#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
python3 -m pip install -r requirements-windows-build.txt
python3 tools/bundle_mesen.py
python3 -m PyInstaller --noconfirm --clean --onedir --windowed --name KOEI_Classic_Enhanced \
 --add-data "third_branch/assets:third_branch/assets" \
 --add-data "third_branch/data:third_branch/data" \
 --add-data "config:config" \
 --add-data "third_party:third_party" \
 --collect-all PIL --hidden-import PIL.ImageTk --hidden-import tkinter \
 --hidden-import third_branch.strategic_game windows_entry.py
printf '%s\n' 'Bundled Linux app is dist/KOEI_Classic_Enhanced/KOEI_Classic_Enhanced'
