#!/usr/bin/env python3
"""EXEC0235 hostile source overlay replay with explicit predecessor and tamper controls."""
from pathlib import Path
import importlib.util, tempfile, hashlib
import sys
script=Path(__file__).with_name('apply_multiplayer_guard.py')
spec=importlib.util.spec_from_file_location('gk235',script)
mod=importlib.util.module_from_spec(spec);spec.loader.exec_module(mod)
root=Path(sys.argv[1]);old=(root/'execution_199/gk1_app_state.c').read_text()
cm=(root/'execution_199/CMakeLists.txt').read_text()
# Caller provides installed 235 source, replay requires subtracting only exact known delta.
a=old.replace(mod.NEW1,mod.OLD1,1).replace(mod.NEW2,mod.OLD2,1)
b=cm.replace(mod.APPEND,'',1)
checks=0
assert a!=old and b!=cm; checks+=1
new,cmnew=mod.transform(a,b);assert new==old and cmnew==cm;checks+=1
for sr,build in ((old,cm),(a.replace(mod.OLD1,'BROKEN',1),b),(a+mod.OLD1,b),(a,b+mod.APPEND)):
 try:mod.transform(sr,build)
 except ValueError:checks+=1
 else:raise AssertionError('overlay accepted invalid predecessor')
with tempfile.TemporaryDirectory() as tmp:
 p=Path(tmp);(p/'execution_199').mkdir()
 s=p/'execution_199/gk1_app_state.c';c=p/'execution_199/CMakeLists.txt';s.write_text(a);c.write_text(b)
 mod.install(p,script.with_name('test_gk235_world_multi_guard.c'))
 assert s.read_text()==old and c.read_text()==cm;checks+=1
 try:mod.install(p,script.with_name('test_gk235_world_multi_guard.c'))
 except ValueError:checks+=1
 else:raise AssertionError('duplicate installation accepted')
print('EXEC0235 hostile source overlay '+str(checks)+'/8 PASS')
