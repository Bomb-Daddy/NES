#!/usr/bin/env python3
"""Reconstruct Run012 from immutable Run011, fail on changed parent, prove output hash.
Usage: python RUN012_REBUILD.py /path/to/Run011_parent.zip /path/to/Run012_QA_evidence.zip /path/to/output.zip
Requires OS `patch` and Python standard library; source program/ROM never fetched.
"""
import sys, hashlib, tempfile, subprocess, zipfile
from pathlib import Path
PARENT_SHA='b62f8c22fad8122dc412a8ef6181da5cbb80511bf3a031ae91a6f02becd5eb56'
CHILD_SHA='5214b118fd350544fa677394233d3a0c1d1db7e9c8d8eec5c64e55a7bd63ad9e'
MOD=['src/rotk2/battle/engine.py','src/rotk2/state/models.py']
NEW='src/tests/test_run012_q09_escape_probability_capture.py'
def sha(f):return hashlib.sha256(Path(f).read_bytes()).hexdigest()
if len(sys.argv)!=4:raise SystemExit('Usage: RUN012_REBUILD.py Run011_parent.zip Run012_evidence.zip output.zip')
parent,ev,out=map(Path,sys.argv[1:]);assert sha(parent)==PARENT_SHA, 'PARENT_SHA_MISMATCH'
with tempfile.TemporaryDirectory() as td:
  root=Path(td)
  with zipfile.ZipFile(parent) as z:
    assert len(z.namelist())==156 and z.testzip() is None
    infos=z.infolist()
    for name in MOD:
      f=root/name;f.parent.mkdir(parents=True,exist_ok=True);f.write_bytes(z.read(name))
  with zipfile.ZipFile(ev) as z:
    patch=z.read('RUN012_PRODUCTION.diff');test=z.read('RUN012_test.py')
  proc=subprocess.run(['patch','-p1','--batch','--fuzz=0'],input=patch,cwd=root,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
  assert proc.returncode==0,(proc.stdout.decode(),proc.stderr.decode())
  newout=root/NEW;newout.parent.mkdir(parents=True,exist_ok=True);newout.write_bytes(test)
  with zipfile.ZipFile(parent) as old,zipfile.ZipFile(out,'w') as dest:
    for inf in infos:
      data=(root/inf.filename).read_bytes() if inf.filename in MOD else old.read(inf.filename)
      dest.writestr(inf,data)
    info=zipfile.ZipInfo(NEW,date_time=(2026,11,6,8,0,0));info.compress_type=zipfile.ZIP_DEFLATED;info.external_attr=0o100644<<16
    dest.writestr(info,test)
  with zipfile.ZipFile(out) as check: assert len(check.namelist())==157 and check.testzip() is None
  assert sha(out)==CHILD_SHA,('CHILD_SHA_MISMATCH',sha(out),CHILD_SHA)
  print('REPRODUCED',out,CHILD_SHA)
