#!/usr/bin/env python3
"""EXEC0303 independent hostile original-0265-provenance-gate migration QA."""
import importlib.util
from pathlib import Path
import tempfile

directory=Path(__file__).resolve().parent
spec=importlib.util.spec_from_file_location('source303',directory/'repair_provenance_303.py')
m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
old=directory.parent/'execution_265_win32_source_provenance'/'test_gk265_current_win32_source.py'
raw=m.canonical(old.read_bytes())  # Git checkout may use CRLF on Win32
assert m.blob(m.canonical(raw))==m.OLD_GATE_GIT_BLOB
checks=0
assert raw.count(m.OLD_TOKEN.encode())==1
previous=raw.replace(m.OLD_TOKEN.encode(),m.PREVIOUS_TOKEN.encode(),1)
a=m.revised_gate(previous)
assert a.count(m.NEW_TOKEN.encode())==1;checks+=1
assert m.OLD_TOKEN.encode() not in a;checks+=1
for token in ('EXPECTED_TEST_SHA','EXPECTED_PREDECESSOR_SHA','for index,values in enumerate(mutations,1)','def audit('):
 assert a.decode().count(token)==raw.decode().count(token)
 checks+=1
assert b'len(mutations)' in a;checks+=1
for mod in (raw,previous+b'#evil',previous.replace(b'EXPECTED_TEST_SHA',b'EXPECTED_TEST_WEAK'),
            previous.replace(b'EXPECTED_APP_SHA',b'SKIP_APP_SHA'),a):
 try:m.revised_gate(mod)
 except ValueError:checks+=1
 else:raise AssertionError('wrong old provenance gate accepted')
assert m.revised_gate(previous.replace(b'\n',b'\r\n'))==a;checks+=1
with tempfile.TemporaryDirectory() as tmp:
 root=Path(tmp);(root/'packaging').mkdir()
 f=root/'packaging'/'test_gk265_current_win32_source.py'
 f.write_bytes(previous)
 # Without exact compiled app ancestry the migration MUST fail without edits.
 for n in ('gk1_app_state.c','gk1_app_state.h','CMakeLists.txt'):
  (root/n).write_bytes(b'not the post-303 complete native C11 source\n')
 try:m.run(root)
 except ValueError:checks+=1
 else:raise AssertionError('unverified native app accepted')
 assert f.read_bytes()==previous;checks+=1
print(f'GK1_EXEC0303 PASS {checks} test-gate provenance/hostile mutation assertions; inherited 0265 source integrity and rollback retained')
