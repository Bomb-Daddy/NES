#!/usr/bin/env python3
"""Reproduce Run011 only from the exact authenticated Run010 complete-source ZIP.

Examples:
  python RUN011_APPLY_HASH_GUARDED.py --parent Run010.zip --diff RUN011_PRODUCTION.diff \
      --new-test test_run011_q09_officer_head_authority.py --output Run011.zip

Runs in an isolated temporary directory; does not modify the original parent.
Needs the standard `patch` utility; calls its zero-fuzz/zero-offset implementation.
"""
import argparse
import hashlib
import json
import subprocess
import tempfile
import zipfile
from pathlib import Path

EXPECTED_PARENT='0912d25e1e86039ae64028dea2b4eb02202389e90ebbabac09ac0bfad6e4edc3'
EXPECTED_RESULT='b62f8c22fad8122dc412a8ef6181da5cbb80511bf3a031ae91a6f02becd5eb56'
EXPECTED_TEST='2e7fec12640dbb7cf0394a438ddc2b7e9e9d38de951570550e3539030a4dda97'
NEW_TEST='src/tests/test_run011_q09_officer_head_authority.py'
MODS=['src/rotk2/battle/engine.py','src/rotk2/state/models.py']

def sha(b): return hashlib.sha256(b).hexdigest()

def main():
    p=argparse.ArgumentParser()
    for k in ['parent','diff','new_test','output']:
        p.add_argument('--'+k.replace('_','-'), type=Path, required=True)
    a=p.parse_args()
    assert sha(a.parent.read_bytes())==EXPECTED_PARENT,'wrong Run010 ancestor; refusing any edit'
    assert sha(a.new_test.read_bytes())==EXPECTED_TEST,'wrong regression payload'
    with zipfile.ZipFile(a.parent) as zi:
        assert zi.testzip() is None,'parent ZIP CRC failed'
        old={m.filename:zi.read(m) for m in zi.infolist() if not m.is_dir()}
    assert NEW_TEST not in old
    with tempfile.TemporaryDirectory(prefix='rotk_run011_') as td:
        root=Path(td)
        for k,buf in old.items():
            target=root/k
            assert root in target.parents and not k.startswith('/'), 'unsafe ZIP member'
            target.parent.mkdir(exist_ok=True,parents=True)
            target.write_bytes(buf)
        patch=subprocess.run(['patch','--batch','--fuzz=0','-p0','--input',str(a.diff.resolve())],cwd=root,text=True,capture_output=True)
        if patch.returncode: raise RuntimeError(f'patch rejected:\n{patch.stdout}\n{patch.stderr}')
        changed=[k for k,v in old.items() if (root/k).read_bytes()!=v]
        assert changed==MODS,(changed,MODS)
        dst=root/NEW_TEST
        dst.parent.mkdir(parents=True,exist_ok=True)
        dst.write_bytes(a.new_test.read_bytes())
        new={k:(root/k).read_bytes() for k in old}
        new[NEW_TEST]=dst.read_bytes()
        with zipfile.ZipFile(a.output,'w',zipfile.ZIP_DEFLATED,compresslevel=9) as zo:
            for key in sorted(new):
                info=zipfile.ZipInfo(key,date_time=(2026,11,6,8,0,0))
                info.compress_type=zipfile.ZIP_DEFLATED
                info.external_attr=0o644<<16
                zo.writestr(info,new[key])
    with zipfile.ZipFile(a.output) as zi:
        assert zi.testzip() is None
        assert len(zi.namelist())==len(old)+1
    got=sha(a.output.read_bytes())
    assert got==EXPECTED_RESULT,(got,EXPECTED_RESULT)
    print(json.dumps({'result':'PASS','parent_sha256':EXPECTED_PARENT,'output_sha256':got,'members':len(new),'production_changed':changed,'test_added':NEW_TEST}))

if __name__=='__main__': main()
