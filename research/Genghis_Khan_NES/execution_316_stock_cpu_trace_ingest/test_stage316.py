#!/usr/bin/env python3
import hashlib
from pathlib import Path
import tempfile
from unittest.mock import patch
from stage316 import stage,prepare,SOURCE_DIR,SOURCE_SHA256

def fixture(root):
 (root/'gk1_app_state.c').write_bytes(
  b'#include "gk1_app_state.h"\n#define GK1_PRE_SETUP_TEMPLATE 0\n#define GK53_SETUP_AWAIT_SOURCE 4\n'
  b'void gk1_app_install_templates(void){}\n')
 (root/'gk1_app_state.h').write_bytes(
  b'#include "gk300_stock_nmi_seed_clock.h"\n#endif\n')
 (root/'CMakeLists.txt').write_bytes(
  b'add_library(gk1_app_state STATIC gk300_stock_nmi_seed_clock.c)\n'
  b'add_test(NAME GK1_257_CurrentWin32Workflow_RejectsStaleSourceAndFalsePass COMMAND true)\n')
def ck(v):
 global checks
 checks+=1
 if not v:raise AssertionError(f'EXEC0316 hostile source stage check #{checks}')
checks=0
with tempfile.TemporaryDirectory() as tmp:
 root=Path(tmp);fixture(root)
 before={p.name:p.read_bytes() for p in root.iterdir()}
 preview=prepare(root)
 ck(len(preview)==7 and 'test_gk316_cpu_claims.c' in preview)
 digests=stage(root)
 ck(len(digests)==7 and all(hashlib.sha256((root/n).read_bytes()).hexdigest()==h for n,h in digests.items()))
 ck((root/'gk1_app_state.c').read_bytes().count(b'GK53_SETUP_AWAIT_SOURCE')==2)
 ck(b'GK1_316_CpuTraceClaimsAtomicAndClassicSetupGate' in (root/'CMakeLists.txt').read_bytes())
 frozen={p.name:p.read_bytes() for p in root.iterdir()}
 try:stage(root)
 except ValueError:pass
 else:raise AssertionError('duplicate stage accepted')
 ck(frozen=={p.name:p.read_bytes() for p in root.iterdir()})
with tempfile.TemporaryDirectory() as tmp:
 root=Path(tmp);fixture(root)
 (root/'CMakeLists.txt').write_text('add_library(x STATIC unrelated.c)\n')
 try:prepare(root)
 except ValueError:pass
 else:raise AssertionError('changed source anchor accepted')
 ck(not (root/'test_gk316_cpu_claims.c').exists())
with tempfile.TemporaryDirectory() as tmp:
 root=Path(tmp);fixture(root)
 original={p.name:p.read_bytes() for p in root.iterdir()}
 real_replace=__import__('os').replace
 count=0
 def fail_second(a,b):
  global count
  count+=1
  if count==2:raise OSError('injected unexpected crash')
  return real_replace(a,b)
 with patch('stage316.os.replace',side_effect=fail_second):
  try:stage(root)
  except OSError:pass
  else:raise AssertionError('failure injection did not fail')
 ck({p.name:p.read_bytes() for p in root.iterdir()}==original)
 ck(not list(root.glob('.gk316-*')))
print(f'EXEC0316 HOSTILE Python source staging PASS: {checks} checks, exact published C11 bytes, duplicate/refusal and atomic rollback')
