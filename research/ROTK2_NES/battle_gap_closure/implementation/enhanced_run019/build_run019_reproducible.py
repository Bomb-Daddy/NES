#!/usr/bin/env python3
"""Rebuild byte-exact Enhanced Run019 from immutable Run018 and patched payloads.

Usage: python build_run019_reproducible.py PARENT_RUN018.zip PAYLOAD_SOURCE_DIR OUTPUT.zip

Only 1 existing production file is replaced; 2 new source files are appended.
Do not infer source ancestry from the archive name. Source SHA pin is mandatory.
"""
from __future__ import annotations
import argparse
import copy
import hashlib
import zipfile
from pathlib import Path

PARENT_SHA = '95a9e1133684d7b18841d275bf8d32d0b45b814b30521836cbf5922dd56e3ba5'
PRODUCTION = 'third_branch/officer_battle_adapter.py'
NEW = ('tests/test_run019_q09_recipient_source_attach.py', 'RUN019_ENHANCED_Q09_RECIPIENT_ATTACH_README.md')

def sha(data:bytes)->str:
    return hashlib.sha256(data).hexdigest()

def build(parent:Path, payload:Path, output:Path)->tuple[str,int]:
    if sha(parent.read_bytes()) != PARENT_SHA: raise ValueError('WRONG ENHANCED PARENT; STOP')
    payloads={member:(payload/member).read_bytes() for member in (PRODUCTION, *NEW)}
    output.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(parent) as old, zipfile.ZipFile(output,'w',allowZip64=True) as dest:
        if old.testzip() is not None: raise ValueError('PARENT ZIP CRC FAILURE')
        names=old.namelist()
        if len(names)!=535 or PRODUCTION not in names or any(n in names for n in NEW):
            raise ValueError('UNEXPECTED PARENT MEMBERS / WRONG SOURCE VERSION')
        for info in old.infolist():
            data=payloads[info.filename] if info.filename==PRODUCTION else old.read(info)
            dest.writestr(copy.copy(info),data)
        for new_member in NEW:
            info=zipfile.ZipInfo(new_member,(2026,11,6,23,0,0))
            info.compress_type=zipfile.ZIP_DEFLATED
            info.external_attr=0o100644<<16
            dest.writestr(info,payloads[new_member])
    with zipfile.ZipFile(output) as z:
        if z.testzip() is not None or len(z.namelist())!=537:raise ValueError('NEW ZIP INVALID')
        with zipfile.ZipFile(parent) as old:
            changed=[x for x in old.namelist() if z.read(x)!=old.read(x)]
        if changed!=[PRODUCTION]:raise ValueError('UNAUTHORIZED PRODUCTION CHANGE: '+repr(changed))
    return sha(output.read_bytes()),output.stat().st_size

if __name__ == '__main__':
    ap=argparse.ArgumentParser()
    ap.add_argument('parent',type=Path);ap.add_argument('payload',type=Path);ap.add_argument('output',type=Path)
    args=ap.parse_args()
    print(build(args.parent,args.payload,args.output))