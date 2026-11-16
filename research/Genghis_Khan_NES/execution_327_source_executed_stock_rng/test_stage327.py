#!/usr/bin/env python3
"""Adversarial Windows checkout/staging regression with fail-closed rollback."""
from __future__ import annotations
import hashlib
from pathlib import Path
import tempfile
import shutil
from stage327 import FILES, SHA256, canonical, prepare, stage, CMAKE_MARKER
checks=0

def ok(v: bool):
    global checks
    checks += 1
    if not v:
        raise AssertionError('assertion %d'%checks)

def fails(fn):
    try:
        fn()
    except (ValueError,OSError):
        ok(True)
    else:
        ok(False)

with tempfile.TemporaryDirectory(prefix='gk327-hostile-') as root:
    root=Path(root);source=root/'source';app=root/'app';source.mkdir();app.mkdir()
    original=Path(__file__).resolve().parent
    for n in FILES: shutil.copyfile(original/n,source/n)
    (app/'CMakeLists.txt').write_text('add_library(gk1_app_state STATIC dummy.c)\n')
    (app/'gk1_app_state.h').write_text('/* inherited */\n')
    (app/'gk326_sha256.h').write_text('/* prior */\n')
    (app/'gk326_sha256.c').write_text('/* prior */\n')
    ok(set(FILES)==set(SHA256))
    ok(all(hashlib.sha256(canonical((source/n).read_bytes())).hexdigest()==SHA256[n] for n in FILES))
    ok(canonical(b'A\r\nB\r\n')==b'A\nB\n')
    for bad in (b'',b'\x00',b'A\rB',b'A\r\r\nB'):
        fails(lambda b=bad: canonical(b))
    # Git-for-Windows may check out text as CRLF; authentic SHA is over canonical LF.
    (source/'gk327_app_rng_bridge.h').write_bytes((source/'gk327_app_rng_bridge.h').read_bytes().replace(b'\r\n', b'\n').replace(b'\n', b'\r\n'))
    ok(hashlib.sha256(canonical((source/'gk327_app_rng_bridge.h').read_bytes())).hexdigest()==SHA256['gk327_app_rng_bridge.h'])
    prepared,cmake=prepare(source,app)
    ok(len(prepared)==6 and CMAKE_MARKER.encode() in cmake)
    (source/'gk327_fixed6502_rng.c').write_bytes((source/'gk327_fixed6502_rng.c').read_bytes()+b'/* injected */\n')
    fails(lambda:stage(source,app))
    ok(not any((app/n).exists() for n in FILES))
    ok(CMAKE_MARKER not in (app/'CMakeLists.txt').read_text())
    shutil.copyfile(original/'gk327_fixed6502_rng.c',source/'gk327_fixed6502_rng.c')
    (source/'gk327_fixed6502_rng.h').write_bytes((source/'gk327_fixed6502_rng.h').read_bytes().replace(b'\n',b'\r\n'))
    stage(source,app)
    ok(all((app/n).read_bytes()==canonical((original/n).read_bytes()) for n in FILES))
    ok(CMAKE_MARKER in (app/'CMakeLists.txt').read_text())
    fails(lambda:stage(source,app))  # duplicate guard
    ok((app/'gk327_fixed6502_rng.h').read_bytes()==canonical((original/'gk327_fixed6502_rng.h').read_bytes()))
    # Regress specific malicious source/path swaps independently.
    (app/'CMakeLists.txt').write_text('wrong source project\n')
    fails(lambda:prepare(source,app))
    (app/'gk326_sha256.h').unlink()
    fails(lambda:prepare(source,app))
print('GK327 hostile source staging: %d assertions PASS'%checks)
