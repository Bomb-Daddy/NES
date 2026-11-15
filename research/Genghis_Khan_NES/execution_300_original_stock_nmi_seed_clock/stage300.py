#!/usr/bin/env python3
"""Fail-closed, SHA-pinned incremental EXEC0300 install into native EXEC0299 C11."""
from pathlib import Path
import hashlib, subprocess, shutil, tempfile, sys
BEFORE={
 'gk1_app_state.c':'79ed178dec0a345886932bda2f887645c427b4b12e721438d937b7f97dbcf581',
 'gk1_app_state.h':'95df64d698550869749d5f740e4d55e8726e5ae3d7637712e676cb482daa999b',
 'CMakeLists.txt':'108527f653959b9da618a95c497f36d772c5f79ab384ff88584753ad35aa0c7f'}
AFTER={
 'gk1_app_state.c':'c4a6707d70f809d9cb3d3bfaa775cc1757c331c81b985a82ec97e3a17f560fc6',
 'gk1_app_state.h':'dbaf417047418e431fa08e2578a64cd506850e6dca079cb5ef6c6c8ae3d1a29e',
 'CMakeLists.txt':'ff5eb9401c3159eb69ce7519583f2c467a1f32a88e65a8441bf2ded4af3e2770',
 'gk300_stock_nmi_seed_clock.c':'9ffa5fb6f36975060829a1053c760da75f25a9376b7cc954d2cc08657ad43c98',
 'gk300_stock_nmi_seed_clock.h':'ff8fbaa39b576abe0869ed8f385f8d117c95ef0352619b6d171bcb8bf92438d8',
 'test_gk300_stock_nmi_seed_clock.c':'42a5b8dcd830d9f38086aca225ba065810300458263946d826cb2d8d16271290'}
def sha(data):return hashlib.sha256(data).hexdigest()
def canon(data):
 data=data.replace(b'\r\n',b'\n')
 if b'\r' in data:raise ValueError('mixed, unsupported CR source')
 return data

def stage(root,release=None):
 root=Path(root);release=Path(release or Path(__file__).resolve().parent)
 if not root.is_dir():raise ValueError('full native application missing')
 patch=release/'GK1_EXEC0300_Predecessor_299_to_300_Production.patch'
 for n,h in BEFORE.items():
  if sha(canon((root/n).read_bytes()))!=h:raise ValueError('incorrect native predecessor or duplicate stage: '+n)
 if any((root/n).exists() for n in AFTER if n not in BEFORE):raise ValueError('new files already exist')
 for n,h in AFTER.items():
  if n not in BEFORE and sha(canon((release/n).read_bytes()))!=h:raise ValueError('source payload tampered: '+n)
 # Patch identity also verified against manifest; do not trust a malformed patch.
 import json
 manifest=json.loads((release/'hash_manifest.json').read_text())
 if sha(patch.read_bytes())!=manifest['patch_sha256']:raise ValueError('patch tampered')
 # Apply and independently hash every output in a disposable workspace,
 # then atomically install only after all sources are authenticated.
 with tempfile.TemporaryDirectory(prefix='gk300_stage_') as tmp:
  temp=Path(tmp)
  for n in BEFORE:(temp/n).write_bytes(canon((root/n).read_bytes()))
  check=subprocess.run(['git','-C',str(temp),'apply','--check',str(patch)],capture_output=True,text=True)
  if check.returncode:raise ValueError('source patch refuses: '+check.stderr[:300])
  p=subprocess.run(['git','-C',str(temp),'apply',str(patch)],capture_output=True,text=True)
  if p.returncode:raise ValueError('source patch failed: '+p.stderr[:300])
  for n in AFTER:
   if n not in BEFORE:(temp/n).write_bytes((release/n).read_bytes())
  for n,h in AFTER.items():
   if sha((temp/n).read_bytes())!=h:raise ValueError('postimage mismatch '+n+' got '+sha((temp/n).read_bytes()))
  originals={n:(root/n).read_bytes() for n in BEFORE}
  written=[]
  try:
   for n in AFTER:
    (root/n).write_bytes((temp/n).read_bytes());written.append(n)
  except BaseException:
   for n in written:
    if n in BEFORE:(root/n).write_bytes(originals[n])
    else:(root/n).unlink(missing_ok=True)
   raise
 print('GK1 EXEC0300 staged into EXACT inherited native C11, all six postimage SHA256 verified')
if __name__=='__main__':
 if len(sys.argv)!=2:sys.exit('Usage: stage300.py /path/to/complete/execution_199')
 try:stage(sys.argv[1])
 except Exception as e:sys.exit('EXEC0300 REFUSED: '+str(e))
