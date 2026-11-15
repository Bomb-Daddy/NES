#!/usr/bin/env python3
"""Adversarial atomic staging and previous-implementation refusal."""
import pathlib, tempfile, sys
sys.path.insert(0,str(pathlib.Path(__file__).resolve().parent))
import stage309 as s
root=pathlib.Path(__file__).resolve().parents[1]
# Usage: test_stage309.py EXACT_EXEC0307_PLUS_STAGED_APP
if len(sys.argv)!=2:raise SystemExit('test_stage309.py EXISTING_PRE_STAGE_APP')
source=pathlib.Path(sys.argv[1])
checked=0
def c(v):
 global checked
 checked+=1
 assert v, 'EXEC0309 hostile gate failure '+str(checked)
def setup(p):
 p.mkdir()
 for n in ['gk162_original_entry.c','CMakeLists.txt']:
  (p/n).write_bytes((source/n).read_bytes())
with tempfile.TemporaryDirectory() as d:
 base=pathlib.Path(d)
 d=base/'clean';setup(d)
 c(s.digest(s.norm((d/'gk162_original_entry.c').read_bytes()))==s.ORIGINAL_GK162_SHA)
 h=s.stage(d);c(len(h)==3)
 c(all(s.digest((d/n).read_bytes())==v for n,v in h.items()))
 c((d/'gk162_original_entry.c').read_text().count('gk53_setup_allowed_lord((uint8_t)w,player)')==1)
 c(s.CMAKE_MARKER in (d/'CMakeLists.txt').read_text())
 try:s.stage(d);c(False)
 except ValueError:c(True)
 for field in ('gk162_original_entry.c','CMakeLists.txt'):
  d=base/field;setup(d);p=d/field;p.write_bytes(p.read_bytes().replace(b'add_test(NAME GK1_307_PhysicalXboxDiagnosticInputFSM_NoHardwareClaim',b'add_test(NAME REMOVED_BY_HOSTILE_TEST') if field=='CMakeLists.txt' else p.read_bytes()+b'\n/*corruption*/')
  old={n:(d/n).read_bytes() for n in ['gk162_original_entry.c','CMakeLists.txt']}
  try:s.stage(d);c(False)
  except ValueError:c(True)
  c(all((d/n).read_bytes()==b for n,b in old.items()))
  c(not (d/'test_gk309_original_player_gate.c').exists())
 d=base/'crlf';setup(d)
 for n in ['gk162_original_entry.c','CMakeLists.txt']:
  p=d/n;p.write_bytes(p.read_bytes().replace(b'\n',b'\r\n'))
 c(len(s.stage(d))==3)
 d=base/'bad-crcrlf';setup(d);p=d/'gk162_original_entry.c';p.write_bytes(p.read_bytes().replace(b'\n',b'\r\r\n',1))
 try:s.stage(d);c(False)
 except ValueError:c(True)
 d=base/'rollback';setup(d);old={n:(d/n).read_bytes() for n in ['gk162_original_entry.c','CMakeLists.txt']}
 try:s.stage(d,fail_after=2);c(False)
 except OSError:c(True)
 c(all((d/n).read_bytes()==b for n,b in old.items()))
 c(not (d/'test_gk309_original_player_gate.c').exists())
 d=base/'wrong-307';setup(d);p=d/'CMakeLists.txt';p.write_bytes(p.read_bytes().replace(b'GK1_307_PhysicalXboxDiagnosticInputFSM_NoHardwareClaim',b'MISSING_INHERITANCE'))
 try:s.stage(d);c(False)
 except ValueError:c(True)
 c(not (d/'test_gk309_original_player_gate.c').exists())
print('EXEC0309 source-stage hostile QA PASS',checked,'checks')
