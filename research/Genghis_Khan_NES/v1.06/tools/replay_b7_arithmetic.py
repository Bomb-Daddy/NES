#!/usr/bin/env python3
"""Bounded replay of original Genghis Khan (USA) NES fixed-bank 6502 B7 native helpers.

Runs authentic instruction bytes in a transparent synthetic CPU/memory harness.
NOT Mesen, NOT a complete original game run, NOT a merchant-price verification.
"""
from pathlib import Path
import os, sys, importlib.util, hashlib, unittest, json
ROOT=Path(__file__).resolve().parent
BASE=ROOT/'vm_selected_cpu_replay.py'
spec=importlib.util.spec_from_file_location('gk_cpu_v104',BASE)
mod=importlib.util.module_from_spec(spec);spec.loader.exec_module(mod)
ROM=mod.ROM; FIX=mod.FIX; PRG=mod.PRG
EXPECTED=mod.EXPECTED

class HelperCpu(mod.Cpu):
    def step(self):
        op=self.m[self.pc]
        if op not in [0xB5,0x75,0x95,0x94,0xB4,0xD5,0xF5,0x45,0x49,0xE0,0xC5,0xC9,0xF9,0xA1,0x81,0x15,0xD1,0x0A]:
            return super().step()
        pc=self.pc;op=self.get();self.count+=1;self.opcounts[op]=self.opcounts.get(op,0)+1
        zp=lambda:self.get()
        if op==0xB5:self.a=self.nz(self.m[(zp()+self.x)&255])
        elif op==0x75:self.adc(self.m[(zp()+self.x)&255])
        elif op==0x95:self.m[(zp()+self.x)&255]=self.a
        elif op==0x94:self.m[(zp()+self.x)&255]=self.y
        elif op==0xB4:self.y=self.nz(self.m[(zp()+self.x)&255])
        elif op==0xD5:
            b=self.m[(zp()+self.x)&255];t=(self.a-b)&255;self.c=int(self.a>=b);self.z=int(t==0);self.n=int(bool(t&128))
        elif op==0xF5:self.adc(self.m[(zp()+self.x)&255]^0xff)
        elif op==0x45:self.a=self.nz(self.a^self.m[zp()])
        elif op==0x49:self.a=self.nz(self.a^self.get())
        elif op==0xE0:
            b=self.get();t=(self.x-b)&255;self.c=int(self.x>=b);self.z=int(t==0);self.n=int(bool(t&128))
        elif op==0xC5:
            b=self.m[zp()];t=(self.a-b)&255;self.c=int(self.a>=b);self.z=int(t==0);self.n=int(bool(t&128))
        elif op==0xC9:
            b=self.get();t=(self.a-b)&255;self.c=int(self.a>=b);self.z=int(t==0);self.n=int(bool(t&128))
        elif op==0xF9:self.adc(self.m[(self.word()+self.y)&65535]^0xff)
        elif op==0xA1:self.a=self.nz(self.m[(self.readind((zp()+self.x)&255))&65535])
        elif op==0x81:self.m[self.readind((zp()+self.x)&255)]=self.a
        elif op==0x15:self.a=self.nz(self.a|self.m[(zp()+self.x)&255])
        elif op==0xD1:
            b=self.m[(self.readind(zp())+self.y)&65535];t=(self.a-b)&255;self.c=int(self.a>=b);self.z=int(t==0);self.n=int(bool(t&128))
        elif op==0x0A:
            t=self.a;self.c=(t>>7)&1;self.a=self.nz(t<<1)
        if self.count>400000:raise RuntimeError('instruction budget exceeded')

def set32(c,p,n):
    for i in range(4):c.m[p+i]=(n>>(i*8))&255

def read32(c,p):return sum(c.m[p+i]<<(i*8) for i in range(4))

def run_b7(helper_index,a,b,max_steps=150000, stack_data=None):
    c=HelperCpu(rate=101,secondary=1)
    c.m[0x6100:0x6103]=bytes([0xB7,helper_index,0x40])
    c.m[0x06]=0x00;c.m[0x07]=0x61
    c.m[0x02]=0;c.m[0x03]=0x78
    set32(c,0x08,a);set32(c,0x0c,b)
    if stack_data is not None:
        for i,v in enumerate(stack_data):c.m[0x7800+i]=v
    c.pc=0xEFC9
    visited=[]
    for i in range(max_steps):
        if i and c.pc==0xEFC9 and c.stream()==0x6102:
            return {'index':helper_index,'input_a':a,'input_b':b,'result_low_32':read32(c,0x08),'other_words':read32(c,0x10),'register_a':c.m[8]|(c.m[9]<<8),'register_b':c.m[12]|(c.m[13]<<8),'register_b_32':read32(c,0x0c),'vm_stack_top_32':read32(c,c.readind(0x02)),'steps':c.count,'cpu_pc':c.pc,'stream_pc':c.stream(),'vm_stack_pointer':c.readind(0x02),'instruction_histogram':{f'{x:02X}':v for x,v in sorted(c.opcounts.items())}}
        c.step()
    raise RuntimeError('B7 helper did not return in instruction budget')

def run_sequence(code,a,b, max_steps=200000):
    c=HelperCpu(rate=101,secondary=1)
    c.m[0x6100:0x6100+len(code)]=bytes(code)
    c.m[0x06]=0;c.m[0x07]=0x61
    c.m[0x02]=0;c.m[0x03]=0x78
    set32(c,0x08,a);set32(c,0x0c,b)
    c.pc=0xEFC9
    for i in range(max_steps):
        if i and c.pc==0xEFC9 and c.stream()==0x6100+len(code):
            return {'a':read32(c,0x08),'b':read32(c,0x0c),'stack_pointer':c.readind(2),
                 'top_32':read32(c,c.readind(2)),'steps':c.count}
        c.step()
    raise RuntimeError('VM sequence did not return')

class NativeSourceChecks(unittest.TestCase):
 def test_rom_hash(self):self.assertEqual(hashlib.sha256(ROM.read_bytes()).hexdigest(),EXPECTED)
 def test_dispatch_original(self):self.assertEqual(FIX[0xF9A8-0xC000:0xF9A8-0xC000+2],bytes.fromhex('b1 06'))
 def test_mul_entry_original(self):self.assertEqual(FIX[0xFA23-0xC000:0xFA23-0xC000+5],bytes.fromhex('84 10 84 11 84'))
 def test_b7_mul_small(self):
  for a,b in [(0,0),(1,1),(2,3),(7,9),(0xffff,2),(0x7fffffff,3),(0xffffffff,1)]:
   with self.subTest(a=a,b=b):self.assertEqual(run_b7(1,a,b)['result_low_32'],(a*b)&0xffffffff)
 def test_mul_zero_negative_control(self):
  self.assertEqual(run_b7(1,0,1023)['result_low_32'],0)
  self.assertNotEqual(run_b7(1,3,7)['result_low_32'],run_b7(1,3,8)['result_low_32'])
 def test_mul_32_bit_wrap(self):self.assertEqual(run_b7(1,65536,65536)['result_low_32'],0)
 def test_div_nonnegative_examples(self):
  for a,b in ((0,1),(7,2),(100,5),(1234,100),(65536,100),(0x7fffffff,1),(0x7fffffff,3)):
   with self.subTest(a=a,b=b):
    o=run_b7(2,a,b);self.assertEqual((o['result_low_32'],o['other_words']),(a//b,a%b))
 def test_div_nonnegative_exhaustive_bounded(self):
  # All values in a small table plus boundary dividend values.
  for a in range(0,70):
   for b in range(1,35):
    o=run_b7(2,a,b)
    self.assertEqual((o['result_low_32'],o['other_words']),(a//b,a%b))
 def test_div_nonnegative_boundary(self):
  for a,b in ((0x7fffffff,65535),(0x7ffffffe,32768),(0x7fffffff,0x7fffffff),(0x80000000,1)):
   o=run_b7(2,a,b)
   self.assertEqual((o['result_low_32'],o['other_words']),(a//b,a%b))
 def test_div_negative_dividend_positive_divisor(self):
  # -7/2 truncates toward zero. Remainder magnitude is 1.
  o=run_b7(2,0xfffffff9,2)
  self.assertEqual((o['result_low_32'],o['other_words']),(0xfffffffd,1))
 def test_div_zero_path_is_not_safe_valid_division(self):
  o=run_b7(2,123,0)
  self.assertEqual((o['result_low_32'],o['other_words']),(0xffffffff,123))
  # Do not promote this sentinel-like outcome to a supported gameplay rule.
 def test_b7_14_pushes_register_a_as_32_bit_word(self):
  o=run_b7(0x14,0x12345678,0xdeadbeef)
  self.assertEqual((o['vm_stack_pointer'],o['vm_stack_top_32'],o['result_low_32']),
                   (0x77fc,0x12345678,0x12345678))
 def test_b7_15_pops_word_to_register_b(self):
  o=run_b7(0x15,0xaaaa5555,0x11112222,stack_data=bytes.fromhex('78 56 34 12'))
  self.assertEqual((o['vm_stack_pointer'],o['register_b_32']),(0x7804,0x12345678))
 def test_b7_26_zero_extends_a(self):
  o=run_b7(0x26,0x12345678,0xdeadbeef)
  self.assertEqual((o['result_low_32'],o['register_b_32']),(0x00005678,0xdeadbeef))
 def test_b7_27_does_not_change_register_a(self):
  o=run_b7(0x27,0x12345678,0xdeadbeef)
  self.assertEqual((o['result_low_32'],o['register_b_32']),(0x12345678,0xdeadbeef))
 def test_b7_push_pop_round_trip(self):
  for a in (0,1,255,256,65535,65536,0x12345678,0xffffffff):
   d=run_sequence([0xB7,0x14,0xB7,0x15],a,0x11111111)
   self.assertEqual((d['a'],d['b'],d['stack_pointer']),(a,a,0x7800))
 def test_b7_mutation_wrong_helper_not_multiplication(self):
  a=run_b7(1,31,7);b=run_b7(0x27,31,7)
  self.assertEqual(a['result_low_32'],217)
  self.assertEqual(b['result_low_32'],31)
 def test_mul_low_word_boundaries(self):
  for a in (0,1,2,255,256,257,65535,65536,65537,0xffff0001,0xffffffff):
   for b in (0,1,2,255,256,65535,65536,0xffffffff):
    with self.subTest(a=a,b=b):self.assertEqual(run_b7(1,a,b)['result_low_32'],(a*b)&0xffffffff)

if __name__=='__main__':
    sample=[]
    for a,b in ((0,0),(7,9),(123,100),(65535,257),(0xffffffff,0xffffffff)):
        try:sample.append(run_b7(1,a,b))
        except Exception as e:sample.append({'a':a,'b':b,'error':repr(e)})
    (ROOT/'b7_01_multiply_replay.json').write_text(json.dumps(sample,indent=2))
    result=unittest.TextTestRunner(verbosity=2).run(unittest.defaultTestLoader.loadTestsFromTestCase(NativeSourceChecks))
    raise SystemExit(0 if result.wasSuccessful() else 1)
