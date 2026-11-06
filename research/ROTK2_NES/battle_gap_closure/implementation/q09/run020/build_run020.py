#!/usr/bin/env python3
"""Run020 exact-parent guard, deterministic source reconstruction."""
from __future__ import annotations
import argparse, hashlib, json, pathlib, subprocess, tempfile, zipfile, shutil
PARENT_SHA='59dec33f717481c93ab7791e3ddc47458acdd56c95c3c8392aa6ade0d841a056'
PATCH_NAMES=['src/rotk2/battle/engine.py','src/rotk2/state/models.py']
NEW_TEST='src/tests/test_run020_q09_source_successor_links.py'
BASE=pathlib.Path(__file__).resolve().parent

def sha(p): return hashlib.sha256(p.read_bytes()).hexdigest()
def build(parent,dest):
    if sha(parent)!=PARENT_SHA: raise SystemExit(f'REFUSE parent {sha(parent)} != {PARENT_SHA}')
    with tempfile.TemporaryDirectory() as t:
        root=pathlib.Path(t)
        with zipfile.ZipFile(parent) as z:
            if z.testzip() is not None: raise SystemExit('REFUSE parent corrupt')
            infos=z.infolist();names=[x.filename for x in infos]
            z.extractall(root)
        for n in PATCH_NAMES:
            p=BASE/(n.replace('/','__')+'.diff')
            r=subprocess.run(['patch','-p0','--batch','--forward','-i',str(p)],cwd=root,capture_output=True,text=True)
            if r.returncode: raise SystemExit(f'REFUSE patch {n}: {r.stdout} {r.stderr}')
        desttest=root/NEW_TEST
        desttest.parent.mkdir(parents=True,exist_ok=True)
        shutil.copyfile(BASE/'test_run020_q09_source_successor_links.py',desttest)
        with zipfile.ZipFile(dest,'w') as out:
            for info in infos: out.writestr(info,(root/info.filename).read_bytes())
            info=zipfile.ZipInfo(NEW_TEST,(2026,11,6,0,0,0))
            info.compress_type=zipfile.ZIP_DEFLATED
            info.external_attr=0o100644<<16
            out.writestr(info,desttest.read_bytes())
        with zipfile.ZipFile(dest) as z:
            assert z.testzip() is None and len(z.namelist())==len(names)+1
        print(json.dumps({'parent_sha256':PARENT_SHA,'new_sha256':sha(dest),'members':len(names)+1,'output':str(dest)}))
if __name__=='__main__':
    a=argparse.ArgumentParser();a.add_argument('--parent',type=pathlib.Path,required=True);a.add_argument('--dest',type=pathlib.Path,required=True)
    o=a.parse_args();build(o.parent,o.dest)