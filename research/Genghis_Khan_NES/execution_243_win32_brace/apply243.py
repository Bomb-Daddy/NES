#!/usr/bin/env python3
"""EXEC0243: fail-closed WinMain WM_PAINT brace correction in exact EXEC0242 native source."""
import hashlib
import sys
from pathlib import Path

PRE_SHA = "651421d0b0a1ada2d568e582e15caaa49becd3e7b76faf2bd7409772748eee23"
POST_SHA = "40327be4dc3eb53d524b25dd61cb7b727032a4a33a88ff453e4b5c9dfe2ff135"
OLD = b'++gk230_painted_frames;gk231_trace("WM_PAINT completed");return 0;}}\n return DefWindowProcA(h,msg,w,l);'
NEW = b'++gk230_painted_frames;gk231_trace("WM_PAINT completed");return 0;}\n return DefWindowProcA(h,msg,w,l);'

def normalize(blob: bytes) -> bytes:
    blob = blob.replace(b'\r\n', b'\n')
    if b'\r' in blob:
        raise ValueError('bare CR refused')
    return blob

def digest(blob: bytes) -> str:
    return hashlib.sha256(blob).hexdigest()

def apply(path: Path) -> str:
    before = normalize(path.read_bytes())
    if digest(before) != PRE_SHA:
        raise ValueError('wrong EXEC0242 predecessor SHA; no game source changed')
    if before.count(OLD) != 1:
        raise ValueError('expected exactly one native WM_PAINT/window_proc defect')
    after = before.replace(OLD, NEW)
    if digest(after) != POST_SHA:
        raise ValueError('postimage SHA mismatch; no game source changed')
    temp = path.with_suffix(path.suffix + '.exec243_tmp')
    try:
        temp.write_bytes(after)
        temp.replace(path)
    finally:
        temp.unlink(missing_ok=True)
    return digest(after)

if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.exit('Usage: apply243.py <hydrated source parent containing execution_199>')
    src = Path(sys.argv[1]) / 'execution_199' / 'gk1_native_shell.c'
    try:
        print('EXEC0243: native WinMain brace fix applied sha256=' + apply(src))
    except (ValueError, OSError) as exc:
        sys.exit('EXEC0243: REFUSED ' + str(exc))
