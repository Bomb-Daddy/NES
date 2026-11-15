#!/usr/bin/env python3
"""Synthetic source events ONLY; real Mesen and real controller are not proven."""
import copy
import pathlib
import sys
import tempfile
import unittest
from gk292_verify_stock_trace_v2 import (
    verify, TraceError, ROM_SHA256, ROM_SHA1, STAGE_FRAMES, MULT, MASK)

class TraceV2Tests(unittest.TestCase):
    def fixture(self):
        rows=[f'GK1_292_STOCK_TRACE_V2,{ROM_SHA256},{ROM_SHA1},FRESH_RESET_WORKRAM']
        seed=bytes((0x12,0x23,0x34,0x45,0x56,0x67))
        for i,val in enumerate(seed):
            rows.append(f'W,{101+i},32816,0,{132+i},0,{val},{val},1,1')
        nxt=(-((int.from_bytes(seed,"big")|1)*MULT))&MASK
        rows.append(f'C,120,180,15,{seed.hex()},{nxt.to_bytes(6,"big").hex()},{(nxt>>32)&32767},1')
        for i,frame in enumerate(STAGE_FRAMES,1):
            rows.append(f'S,{i},{frame},1175,0,{3 if i==14 else 0},1,1')
        return rows
    def test_valid(self):
        result=verify(self.fixture())
        self.assertEqual((result['writes'],result['calls'],result['stages']),(6,1,14))
        self.assertTrue(result['stock_first_spring_state_candidate'])
        self.assertFalse(result['controller_command_menu_visual_confirmed'])
        self.assertFalse(result['native_stock_spring_implemented'])
        self.assertFalse(result['mesen_execution_independently_proven'])
    def test_distinct_non_ready_is_not_a_failure(self):
        rows=self.fixture();rows[-1]=rows[-1].replace('1175,0,3,1,1','1206,0,3,1,1')
        self.assertFalse(verify(rows)['stock_first_spring_state_candidate'])
        rows=self.fixture();rows[-1]=rows[-1].replace('1175,0,3,1,1','1175,0,2,1,1')
        self.assertFalse(verify(rows)['stock_first_spring_state_candidate'])
    def test_real_rom_hash(self):
        actual=pathlib.Path(sys.argv[1]) if len(sys.argv)>1 else None
        if actual:
            self.assertEqual(verify(self.fixture(),rom_path=actual)['calls'],1)
        with tempfile.TemporaryDirectory() as d:
            wrong=pathlib.Path(d)/'bad.nes';wrong.write_bytes(b'NES\x1a'+b'\0'*262156)
            with self.assertRaises(TraceError): verify(self.fixture(),rom_path=wrong)
    def test_hostile_negative_controls(self):
        changes=[
            lambda r:r.pop(0),
            lambda r:r.__setitem__(0,r[0].replace('FRESH_RESET_WORKRAM','ARCHIVED_SAVERAM')),
            lambda r:r.__setitem__(0,r[0].replace(ROM_SHA1,'0'*40)),
            lambda r:r.__setitem__(0,r[0].replace(ROM_SHA256,'0'*64)),
            lambda r:r.__setitem__(1,r[1].replace('W,101','W,-1')),
            lambda r:r.__setitem__(1,r[1].replace('W,101','W,1.5')),
            lambda r:r.__setitem__(1,r[1].replace('W,101','W,0101')),
            lambda r:r.__setitem__(1,r[1].replace('32816','4096')),
            lambda r:r.__setitem__(1,r[1].replace('32816','65536')),
            lambda r:r.__setitem__(1,r[1].replace(',0,132',',16,132')),
            lambda r:r.__setitem__(1,r[1].replace(',0,132',',-1,132')),
            lambda r:r.__setitem__(1,r[1].replace(',132,',',131,')),
            lambda r:r.__setitem__(1,r[1].replace(',132,',',138,')),
            lambda r:r.__setitem__(1,r[1].replace(',18,18,1',',18,2,1')),
            lambda r:r.__setitem__(1,r[1].replace(',18,18,1',',18,18,0')),
            lambda r:r.__setitem__(1,r[1].replace(',18,18,1',',18,18,2')),
            lambda r:r.__setitem__(1,r[1][:-1]+'3000'),
            lambda r:r.__setitem__(2,r[2].replace('W,102','W,101')),
            lambda r:r.__setitem__(7,r[7].replace('C,120','C,106')),
            lambda r:r.__setitem__(7,r[7].replace('C,120,180','C,120,119')),
            lambda r:r.__setitem__(7,r[7].replace(',15,',',14,')),
            lambda r:r.__setitem__(7,r[7].replace(',15,','15,',1)),
            lambda r:r.__setitem__(7,r[7].replace(',15,','15,',1)),
            lambda r:r.__setitem__(7,r[7].replace(',1',',0',1)),
            lambda r:r.__setitem__(7,r[7][:-1]+'3000'),
            lambda r:r.__setitem__(7,r[7][:-1]+'bad'),
            lambda r:r.pop(),
            lambda r:r.__setitem__(-1,r[-1].replace('S,14','S,15')),
            lambda r:r.__setitem__(-1,r[-1].replace(str(STAGE_FRAMES[-1]),str(STAGE_FRAMES[-1]-1))),
            lambda r:r.__setitem__(-1,r[-1].replace(',1,1',',2,1')),
            lambda r:r.__setitem__(-1,r[-1].replace(',1,1',',1,0')),
            lambda r:r.append('X,77'),
            lambda r:r.append(r[7]),
            lambda r:r.insert(8,'W,181,32816,0,132,12,20,20,1,1'),
            lambda r:r.insert(8,'W,150,32816,0,132,12,20,20,1,1'),
            lambda r:r.__setitem__(8,r[8].replace('S,1,365','S,1,364')),
            lambda r:r.__setitem__(9,r[9].replace('S,2,490','S,2,365')),
            lambda r:r.__setitem__(9,r[9].replace(',1,1',',1,2')),
        ]
        success=0
        for mutate in changes:
            rows=self.fixture();mutate(rows)
            with self.subTest(i=success,mutation=rows[0]):
                with self.assertRaises(TraceError):verify(rows)
            success+=1
        print(f'GK292 {success} hostile V2 negatives passed (synthetic only)')

if __name__=='__main__':
    unittest.main(argv=[sys.argv[0]],verbosity=2)
