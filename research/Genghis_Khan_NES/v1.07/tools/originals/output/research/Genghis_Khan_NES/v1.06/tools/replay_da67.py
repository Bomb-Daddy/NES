#!/usr/bin/env python3
"""Genghis Khan I NES: original 6502 VM instruction replay of $DA67 arithmetic.

Evidence: fixed PRG bank 15 source bytes, synthetic operand frame and stack.
Not a Mesen game execution and not proof the Trade menu invokes this function.
"""
from pathlib import Path
import sys, json, unittest, hashlib, importlib.util, csv
OUT=Path(__file__).resolve().parent
q=importlib.util.spec_from_file_location('helper',OUT/'replay_b7_arithmetic.py')
h=importlib.util.module_from_spec(q);q.loader.exec_module(h)
ROM=h.ROM;F=h.FIX
# DA67 = native JSR EF85 entry (5-byte prologue), so VM starts at $DA6C.
ENTRY=0xDA6C; NONZERO_STOP=0xDA8A; ZERO_STOP=0xDA72
SOURCE=bytes.fromhex('0e d7 73 da 89 ff cf 0e b7 26 b7 14 0d b7 26 b7 14 0c b7 26 b7 15 b7 01 b7 15 b7 02 b7 27 cf')
assert F[ENTRY-0xC000:ENTRY-0xC000+len(SOURCE)]==SOURCE

def run_da67(x,y,z,mutation=None,max_steps=50000):
    assert all(isinstance(v,int) and 0<=v<=65535 for v in (x,y,z))
    c=h.HelperCpu(rate=101,secondary=1)
    c.m[0x06]=ENTRY&255;c.m[0x07]=ENTRY>>8;c.pc=0xEFC9
    c.m[0x02]=0;c.m[0x03]=0x78  # synthetic VM stack
    c.m[0x04]=0;c.m[0x05]=0x72  # synthetic function-parameter frame
    for pos,val in ((0x720B,x),(0x720D,y),(0x720F,z)):
        c.m[pos]=val&255;c.m[pos+1]=val>>8
    if mutation:
        for addr,byte in mutation.items():c.m[addr]=byte
    checkpoints=[];visited=set()
    stop=ZERO_STOP if z==0 else NONZERO_STOP
    for i in range(max_steps):
        if c.pc==0xEFC9 and c.stream() not in visited:
            p=c.stream();visited.add(p)
            checkpoints.append({'vm_pc':f'${p:04X}','vm_byte':f'{c.m[p]:02X}','reg_a32':h.read32(c,8),'reg_b32':h.read32(c,12),
                         'stack_ptr':f'${c.readind(2):04X}','step':c.count})
        if c.pc==0xEFC9 and c.stream()==stop:
            return {'x':x,'y':y,'divisor':z,'result_word':c.m[8]|(c.m[9]<<8),'result_32':h.read32(c,8),
                    'remainder_32':h.read32(c,0x10),'cpu_steps':c.count,'vm_pc':f'${c.stream():04X}',
                    'vm_stack_restored':c.readind(2)==0x7800,'trace':checkpoints,
                    'opcodes_cpu_count':len(c.opcounts)}
        c.step()
    raise RuntimeError(f'function did not reach stop ${stop:04X}; got ${c.stream():04X}, CPU ${c.pc:04X}')

class DA67Evidence(unittest.TestCase):
    def test_rom_identity(self):self.assertEqual(hashlib.sha256(ROM.read_bytes()).hexdigest(),h.EXPECTED)
    def test_function_native_prologue(self):self.assertEqual(F[0xDA67-0xC000:0xDA6C-0xC000],bytes.fromhex('20 85 ef 00 00'))
    def test_vm_source_sequence(self):self.assertEqual(F[ENTRY-0xC000:ENTRY-0xC000+len(SOURCE)],SOURCE)
    def test_native_helper_id_sequence(self):
        pairs=[(SOURCE[i],SOURCE[i+1]) for i in range(len(SOURCE)-1) if SOURCE[i]==0xB7]
        self.assertEqual([q[1] for q in pairs],[0x26,0x14,0x26,0x14,0x26,0x15,0x01,0x15,0x02,0x27])
    def test_expected_100x7_over5(self):
        r=run_da67(100,7,5)
        self.assertEqual((r['result_word'],r['remainder_32'],r['vm_stack_restored']),(140,0,True))
    def test_all_small_values_bounded(self):
        # Carefully bounded Cartesian product, each case executes 6502 source bytes.
        for x in (0,1,2,7,10,100,255,256,999):
            for y in (0,1,3,7,11,100,255):
                for z in (1,2,3,7,11,100):
                    with self.subTest(x=x,y=y,z=z):
                        r=run_da67(x,y,z)
                        self.assertEqual(r['result_32'],(x*y)//z)
                        self.assertEqual(r['remainder_32'],(x*y)%z)
                        self.assertTrue(r['vm_stack_restored'])
    def test_signed_32_bound_not_falsely_generalized(self):
        # Only assert the input and limit; no 32-bit unsigned full-range claim.
        self.assertLessEqual(65535*32768,0x7fffffff)
        self.assertGreater(65535*32769,0x7fffffff)
    def test_divisor_zero_branch(self):
        r=run_da67(100,7,0)
        self.assertEqual((r['vm_pc'],r['result_word']),('$DA72',0xffff))
    def test_noop_denominator_zero_stack_unchanged(self):
        r=run_da67(0,0,0)
        self.assertTrue(r['vm_stack_restored'])
    def test_multiplier_mutation_negative_control(self):
        baseline=run_da67(100,7,5)['result_32']
        # Mutation at $DA83 changes helper $01 (multiply) to $27 (no-op).
        altered=run_da67(100,7,5,mutation={0xDA83:0x27})['result_32']
        self.assertEqual(baseline,140)
        self.assertNotEqual(altered,baseline)
    def test_division_mutation_negative_control(self):
        baseline=run_da67(100,7,5)['result_32']
        # $DA87 changes $02 division helper to $27 no-op.
        altered=run_da67(100,7,5,mutation={0xDA87:0x27})['result_32']
        self.assertNotEqual(altered,baseline)
    def test_input_parameter_mutation_controls(self):
        a=run_da67(100,7,5)['result_32']
        self.assertNotEqual(a,run_da67(101,7,5)['result_32'])
        self.assertNotEqual(a,run_da67(100,8,5)['result_32'])
        self.assertNotEqual(a,run_da67(100,7,6)['result_32'])
    def test_no_private_rom_in_report(self):
        self.assertFalse(any(z.suffix.lower() in ('.nes','.sfc','.smc','.sram') for z in OUT.iterdir()))

if __name__=='__main__':
    samples=[run_da67(*x) for x in [(100,7,5),(101,17,100),(223,17,100),(1000,19,100),(999,999,999),(100,7,0)]]
    (OUT/'da67_bounded_replay.json').write_text(json.dumps(samples,indent=2))
    r=unittest.TextTestRunner(verbosity=2).run(unittest.defaultTestLoader.loadTestsFromTestCase(DA67Evidence))
    print('DA67 source replays :',r.testsRun,'test methods / 378 bounded x*y/z cases; success',r.wasSuccessful())
    raise SystemExit(0 if r.wasSuccessful() else 1)
