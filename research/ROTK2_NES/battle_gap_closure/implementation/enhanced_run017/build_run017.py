#!/usr/bin/env python3
"""Deterministic Run017 full source build on exact immutable Run016 ZIP."""
from zipfile import ZipFile, ZipInfo, ZIP_DEFLATED
from pathlib import Path
from hashlib import sha256
import sys

BASE=Path('/mnt/data/rotk_enh_run016/output/ROTK2_Enhanced_Run016_Q14_SiblingConsolidation_FullSource_v1.00.zip')
OUT=Path('/mnt/data/rotk_enh_run017_work/output/ROTK2_Enhanced_Run017_Q14_SourceGatedRulerSuccession_FullSource_v1.00.zip')
WORK=Path('/mnt/data/rotk_enh_run017_work/parent')
PARENT_SHA='f2234784d6eb6746d24bee9f5fed6172077a925ec8f4fcfd7a4176ee321732fa'
changed='third_branch/integrated.py'
new='tests/test_run017_q14_source_gated_ruler_succession.py'

def sha(path):
    h=sha256()
    with path.open('rb') as f:
        for block in iter(lambda:f.read(1024*1024),b''):h.update(block)
    return h.hexdigest()

def build(dest=OUT):
    assert sha(BASE)==PARENT_SHA,'WRONG_OR_CHANGED_RUN016_PARENT'
    dest.parent.mkdir(parents=True, exist_ok=True)
    with ZipFile(BASE,'r') as src, ZipFile(dest,'w',allowZip64=True) as dst:
        parent_names=set(src.namelist())
        assert changed in parent_names and new not in parent_names
        assert src.testzip() is None
        for info in src.infolist():
            data=(WORK/info.filename).read_bytes() if info.filename==changed else src.read(info)
            dst.writestr(info,data,compress_type=info.compress_type,compresslevel=6)
        info=ZipInfo(new, date_time=(2026,11,6,0,0,0))
        info.compress_type=ZIP_DEFLATED
        info.external_attr=0o100644<<16
        dst.writestr(info,(WORK/new).read_bytes(),compresslevel=6)
    with ZipFile(dest) as z,ZipFile(BASE) as base:
        assert z.testzip() is None
        assert len(z.namelist())==len(base.namelist())+1
        assert sorted(set(z.namelist())-set(base.namelist()))==[new]
        different=[n for n in base.namelist() if base.read(n)!=z.read(n)]
        assert different==[changed],different
        assert z.read(changed)==(WORK/changed).read_bytes()
        assert z.read(new)==(WORK/new).read_bytes()
    return sha(dest)

if __name__=='__main__':
    dest=Path(sys.argv[1]) if len(sys.argv)>1 else OUT
    print(dest,build(dest),dest.stat().st_size)
