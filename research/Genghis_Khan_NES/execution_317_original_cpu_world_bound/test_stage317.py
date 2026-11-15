#!/usr/bin/env python3
"""Executable EXEC0317 source integrity, refusal and rollback checks."""
import hashlib
import os
from pathlib import Path
import tempfile
from unittest.mock import patch
import stage317
checks=0

def ck(v):
 global checks
 checks+=1
 if not v: raise AssertionError(f'EXEC0317 hostile stage check {checks}')
def fixture(p):
 (p/'gk1_app_state.c').write_bytes(b'#include <stdlib.h>\n'+stage317.ORIGINAL.encode())
 (p/'CMakeLists.txt').write_bytes(b'add_library(gk1_app_state STATIC existing.c)\n'+stage317.stage316.CMAKE.encode())
with tempfile.TemporaryDirectory() as t:
 root=Path(t);fixture(root)
 before={p.name:p.read_bytes() for p in root.iterdir()}
 preview=stage317.prepare(root)
 ck(len(preview)==3 and 'test_gk317_world_bound.c' in preview)
 actual=stage317.stage(root)
 ck(len(actual)==3 and all(hashlib.sha256((root/n).read_bytes()).hexdigest()==h for n,h in actual.items()))
 ck((root/'gk1_app_state.c').read_bytes().count(b'candidate->world!=app->active_world')==1)
 ck((root/'gk1_app_state.c').read_bytes().count(b'#include <stdlib.h>\n\n/* EXEC0316')==1)
 ck(b'GK1_317_OriginalCpuClaimsAreWorldBoundWithoutOrders' in (root/'CMakeLists.txt').read_bytes())
 after={p.name:p.read_bytes() for p in root.iterdir()}
 try:stage317.stage(root)
 except ValueError:pass
 else:raise AssertionError('duplicate stage accepted')
 ck(after=={p.name:p.read_bytes() for p in root.iterdir()})
with tempfile.TemporaryDirectory() as t:
 root=Path(t);fixture(root)
 (root/'gk1_app_state.c').write_bytes(b'evil stage')
 try:stage317.stage(root)
 except ValueError:pass
 else:raise AssertionError('wrong complete app source accepted')
 ck(not (root/'test_gk317_world_bound.c').exists())
with tempfile.TemporaryDirectory() as t:
 root=Path(t);fixture(root)
 (root/'CMakeLists.txt').write_bytes(b'add_library(gk1_app_state STATIC existing.c)\n')
 try:stage317.stage(root)
 except ValueError:pass
 else:raise AssertionError('missing previous CMake registration accepted')
 ck(not (root/'test_gk317_world_bound.c').exists())
with tempfile.TemporaryDirectory() as t:
 root=Path(t);fixture(root)
 before={p.name:p.read_bytes() for p in root.iterdir()}
 real=os.replace;count=0
 def crash_after_one(*args):
  global count
  count+=1
  if count==2:raise OSError('injected failure')
  return real(*args)
 with patch('stage317.os.replace',side_effect=crash_after_one):
  try:stage317.stage(root)
  except OSError:pass
  else:raise AssertionError('crash not exercised')
 ck(before=={p.name:p.read_bytes() for p in root.iterdir()})
 ck(not list(root.glob('.gk317-*')))
print(f'EXEC0317 hostile source staging PASS ({checks} checks)')
