"""Deterministically rebuild Original-KOEI Enhanced Run039 from exact Run038.

Usage: python build_run039.py /path/to/Run038.zip /path/to/Run039.zip
The parent SHA is enforced; does not use Classic NES or DOS source trees.
"""
from pathlib import Path
from zipfile import ZipFile, ZipInfo, ZIP_DEFLATED
from hashlib import sha256
from copy import copy
import sys, json
HERE=Path(__file__).resolve().parent
PARENT=Path(sys.argv[1]); OUT=Path(sys.argv[2]); SOURCE=HERE/'run039_changed_source'
PARENT_SHA='1a1b193c1bb8f4aa5f5f343bbcc9c5617d4889cd2b550f5a8a03aa2041c3e14f'
EXPECTED_SHA='e2a1581af46f900fe16cc311566a27946320689ffb99e8a0ad21e72278153935'
CHANGES=['third_branch/officer_battle_adapter.py','tests/test_run037_q08_fire_empty_hex.py']
NEW='tests/test_run039_q08_friendly_cell_fire.py'
def hash_file(p):
 h=sha256()
 with p.open('rb') as f:
  for b in iter(lambda:f.read(1<<20),b''):h.update(b)
 return h.hexdigest()
assert hash_file(PARENT)==PARENT_SHA, 'BLOCKED WRONG IMMUTABLE RUN038 PARENT'
with ZipFile(PARENT) as old:
 assert old.testzip() is None and len(old.namelist())==556
 assert all(n in old.namelist() for n in CHANGES) and NEW not in old.namelist()
 OUT.parent.mkdir(parents=True,exist_ok=True)
 with ZipFile(OUT,'w',allowZip64=True) as dst:
  for info in old.infolist():
   data=(SOURCE/info.filename).read_bytes() if info.filename in CHANGES else old.read(info.filename)
   dst.writestr(copy(info),data,compress_type=info.compress_type,compresslevel=6)
  meta=ZipInfo(NEW,date_time=(1980,1,1,0,0,0))
  meta.compress_type=ZIP_DEFLATED;meta.create_system=3;meta.external_attr=0o100644<<16
  dst.writestr(meta,(SOURCE/NEW).read_bytes(),compresslevel=6)
with ZipFile(OUT) as new:
 assert len(new.namelist())==557 and new.testzip() is None
 with ZipFile(PARENT) as reference:
  assert sum(reference.read(n)==new.read(n) for n in reference.namelist())==554
assert hash_file(OUT)==EXPECTED_SHA,(hash_file(OUT),EXPECTED_SHA)
print(json.dumps({'run':'Run039','parent_sha256':PARENT_SHA,'new_sha256':EXPECTED_SHA,'bytes':OUT.stat().st_size,'entries':557,'unchanged':554},indent=2))
