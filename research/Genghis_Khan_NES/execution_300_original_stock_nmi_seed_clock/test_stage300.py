#!/usr/bin/env python3
import tempfile,unittest,zipfile,hashlib
from pathlib import Path
import stage300
Z='/mnt/data/GK1_EXEC0297_Complete_Inherited_C11_Source_QA.zip'
class Stage(unittest.TestCase):
 def setUp(self):
  self.temp=tempfile.TemporaryDirectory();self.root=Path(self.temp.name)
  with zipfile.ZipFile(Z) as z:
   for n in stage300.BEFORE:
    (self.root/n).write_bytes(z.read('execution_199/'+n))
 def tearDown(self):self.temp.cleanup()
 def test_stage_clean(self):
  stage300.stage(self.root)
  for n,h in stage300.AFTER.items():self.assertEqual(stage300.sha((self.root/n).read_bytes()),h)
  with self.assertRaises(ValueError):stage300.stage(self.root)
 def test_negative_wrong_predecessor(self):
  b=self.root/'gk1_app_state.c';b.write_bytes(b.read_bytes()+b'\nMALICIOUS')
  before={n:(self.root/n).read_bytes() for n in stage300.BEFORE}
  with self.assertRaises(ValueError):stage300.stage(self.root)
  self.assertEqual(before,{n:(self.root/n).read_bytes() for n in stage300.BEFORE})
 def test_mixed_cr_rejected(self):
  p=self.root/'gk1_app_state.h';p.write_bytes(p.read_bytes().replace(b'\n',b'\r',1))
  with self.assertRaises(ValueError):stage300.stage(self.root)
 def test_crlf_normalized(self):
  for n in stage300.BEFORE:
   p=self.root/n;p.write_bytes(p.read_bytes().replace(b'\n',b'\r\n'))
  stage300.stage(self.root)
  for n,h in stage300.AFTER.items():self.assertEqual(stage300.sha((self.root/n).read_bytes()),h)
 def test_source_payload_tamper(self):
  r=Path(stage300.__file__).parent; src=r/'gk300_stock_nmi_seed_clock.h';data=src.read_bytes()
  try:
   src.write_bytes(data+b'Q')
   with self.assertRaises(ValueError):stage300.stage(self.root)
  finally:src.write_bytes(data)
 def test_patch_tamper(self):
  r=Path(stage300.__file__).parent;src=r/'GK1_EXEC0300_Predecessor_299_to_300_Production.patch';data=src.read_bytes()
  try:
   src.write_bytes(data+b'Q')
   with self.assertRaises(ValueError):stage300.stage(self.root)
  finally:src.write_bytes(data)
if __name__=='__main__':unittest.main()
