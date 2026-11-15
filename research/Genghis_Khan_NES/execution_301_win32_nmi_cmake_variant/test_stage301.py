#!/usr/bin/env python3
"""Actual native C11 source regression for EXEC0301 Win32 CMake variant."""
import os,tempfile,shutil,hashlib,unittest
from pathlib import Path
from unittest.mock import patch
import stage301

BASE=Path(os.environ.get('GK301_PRE300_APP',''))
NAMES=tuple(stage301.BEFORE)+('CMakeLists.txt',)

def sha(b):return hashlib.sha256(b).hexdigest()

class Win32NmiStage(unittest.TestCase):
 def setUp(self):
  if not BASE.is_dir(): raise AssertionError('GK301_PRE300_APP full native predecessor required; no skipped tests')
  self.tmp=tempfile.TemporaryDirectory(prefix='gk301_hostile_')
  self.root=Path(self.tmp.name)
  for n in NAMES: shutil.copyfile(BASE/n,self.root/n)
  for n,h in stage301.BEFORE.items():
   self.assertEqual(sha(stage301.canon((self.root/n).read_bytes())),h)
 def tearDown(self):self.tmp.cleanup()
 def snapshot(self):return {p.name:sha(p.read_bytes()) for p in self.root.iterdir() if p.is_file()}
 def reject(self):
  before=self.snapshot()
  with self.assertRaises((ValueError,OSError,UnicodeError)):stage301.stage(self.root)
  self.assertEqual(before,self.snapshot(),'failed source gate modified the native game')
 def test_reviewed_postimages_and_double_stage_refusal(self):
  stage301.stage(self.root)
  for n,h in stage301.AFTER.items():self.assertEqual(sha((self.root/n).read_bytes()),h)
  self.reject()
 def test_real_win32_extra_ctests_preserved(self):
  cm=self.root/'CMakeLists.txt';extra='\n# EXTRA WIN32 PRIOR STAGING\nadd_test(NAME Win32_prior_contract COMMAND ${CMAKE_COMMAND} -E true)\n'
  cm.write_bytes(cm.read_bytes()+extra.encode())
  stage301.stage(self.root)
  self.assertIn(extra,(self.root/'CMakeLists.txt').read_text())
  self.assertIn('GK1_300_OriginalStockNmiGateClockAndRollback',(self.root/'CMakeLists.txt').read_text())
 def test_crlf_windows_line_endings(self):
  for n in NAMES:
   p=self.root/n;p.write_bytes(p.read_bytes().replace(b'\n',b'\r\n'))
  stage301.stage(self.root)
  for n,h in stage301.AFTER.items():self.assertEqual(sha((self.root/n).read_bytes()),h)
 def test_lone_cr_refusal(self):
  p=self.root/'gk1_app_state.c';p.write_bytes(p.read_bytes()+b'\r');self.reject()
 def test_wrong_app_hash_refusal(self):
  p=self.root/'gk1_app_state.h';p.write_bytes(p.read_bytes()+b'CORRUPTION');self.reject()
 def test_missing_old_ctest_refusal(self):
  p=self.root/'CMakeLists.txt';p.write_bytes(p.read_bytes().replace(b'GK1_297_StockResetInitializerAndUnknownC289Preseed',b'REMOVED_297'))
  self.reject()
 def test_wrong_library_anchor_refusal(self):
  p=self.root/'CMakeLists.txt';p.write_bytes(p.read_bytes().replace(b'gk297_stock_reset_seed.c gk282_stock_rng_trace.c',b'gk297_stock_reset_seed.c SOME_OTHER.c'))
  self.reject()
 def test_duplicate_nmi_ctest_refusal(self):
  p=self.root/'CMakeLists.txt';p.write_bytes(p.read_bytes()+b'\nGK1_300_OriginalStockNmiGateClockAndRollback\n')
  self.reject()
 def test_no_write_on_source_error(self):
  p=self.root/'gk1_app_state.c';p.write_bytes(p.read_bytes()+b'X')
  with patch.object(stage301.os,'replace',side_effect=AssertionError('should not write')):self.reject()
 def test_missing_nmi_payload(self):
  with patch.object(stage301,'SOURCE',Path('/missing/exec300')):self.reject()
 def test_win32_source_gate_is_advanced_without_false_acceptance(self):
  gate=self.root/'packaging/test_gk265_current_win32_source.py'
  gate.parent.mkdir(exist_ok=True)
  old="EXPECTED_APP_SHA = '"+stage301.BEFORE['gk1_app_state.c']+"'"
  gate.write_text(old+'\n')
  stage301.stage(self.root)
  self.assertIn(stage301.AFTER['gk1_app_state.c'],gate.read_text())
 def test_wrong_win32_source_gate_refuses_atomically(self):
  gate=self.root/'packaging/test_gk265_current_win32_source.py'
  gate.parent.mkdir(exist_ok=True)
  gate.write_text("EXPECTED_APP_SHA = 'WRONG'\n")
  self.reject()
 def test_preserves_existing_unrelated_cmake_comments(self):
  p=self.root/'CMakeLists.txt'
  extra='\n# DO NOT DELETE PRIOR WIN32 HOSTILE QA\nset(GK1_USER_SAVE_PROTECTION TRUE)\n'
  p.write_bytes(p.read_bytes()+extra.encode())
  stage301.stage(self.root)
  self.assertIn(extra,(self.root/'CMakeLists.txt').read_text())

if __name__=='__main__':unittest.main(verbosity=2)
