#!/usr/bin/env python3
"""Deterministic Run019 full archive builder (parent SHA lease + member CRC)."""
from __future__ import annotations
import argparse, hashlib, pathlib, subprocess, tempfile, zipfile, json, shutil
PARENT_SHA = 'f343fbdbd13126bcda2b7ecc26d53cdff098fa767e23e3cf702c38d28186247b'
PATCH_NAMES = ['src/rotk2/battle/engine.py', 'src/tests/test_run014_q09_immediate_source_flee_writer.py']
NEW_TEST = 'src/tests/test_run019_q09_live_insertion_head_order.py'
NEW_ZIP = 'ROTK2_NES_Classic46_BPF_Run019_Q09_LiveHeadInsertion_FullSource_v1.00.zip'
BASE = pathlib.Path(__file__).resolve().parent

def sha(p): return hashlib.sha256(p.read_bytes()).hexdigest()
def build(parent: pathlib.Path, dest: pathlib.Path):
    if sha(parent) != PARENT_SHA: raise SystemExit('REFUSE: parent mismatch ' + sha(parent))
    with tempfile.TemporaryDirectory() as temp:
        root=pathlib.Path(temp)
        with zipfile.ZipFile(parent) as z:
            if z.testzip() is not None: raise SystemExit('REFUSE: corrupt parent')
            z.extractall(root)
            names=z.namelist()
            infos={n:z.getinfo(n) for n in names}
        for n in PATCH_NAMES:
            patch=BASE/(n.replace('/', '__')+'.diff')
            p=subprocess.run(['patch','-p0','--batch','--forward','-i',str(patch)],cwd=root,
                             text=True,capture_output=True)
            if p.returncode: raise SystemExit('REFUSE: patch failed '+n+' '+p.stdout+p.stderr)
        newpath=root/NEW_TEST
        newpath.parent.mkdir(parents=True,exist_ok=True)
        shutil.copyfile(BASE/'test_run019_q09_live_insertion_head_order.py',newpath)
        with zipfile.ZipFile(dest,'w') as out:
            for n in names:
                out.writestr(infos[n],(root/n).read_bytes())
            info=zipfile.ZipInfo(NEW_TEST,(2026,11,6,0,0,0))
            info.compress_type=zipfile.ZIP_DEFLATED
            info.external_attr=0o100644 << 16
            out.writestr(info,newpath.read_bytes())
        with zipfile.ZipFile(dest) as z:
            assert z.testzip() is None and len(z.namelist())==len(names)+1
        print(json.dumps({'parent_sha256':PARENT_SHA,'new_sha256':sha(dest),'member_count':len(names)+1,'output':str(dest)}))

if __name__=='__main__':
    parser=argparse.ArgumentParser()
    parser.add_argument('--parent',type=pathlib.Path,required=True)
    parser.add_argument('--dest',type=pathlib.Path,required=True)
    a=parser.parse_args()
    build(a.parent,a.dest)
