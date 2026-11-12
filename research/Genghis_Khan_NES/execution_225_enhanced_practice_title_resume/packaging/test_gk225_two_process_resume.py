#!/usr/bin/env python3
"""EXEC0225 X11 evidence: two separate app processes and disk corruption gate.

Software-injected controller events through native input adapter, not hardware Xbox.
"""
import hashlib
import os
from pathlib import Path
import subprocess
import sys
import tempfile


def run(executable, action, env):
    command = ['xvfb-run', '-a', '-s', '-screen 0 1280x720x24',
               executable, action, '120']
    result = subprocess.run(command, env=env, capture_output=True,
                            text=True, timeout=45)
    if result.returncode or 'PASS' not in result.stdout:
        print('FAIL:', action, result.returncode, result.stdout, result.stderr)
        return False
    return True


def main():
    if len(sys.argv) != 2:
        return 2
    with tempfile.TemporaryDirectory(prefix='gk225_two_process_') as scratch:
        slot = Path(scratch) / 'source-locked.gk24'
        env = os.environ.copy()
        env['GK1_ENHANCED_PRACTICE_SAVE_FILE'] = str(slot)
        if not run(sys.argv[1], '--smoke-enhanced-practice-save', env):
            return 1
        original = slot.read_bytes()
        if len(original) != 80 or original[:8] != b'GK1P224!' or \
                hashlib.sha256(original[:48]).digest() != original[48:]:
            return 3
        # Fresh application process must enter via TITLE Y, not the saved process.
        if not run(sys.argv[1], '--smoke-enhanced-practice-title-resume', env):
            return 4
        if slot.read_bytes() != original:
            return 5
        altered = bytearray(original)
        altered[19] ^= 0x80
        slot.write_bytes(altered)
        if not run(sys.argv[1], '--smoke-enhanced-practice-title-reject', env):
            return 6
        if slot.read_bytes() != altered:
            return 7
    print('EXEC0225 actual 3 separate X11 processes + controller path + SHA corruption rejection PASS; no physical Xbox/Win32/NES parity')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
