#!/usr/bin/env python3
"""EXEC0059 actual extracted Linux .deb path/contract/cold-load QA.
Not original NES, Windows, or physical Xbox proof.
"""
import hashlib
import os
from pathlib import Path
import subprocess
import sys
import tempfile

ZIP_DATASET_SHA = 'fa3958f156f09d4b4c15bb559c12aa357fc617a9b1f9217505fe07e8800de581'
if len(sys.argv) != 2:
    raise SystemExit('usage: test_installed_layout.py PACKAGE.deb')
archive = Path(sys.argv[1]).resolve()
assert archive.is_file()
with tempfile.TemporaryDirectory(prefix='gk1_059_installed_') as t:
    base = Path(t)
    subprocess.run(['dpkg-deb', '-x', str(archive), str(base)], check=True, capture_output=True, text=True)
    exe = base / 'opt/gk1-native-research/gk1_native_shell'
    csv = base / 'opt/gk1-native-research/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv'
    launcher = base / 'usr/bin/gk1-native-research'
    desktop = base / 'usr/share/applications/gk1-native-research.desktop'
    readme = base / 'usr/share/doc/gk1-native-research/README_EXEC0059.md'
    readme_60 = base / 'usr/share/doc/gk1-native-research/README_EXEC0060.md'
    readme_62 = base / 'usr/share/doc/gk1-native-research/README_EXEC0062.md'
    readme_77 = base / 'usr/share/doc/gk1-native-research/README_EXEC0077.md'
    readme_78 = base / 'usr/share/doc/gk1-native-research/README_EXEC0078.md'
    readme_79 = base / 'usr/share/doc/gk1-native-research/README_EXEC0079.md'
    readme_80 = base / 'usr/share/doc/gk1-native-research/README_EXEC0080.md'
    readme_86 = base / 'usr/share/doc/gk1-native-research/README_EXEC0086.md'
    assert all(p.is_file() for p in (exe, csv, launcher, desktop, readme, readme_60, readme_62, readme_77, readme_78, readme_79, readme_80, readme_86, base / "usr/share/doc/gk1-native-research/README_EXEC0089.md", base / "usr/share/doc/gk1-native-research/README_EXEC0090.md", base / "usr/share/doc/gk1-native-research/README_EXEC0091.md", base / "usr/share/doc/gk1-native-research/README_EXEC0092.md", base / "usr/share/doc/gk1-native-research/README_EXEC0093.md", base / "usr/share/doc/gk1-native-research/README_EXEC0094.md"))
    assert all(os.access(p, os.X_OK) for p in (exe, launcher))
    assert hashlib.sha256(csv.read_bytes()).hexdigest() == ZIP_DATASET_SHA
    assert 'Name=Genghis Khan I Research 1.19.0-dev1' in desktop.read_text()
    for wrong in ('share/applications/gk1-native-research.desktop', 'bin/gk1-native-research'):
        assert not (base / wrong).exists(), wrong
    data = base / 'isolated_user_data'
    data.mkdir()
    env = os.environ.copy()
    env['XDG_DATA_HOME'] = str(data)
    env['GK1_RESEARCH_BINARY_OVERRIDE'] = str(exe)
    result = subprocess.run([str(launcher), '--slot-export', 'contract.gkp', '2', '2', '1', '3'],
                            env=env, cwd=base, capture_output=True, text=True, timeout=15)
    assert result.returncode == 0, (result.stdout, result.stderr)
    slot = data / 'gk1-native-research/contract.gkp.slot2'
    assert slot.is_file() and len(slot.read_bytes()) == 2734
    assert (slot.stat().st_mode & 0o777) == 0o600
    check = subprocess.run([str(launcher), '--slot-import', 'contract.gkp', '2', '2', '1'],
                           env=env, cwd=base, capture_output=True, text=True, timeout=15)
    assert check.returncode == 0, (check.stdout, check.stderr)
    assert 'troops=112 town=8' in check.stdout
    collision = subprocess.run([str(launcher), '--slot-export', 'contract.gkp', '2', '2', '1', '3'],
                               env=env, cwd=base, capture_output=True, text=True, timeout=15)
    assert collision.returncode == 6
    assert not (base/'contract.gkp.slot2').exists()  # no CWD leakage
    print('EXEC0111 TESTED_PORT_LINUX extracted Debian layout, user-data isolation, cold-load, collision: PASS')
