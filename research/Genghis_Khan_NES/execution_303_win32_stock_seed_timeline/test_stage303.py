#!/usr/bin/env python3
"""EXEC0303 hostile Windows source staging checks. No original ROM required."""
import hashlib
import importlib.util
from pathlib import Path
import tempfile

spec=importlib.util.spec_from_file_location('stage303',Path(__file__).with_name('stage303.py'))
m=importlib.util.module_from_spec(spec)
spec.loader.exec_module(m)
H='#include "gk300_stock_nmi_seed_clock.h"\n#ifndef G\n#define G\n#endif\n'
C='#include "gk1_app_state.h"\nint baseline=1;\n'
CM='add_library(gk1_app_state STATIC gk300_stock_nmi_seed_clock.c)\n'+''.join(
 'add_test(NAME '+n+' COMMAND baseline)\n' for n in [
 'GK1_257_CurrentWin32Workflow_RejectsStaleSourceAndFalsePass',
 'GK1_297_StockResetInitializerAndUnknownC289Preseed',
 'GK1_300_OriginalStockNmiGateClockAndRollback'])
SRC={
 'gk302_stock_seed_timeline.c':b'int source=1;\n',
 'gk302_stock_seed_timeline.h':b'#ifndef SOURCE\n#define SOURCE\n#endif\n',
 'test_gk302_stock_seed_timeline.c':b'int main(void){return 0;}\n'}
passed=0
# Ensure all fixtures use LF on Windows as well as Linux.
def refused(root):
 global passed
 previous={p.name:p.read_bytes() for p in root.iterdir()}
 try:m.stage(root)
 except (ValueError,OSError):pass
 else:raise AssertionError('invalid predecessor accepted')
 assert previous=={p.name:p.read_bytes() for p in root.iterdir()}
 passed+=1

with tempfile.TemporaryDirectory() as temp:
 root=Path(temp)/'app';root.mkdir()
 source=Path(temp)/'verified';source.mkdir()
 for n,s in [('gk1_app_state.c',C),('gk1_app_state.h',H),('CMakeLists.txt',CM)]:
  (root/n).write_bytes(s.encode('utf-8'))
 for n,b in SRC.items():(source/n).write_bytes(b)
 m.SOURCE_DIRECTORY=source
 m.EXPECTED_APP={n:hashlib.sha256((root/n).read_bytes()).hexdigest()
   for n in ['gk1_app_state.c','gk1_app_state.h']}
 m.SOURCE_BLOBS={n:m.source_blob(b) for n,b in SRC.items()}
 (root/'gk1_app_state.c').write_bytes((C+'// altered\n').encode('utf-8'));refused(root)
 (root/'gk1_app_state.c').write_bytes(C.encode('utf-8'))
 (source/'gk302_stock_seed_timeline.c').write_bytes(SRC['gk302_stock_seed_timeline.c']+b'X');refused(root)
 (source/'gk302_stock_seed_timeline.c').write_bytes(SRC['gk302_stock_seed_timeline.c'])
 (root/'gk1_app_state.h').write_bytes(H.replace('gk300_stock_nmi_seed_clock.h','wrong.h').encode('utf-8'))
 m.EXPECTED_APP['gk1_app_state.h']=hashlib.sha256((root/'gk1_app_state.h').read_bytes()).hexdigest()
 refused(root)
 (root/'gk1_app_state.h').write_bytes(H.encode('utf-8'))
 m.EXPECTED_APP['gk1_app_state.h']=hashlib.sha256(H.encode()).hexdigest()
 (root/'CMakeLists.txt').write_bytes(CM.replace('gk300_stock_nmi_seed_clock.c','missing.c').encode('utf-8'));refused(root)
 (root/'CMakeLists.txt').write_bytes(CM.replace('GK1_257_CurrentWin32Workflow_RejectsStaleSourceAndFalsePass','MISSING').encode('utf-8'));refused(root)
 (root/'CMakeLists.txt').write_bytes(CM.encode('utf-8'))
 (root/'gk1_app_state.c').write_bytes(C.encode()+b'\rX');refused(root)
 (root/'gk1_app_state.c').write_bytes(C.encode('utf-8'))
 result=m.stage(root)
 assert len(result)==6
 assert 'gk302_stock_seed_timeline.c' in (root/'CMakeLists.txt').read_text()
 assert 'Gk302Status gk1_app_original_seed_timeline' in (root/'gk1_app_state.c').read_text()
 assert 'GK1_257_CurrentWin32Workflow' in (root/'CMakeLists.txt').read_text()
 passed+=4
 refused(root)
 print('GK1_EXEC0303 PASS',passed,'hostile and positive staging checks')
