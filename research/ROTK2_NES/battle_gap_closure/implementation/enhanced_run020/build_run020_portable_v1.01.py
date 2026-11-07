#!/usr/bin/env python3
"""Deterministically construct Enhanced Run020 from its immutable Run019B source.

Usage:
 python build_run020_portable_v1.01.py --parent Run019B.zip \
   --work /path/to/Run019B_extracted_with_Run020_patch --out /path/to/output

The work directory must contain original Run019B files plus the Run020 patch
applied to third_branch/officer_battle_adapter.py and the new Run020 test.
The builder never reads Classic NES or DOS source and never overwrites the parent.
"""
from pathlib import Path
from hashlib import sha256
from zipfile import ZipFile, ZipInfo, ZIP_DEFLATED
import argparse
import json

PARENT_SHA='9a47e07278d5d98a9f3cedcdc0e366a4dbb435241ce9f1777c607def32e65ad6'
TARGET_SHA='0f05ba1c3f46e433609a116a903e198942049ab0ef49ab797cab5e25bc2ecc15'
NEW_NAME='ROTK2_Enhanced_Run020_Q09_PlayerDefenderInitialSelection_FullSource_v1.00.zip'
MODULE='third_branch/officer_battle_adapter.py'
TEST='tests/test_run020_q09_human_defender_selected.py'

def sha256_file(path):
    h=sha256()
    with path.open('rb') as f:
        for block in iter(lambda:f.read(1024*1024),b''):
            h.update(block)
    return h.hexdigest()

def build(parent,work,out):
    if sha256_file(parent)!=PARENT_SHA:
        raise ValueError('STOP: wrong parent ZIP; expected independently verified Run019B')
    out.mkdir(parents=True,exist_ok=True)
    target=out/NEW_NAME
    if target.exists():
        if sha256_file(target)==TARGET_SHA:
            print('Existing verified Run020 source already present:',target)
            return target
        raise FileExistsError('STOP: output filename exists with different hash; preserve it and choose clean output folder')
    with ZipFile(parent) as old, ZipFile(target,'w') as new:
        names=old.namelist()
        if len(names)!=537 or MODULE not in names or TEST in names:
            raise ValueError('STOP: unexpected parent contents')
        for info in old.infolist():
            data=(work/MODULE).read_bytes() if info.filename==MODULE else old.read(info)
            new.writestr(info,data)
        info=ZipInfo(TEST,date_time=(2026,11,7,0,0,0))
        info.compress_type=ZIP_DEFLATED;info.create_system=3;info.external_attr=0o644<<16
        new.writestr(info,(work/TEST).read_bytes())
    with ZipFile(target) as final,ZipFile(parent) as old:
        if len(final.infolist())!=538 or final.testzip() is not None:
            raise ValueError('Archive structural verification failed')
        if sum(final.read(i.filename)==old.read(i.filename) for i in old.infolist()) !=536:
            raise ValueError('More source members changed than intended')
    actual=sha256_file(target)
    if actual != TARGET_SHA:
        raise ValueError(f'Wrong reproducibility hash: {actual} != {TARGET_SHA}')
    print(json.dumps({'source':str(target),'sha256':actual,'bytes':target.stat().st_size,'members':538,'parent':PARENT_SHA},indent=2))
    return target

if __name__=='__main__':
    p=argparse.ArgumentParser()
    p.add_argument('--parent',required=True,type=Path)
    p.add_argument('--work',required=True,type=Path)
    p.add_argument('--out',required=True,type=Path)
    a=p.parse_args()
    build(a.parent,a.work,a.out)