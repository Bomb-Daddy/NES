#!/usr/bin/env python3
"""Hostile, isolated tests for the actual EXEC0298 Win32 gk294-link gap."""
import hashlib,os,shutil,tempfile,unittest
from pathlib import Path
from unittest.mock import patch
import stage299

BASE=Path(__file__).resolve().parent
APP=Path(os.environ.get('GK299_POST298_APP',''))
OLD=Path(os.environ.get('GK299_GK294_OLD',''))
FILES=('gk1_app_state.c','CMakeLists.txt','gk296_stock_seed_origin.c')
OLD_FILES=tuple(stage299.BEFORE)
def sha(b):return hashlib.sha256(b).hexdigest()

class MaskedLinkRepair(unittest.TestCase):
 def setUp(self):
  if not APP.is_dir() or not OLD.is_dir():self.skipTest('Actual staged app and original gk294 sources required')
  self.temp=tempfile.TemporaryDirectory(prefix='gk299_')
  self.base=Path(self.temp.name)
  for n in FILES:shutil.copyfile(APP/n,self.base/n)
  for n in OLD_FILES:shutil.copyfile(OLD/n,self.base/n)
  self.assertEqual(sha((self.base/'gk294_seed_writer_attest.c').read_bytes()),stage299.BEFORE['gk294_seed_writer_attest.c'])
 def tearDown(self):
  if hasattr(self,'temp'):self.temp.cleanup()
 def snapshot(self):
  return {x.name:sha(x.read_bytes()) for x in self.base.iterdir() if x.is_file()}
 def reject(self):
  before=self.snapshot()
  with self.assertRaises((ValueError,OSError)):stage299.stage(self.base)
  self.assertEqual(before,self.snapshot(),'Refusal modified the inherited source!')
 def test_real_windows_preimage_to_exact_0296_postimage(self):
  stage299.stage(self.base)
  self.assertEqual({n:sha((self.base/n).read_bytes()) for n in OLD_FILES},stage299.AFTER)
  self.assertIn('return gk294_attest_seed_writers_masked(rom,writes,count,0x3fu,out);',(self.base/'gk294_seed_writer_attest.c').read_text())
  self.reject() # never silently reapply
 def test_windows_crlf_normalizes_only_crlf(self):
  for n in FILES+OLD_FILES:
   p=self.base/n;p.write_bytes(p.read_bytes().replace(b'\n',b'\r\n'))
  stage299.stage(self.base)
  self.assertEqual(sha((self.base/'gk294_seed_writer_attest.c').read_bytes()),stage299.AFTER['gk294_seed_writer_attest.c'])
 def test_lone_cr_refusal(self):
  p=self.base/'gk294_seed_writer_attest.c';p.write_bytes(p.read_bytes()+b'\r')
  self.reject()
 def test_wrong_source_byte(self):
  p=self.base/'gk294_seed_writer_attest.c';p.write_bytes(p.read_bytes().replace(b'if(mask!=0x3fu)',b'if(mask!=0x7fu)'))
  self.reject()
 def test_truncated_header(self):
  p=self.base/'gk294_seed_writer_attest.h';p.write_bytes(p.read_bytes()[:-10])
  self.reject()
 def test_unrelated_game_predecessor(self):
  p=self.base/'gk1_app_state.c';p.write_bytes(p.read_bytes()+b'\n')
  self.reject()
 def test_missing_game_source(self):
  (self.base/'gk1_app_state.c').unlink();self.reject()
 def test_missing_cmake_registration(self):
  p=self.base/'CMakeLists.txt';p.write_bytes(p.read_bytes().replace(b'gk296_stock_seed_origin.c',b'missing_source.c'))
  self.reject()
 def test_missing_297_ctest(self):
  p=self.base/'CMakeLists.txt';p.write_bytes(p.read_bytes().replace(b'GK1_297_StockResetInitializerAndUnknownC289Preseed',b'missing_297'))
  self.reject()
 def test_wrong_caller(self):
  p=self.base/'gk296_stock_seed_origin.c';p.write_bytes(p.read_bytes().replace(b'gk294_attest_seed_writers_masked(',b'gk294_attest_seed_writers('))
  self.reject()
 def test_missing_caller(self):
  (self.base/'gk296_stock_seed_origin.c').unlink();self.reject()
 def test_invalid_header_already_patched(self):
  p=self.base/'gk294_seed_writer_attest.h';p.write_bytes(p.read_bytes()+b'\n/* duplicate marker */')
  self.reject()
 def test_fail_closed_before_any_write(self):
  p=self.base/'gk294_seed_writer_attest.h';p.write_bytes(p.read_bytes()+b'\ncorrupt!')
  with patch.object(stage299.os,'replace',side_effect=AssertionError('must never touch disk')):
   self.reject()

if __name__=='__main__':unittest.main(verbosity=2)
