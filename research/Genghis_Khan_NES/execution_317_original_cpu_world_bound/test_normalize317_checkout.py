#!/usr/bin/env python3
from pathlib import Path
import tempfile
from unittest.mock import patch
import normalize317_checkout as n
calls=0
def ck(ok):
 global calls
 calls+=1
 if not ok:raise AssertionError('normalizer hostile check '+str(calls))
source=b'#include <stdlib.h>\nint main(void){return 0;}\n'
# Source pin is test-controlled only, NEVER a production pin.
with tempfile.TemporaryDirectory() as td:
 p=Path(td)/'test_gk317_world_bound.c'
 with patch.dict(n.PINS,{p.name:n.sha(source)}):
  p.write_bytes(source)
  ck(n.normalize(p)==n.sha(source))
  ck(p.read_bytes()==source)
  p.write_bytes(source.replace(b'\n',b'\r\n'))
  ck(n.normalize(p)==n.sha(source))
  ck(p.read_bytes()==source)
  bad=source.replace(b'\n',b'\r\r\n');p.write_bytes(bad)
  try:n.normalize(p)
  except ValueError:pass
  else:raise AssertionError('CRCRLF accepted')
  ck(p.read_bytes()==bad)
  bad=source+b'EVIL';p.write_bytes(bad)
  try:n.normalize(p)
  except ValueError:pass
  else:raise AssertionError('sha-altered source accepted')
  ck(p.read_bytes()==bad)
  bad=source.replace(b'\n',b'\r');p.write_bytes(bad)
  try:n.normalize(p)
  except ValueError:pass
  else:raise AssertionError('bare CR accepted')
  ck(p.read_bytes()==bad)
  q=Path(td)/'arbitrary.c';q.write_bytes(source)
  try:n.normalize(q)
  except ValueError:pass
  else:raise AssertionError('unreviewed basename accepted')
  ck(q.read_bytes()==source)
  # Failed transactional replace may not mutate the existing source.
  p.write_bytes(source.replace(b'\n',b'\r\n'));prior=p.read_bytes()
  with patch('normalize317_checkout.os.replace',side_effect=OSError('simulated')):
   try:n.normalize(p)
   except OSError:pass
   else:raise AssertionError('write failure not surfaced')
  ck(p.read_bytes()==prior)
  ck(not list(Path(td).glob('.gk317-normalize-*')))
print(f'EXEC0317 generic-exact-source checkout LF/CRLF hostile QA PASS ({calls} assertions)')
