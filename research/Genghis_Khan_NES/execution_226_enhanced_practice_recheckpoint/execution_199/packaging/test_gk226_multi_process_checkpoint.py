#!/usr/bin/env python3
"""Four distinct real X11 processes. Software pad events, not physical Xbox."""
import hashlib
import os
from pathlib import Path
import subprocess
import sys
import tempfile


def smoke(binary, flag, env):
    p = subprocess.run(['xvfb-run', '-a', '-s', '-screen 0 1280x720x24',
                        binary, flag, '120'], env=env,
                       capture_output=True, text=True, timeout=60)
    if p.returncode or 'PASS' not in p.stdout:
        raise AssertionError((flag, p.returncode, p.stdout, p.stderr))


def verify(path, actions):
    raw = path.read_bytes()
    assert len(raw) == 80 and raw[:8] == b'GK1P224!'
    assert raw[10] == actions
    assert hashlib.sha256(raw[:48]).digest() == raw[48:]
    return raw


def main():
    if len(sys.argv) != 2:
        return 2
    with tempfile.TemporaryDirectory(prefix='gk226_fourprocess_') as directory:
        slot = Path(directory) / 'first.gk24'
        env = dict(os.environ, GK1_ENHANCED_PRACTICE_SAVE_FILE=str(slot))
        smoke(sys.argv[1], '--smoke-enhanced-practice-save', env)
        first = verify(slot, 1)
        smoke(sys.argv[1], '--smoke-enhanced-practice-title-update', env)
        second = verify(slot, 2)
        assert first != second
        smoke(sys.argv[1], '--smoke-enhanced-practice-title-verify', env)
        third = verify(slot, 3)
        assert third != second
        smoke(sys.argv[1], '--smoke-enhanced-practice-title-final', env)
        assert verify(slot, 3) == third
        assert list(Path(directory).glob('*.gk226.*')) == []
    print('EXEC0226 four independent native X11 processes: create/overwrite/overwrite/cold-replay PASS; controller software injection, Enhanced only')
    return 0


if __name__ == '__main__':
    sys.exit(main())
