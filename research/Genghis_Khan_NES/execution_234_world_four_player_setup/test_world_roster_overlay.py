#!/usr/bin/env python3
"""Independent hostile assurance: clean overlay, source equivalence and refusal."""
from pathlib import Path
import hashlib
import importlib.util
import shutil
import tempfile
import sys
import zipfile

here=Path(__file__).resolve().parent
module=importlib.util.spec_from_file_location('gk234',here/'apply_world_roster.py')
mod=importlib.util.module_from_spec(module);module.loader.exec_module(mod)
if len(sys.argv)!=3: raise SystemExit('usage test.py EXEC0230.zip EXPECTED_MODIFIED_SOURCE_DIR')
original_zip=Path(sys.argv[1]);expected=Path(sys.argv[2]); checks=0
with tempfile.TemporaryDirectory(prefix='gk234-') as t:
  root=Path(t); src=root/'execution_199';src.mkdir()
  with zipfile.ZipFile(original_zip) as z:
    for name in ['gk1_native_shell.c','CMakeLists.txt']:
      (src/name).write_bytes(z.read('execution_199/'+name))
  mod.apply(root,here/'test_gk234_world_four_players.c');checks+=1
  for name in ['gk1_native_shell.c','CMakeLists.txt','test_gk234_world_four_players.c']:
    assert (src/name).read_bytes()==(expected/name).read_bytes(),name
    checks+=1
  before={p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in src.iterdir()}
  try:mod.apply(root,here/'test_gk234_world_four_players.c')
  except ValueError: checks+=1
  else:raise AssertionError('accepted duplicate overlay')
  assert before=={p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in src.iterdir()};checks+=1
  # A source anchor tampered by 1 byte must fail before any writes.
  orig=src/'gk1_native_shell.c';target=orig.read_text()
  orig.write_text(target.replace('COMMITTED WORLD LORD COUNTRIES:', 'CORRUPTED WORLD LORD COUNTRIES:',1))
  snapshot={p.name:p.read_bytes() for p in src.iterdir()}
  try:mod.apply(root,here/'test_gk234_world_four_players.c')
  except ValueError:checks+=1
  else:raise AssertionError('accepted modified prior HUD source')
  assert snapshot=={p.name:p.read_bytes() for p in src.iterdir()};checks+=1
print('EXEC0234 exact original-source replay and seven hostile/equivalence gates PASS',checks)
