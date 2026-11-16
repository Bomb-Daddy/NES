#!/usr/bin/env python3
"""Conservative EXEC0329 staging into the current inherited Genghis C11 source.
Never overwrites an existing source file, never embeds ROM/archives, and
rolls back the CMake write if any file validation fails. No source generation.
"""
from __future__ import annotations
import hashlib
from pathlib import Path
import os
import sys

HERE = Path(__file__).resolve().parent
PUBLIC_FILES = ('gk329_archived_seed_replay.c','gk329_archived_seed_replay.h',
                'gk329_app_bridge.c','gk329_app_bridge.h',
                'test_gk329_public.c','test_gk329_private.c','test_gk329_app_bridge.c')
REQUIRED = ('CMakeLists.txt','gk1_app_state.c','gk1_app_state.h',
            'gk326_sha256.c','gk326_sha256.h',
            'gk327_fixed6502_rng.c','gk327_fixed6502_rng.h',
            'gk139_mesen_state.c','gk139_mesen_state.h',
            'gk137_stock_rng.c','gk137_stock_rng.h')
MARKER = '# EXEC0329 authenticated archived original CPU RNG replay'
APPEND = '\n' + MARKER + '''
target_sources(gk1_app_state PRIVATE gk329_archived_seed_replay.c gk329_app_bridge.c)
add_executable(test_gk329_public test_gk329_public.c)
target_link_libraries(test_gk329_public PRIVATE gk1_app_state)
add_test(NAME GK1_329_ArchivedOriginalSourceRngRefusalNoMutation COMMAND test_gk329_public)
add_executable(test_gk329_app_bridge test_gk329_app_bridge.c)
target_link_libraries(test_gk329_app_bridge PRIVATE gk1_app_state)
add_test(NAME GK1_329_RealAppNoForgedSpringAcceptance COMMAND test_gk329_app_bridge)
'''


def stage(target: Path) -> bool:
    target = target.resolve()
    if not target.is_dir():
        raise ValueError('missing application directory')
    for name in REQUIRED:
        if not (target / name).is_file() or not (target / name).stat().st_size:
            raise ValueError('missing required inherited source: ' + name)
    originals = {p.name: p.read_bytes() for p in (HERE / n for n in PUBLIC_FILES)}
    if any(not b or b'\r' in b for b in originals.values()):
        raise ValueError('invalid source newline/empty bytes')
    # Check all local bytes BEFORE any modification. Do not overwrite conflicts.
    missing = []
    for name, raw in originals.items():
        dest = target / name
        if dest.exists():
            if dest.read_bytes().replace(b'\r\n', b'\n') != raw:
                raise ValueError('conflicting inherited EXEC0329 source: ' + name)
        else:
            missing.append(name)
    cmake = target / 'CMakeLists.txt'
    raw_cmake = cmake.read_bytes()
    if b'add_library(gk1_app_state ' not in raw_cmake and b'add_library(gk1_app_state\n' not in raw_cmake:
        raise ValueError('not an inherited application CMake target')
    original_text = raw_cmake.decode('utf-8')
    if MARKER in original_text:
        if APPEND.strip() not in original_text:
            raise ValueError('partial or altered EXEC0329 CMake integration')
        if missing:
            # Existing CMake marker with missing files is invalid. No mutation.
            raise ValueError('partial preexisting integration')
        return False
    created = []
    try:
        for name in missing:
            p = target / name
            with p.open('xb') as f:
                f.write(originals[name])
            created.append(p)
        tmp = target / '.gk329_cmake_pending'
        with tmp.open('xb') as f:
            f.write(raw_cmake + APPEND.encode('utf-8'))
        os.replace(tmp, cmake)
        return True
    except BaseException:
        for p in created:
            p.unlink(missing_ok=True)
        (target / '.gk329_cmake_pending').unlink(missing_ok=True)
        raise


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('usage: stage329.py /path/to/latest/native/app', file=sys.stderr)
        raise SystemExit(2)
    try:
        changed = stage(Path(sys.argv[1]))
    except (OSError, ValueError, UnicodeError) as exc:
        print('EXEC0329 REFUSED: ' + str(exc), file=sys.stderr)
        raise SystemExit(1)
    print('EXEC0329 staged' if changed else 'EXEC0329 already present, unchanged')
