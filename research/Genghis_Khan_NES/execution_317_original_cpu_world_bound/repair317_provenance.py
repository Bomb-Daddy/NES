#!/usr/bin/env python3
"""EXEC0317: retain the inherited hostile app-source SHA test after 316/317.
Require exact backwards reversal of the new code to the already-verified
EXEC0314 app hash, not just a token or a self-reported QA result.
"""
import hashlib
import os
from pathlib import Path
import re
import sys
import tempfile
import stage317

def sha(raw):return hashlib.sha256(raw).hexdigest()
def old_app_from_current(raw):
 raw=stage317.norm(raw)
 suffix=('\n'+stage317.CORRECTED).encode()
 if raw.count(suffix)!=1 or raw.count(stage317.ORIGINAL.encode()):
  raise ValueError('unexpected or duplicate EXEC0317 native C11 source')
 if not raw.endswith(suffix):raise ValueError('source code appended in unexpected position')
 return raw[:-len(suffix)]

def repair(root):
 root=Path(root)
 gate=root/'packaging'/'test_gk265_current_win32_source.py'
 orig=gate.read_bytes()
 norm=stage317.norm(orig)
 pat=rb"(?m)^EXPECTED_APP_SHA = '([0-9a-f]{64})'$"
 matches=list(re.finditer(pat,norm))
 if len(matches)!=1:raise ValueError('missing/duplicate inherited hostile app SHA gate')
 before=matches[0].group(1).decode('ascii')
 app=stage317.norm((root/'gk1_app_state.c').read_bytes())
 if sha(old_app_from_current(app))!=before:
  raise ValueError('EXEC0314 verified inherited native C11 predecessor hash mismatch')
 cm=stage317.norm((root/'CMakeLists.txt').read_bytes())
 if cm.count(stage317.CMAKE_APPEND.encode())!=1:
  raise ValueError('world-bound C11 native test registry missing/duplicated')
 test=(root/'test_gk317_world_bound.c').read_bytes()
 if sha(test)!=stage317.SOURCE_SHA:
  raise ValueError('native cross-world regression source was modified')
 for token in (b'EXPECTED_TEST_SHA',b'EXPECTED_PREDECESSOR_SHA',
  b'for index,values in enumerate(mutations,1)',
  b'GK1_265_CurrentWin32BuildSourceMustMatchEXEC0264'):
  if token not in norm:raise ValueError('inherited hostile provenance control missing')
 after=norm[:matches[0].start(1)]+sha(app).encode()+norm[matches[0].end(1):]
 fd,p=tempfile.mkstemp(prefix='.gk317-gate-',dir=gate.parent)
 try:
  with os.fdopen(fd,'wb') as stream:
   stream.write(after);stream.flush();os.fsync(stream.fileno())
  os.replace(p,gate)
  if gate.read_bytes()!=after:raise OSError('hostile provenance source readback failed')
 except BaseException:
  gate.write_bytes(orig)
  raise
 finally:
  if os.path.exists(p):os.unlink(p)
 print('EXEC0317 inherited hostile provenance preserved; 314 preimage',before,'317 source SHA',sha(app))
 return sha(app)
if __name__=='__main__':
 if len(sys.argv)!=2:sys.exit('Usage: repair317_provenance.py CURRENT_COMPLETE_APP_DIR')
 try:repair(sys.argv[1])
 except (ValueError,OSError) as e:sys.exit('EXEC0317 provenance refused: '+str(e))
