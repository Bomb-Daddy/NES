#!/usr/bin/env python3
"""Apply one SHA-pinned C11 change to the exact old native source, atomically.

Git-for-Windows git apply failed to match the byte-identical preimage on run
43524230853. This independent, fail-closed patch interpreter checks both
whole-file SHA values and the complete context hunk. No source is changed
when either identity or context disagrees. The original raw ROM is not read.
"""
from pathlib import Path
import hashlib
import os
import sys
import tempfile

BEFORE = '193a5c2f5d5c54524b8b70ad0d40992e8170a7a5d1d8a6de083bfa675fbb0926'
AFTER = 'a0edd0a5dc4974a17485320368c6613240785a2c813a941dcaef527fd632c05c'
PATCH_SHA = 'c38ce72b2d63c0921a66614263d8415060f07fc2d68f5a933294e51cc8241e20'

def sha(data): return hashlib.sha256(data).hexdigest()

def interpret_patch(data):
    if sha(data) != PATCH_SHA: raise ValueError('unexpected published patch SHA')
    lines=data.splitlines(keepends=True)
    if sum(x.startswith(b'@@ ') for x in lines)!=1: raise ValueError('not one hunk')
    body=lines[next(i for i,x in enumerate(lines) if x.startswith(b'@@ '))+1:]
    if not body or any(x[:1] not in (b' ',b'-',b'+') for x in body): raise ValueError('unsupported source patch')
    old=b''.join(x[1:] for x in body if x[:1] in (b' ',b'-'))
    new=b''.join(x[1:] for x in body if x[:1] in (b' ',b'+'))
    return old,new

def transform(source,patch):
    if sha(source)!=BEFORE: raise ValueError('inherited EXEC0256 source SHA mismatch')
    old,new=interpret_patch(patch)
    if source.count(old)!=1: raise ValueError('original C11 source hunk not uniquely present')
    out=source.replace(old,new,1)
    if sha(out)!=AFTER: raise ValueError('source-after SHA mismatch')
    return out

def main(argv):
    if len(argv)!=3:
        print('Usage: apply_gk265_atomic_restart.py app_state.c patch265',file=sys.stderr);return 2
    src=Path(argv[1]);patch=Path(argv[2]);source=src.read_bytes();pbytes=patch.read_bytes()
    try:
        result=transform(source,pbytes)
        for badsrc,badpatch in ((source+b'\n',pbytes),(source,pbytes+b'\n'),(result,pbytes)):
            try: transform(badsrc,badpatch)
            except ValueError: pass
            else: raise ValueError('hostile source/patch mutation escaped rejection')
        fd,tmp=tempfile.mkstemp(prefix='.gk265-',dir=str(src.parent))
        try:
            with os.fdopen(fd,'wb') as f: f.write(result)
            os.replace(tmp,src)
        finally:
            if os.path.exists(tmp): os.unlink(tmp)
        if sha(src.read_bytes())!=AFTER: raise ValueError('committed source differs from expected')
    except Exception as exc:
        print(f'GK1 EXEC0265 SOURCE PATCH REFUSED: {exc}',file=sys.stderr)
        return 1
    print('GK1 EXEC0265 byte-identical source-gated C11 transform and 3 hostile controls PASS')
    return 0

if __name__=='__main__':raise SystemExit(main(sys.argv))
