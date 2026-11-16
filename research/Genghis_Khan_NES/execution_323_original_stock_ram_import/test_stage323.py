#!/usr/bin/env python3
import tempfile
from pathlib import Path
import hashlib
import stage323
N=0
S=Path(__file__).resolve().parent
def check(x):
 global N
 N+=1
 if not x:raise AssertionError('check '+str(N))
def fake(path):
 path.mkdir()
 (path/'CMakeLists.txt').write_bytes(b'add_library(gk1_app_state STATIC gk1_app_state.c gk321_first_spring_phase.c)\n')
for n,hash_ in stage323.FILES.items():check(hashlib.sha256((S/n).read_bytes()).hexdigest()==hash_)
with tempfile.TemporaryDirectory() as p:
 root=Path(p); app=root/'app';fake(app); cm=app/'CMakeLists.txt';original=cm.read_bytes()
 for scenario in ['after_first_copy','after_cmake']:
  try:stage323.install(S,app,scenario)
  except OSError:pass
  else:raise AssertionError('rollback must reject')
  check(cm.read_bytes()==original)
  check(not any(x.name.startswith('gk323') or x.name.startswith('test_gk323') for x in app.iterdir()))
 source=S/'gk323_original_spring.c'
 changed=source.read_bytes().replace(b'0x22+2*j',b'0x24+2*j')
 check(changed!=source.read_bytes())
 check(hashlib.sha256(changed).hexdigest()!=stage323.FILES[source.name])
 check(stage323.normalize(b'abc\r\n')==b'abc\n')
 try:stage323.normalize(b'abc\r\r\n')
 except stage323.Refused:check(True)
 else:raise AssertionError('CRCRLF accepted')
 try:stage323.changed_cmake(original.replace(b'gk321_',b'gk999_'))
 except stage323.Refused:check(True)
 else:raise AssertionError('wrong base accepted')
 stage323.install(S,app)
 check(b'gk323_original_spring.c' in cm.read_bytes())
 check(b'GK1_323_LiveGk1App' in cm.read_bytes())
 check(all((app/n).read_bytes()==(S/n).read_bytes() for n in stage323.FILES))
 try:stage323.install(S,app)
 except stage323.Refused:check(True)
 else:raise AssertionError('duplicate accepted')
 print('GK323 hostile staging PASS',N,'assertions, source SHA/offset mutation, rollback, wrong source, real native test registration')