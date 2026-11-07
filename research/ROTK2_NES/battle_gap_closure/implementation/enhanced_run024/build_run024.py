#!/usr/bin/env python3
"""Hash-pinned, non-overwriting ORIGINAL-KOEI Enhanced Run024 ZIP builder.

Only TacticalWindow's Q09 modal actor confirmation and its regression tests
are changed; Classic NES and DOS trees are not inputs.
"""
import argparse,copy,hashlib,zipfile
from pathlib import Path
PARENT_SHA='776d20447d5d5f7820dbbca0ae2cb486e02a1c1edd33ccf902c3497937e9b157'
CHANGED='third_branch/tactical_view.py'
ADDED='tests/test_run024_q09_flee_modal_actor_integrity.py'

def sha(p):
    with open(p,'rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()

def build(parent,source,out):
    parent=Path(parent);source=Path(source);out=Path(out)
    assert parent.is_file() and sha(parent)==PARENT_SHA,'WRONG_PARENT_SHA'
    assert out.resolve()!=parent.resolve() and not out.exists(),'REFUSE_OVERWRITE'
    assert (source/CHANGED).exists() and (source/ADDED).exists(),'MISSING_SOURCE'
    with zipfile.ZipFile(parent) as original,zipfile.ZipFile(out,'x') as target:
        assert len(original.namelist())==541 and original.testzip() is None
        assert CHANGED in original.namelist() and ADDED not in original.namelist()
        for info in original.infolist():
            content=(source/info.filename).read_bytes() if info.filename==CHANGED else original.read(info)
            target.writestr(copy.copy(info),content)
        i=zipfile.ZipInfo(ADDED,date_time=(2026,11,7,4,0,0))
        i.compress_type=zipfile.ZIP_DEFLATED
        i.create_system=3
        i.external_attr=0o100644<<16
        target.writestr(i,(source/ADDED).read_bytes())
    with zipfile.ZipFile(out) as z:
        assert z.testzip() is None and len(z.namelist())==542
    return sha(out)

if __name__=='__main__':
    p=argparse.ArgumentParser()
    p.add_argument('--parent',required=True);p.add_argument('--source',required=True);p.add_argument('--output',required=True)
    a=p.parse_args();print(build(a.parent,a.source,a.output))