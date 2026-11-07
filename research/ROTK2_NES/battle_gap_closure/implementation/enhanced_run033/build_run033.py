#!/usr/bin/env python3
"""Deterministic, guarded Enhanced Run033 full-source ZIP from immutable Run032.

Usage: python build_run033.py RUN032_FULL.zip MODIFIED_tactical_view.py NEW_test.py OUTPUT.zip
Only the Q14 GUI module and new regression are changed; no NES/DOS source.
"""
from pathlib import Path
import argparse, copy, hashlib, json, zipfile

PARENT_SHA='014ae2dcdc189395ee17f6d52d4568d526952180a7d4a4311583033741bd0e95'
SOURCE_SHA='cb26bf9eb0d4379954c69e11367bd8bf83433a4f6e2ae8fdc26f46ed41f40006'
TEST_SHA='0e0bb58c1ade777dee278fe801d8457fc1f87d379acd0326c0d0a6b5f700c266'
SOURCE='third_branch/tactical_view.py'
TEST='tests/test_run033_q14_prisoner_visible_choices.py'

def sha(data):return hashlib.sha256(data).hexdigest()

def build(parent,src,test,out):
    parent=Path(parent);src=Path(src);test=Path(test);out=Path(out)
    if sha(parent.read_bytes())!=PARENT_SHA:raise ValueError('Incorrect Enhanced Run032 parent ZIP')
    s=src.read_bytes();t=test.read_bytes()
    if sha(s)!=SOURCE_SHA or sha(t)!=TEST_SHA:raise ValueError('Modified Run033 source/test: refuse unknown bytes')
    with zipfile.ZipFile(parent,'r') as original:
        if original.testzip() is not None:raise ValueError('Parent CRC fail')
        names=original.namelist()
        if len(names)!=550 or SOURCE not in names or TEST in names:raise ValueError('Unexpected parent member inventory')
        out.parent.mkdir(parents=True,exist_ok=True)
        with zipfile.ZipFile(out,'w') as dest:
            dest.comment=original.comment
            for info in original.infolist():
                data=s if info.filename==SOURCE else original.read(info.filename)
                dest.writestr(copy.copy(info),data)
            info=zipfile.ZipInfo(TEST,(2026,11,7,10,0,0))
            info.compress_type=zipfile.ZIP_DEFLATED
            info.external_attr=0o100644 << 16
            dest.writestr(info,t)
    with zipfile.ZipFile(out) as new:
        assert len(new.infolist())==551 and new.testzip() is None
    print(json.dumps({'parent_sha256':PARENT_SHA,'output_sha256':sha(out.read_bytes()),'entries':551,'bytes':out.stat().st_size},indent=2))

if __name__=='__main__':
    ap=argparse.ArgumentParser();ap.add_argument('parent');ap.add_argument('source');ap.add_argument('test');ap.add_argument('output');a=ap.parse_args()
    build(a.parent,a.source,a.test,a.output)
