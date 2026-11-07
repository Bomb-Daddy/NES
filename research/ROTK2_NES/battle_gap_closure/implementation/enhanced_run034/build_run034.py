#!/usr/bin/env python3
"""Deterministic full-source Run034 ZIP from byte-pinned Run033 and 2 source replacements.
Not a standalone installer. Does not change canonical Classic NES or DOS.
"""
import argparse, hashlib, json, zipfile
from pathlib import Path
PARENT_SHA='76b9247f9d1f0e21d143352134e8eaf93fe21f3daf529b757460017f1684a95e'
CHANGED=('third_branch/officer_battle_adapter.py','third_branch/tactical_view.py')
NEW='tests/test_run034_q10_returning_switched_officer.py'
def sha(x):return hashlib.sha256(x).hexdigest()
def build(parent: Path, work: Path, dest: Path):
    if sha(parent.read_bytes())!=PARENT_SHA:raise SystemExit('PARENT_SHA_MISMATCH')
    dest.parent.mkdir(parents=True,exist_ok=True)
    with zipfile.ZipFile(parent,'r') as base, zipfile.ZipFile(dest,'w') as out:
        old=base.namelist()
        if NEW in old or not all(n in old for n in CHANGED):raise SystemExit('SOURCE_ANCESTRY_MISMATCH')
        for info in base.infolist():
            data=(work / info.filename).read_bytes() if info.filename in CHANGED else base.read(info.filename)
            out.writestr(info,data)
        meta=zipfile.ZipInfo(NEW,date_time=(1980,1,1,0,0,0))
        meta.compress_type=zipfile.ZIP_DEFLATED
        meta.external_attr=(0o100644<<16)
        out.writestr(meta,(work / NEW).read_bytes())
    with zipfile.ZipFile(dest,'r') as z:
        bad=z.testzip()
        if bad:raise SystemExit(f'ZIP_CRC_FAILURE {bad}')
        if len(z.namelist())!=len(old)+1:raise SystemExit('ZIP_MEMBERS_MISMATCH')
    return {'file':str(dest),'parent_sha256':PARENT_SHA,'full_sha256':sha(dest.read_bytes()),'size_bytes':dest.stat().st_size,'zip_entries':len(old)+1,'changed_production':list(CHANGED),'added_test':NEW,'inherited_unchanged':len(old)-len(CHANGED)}
if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--parent',required=True);p.add_argument('--work',required=True);p.add_argument('--output',required=True);a=p.parse_args()
    print(json.dumps(build(Path(a.parent),Path(a.work),Path(a.output)),indent=2))
