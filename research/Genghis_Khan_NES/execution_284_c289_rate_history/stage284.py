#!/usr/bin/env python3
"""EXEC0284 complete C11 rate-history source staging for the inherited Win32 game.

Run AFTER EXEC0283 has linked the original C289 validator. Fail closed on stale
app source and changes to reviewed 0284 files. No ROM/game-state data involved.
"""
from __future__ import annotations
import hashlib
from pathlib import Path
import sys

PRE = {
    'gk282_stock_rng_trace.c': '659a5ca4c1ea4c2299bf77da1407ed21932b975827db315e3ca86b5b3672f1e5',
    'gk282_stock_rng_trace.h': 'c6d3d9be633a25766689b3d439ab85cc10bbb6c605513d651a4c31f3af882e44',
}
POST_GIT_BLOB = {
    'gk282_stock_rng_trace.c': 'ba172f4da23b8213e9a8210028f03aba8aca4657',
    'gk282_stock_rng_trace.h': '926c7da459e2af5baf82e28fcf67bf6d3ff4c584',
    'test_gk284_rate_history.c': '2ae54dd993dbcb36374bde58b9b4f20a80c9d4d8',
}
REGISTRATION = '''
# EXEC0284: bounded all-history C289 Rate candidate regression (no-ROM refusal in public CI).
add_executable(test_gk284_rate_history test_gk284_rate_history.c)
target_link_libraries(test_gk284_rate_history PRIVATE gk1_app_state)
add_test(NAME GK1_284_OriginalC289AllHistoryRateCandidateRollback
 COMMAND test_gk284_rate_history
 "$ENV{GK1_PRIVATE_ORIGINAL_ROM}" "${CMAKE_CURRENT_BINARY_DIR}/gk284_rate_history.tmp")
'''

def normalized(path: Path) -> bytes:
    return path.read_bytes().replace(b'\r\n', b'\n')

def blob_id(data: bytes) -> str:
    return hashlib.sha1(b'blob ' + str(len(data)).encode('ascii') + b'\0' + data).hexdigest()

def stage(dest: Path, sources: Path) -> None:
    if not dest.is_dir():
        raise ValueError('EXEC0284 target inherited native C11 game missing')
    cmake = dest / 'CMakeLists.txt'
    cm = normalized(cmake)
    if b'gk282_original_c289_trace' not in cm or b'gk282_stock_rng_trace.c' not in cm:
        raise ValueError('EXEC0283 authentic validator must already be linked')
    if b'GK1_284_OriginalC289AllHistoryRateCandidateRollback' in cm or (dest / 'test_gk284_rate_history.c').exists():
        raise ValueError('EXEC0284 duplicate/stale staging refused')
    # Preflight EVERY source, CMake, and preimage before making ANY writes.
    replacement = {}
    for name, expected in POST_GIT_BLOB.items():
        p = sources / name
        b = normalized(p)
        if blob_id(b) != expected:
            raise ValueError(f'EXEC0284 reviewed source changed: {name}')
        replacement[name] = b
        dst = dest / name
        if name in PRE:
            if hashlib.sha256(normalized(dst)).hexdigest() != PRE[name]:
                raise ValueError(f'EXEC0284 inherited preimage mismatch: {name}')
        elif dst.exists():
            raise ValueError(f'EXEC0284 new test already exists: {name}')
    if cm.count(b'add_test(NAME gk282_original_c289_trace') != 1:
        raise ValueError('EXEC0283 prior C289 CTest missing/ambiguous')
    if b'target_link_libraries(test_gk282_stock_rng_trace PRIVATE gk1_app_state)' not in cm:
        raise ValueError('EXEC0283 trace test not linked to native game')
    # Atomic for all expected preflight failures; overwrite only exact inherited
    # C/H and add one test. No source payload, ROM or private capture is copied.
    for name, data in replacement.items():
        (dest / name).write_bytes(data)
    cmake.write_bytes(cm + REGISTRATION.encode('utf-8'))
    for name, expected in POST_GIT_BLOB.items():
        if blob_id(normalized(dest / name)) != expected:
            raise AssertionError(f'EXEC0284 source changed during staging: {name}')
    if (dest / 'CMakeLists.txt').read_bytes().count(b'GK1_284_OriginalC289AllHistoryRateCandidateRollback') != 1:
        raise AssertionError('EXEC0284 CTest registration failed')
    print('EXEC0284 native C11 all-history source and CTest staged; original Spring NOT generated')

if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit('Usage: python stage284.py current-gk1/execution_199')
    here = Path(__file__).resolve().parent
    try:
        stage(Path(sys.argv[1]), here)
    except (OSError, ValueError, AssertionError) as exc:
        print(str(exc), file=sys.stderr)
        raise SystemExit(1)
