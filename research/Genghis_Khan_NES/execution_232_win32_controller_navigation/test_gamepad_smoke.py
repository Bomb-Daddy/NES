#!/usr/bin/env python3
"""Fail-closed EXEC0232 executable source integration assertions (not NES parity)."""
from pathlib import Path
from tempfile import TemporaryDirectory
from importlib.util import spec_from_file_location, module_from_spec

p=Path(__file__).with_name('apply_gamepad_smoke.py')
spec=spec_from_file_location('gk232',p)
m=module_from_spec(spec);spec.loader.exec_module(m)
predecessor = '\n'.join(['/* EXEC0231: opt-in per-stage */'] + [a for a,b in m.CHANGES])
new=m.apply(predecessor)
assert new.count('EXEC0232 scripted raw XInput') == 1
assert 'pad_actions(&decoded)' in new
assert 'memcmp(gk232_original_worlds,app.worlds' in new
assert 'app.rules_profile==0u' in new
assert 'gk230_win32_smoke_frames(' in new
try: m.apply(new); raise AssertionError('duplicate overlay accepted')
except ValueError: pass
try: m.apply(predecessor.replace('EXEC0231:', 'STALE:')); raise AssertionError('stale accepted')
except ValueError: pass
for anchor,_ in m.CHANGES:
    try: m.apply(predecessor.replace(anchor,'TAMPERED'))
    except ValueError: pass
    else: raise AssertionError('mutated anchor accepted')
    try: m.apply(predecessor.replace(anchor,anchor+'\n'+anchor))
    except ValueError: pass
    else: raise AssertionError('duplicate anchor accepted')
with TemporaryDirectory() as d:
    root=Path(d);(root/'execution_199').mkdir()
    shell=root/'execution_199/gk1_native_shell.c'
    shell.write_text(predecessor)
    m.install(root)
    assert shell.read_text()==new
    try: m.install(root);raise AssertionError('two applications accepted')
    except ValueError: pass
print('EXEC0232 hostile overlay/duplicate/stale/tamper tests PASS')
