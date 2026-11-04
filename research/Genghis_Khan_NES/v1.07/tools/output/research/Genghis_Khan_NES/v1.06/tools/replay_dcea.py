#!/usr/bin/env python3
"""Static original-6502 replay of the $DCEA Rate-linked table function.

Reads REAL ARCHIVED Mesen SRAM bytes for the underlying table/rate but uses a
synthetic caller frame. This is NOT an original Trade command execution.
"""
from pathlib import Path
import importlib.util,json, unittest,hashlib,zipfile, csv, os
ROOT=Path(__file__).resolve().parent
sp=importlib.util.spec_from_file_location('h',ROOT/'replay_b7_arithmetic.py');h=importlib.util.module_from_spec(sp);sp.loader.exec_module(h)
START=0xDCEA; STOP=0xDD00; TABLE_BASE=0x7DE5
F=h.FIX; FUNCTION=bytes.fromhex('a4 8a 7a 8f 64 b3 0c 8b 11 b5 1d bb 8c e5 7d bb d3 b3 e9 8b da 04 cf')
assert F[START-0xc000:START-0xc000+len(FUNCTION)]==FUNCTION
MEMBER='External baseline supplied by GK_SRAM'
RAM=Path(os.environ['GK_SRAM']).read_bytes()
assert len(RAM)==8192
EXPECTED='dd22847140cc0397ab8f42e29f1832681628ed52bfa17e95041c288afc4ef773'
RAM_TABLE=RAM[TABLE_BASE-0x6000:]
ARCHIVED_RATE=RAM[0x7A8A-0x6000]|(RAM[0x7A8B-0x6000]<<8)

def run_dcea(rate,row,col,table=RAM_TABLE,mutations=None,max_steps=40000):
 assert 0<=rate<=65535 and 0<=row<=30 and 0<=col<=16
 c=h.HelperCpu(rate=rate,secondary=1)
 c.m[6]=START&255;c.m[7]=START>>8;c.pc=0xEFC9
 c.m[2]=0;c.m[3]=0x78;c.m[4]=0;c.m[5]=0x72
 for addr,v in ((0x720B,row),(0x720D,col)):
  c.m[addr]=v&255;c.m[addr+1]=v>>8
 c.m[TABLE_BASE:TABLE_BASE+len(table)]=table
 if mutations:
  for addr,v in mutations.items():c.m[addr]=v
 trace=[];last=-1
 for i in range(max_steps):
  if c.pc==0xEFC9 and c.stream()!=last:
   last=c.stream()
   if last in (0xDCED,0xDCF0,0xDCFA,0xDCFB,0xDCFC,0xDA90,0xDA94,0xDA6C,0xDA8A,0xDA98,STOP):
    trace.append({'vm_pc':f'${last:04X}','A':h.read32(c,8),'B':h.read32(c,12),'stack':f'${c.readind(2):04X}','frame':f'${c.readind(4):04X}'})
  if c.pc==0xEFC9 and c.stream()==STOP:
   addr=TABLE_BASE+17*row+col
   return {'rate':rate,'row':row,'column':col,'table_address':f'${addr:04X}',
      'source_table_byte':c.m[addr],'result':h.read32(c,8),'cpu_steps':c.count,
      'frame_restored':c.readind(4)==0x7200,'stack_restored':c.readind(2)==0x7800,
      'vm_trace':trace}
  c.step()
 raise RuntimeError('DCEA did not complete')

class RateFunctionProofs(unittest.TestCase):
 def test_rom_sha(self):self.assertEqual(hashlib.sha256(h.ROM.read_bytes()).hexdigest(),h.EXPECTED)
 def test_sram_sha(self):self.assertEqual(hashlib.sha256(RAM).hexdigest(),EXPECTED)
 def test_recorded_rate(self):self.assertEqual(ARCHIVED_RATE,101)
 def test_original_function_prologue(self):self.assertEqual(F[0xDCE5-0xc000:START-0xc000],bytes.fromhex('20 85 ef 00 00'))
 def test_original_function_bytes(self):self.assertEqual(F[START-0xc000:START-0xc000+len(FUNCTION)],FUNCTION)
 def test_original_percent_callee(self):self.assertEqual(F[0xDCFC-0xc000:0xDD00-0xc000],bytes.fromhex('E9 8B DA 04'))
 def test_real_table_shape_and_base(self):
  self.assertEqual(RAM[TABLE_BASE-0x6000],10)
  self.assertEqual(RAM[TABLE_BASE-0x6000+1],16)
  self.assertEqual(len(RAM_TABLE),0x8000-TABLE_BASE)
 def test_sample_row_column_original_sram(self):
  r=run_dcea(101,0,1)
  self.assertEqual((r['source_table_byte'],r['result'],r['table_address']),(16,32,'$7DE6'))
 def test_3_rate_comparisons_original_table(self):
  for rate in (69,101,123,190):
   for row in (0,1,2,4,7,10,20):
    for col in (0,1,4,11,16):
     r=run_dcea(rate,row,col)
     with self.subTest(rate=rate,row=row,col=col):
      self.assertEqual(r['result'],(r['source_table_byte']*(rate+100))//100)
      self.assertTrue(r['stack_restored'] and r['frame_restored'])
 def test_16_bit_multiplier_not_8_bit_overflow(self):
  r=run_dcea(190,0,2) # byte 0x20 => 32; product 9280
  self.assertEqual(r['result'],(32*290)//100)
 def test_byte_dereference_not_word(self):
  addr=TABLE_BASE+1
  original=run_dcea(123,0,1)['result']
  adjacent=run_dcea(123,0,1,mutations={addr+1:0xff})['result']
  direct=run_dcea(123,0,1,mutations={addr:17})['result']
  self.assertEqual(original,adjacent)
  self.assertNotEqual(original,direct)
 def test_negative_rate_operand_mutation(self):
  default=run_dcea(123,0,1)['result']
  # $DCEE holds signed immediate 100. Mutating synthetic memory to 0 changes multiplier.
  altered=run_dcea(123,0,1,mutations={0xDCEE:0})['result']
  self.assertEqual((default,altered),(35,19))
 def test_index_independence_negative_controls(self):
  self.assertNotEqual(run_dcea(123,0,0)['result'],run_dcea(123,0,1)['result'])
  self.assertNotEqual(run_dcea(123,0,1)['result'],run_dcea(123,1,1)['result'])
 def test_not_rom_distribution(self):self.assertFalse(any(p.suffix.lower()=='.nes' for p in ROOT.iterdir()))

if __name__=='__main__':
 samples=[run_dcea(rate,row,col) for rate,row,col in [(69,0,1),(101,0,1),(123,0,1),(190,0,1),(123,1,0),(123,1,1)]]
 (ROOT/'dcea_archived_sram_replay.json').write_text(json.dumps({'source_sram':MEMBER,'source_sha256':EXPECTED,'original_market_rate':ARCHIVED_RATE,'samples':samples},indent=2))
 res=unittest.TextTestRunner(verbosity=2).run(unittest.defaultTestLoader.loadTestsFromTestCase(RateFunctionProofs))
 print('DCEA replay',res.testsRun,'methods, 140 representative rate/row/column combinations, pass',res.wasSuccessful())
 raise SystemExit(0 if res.wasSuccessful() else 1)
