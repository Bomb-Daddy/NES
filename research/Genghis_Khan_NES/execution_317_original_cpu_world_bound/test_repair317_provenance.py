#!/usr/bin/env python3
import tempfile
from pathlib import Path
import stage317
import repair317_provenance as p
checks=0
TOKENS=(b'EXPECTED_TEST_SHA',b'EXPECTED_PREDECESSOR_SHA',
        b'for index,values in enumerate(mutations,1)',
        b'GK1_265_CurrentWin32BuildSourceMustMatchEXEC0264')
def ck(v):
 global checks
 checks+=1
 if not v:raise AssertionError(f'EXEC0317 provenance check {checks}')
def setup(root):
 source=b'#include <stdlib.h>\n#include "gk1_app_state.h"\n'
 original=source+b'\n'+stage317.CORRECTED.encode()
 (root/'gk1_app_state.c').write_bytes(original)
 (root/'CMakeLists.txt').write_bytes(b'old\n'+stage317.CMAKE_APPEND.encode())
 (root/'test_gk317_world_bound.c').write_bytes((Path(stage317.__file__).parent/'test_gk317_world_bound.c').read_bytes())
 (root/'packaging').mkdir()
 (root/'packaging/test_gk265_current_win32_source.py').write_bytes(
  ("EXPECTED_APP_SHA = '"+p.sha(source)+"'\n").encode()+b'\n'.join(TOKENS)+b'\n')
 return original
with tempfile.TemporaryDirectory() as tmp:
 root=Path(tmp);original=setup(root)
 before=(root/'packaging/test_gk265_current_win32_source.py').read_bytes()
 ck(p.old_app_from_current(original)==b'#include <stdlib.h>\n#include "gk1_app_state.h"\n')
 ck(p.repair(root)==p.sha(original))
 gate=(root/'packaging/test_gk265_current_win32_source.py').read_bytes()
 ck(p.sha(original).encode() in gate)
 ck(all(x in gate for x in TOKENS))
 ck(before!=gate)
 # Drift in new source fails without modifying the updated hostile gate.
 (root/'gk1_app_state.c').write_bytes(original+b'// forged\n')
 try:p.repair(root)
 except ValueError:pass
 else:raise AssertionError('forged native postimage accepted')
 ck(gate==(root/'packaging/test_gk265_current_win32_source.py').read_bytes())
with tempfile.TemporaryDirectory() as tmp:
 root=Path(tmp);setup(root)
 gate=root/'packaging/test_gk265_current_win32_source.py'
 gate.write_bytes(gate.read_bytes().replace(b'EXPECTED_TEST_SHA',b'INERT_TEST'))
 prior=gate.read_bytes()
 try:p.repair(root)
 except ValueError:pass
 else:raise AssertionError('broken inherited hostile gate accepted')
 ck(prior==gate.read_bytes())
print(f'EXEC0317 hardened provenance QA PASS ({checks} checks)')
