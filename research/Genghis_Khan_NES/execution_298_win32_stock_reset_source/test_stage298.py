#!/usr/bin/env python3
"""Hostile EXEC0298 tests against copied, actual EXEC0295 Win32 application.
Set GK298_PREDECESSOR_DIR to the fully staged Win32 execution_199 tree.
All tests operate on temporary copies and never alter a user's saves/ROM.
"""
import hashlib, os, shutil, tempfile, unittest
from pathlib import Path
from unittest.mock import patch
import stage298 as stage_mod

SOURCES=('gk1_app_state.c','gk1_app_state.h','CMakeLists.txt',
         'packaging/test_gk265_current_win32_source.py')

def sha(b):return hashlib.sha256(b).hexdigest()

class Stage298Tests(unittest.TestCase):
 def setUp(self):
  predecessor=os.environ.get('GK298_PREDECESSOR_DIR')
  if not predecessor:self.skipTest('requires inherited EXEC0295 actual Win32 staged source')
  predecessor=Path(predecessor)
  self.temp=tempfile.TemporaryDirectory(prefix='gk298_hostile_')
  self.dst=Path(self.temp.name)/'execution_199';self.dst.mkdir()
  for name in SOURCES:
   p=self.dst/name;p.parent.mkdir(parents=True,exist_ok=True)
   shutil.copyfile(predecessor/name,p)
  self.assertEqual(sha(stage_mod.canonical((self.dst/SOURCES[0]).read_bytes(),SOURCES[0])),stage_mod.OLD_C_HASH)
  self.assertEqual(sha(stage_mod.canonical((self.dst/SOURCES[1]).read_bytes(),SOURCES[1])),stage_mod.OLD_H_HASH)
 def tearDown(self):
  if hasattr(self,'temp'):self.temp.cleanup()
 def snap(self):
  return {str(p.relative_to(self.dst)):sha(p.read_bytes())
     for p in self.dst.rglob('*') if p.is_file()}
 def rejects_without_changes(self,source):
  old=self.snap()
  with self.assertRaises((ValueError,OSError,UnicodeError)):
   source()
  self.assertEqual(old,self.snap())
 def test_stage_exact_and_repeat_refusal(self):
  stage_mod.stage(self.dst)
  self.assertEqual(sha((self.dst/'gk1_app_state.c').read_bytes()),stage_mod.NEW_C_HASH)
  self.assertEqual(sha((self.dst/'gk1_app_state.h').read_bytes()),stage_mod.NEW_H_HASH)
  for name,digest in stage_mod.MODULES.items():
   self.assertEqual(sha((self.dst/name).read_bytes()),digest)
  cm=(self.dst/'CMakeLists.txt').read_text()
  self.assertIn(stage_mod.NAME_296,cm);self.assertIn(stage_mod.NAME_297,cm)
  self.assertIn(stage_mod.CMAKE_LIB_NEW,cm)
  self.assertIn(stage_mod.NEW_C_HASH,(self.dst/SOURCES[3]).read_text())
  self.rejects_without_changes(lambda:stage_mod.stage(self.dst))
 def test_source_corruption_atomic(self):
  p=self.dst/'gk1_app_state.c';p.write_bytes(p.read_bytes()+b'\n')
  self.rejects_without_changes(lambda:stage_mod.stage(self.dst))
 def test_header_corruption_atomic(self):
  p=self.dst/'gk1_app_state.h';p.write_bytes(p.read_bytes().replace(b'Gk1App;',b'Gk1App_bad;'))
  self.rejects_without_changes(lambda:stage_mod.stage(self.dst))
 def test_duplicate_module_atomic(self):
  (self.dst/'gk296_stock_seed_origin.h').write_text('not allowed')
  self.rejects_without_changes(lambda:stage_mod.stage(self.dst))
 def test_no_cmake_predecessor_atomic(self):
  p=self.dst/'CMakeLists.txt'
  p.write_bytes(p.read_bytes().replace(b'GK1_294_OriginalRomSourceSeedWriterAndAtomicRefusals',b'missing_294_test'))
  self.rejects_without_changes(lambda:stage_mod.stage(self.dst))
 def test_corrupt_cmake_library_atomic(self):
  p=self.dst/'CMakeLists.txt'
  p.write_bytes(p.read_bytes().replace(stage_mod.CMAKE_LIB_OLD.encode(),b'BAD_LIB'))
  self.rejects_without_changes(lambda:stage_mod.stage(self.dst))
 def test_missing_compiled_gate_atomic(self):
  (self.dst/SOURCES[3]).unlink()
  self.rejects_without_changes(lambda:stage_mod.stage(self.dst))
 def test_wrong_compiled_gate_atomic(self):
  (self.dst/SOURCES[3]).write_text("EXPECTED_APP_SHA = '0000'\n")
  self.rejects_without_changes(lambda:stage_mod.stage(self.dst))
 def test_duplicate_ctest_atomic(self):
  p=self.dst/'CMakeLists.txt'
  p.write_bytes(p.read_bytes()+b'\n# GK1_297_StockResetInitializerAndUnknownC289Preseed\n')
  self.rejects_without_changes(lambda:stage_mod.stage(self.dst))
 def test_normalizes_tracked_crlf_source(self):
  for name in SOURCES:
   p=self.dst/name;p.write_bytes(p.read_bytes().replace(b'\n',b'\r\n'))
  stage_mod.stage(self.dst)
  self.assertEqual(sha((self.dst/'gk1_app_state.c').read_bytes()),stage_mod.NEW_C_HASH)
 def test_lone_cr_rejected(self):
  p=self.dst/'gk1_app_state.c';p.write_bytes(p.read_bytes()+b'\r')
  self.rejects_without_changes(lambda:stage_mod.stage(self.dst))
 def test_upstream_source_tamper_atomic(self):
  source=stage_mod.SOURCE_297
  with tempfile.TemporaryDirectory() as d:
   temp=Path(d)
   for n in stage_mod.MODULES:
    if '297' in n:shutil.copyfile(source/n,temp/n)
   p=temp/'gk297_stock_reset_seed.c';p.write_bytes(p.read_bytes()+b'\n/* corrupt */\n')
   old=self.snap()
   with patch.object(stage_mod,'SOURCE_297',temp):
    with self.assertRaises(ValueError):stage_mod.stage(self.dst)
   self.assertEqual(self.snap(),old)
 def test_wrong_campaign_dummy_does_not_unlock(self):
  # Source integration itself cannot mark a Spring, alter external files,
  # import a ROM, or create a user checkpoint; only registered readonly tests.
  stage_mod.stage(self.dst)
  contents=(self.dst/'gk1_app_state.c').read_text()
  self.assertIn('return gk297_reset_initializer_from_source(rom,tail,out);',contents)
  self.assertIn('return gk296_check_four_writer_first_draw(',contents)
  self.assertNotIn('GK1_PLAYING',stage_mod.APP_APPEND)

if __name__=='__main__':unittest.main(verbosity=2)
