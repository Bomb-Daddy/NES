"""Bounded original-6502 replay; requires private ROM and archived table.

This exercises source instructions, not a whole game or player command.
"""
from pathlib import Path
import importlib.util,json,unittest
ROOT=Path(__file__).resolve().parent
spec=importlib.util.spec_from_file_location('prior',ROOT/'replay_dcea.py')
p=importlib.util.module_from_spec(spec);spec.loader.exec_module(p)

def run(rate,row=0,col=0,kind='sell',mutations=None):
 c=p.h.HelperCpu(rate=rate,secondary=1)
 start,stop=(0xDD06,0xDD19) if kind=='sell' else (0x8676,0x8682)
 if kind=='hire':c.m[0x8000:0xc000]=p.h.PRG[3*16384:4*16384]
 c.m[6]=start&255;c.m[7]=start>>8;c.pc=0xEFC9
 c.m[2]=0;c.m[3]=0x78;c.m[4]=0;c.m[5]=0x72
 for a,v in ((0x720b,row),(0x720d,col)):
  c.m[a]=v&255;c.m[a+1]=v>>8
 c.m[p.TABLE_BASE:p.TABLE_BASE+len(p.RAM_TABLE)]=p.RAM_TABLE
 for a,v in (mutations or {}).items():c.m[a]=v
 for _ in range(80000):
  if c.pc==0xEFC9 and c.stream()==stop:
   return {'result':p.h.read32(c,8),'steps':c.count,'stack_restored':c.readind(2)==0x7800,'frame_restored':c.readind(4)==0x7200}
  c.step()
 raise RuntimeError('source function did not return')

def purchase_gold(gold,quantity,quote):
 c=p.h.HelperCpu(rate=101,secondary=1)
 c.m[0x8000:0xc000]=p.h.PRG[3*16384:4*16384]
 c.m[6]=0x49;c.m[7]=0x86;c.pc=0xEFC9
 c.m[2]=0;c.m[3]=0x78;c.m[4]=0;c.m[5]=0x72
 for a,v in ((0x720b,0x6dcd),(0x720d,quantity),(0x720f,quote),(0x6e0d,gold)):
  c.m[a]=v&255;c.m[a+1]=v>>8
 for _ in range(80000):
  if c.pc==0xEFC9 and c.stream()==0x865c:return c.m[0x6e0d]+256*c.m[0x6e0e]
  c.step()
 raise RuntimeError('purchase routine did not return')

class SourcePriceChecks(unittest.TestCase):
 def test_original_purchase_gold_rounding(self):
  for gold in (1,7,30,439,999):
   for quote in (1,5,18,22,64,120,181):
    for quantity in (0,1,2,3,10,11,17):
     if quantity*quote<=gold*10:
      with self.subTest(gold=gold,quote=quote,quantity=quantity):
       self.assertEqual(purchase_gold(gold,quantity,quote),(gold*10-quantity*quote)//10)
 def test_original_purchase_observed_examples(self):
  self.assertEqual(purchase_gold(439,1,64),432)
  self.assertEqual(purchase_gold(439,11,64),368)
 def test_sell_quote_table_grid(self):
  for rate in (0,1,69,101,123,190,255):
   for row in range(3):
    for col in range(17):
     with self.subTest(rate=rate,row=row,col=col):
      q=p.run_dcea(rate,row,col)['result'];r=run(rate,row,col)
      self.assertEqual(r['result'],max(1,q*90//100))
      self.assertTrue(r['stack_restored'] and r['frame_restored'])
 def test_observed_china_fur_quote(self):self.assertEqual(run(101,0,0)['result'],18)
 def test_observed_islam_item2_quote_candidate(self):self.assertEqual(run(101,1,2)['result'],50)
 def test_hire_rate_grid(self):
  for rate in (0,1,69,101,123,190,255):
   with self.subTest(rate=rate):self.assertEqual(run(rate,kind='hire')['result'],60*(rate+100)//100)
 def test_sell_spread_literal_negative_control(self):
  self.assertNotEqual(run(101,0,0)['result'],run(101,0,0,mutations={0xdd08:80})['result'])
 def test_hire_base_negative_control(self):
  self.assertNotEqual(run(101,kind='hire')['result'],run(101,kind='hire',mutations={0x867d:50})['result'])

if __name__=='__main__':
 print(json.dumps({'sell101_row0_col0':run(101,0,0),'hire101':run(101,kind='hire')},indent=2))
 unittest.main(verbosity=2)
