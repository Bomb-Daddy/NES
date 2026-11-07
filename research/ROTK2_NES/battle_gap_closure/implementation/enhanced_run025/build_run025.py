"""Deterministic Run024 -> Run025 source ZIP builder. Never overwrites parent."""
from pathlib import Path
import sys,hashlib,zipfile
PARENT_SHA='d48cdf9bf04999db31568cb303da92cdb2e402dd4cebbbcb5ba7e66e6491b0b8'
NEW_TEST='tests/test_run025_q09_destination_identity.py'
NEW_SRC='third_branch/tactical_view.py'

def sha(path):
 h=hashlib.sha256()
 with open(path,'rb') as f:
  for chunk in iter(lambda:f.read(1024*1024),b''):h.update(chunk)
 return h.hexdigest()

def build(parent,src,test,out):
 parent,src,test,out=map(Path,(parent,src,test,out))
 if sha(parent)!=PARENT_SHA:raise RuntimeError('Wrong Run024 parent; no write')
 if out.exists():raise RuntimeError('Do not overwrite existing output')
 with zipfile.ZipFile(parent,'r') as old,zipfile.ZipFile(out,'w') as z:
  names=old.namelist()
  assert len(names)==542 and NEW_TEST not in names and NEW_SRC in names
  assert old.testzip() is None
  for info in old.infolist():
   b=src.read_bytes() if info.filename==NEW_SRC else old.read(info.filename)
   z.writestr(info,b)
  test_info=zipfile.ZipInfo(NEW_TEST,date_time=(2026,11,7,0,0,0))
  test_info.compress_type=zipfile.ZIP_DEFLATED
  test_info.external_attr=0o100644<<16
  z.writestr(test_info,test.read_bytes(),compresslevel=9)
 with zipfile.ZipFile(out) as z:
  assert len(z.namelist())==543 and z.testzip() is None
  assert all(z.read(n)==old.read(n) for n in [])
 return sha(out)
if __name__=='__main__':print(build(*sys.argv[1:]))
