import unittest
import tempfile
import pathlib
import shutil
from stage331 import stage, REQUIRED, SOURCES, APPEND
H=pathlib.Path(__file__).resolve().parent
class Stage331Tests(unittest.TestCase):
 def fixture(self):
  t=tempfile.TemporaryDirectory()
  root=pathlib.Path(t.name)
  for name in REQUIRED:(root/name).write_text('add_library(gk1_app_state STATIC gk1_app_state.c)\n' if name=='CMakeLists.txt' else 'int source=1;\n')
  return t,root
 def test_atomic_positive_idempotent(self):
  temp,root=self.fixture()
  with temp:
   self.assertTrue(stage(root))
   self.assertFalse(stage(root))
   self.assertIn('target_sources(gk1_app_state PRIVATE gk331_stock_offer_rng.c', (root/'CMakeLists.txt').read_text())
   for name in SOURCES:self.assertEqual((root/name).read_bytes(),(H/name).read_bytes())
 def test_reject_wrong_inherited_app(self):
  temp,root=self.fixture()
  with temp:
   (root/'gk1_app_state.c').unlink()
   before=(root/'CMakeLists.txt').read_bytes()
   with self.assertRaises(ValueError):stage(root)
   self.assertEqual((root/'CMakeLists.txt').read_bytes(),before)
   self.assertFalse(any((root/n).exists() for n in SOURCES))
 def test_reject_source_conflict_rollback(self):
  temp,root=self.fixture()
  with temp:
   (root/SOURCES[2]).write_text('a poisoned source\n')
   before=(root/'CMakeLists.txt').read_bytes()
   with self.assertRaises(ValueError):stage(root)
   self.assertEqual((root/'CMakeLists.txt').read_bytes(),before)
   self.assertFalse((root/SOURCES[0]).exists())
 def test_reject_marker_missing_source(self):
  temp,root=self.fixture()
  with temp:
   self.assertTrue(stage(root))
   (root/SOURCES[0]).unlink()
   before=(root/'CMakeLists.txt').read_bytes()
   with self.assertRaises(ValueError):stage(root)
   self.assertEqual((root/'CMakeLists.txt').read_bytes(),before)
 def test_accept_crlf_checkout_only_for_identical(self):
  temp,root=self.fixture()
  with temp:
   self.assertTrue(stage(root))
   for name in SOURCES:
    raw=(root/name).read_bytes()
    (root/name).write_bytes(raw.replace(b'\n',b'\r\n'))
   self.assertFalse(stage(root))
 def test_reject_noncanonical_lone_cr(self):
  temp,root=self.fixture()
  with temp:
   self.assertTrue(stage(root))
   (root/SOURCES[1]).write_bytes((root/SOURCES[1]).read_bytes()+b'\r')
   with self.assertRaises(ValueError):stage(root)
if __name__=='__main__':unittest.main()
