#!/usr/bin/env python3
"""EXEC0233 source-integrity/hostile mutation checks; Win32 execution is CI-only."""
from importlib.util import spec_from_file_location,module_from_spec
from pathlib import Path
from tempfile import TemporaryDirectory
p=Path(__file__).with_name('apply_practice_smoke.py')
spec=spec_from_file_location('gk233',p);m=module_from_spec(spec);spec.loader.exec_module(m)
base='\n'.join(['/* EXEC0232 scripted raw XInput */']+[before for before,_ in m.CHANGES])
result=m.apply(base)
assert 'EXEC0233: private, Enhanced-only' in result
assert result.count('EXEC0233')>=4
assert 'gk99_xinput_sample(&raw,&decoded)' in result
assert 'memcmp(gk232_original_worlds,app.worlds,sizeof gk232_original_worlds)==0' in result
assert 'app.port_spring.actions_committed==2u' in result
assert 'CORRUPT - UNCHANGED' in result
assert 'gk233_practice_mode?"--smoke-frames=4":cmd' in result
checks=0
for mutant in [result,base.replace('EXEC0232 scripted raw XInput','STALE')]:
    try:m.apply(mutant)
    except ValueError:checks+=1
    else:raise AssertionError('accepted repeated or stale predecessor')
for anchor,_ in m.CHANGES:
    for mutant in [base.replace(anchor,'TAMPERED'),base.replace(anchor,anchor+'\n'+anchor)]:
        try:m.apply(mutant)
        except ValueError:checks+=1
        else:raise AssertionError('accepted missing/duplicate source anchor')
with TemporaryDirectory() as d:
    root=Path(d); (root/'execution_199').mkdir()
    target=root/'execution_199/gk1_native_shell.c';target.write_text(base)
    m.install(root); assert target.read_text()==result
    try:m.install(root)
    except ValueError:checks+=1
    else:raise AssertionError('accepted duplicate install')
print(f'EXEC0233 VERIFIED_STATIC {checks} hostile source-integrity and mutation negative checks PASS')