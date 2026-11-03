"""Keep campaign saves out of read-only installation folders (Windows Program Files)."""
from __future__ import annotations
import os
import sys
from pathlib import Path

DEFAULT_SAVE_NAME = 'new_branch_41_save.json'

def default_save_path(platform=None, environ=None, home=None, legacy_dir=None):
    platform = sys.platform if platform is None else platform
    environ = os.environ if environ is None else environ
    home = Path.home() if home is None else Path(home)
    # Preserve saves created by older releases, rather than abandoning them.
    legacy_dir = Path.cwd() if legacy_dir is None else Path(legacy_dir)
    legacy = legacy_dir / DEFAULT_SAVE_NAME
    if legacy.is_file():
        return legacy
    if platform == 'win32':
        base = Path(environ.get('LOCALAPPDATA') or home / 'AppData' / 'Local')
        return base / 'KOEI_Original_Third_Branch' / 'saves' / DEFAULT_SAVE_NAME
    base = Path(environ.get('XDG_STATE_HOME') or home / '.local' / 'state')
    return base / 'koei-original-third-branch' / 'saves' / DEFAULT_SAVE_NAME

def prepare_save_path(path):
    path = Path(path).expanduser()
    path.parent.mkdir(parents=True, exist_ok=True)
    return path
