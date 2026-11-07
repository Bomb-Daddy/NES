#!/usr/bin/env python3
"""Run032 immutable Run031 source parent -> original-game Enhanced Q10 controller correction.
Reproducibly preserve parent member metadata and payloads except one production file;
append one test file. Requires real verified Run031 zip and the two new files.
"""
from pathlib import Path
import hashlib, zipfile, sys
ROOT=Path(__file__).resolve().parent
PARENT=ROOT/'parent/Run031_source.zip'
PRODUCTION=ROOT/'child/third_branch/tactical_view.py'
TEST=ROOT/'child/tests/test_run032_q10_switch_sides_controller_target.py'
OUTPUT=Path(sys.argv[1]) if len(sys.argv)>1 else ROOT/'output/ROTK2_Enhanced_Run032_Q10_ControllerBribeTarget_FullSource_v1.00.zip'
PARENT_SHA='6f3e6f62cd4bfd167464f727b8a942ef0fb6e0301209951d983330edcfe9c74c'
def sha(p):
 h=hashlib.sha256()
 with p.open('rb') as f:
  for b in iter(lambda:f.read(1024*1024),b''):h.update(b)
 return h.hexdigest()
assert sha(PARENT)==PARENT_SHA, 'run031 immutable parent hash mismatch'
new={'third_branch/tactical_view.py':PRODUCTION.read_bytes(),'tests/test_run032_q10_switch_sides_controller_target.py':TEST.read_bytes()}
OUTPUT.parent.mkdir(parents=True,exist_ok=True)
with zipfile.ZipFile(PARENT) as old, zipfile.ZipFile(OUTPUT,'w',allowZip64=True) as dest:
 assert len(old.namelist())==549
 assert 'tests/test_run032_q10_switch_sides_controller_target.py' not in old.namelist()
 for member in old.infolist():
  dest.writestr(member,new.get(member.filename,old.read(member)))
 info=zipfile.ZipInfo('tests/test_run032_q10_switch_sides_controller_target.py',date_time=(2026,11,7,0,0,0))
 info.compress_type=zipfile.ZIP_DEFLATED
 info.create_system=3
 info.external_attr=(0o100644<<16)
 dest.writestr(info,new[info.filename])
with zipfile.ZipFile(OUTPUT) as z:
 assert len(z.namelist())==550 and z.testzip() is None
with zipfile.ZipFile(PARENT) as old, zipfile.ZipFile(OUTPUT) as cur:
 changed=[name for name in old.namelist() if old.read(name)!=cur.read(name)]
 assert changed==['third_branch/tactical_view.py'],changed
print('PARENT_SHA',PARENT_SHA)
print('CHILD_SHA',sha(OUTPUT))
print('CHILD_SIZE',OUTPUT.stat().st_size)
print('CHILD_ENTRIES',550)
print('INHERITED_UNCHANGED',548)
print('PRODUCTION_CHANGED',changed)
