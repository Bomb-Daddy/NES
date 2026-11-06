#!/usr/bin/env python3
"""Run014 reproducible full source builder; refuses any wrong Run013 parent bytes.

Usage: python RUN014_REBUILD.py PARENT_RUN013.zip ENGINE_RUN014.py TEST_RUN014.py OUT.zip
Only the specified source module is replaced; new regression is added. This
never modifies its parent ZIP and does not assert canonical Drive publication.
"""
import hashlib, sys, zipfile
from pathlib import Path
PARENT_SHA='28e4d7c0296621941b274ffcbc5d41651a1a76b0053ee45d35086fddb4b51c68'
ENGINE='src/rotk2/battle/engine.py'
TEST='src/tests/test_run014_q09_immediate_source_flee_writer.py'
def digest(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def build(p,e,t,out):
    if digest(p)!=PARENT_SHA:raise SystemExit('PARENT SHA MISMATCH: refuse to build against a different source')
    with zipfile.ZipFile(p) as z, zipfile.ZipFile(out,'w') as w:
        if z.testzip() is not None:raise SystemExit('PARENT ZIP CRC FAILURE')
        if TEST in z.namelist() or ENGINE not in z.namelist():raise SystemExit('UNEXPECTED PARENT MEMBER INVENTORY')
        for info in z.infolist():
            data=Path(e).read_bytes() if info.filename==ENGINE else z.read(info)
            w.writestr(info,data)
        new=zipfile.ZipInfo(TEST,(2026,11,6,8,0,0))
        new.create_system=3;new.external_attr=0o100644<<16
        new.compress_type=zipfile.ZIP_DEFLATED
        w.writestr(new,Path(t).read_bytes())
    with zipfile.ZipFile(out) as z:
        if len(z.namelist())!=159 or z.testzip() is not None:raise SystemExit('CHILD INVENTORY/CRC FAILURE')
    print('PARENT',PARENT_SHA,'CHILD',digest(out))
if __name__=='__main__':
    if len(sys.argv)!=5:raise SystemExit(__doc__)
    build(*sys.argv[1:])
