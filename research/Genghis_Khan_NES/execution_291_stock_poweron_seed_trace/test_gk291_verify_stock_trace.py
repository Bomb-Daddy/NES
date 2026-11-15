#!/usr/bin/env python3
"""Synthetic events: verify the audit ONLY, NEVER evidence of real stock NES."""
import copy
import pathlib
import tempfile
import unittest
from gk291_verify_stock_trace import verify, TraceError, ROM_SHA, MULT, MASK

class Tests(unittest.TestCase):
    def trace(self):
        rows=[f'GK1_291_STOCK_TRACE_V1,{ROM_SHA},FRESH_RESET_WORKRAM']
        seed=[0x12,0x23,0x34,0x45,0x56,0x67]
        for i,v in enumerate(seed):
            rows.append(f'W,{101+i},32816,0,{0x84+i},0,{v},{v},1,1')
        n=int.from_bytes(bytes(seed),'big')
        nxt=(-((n|1)*MULT))&MASK
        rows.append(f'C,120,180,15,{bytes(seed).hex()},{nxt.to_bytes(6,"big").hex()},{(nxt>>32)&32767},1')
        for j in range(1,15):rows.append(f'S,{j},{j*125},1175,0,{3 if j==14 else 0},1,1')
        return rows
    def check_red(self,change):
        rows=self.trace();change(rows)
        with self.assertRaises(TraceError): verify(rows)
    def test_positive(self):
        result=verify(self.trace())
        self.assertEqual((result['writes'],result['calls'],result['stages']),(6,1,14))
        self.assertTrue(result['observed_first_spring_menu'])
        self.assertFalse(result['mesen_origin_independently_authenticated'])
        self.assertFalse(result['stock_spring_generator_implemented'])
    def test_undercounted_spring_not_universal(self):
        rows=self.trace();rows[-1]=rows[-1].replace('1175,0,3,1,1','1175,0,0,1,1')
        self.assertFalse(verify(rows)['observed_first_spring_menu'])
    def test_rom_hash_reject(self):
        with tempfile.TemporaryDirectory() as d:
            p=pathlib.Path(d)/'bad.nes';p.write_bytes(b'NES\x1a'+b'\0'*262156)
            with self.assertRaisesRegex(TraceError,'ROM hash'):
                verify(self.trace(),rom_path=p)
    def test_hostile(self):
        bad=[
         lambda r:r.__setitem__(0,r[0].replace('FRESH_RESET_WORKRAM','ARCHIVED_SAVERAM')),
         lambda r:r.pop(0),
         lambda r:r.pop(1),
         lambda r:r.__setitem__(1,r[1].replace(',1,1',',0,1')),
         lambda r:r.__setitem__(1,r[1].replace('32816,0','32816,-1')),
         lambda r:r.__setitem__(1,r[1].replace('32816,0','1000,0')),
         lambda r:r.__setitem__(1,r[1].replace('132,0','777,0')),
         lambda r:r.__setitem__(1,r[1].replace(',18,18,1',',18,99,1')),
         lambda r:r.__setitem__(1,r[1].replace('W,101','W,0')),
         lambda r:r.__setitem__(2,r[2].replace('W,102','W,101')),
         lambda r:r.__setitem__(7,r[7].replace('C,120','C,106')),
         lambda r:r.__setitem__(7,r[7].replace('C,120,180','C,120,119')),
         lambda r:r.__setitem__(7,r[7].replace(',15,',',14,')),
         lambda r:r.__setitem__(7,r[7].replace(',15,','15,',1)),
         lambda r:r.__setitem__(7,r[7].replace(',1',',2',1)),
         lambda r:r.__setitem__(7,r[7].replace(',120,180,15,',',120,180,15,ffff')),
         lambda r:r.__setitem__(7,r[7][:-1]+'x'),
         lambda r:r.pop(),
         lambda r:r.__setitem__(-1,r[-1].replace('S,14','S,15')),
         lambda r:r.__setitem__(-1,r[-1].replace(',1,1',',3,1')),
         lambda r:r.__setitem__(-1,r[-1].replace(',1,1',',1,2')),
         lambda r:r.append('X,77'),
         lambda r:r.append(r[7]),
        ]
        passed=0
        for change in bad:
            try:self.check_red(change)
            except AssertionError as e:raise AssertionError(f'negative {passed} failed: {e}')
            passed+=1
        print(f'GK291 {passed} hostile synthetic negatives PASS (not Mesen runtime)')

if __name__=='__main__': unittest.main(verbosity=2)