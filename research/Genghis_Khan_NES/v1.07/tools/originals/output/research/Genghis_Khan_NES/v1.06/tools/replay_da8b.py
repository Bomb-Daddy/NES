#!/usr/bin/env python3
"""Original 6502 VM executed in bounded synthetic memory for $DA8B 16x16/100 function.
No original Mesen scene, and no verified link from inputs to an observed merchant quote.
"""
import importlib.util, json, unittest, hashlib, sys
from pathlib import Path
ROOT=Path(__file__).resolve().parent
s=importlib.util.spec_from_file_location('helper',ROOT/'replay_b7_arithmetic.py');h=importlib.util.module_from_spec(s);s.loader.exec_module(h)
F=h.FIX; ENTRY=0xDA90;STOP=0xDA98
SOURCE=bytes.fromhex('8d 64 3d 3c e9 67 da 06 cf')
assert F[ENTRY-0xC000:ENTRY-0xC000+len(SOURCE)]==SOURCE

def run_da8b(a,b,mutation=None,max_steps=30000):
 assert 0<=a<=65535 and 0<=b<=65535
 c=h.HelperCpu(rate=101,secondary=1)
 c.m[6]=ENTRY&255;c.m[7]=ENTRY>>8
 c.m[2]=0;c.m[3]=0x78;c.m[4]=0;c.m[5]=0x72;c.pc=0xEFC9
 for p,v in ((0x720b,a),(0x720d,b)):
  c.m[p]=v&255;c.m[p+1]=v>>8
 if mutation:
  for addr,value in mutation.items():c.m[addr]=value
 visited=set();trace=[]
 for i in range(max_steps):
  if c.pc==0xEFC9 and c.stream() not in visited:
   ptr=c.stream();visited.add(ptr);trace.append({'vm_pc':f'${ptr:04X}','opcode':f'{c.m[ptr]:02X}',
                             'A':h.read32(c,8),'B':h.read32(c,12),'sp':f'${c.readind(2):04X}',
                             'frame':f'${c.readind(4):04X}','cpu_steps':c.count})
  if c.pc==0xEFC9 and c.stream()==STOP:
   return {'x':a,'y':b,'value':h.read32(c,8),'cpu_steps':c.count,'stack_restored':c.readind(2)==0x7800,
           'frame_restored':c.readind(4)==0x7200,'visited_vm':trace}
  c.step()
 raise RuntimeError('DA8B did not return from DA67')

class DA8BProof(unittest.TestCase):
 def test_rom_sha(self):self.assertEqual(hashlib.sha256(h.ROM.read_bytes()).hexdigest(),h.EXPECTED)
 def test_rom_native_entry(self):self.assertEqual(F[0xDA8B-0xC000:ENTRY-0xC000],bytes.fromhex('20 85 EF 00 00'))
 def test_function_source_bytes(self):self.assertEqual(F[ENTRY-0xC000:ENTRY-0xC000+len(SOURCE)],SOURCE)
 def test_function_call_da67(self):self.assertEqual(F[0xDA94-0xC000:0xDA98-0xC000],bytes.fromhex('E9 67 DA 06'))
 def test_223_17_percent(self):
  r=run_da8b(223,17);self.assertEqual(r['value'],37);self.assertTrue(r['stack_restored']);self.assertTrue(r['frame_restored'])
 def test_rate_plus_100_sample_values(self):
  for rate in (0,69,101,123,190,255):
   for base in (0,1,7,10,17,100,999):
    r=run_da8b(base,rate+100)
    with self.subTest(rate=rate,base=base):self.assertEqual(r['value'],(base*(rate+100))//100)
 def test_all_small_values(self):
  for x in (0,1,2,3,7,10,11,19,100,255,256,999):
   for y in (0,1,7,17,49,100,101,169,223,290,355):
    with self.subTest(x=x,y=y):self.assertEqual(run_da8b(x,y)['value'],(x*y)//100)
 def test_percent_literal_mutation_negative_control(self):
  # Script byte $DA91 is literal 100, altered in synthetic RAM only.
  normal=run_da8b(100,17)['value']
  altered=run_da8b(100,17,mutation={0xDA91:101})['value']
  self.assertEqual((normal,altered),(17,16))
 def test_multiplicand_mutation_negative_controls(self):
  self.assertNotEqual(run_da8b(100,223)['value'],run_da8b(101,223)['value'])
  self.assertNotEqual(run_da8b(100,223)['value'],run_da8b(100,224)['value'])
 def test_no_private_rom_artifacts(self):self.assertFalse(any(p.suffix.lower() in ['.nes','.sfc','.smc','.sram'] for p in ROOT.iterdir()))

if __name__=='__main__':
 samples=[run_da8b(*q) for q in [(100,223),(100,169),(100,100),(17,223),(10,169),(999,290),(0,100)]]
 (ROOT/'da8b_bounded_replay.json').write_text(json.dumps(samples,indent=2))
 result=unittest.TextTestRunner(verbosity=2).run(unittest.defaultTestLoader.loadTestsFromTestCase(DA8BProof))
 print('DA8B checks',result.testsRun,'pass',result.wasSuccessful())
 raise SystemExit(0 if result.wasSuccessful() else 1)
