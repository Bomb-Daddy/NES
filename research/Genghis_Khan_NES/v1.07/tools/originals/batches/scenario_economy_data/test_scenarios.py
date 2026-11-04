#!/usr/bin/env python3
"""Source-byte replay and independent provenance checks; no PPU or emulator claim."""
import sys
sys.dont_write_bytecode=True
import unittest,hashlib,json
from pathlib import Path
import extract_scenarios as e
R=e.ROOT
EXPECTED_SPECS={1:[0,0,0,0,5,0,0,3,0,2,0,5,0,0],2:[0,1,0,0,6,3,8,3,7,8,3,2,7,2,9,0,5,4,1,2,7,8,4,9,8,6,5]}
def execute(c,start,stop,args=(),bank=None):
 if bank is not None:c.m[0x8000:0xc000]=e.prg[bank*16384:(bank+1)*16384]
 c.pc=0xefc9;c.sp=255;c.setword(2,0x1800);c.setword(4,0x700);c.setword(6,start)
 for j,v in enumerate(args):c.setword(0x70b+2*j,v)
 return c.run(stop,maxsteps=c.count+3000000,trace=False)
def stockvector(c,sc):return [[c.word(0x6dcd+83*i+0x22+2*j) for j in range(10)] for i in range(14 if sc==1 else 27)]
class Tests(unittest.TestCase):
 @classmethod
 def setUpClass(cls):cls.c={sc:e.loader(sc) for sc in (1,2)};cls.rows={sc:e.snapshot(sc,cls.c[sc]) for sc in (1,2)}
 def test_rom_hash(self):self.assertEqual(hashlib.sha256(e.raw).hexdigest(),e.ROM_SHA)
 def test_scenario_metadata_and_banks(self):
  for sc,count,year in ((1,14,1174),(2,27,1205)):
   c=self.c[sc];self.assertEqual((c.word(0x65b9),c.word(0x65bb),c.word(0x65bd)),(count,year,3));self.assertEqual([b['bank'] for b in c.bank_events],[7,0,7,0])
 def test_all_copied_bytes_and_relocated_lord_pointers(self):
  for sc,c in self.c.items():
   src=0x8004 if sc==1 else 0x96b4;off=16+7*16384+src-0x8000
   expected=bytearray(e.raw[off:off+0x15ea])
   for i in range(c.word(0x65b9)):
    p=0x6e1e+83*i-0x63ea;expected[p:p+2]=(0x65bf+31*i).to_bytes(2,'little')
   self.assertEqual(c.m[0x63ea:0x79d4],expected)
   common=16+7*16384+0xad74-0x8000;self.assertEqual(c.m[0x79d4:0x7ee6],e.raw[common:common+0x512])
 def test_copy_bounds_canaries(self):
  c=e.LoaderCpu(0);c.m[0x63e9]=0xa5;c.m[0x7ee6]=0x5a
  execute(c,0x97dd,0x9851,[2]);self.assertEqual((c.m[0x63e9],c.m[0x7ee6]),(0xa5,0x5a))
 def test_all_specialties(self):
  for sc,rows in self.rows.items():self.assertEqual([r['specialty_index'] for r in rows],EXPECTED_SPECS[sc])
 def test_every_csv_value_against_rom_offset(self):
  allrows=json.loads((R/'scenario_economy_data.json').read_text());self.assertEqual(len(allrows),41)
  for r in allrows:
   o=int(r['country_rom_file_offset'],16)
   for j,name in enumerate(e.NAMES):self.assertEqual(r[name],int.from_bytes(e.raw[o+0x22+2*j:o+0x24+2*j],'little'))
   self.assertEqual(r['specialty_index'],e.raw[int(r['specialty_rom_file_offset'],16)])
   n=int(r['name_rom_file_offset'],16);self.assertEqual(r['country_name'],e.string(e.raw,n,17))
 def test_all_country_address_index_and_specialty_helpers(self):
  results=[]
  for sc,c in self.c.items():
   for r in self.rows[sc]:
    i=r['country_index_zero_based'];p=int(r['country_sram_base'][1:],16)
    self.assertEqual(execute(c,0xdbac,0xdbb4,[i]),p)
    self.assertEqual(execute(c,0xdbd2,0xdbda,[p]),i)
    self.assertEqual(execute(c,0xdef8,0xdf04,[p]),r['specialty_index'])
    results.append({'scenario':sc,'country_id':i+1,'pointer':hex(p),'specialty':r['specialty_index']})
  (R/'regional_helper_replay.json').write_text(json.dumps(results,indent=2)+'\n')
 def test_original_drafted_sum_and_draft_limit_all_countries(self):
  for sc,c in self.c.items():
   for r in self.rows[sc]:
    p=int(r['country_sram_base'][1:],16)
    self.assertEqual(execute(c,0xded2,0xdef2,[p]),r['Drafted_total'])
    self.assertEqual(execute(c,0x90a2,0x90c2,[p],bank=3),min(r['Eligible_draft_count'],999-r['Drafted_total']))
 def test_drafted_sum_excludes_undrafted_negative_control(self):
  c=e.loader(1);p=0x6dcd;original=execute(c,0xded2,0xdef2,[p]);c.setword(p+0x49,999)
  self.assertEqual(execute(c,0xded2,0xdef2,[p]),original)
  c.setword(p+0x18,c.word(p+0x18)+1);self.assertEqual(execute(c,0xded2,0xdef2,[p]),original+1)
 def test_eligibility_is_byte_and_residents_are_adjacent_word(self):
  c=e.loader(1);p=0x6dcd;c.m[p+0x48]=30
  for population in (1,255,256,511,999):
   c.setword(p+0x49,population);self.assertEqual(execute(c,0x90a2,0x90c2,[p],bank=3),min(population*30//100,899))
 def test_draft_capacity_boundary(self):
  c=e.loader(1);p=0x6dcd;c.m[p+0x48]=50;c.setword(p+0x49,999)
  for total in (0,998,999):
   for off in (0x18,0x1c,0x20,0x3a,0x3e):c.setword(p+off,0)
   c.setword(p+0x18,total);self.assertEqual(execute(c,0x90a2,0x90c2,[p],bank=3),min(499,999-total))
 def test_template_stock_mutation_follows_loader(self):
  original=e.prg
  try:
   changed=bytearray(original);o=7*16384+0x8004-0x8000+(0x6dcd-0x63ea)+0x22+2*2;changed[o:o+2]=(777).to_bytes(2,'little');e.prg=bytes(changed)
   c=e.loader(1);actual=stockvector(c,1)
   originalrows=self.rows[1];expected=[[r[n] for n in e.NAMES[:10]] for r in originalrows];expected[0][2]=777
   self.assertEqual(actual,expected)
  finally:e.prg=original
 def test_specialty_mutation_follows_native_lookup(self):
  original=e.prg
  try:
   changed=bytearray(original);o=7*16384+0x96b4-0x8000+(0x7692-0x63ea)+12*21;changed[o]=9;e.prg=bytes(changed)
   c=e.loader(2);self.assertEqual(execute(c,0xdef8,0xdf04,[0x6dcd+83*21]),9)
   self.assertEqual(execute(c,0xdef8,0xdf04,[0x6dcd+83*22]),4)
  finally:e.prg=original
if __name__=='__main__':unittest.main(verbosity=2)
