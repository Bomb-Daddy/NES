#!/usr/bin/env python3
"""Rebuild Enhanced Run021 from exact independently Drive-verified Run020.

    python build_run021.py --parent <Run020 source.zip> --work <patched extracted tree> --out <new output folder>
No Classic NES or DOS source is touched. Files with mismatched hashes fail closed.
"""
from pathlib import Path
from hashlib import sha256
from zipfile import ZipFile,ZipInfo,ZIP_DEFLATED
import argparse,json
PARENT_SHA='0f05ba1c3f46e433609a116a903e198942049ab0ef49ab797cab5e25bc2ecc15'
NAME='ROTK2_Enhanced_Run021_Q09_ControllerActorIdentity_FullSource_v1.00.zip'
MODULE='third_branch/tactical_view.py'
TEST='tests/test_run021_q09_controller_actor_identity.py'

def digest(path):
 h=sha256()
 with Path(path).open('rb') as f:
  for block in iter(lambda:f.read(1024*1024),b''):h.update(block)
 return h.hexdigest()

def build(parent,work,out):
 if digest(parent)!=PARENT_SHA:raise ValueError('WRONG_PARENT_SOURCE_RUN020')
 out=Path(out);out.mkdir(parents=True,exist_ok=True)
 target=out/NAME
 if target.exists():raise FileExistsError('OUTPUT_EXISTS_NEVER_OVERWRITE')
 with ZipFile(parent) as old,ZipFile(target,'w') as new:
  names=old.namelist()
  if len(names)!=538 or MODULE not in names or TEST in names:raise ValueError('UNEXPECTED_PARENT_TREE')
  for info in old.infolist():
   new.writestr(info,(Path(work)/MODULE).read_bytes() if info.filename==MODULE else old.read(info))
  info=ZipInfo(TEST,date_time=(2026,11,7,6,0,0));info.compress_type=ZIP_DEFLATED
  info.create_system=3;info.external_attr=0o644<<16
  new.writestr(info,(Path(work)/TEST).read_bytes())
 with ZipFile(target) as new,ZipFile(parent) as old:
  if len(new.namelist())!=539 or new.testzip() is not None:raise ValueError('BAD_ZIP')
  changed=[i.filename for i in old.infolist() if new.read(i.filename)!=old.read(i.filename)]
  if changed!=[MODULE]:raise ValueError('UNEXPECTED_DIFF '+str(changed))
 print(json.dumps({'file':str(target),'source_sha256':digest(target),'bytes':target.stat().st_size,'members':539,'parent_sha256':PARENT_SHA,'modified':changed,'added':[TEST]},indent=2))
 return target
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--parent',required=True,type=Path)
 p.add_argument('--work',required=True,type=Path);p.add_argument('--out',required=True,type=Path)
 a=p.parse_args();build(a.parent,a.work,a.out)
