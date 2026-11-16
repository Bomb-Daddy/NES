#!/usr/bin/env python3
"""EXEC0327: stage exact copyright-clean source into the already inherited GK1 C11 app.
Never stage a ROM/checkpoint or modify PRE_SETUP to falsely unlock orders.
Source bytes are checked against exact SHA256, normalized only for Windows CRLF.
"""
from __future__ import annotations
import hashlib
import os
from pathlib import Path
import sys

FILES = (
    'gk327_fixed6502_rng.c', 'gk327_fixed6502_rng.h',
    'gk327_app_rng_bridge.c', 'gk327_app_rng_bridge.h',
    'test_gk327_cpu_rng.c', 'test_gk327_app_rng_bridge.c',
)
SHA256 = {  # exact tested LF source, no ROM/private data
    'gk327_fixed6502_rng.c': '4ef3c044d27875c840fd21fad8f18f489965865f9e09593e8b87c3f36b0df4c5',
    'gk327_fixed6502_rng.h': 'bd0c7c4c503aefc474502aefe8b52078a239a2bfbd726de8b9ac2be7211c9d31',
    'gk327_app_rng_bridge.c': '2017585071b2a6adae63867e06321807483586c2d6e8d04cd714aed546c5e930',
    'gk327_app_rng_bridge.h': '1a7f181b73859e4d5021726484f6fc7f59fdf69e7e639525286a392113d51d38',
    'test_gk327_cpu_rng.c': 'cdae1ff65d7e0d96dc44713dd4b1c161244d4eed370a5fa5ba4d3a3f41b75054',
    'test_gk327_app_rng_bridge.c': '6f4587ac81e450333272e961525209fa089f1faeeb5b03aa42c61e6cc872fef0',
}
CMAKE_MARKER = '# EXEC0327: source-executed original fixed-bank RNG, still NO first playable Spring.'
CMAKE_APPEND = '''
%s
target_sources(gk1_app_state PRIVATE gk327_fixed6502_rng.c gk327_app_rng_bridge.c)
add_executable(test_gk327_cpu_rng test_gk327_cpu_rng.c)
target_link_libraries(test_gk327_cpu_rng PRIVATE gk1_app_state)
add_test(NAME GK1_327_BoundedFixedBank6502InterpreterAndRollback COMMAND test_gk327_cpu_rng)
add_executable(test_gk327_app_rng_bridge test_gk327_app_rng_bridge.c)
target_link_libraries(test_gk327_app_rng_bridge PRIVATE gk1_app_state)
add_test(NAME GK1_327_ActualGk1AppSourceRngNoOrdersAndRollback COMMAND test_gk327_app_rng_bridge)
''' % CMAKE_MARKER

def canonical(data: bytes) -> bytes:
    if not data or b'\x00' in data or b'\r\r\n' in data:
        raise ValueError('missing/binary/malformed exact source')
    clean = data.replace(b'\r\n', b'\n')
    if b'\r' in clean:
        raise ValueError('bare CR is not canonical LF source')
    return clean

def prepare(source_dir: Path, app: Path) -> tuple[dict[str, bytes], bytes]:
    if not app.is_dir() or not (app/'gk1_app_state.h').is_file():
        raise ValueError('not the inherited Gk1App native source')
    if not (app/'gk326_sha256.c').is_file() or not (app/'gk326_sha256.h').is_file():
        raise ValueError('EXEC0326 SHA module must be present from predecessor')
    cmake_path = app/'CMakeLists.txt'
    cmake = cmake_path.read_bytes()
    if b'\x00' in cmake or b'add_library(gk1_app_state STATIC' not in cmake:
        raise ValueError('inherited CMake native library anchor missing')
    if CMAKE_MARKER.encode() in cmake or b'GK1_327_' in cmake:
        raise ValueError('duplicate integration refused')
    files: dict[str, bytes] = {}
    for name in FILES:
        if (app/name).exists():
            raise ValueError('refusing to overwrite a game source file: '+name)
        raw = canonical((source_dir/name).read_bytes())
        if hashlib.sha256(raw).hexdigest() != SHA256[name]:
            raise ValueError('source SHA256 mismatch: '+name)
        files[name] = raw
    return files, cmake + CMAKE_APPEND.encode()

def stage(source_dir: Path, app: Path) -> None:
    files, new_cmake = prepare(source_dir, app)  # check entire set before mutation
    old_cmake = (app/'CMakeLists.txt').read_bytes()
    touched: list[Path] = []
    try:
        for name, data in files.items():
            dst = app/name
            with dst.open('xb') as f:
                f.write(data)
            touched.append(dst)
            if dst.read_bytes() != data:
                raise OSError('source write readback mismatch')
        tmp = app/'CMakeLists.txt.exec327-temp'
        with tmp.open('xb') as f:
            f.write(new_cmake)
        touched.append(tmp)
        os.replace(tmp,app/'CMakeLists.txt')
        touched.remove(tmp)
        if (app/'CMakeLists.txt').read_bytes() != new_cmake:
            raise OSError('CMake write readback mismatch')
    except Exception:
        for path in touched:
            path.unlink(missing_ok=True)
        (app/'CMakeLists.txt').write_bytes(old_cmake)
        raise

if __name__ == '__main__':
    if len(sys.argv)!=2:
        raise SystemExit('usage: stage327.py <inherited-native-C11-app-dir>')
    stage(Path(__file__).resolve().parent, Path(sys.argv[1]).resolve())
    print('EXEC0327 six authenticated source/test files linked into inherited Gk1App; original first Spring NOT unlocked')
