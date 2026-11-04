#!/usr/bin/env python3
"""Original NES 6502 fixed-bank interpreter *selected-path* deterministic ROM-byte replay.
This is an isolated CPU instruction harness, not console emulation, scenario boot or full game behavior.
No copied ROM or copyrighted art enters the deliverables.
"""
import os, csv, hashlib, json, unittest, sys
from pathlib import Path
ROM=Path(os.environ.get('GK_ROM','/mnt/data/Genghis Khan (USA).nes'))
EXPECTED='ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3'
DATA=ROM.read_bytes(); PRG=DATA[16:]; OUT=Path(__file__).resolve().parent; OUT.mkdir(exist_ok=True)
B2=PRG[2*0x4000:3*0x4000]; FIX=PRG[15*0x4000:16*0x4000]
START=0x91D0 # physical PRG bank 2, offset 0x11D0
SNIPPET=bytes.fromhex('a4 8a 7a 51 be b3 a4 88 7a 55 b5 b3')
assert B2[0x11D0:0x11D0+len(SNIPPET)]==SNIPPET

class Cpu:
    def __init__(self,rate=101,secondary=11,script_mutation=None,changed_adjacent=None):
        self.m=bytearray(65536)
        self.m[0x8000:0xC000]=B2
        self.m[0xC000:]=FIX
        self.m[0x7A8A]=rate&255;self.m[0x7A8B]=(rate>>8)&255
        self.m[0x7A88]=secondary&255;self.m[0x7A89]=(secondary>>8)&255
        if changed_adjacent is not None:self.m[0x7A8C]=changed_adjacent&255
        if script_mutation:
            for k,v in script_mutation.items():self.m[k]=v
        self.pc=0xEFC9;self.a=0;self.x=0;self.y=0;self.sp=0xFF
        self.c=0;self.z=0;self.n=0;self.v=0; self.count=0;self.opcounts={}
        self.m[0x06]=START&255;self.m[0x07]=START>>8
        self.steps=[]
    def stream(self):return self.m[0x06]|(self.m[0x07]<<8)
    def get(self):
        q=self.m[self.pc];self.pc=(self.pc+1)&65535;return q
    def word(self):
        l=self.get();return l|(self.get()<<8)
    def readind(self,z):return self.m[z]|(self.m[(z+1)&255]<<8)
    def nz(self,x):self.z=int((x&255)==0);self.n=int((x&128)!=0);return x&255
    def push(self,v):self.m[0x100+self.sp]=v&255;self.sp=(self.sp-1)&255
    def pop(self):self.sp=(self.sp+1)&255;return self.m[0x100+self.sp]
    def adc(self,b):
        t=self.a+b+self.c;self.v=int((~(self.a^b)&(self.a^t)&0x80)!=0);self.c=int(t>255);self.a=self.nz(t)
    def branch(self,take):
        off=self.get();off=off-256 if off&0x80 else off
        if take:self.pc=(self.pc+off)&65535
    def step(self):
        pc=self.pc;op=self.get();self.count+=1;self.opcounts[op]=self.opcounts.get(op,0)+1
        if op==0xA0:self.y=self.nz(self.get())
        elif op==0xA2:self.x=self.nz(self.get())
        elif op==0xA9:self.a=self.nz(self.get())
        elif op==0xA5:self.a=self.nz(self.m[self.get()])
        elif op==0xA6:self.x=self.nz(self.m[self.get()])
        elif op==0xB1:self.a=self.nz(self.m[(self.readind(self.get())+self.y)&65535])
        elif op==0xBD:self.a=self.nz(self.m[(self.word()+self.x)&65535])
        elif op==0xB9:self.a=self.nz(self.m[(self.word()+self.y)&65535])
        elif op==0x71:self.adc(self.m[(self.readind(self.get())+self.y)&65535])
        elif op==0x85:self.m[self.get()]=self.a
        elif op==0x84:self.m[self.get()]=self.y
        elif op==0x86:self.m[self.get()]=self.x
        elif op==0x91:self.m[(self.readind(self.get())+self.y)&65535]=self.a
        elif op==0x99:self.m[(self.word()+self.y)&65535]=self.a
        elif op==0xAA:self.x=self.nz(self.a)
        elif op==0x8A:self.a=self.nz(self.x)
        elif op==0xA8:self.y=self.nz(self.a)
        elif op==0x98:self.a=self.nz(self.y)
        elif op==0x88:self.y=self.nz(self.y-1)
        elif op==0xC8:self.y=self.nz(self.y+1)
        elif op==0xCA:self.x=self.nz(self.x-1)
        elif op==0xE8:self.x=self.nz(self.x+1)
        elif op==0xE6:
            z=self.get();self.m[z]=self.nz(self.m[z]+1)
        elif op==0xC6:
            z=self.get();self.m[z]=self.nz(self.m[z]-1)
        elif op==0x18:self.c=0
        elif op==0x38:self.c=1
        elif op==0x65:self.adc(self.m[self.get()])
        elif op==0x69:self.adc(self.get())
        elif op==0xE9:self.adc(self.get()^0xFF)
        elif op==0xE5:self.adc(self.m[self.get()]^0xFF)
        elif op==0x29:self.a=self.nz(self.a&self.get())
        elif op==0x09:self.a=self.nz(self.a|self.get())
        elif op==0x05:self.a=self.nz(self.a|self.m[self.get()])
        elif op==0x4A:
            self.c=self.a&1;self.a=self.nz(self.a>>1)
        elif op==0x46:
            z=self.get();self.c=self.m[z]&1;self.m[z]=self.nz(self.m[z]>>1)
        elif op==0x66:
            z=self.get();t=self.m[z];self.m[z]=self.nz((self.c<<7)|(t>>1));self.c=t&1
        elif op==0x06:
            z=self.get();t=self.m[z];self.c=(t>>7)&1;self.m[z]=self.nz(t<<1)
        elif op==0x26:
            z=self.get();t=self.m[z];self.m[z]=self.nz((t<<1)|self.c);self.c=(t>>7)&1
        elif op==0xC0:
            b=self.get();t=(self.y-b)&511;self.c=int(self.y>=b);self.z=int((t&255)==0);self.n=int(bool(t&128))
        elif op==0x90:self.branch(self.c==0)
        elif op==0xB0:self.branch(self.c==1)
        elif op==0xD0:self.branch(self.z==0)
        elif op==0xF0:self.branch(self.z==1)
        elif op==0x10:self.branch(self.n==0)
        elif op==0x30:self.branch(self.n==1)
        elif op==0x4C:self.pc=self.word()
        elif op==0x6C:
            ptr=self.word();self.pc=self.m[ptr]|(self.m[(ptr&0xff00)|((ptr+1)&255)]<<8)
        elif op==0x20:
            target=self.word();returnpc=(self.pc-1)&65535;self.push(returnpc>>8);self.push(returnpc&255);self.pc=target
        elif op==0x60:self.pc=((self.pop()|(self.pop()<<8))+1)&65535
        elif op==0x48:self.push(self.a)
        elif op==0x68:self.a=self.nz(self.pop())
        elif op==0xEA:pass
        else:raise RuntimeError(f'UNSUPPORTED CPU OPCODE {op:02X} AT ${pc:04X} after {self.count} instructions')
        if self.count>100000:raise RuntimeError('instruction budget exceeded')
    def advance_vm(self,stop_stream,limit=10000):
        for _ in range(limit):
            if self.pc==0xEFC9 and self.stream()==stop_stream:return self.vm_state()
            self.step()
        raise RuntimeError('VM requested boundary not reached; last PC '+hex(self.pc)+' stream '+hex(self.stream()))
    def vm_state(self):return {'vm_pc':f'{self.stream():04X}','reg_A':self.m[8]|(self.m[9]<<8),'reg_B':self.m[12]|(self.m[13]<<8),'steps':self.count,'cpu_pc':f'{self.pc:04X}'}

# Bank 2 @ $91D0: A4 $7A8A (read 16-bit rate), 51 (set register B=1), BE (logical shift A right by B).
# At $91D5 the next byte is B3 and the rate/2 result must be in VM register A.
def replay_rate(rate,secondary=11,mutation=None,adjacent=None):
    cpu=Cpu(rate,secondary,mutation,adjacent)
    a=cpu.advance_vm(START+3)
    b=cpu.advance_vm(START+4)
    c=cpu.advance_vm(START+5)
    return {'rate_input':rate,'secondary_input':secondary,'after_read_A':a['reg_A'],'after_setB_B':b['reg_B'],'after_shift_A':c['reg_A'],'steps':c['steps'],'opcode_cpu_histogram':{f'{x:02X}':v for x,v in sorted(cpu.opcounts.items())}}

# Continue past B3 (VM push A) then A4 $7A88, 55 (B=5), B5 (unsigned A*B), before B3 at $91DB
# Note: B3 pushes VM A; $91D5->B3, $91D6->A4 88 7A, $91D9->55, $91DA->B5, $91DB->B3.
def replay_secondary(rate,secondary):
    cpu=Cpu(rate,secondary)
    checkpoints={}
    for pos in (START+5,START+6,START+9,START+10,START+11):
        checkpoints[f'{pos:04X}']=cpu.advance_vm(pos)
    return checkpoints

class StaticCpuProof(unittest.TestCase):
    def test_hash(self):self.assertEqual(hashlib.sha256(DATA).hexdigest(),EXPECTED)
    def test_cartridge_script_is_actual_rom(self):self.assertEqual(B2[0x11D0:0x11D0+len(SNIPPET)],SNIPPET)
    def test_bank_switch_mapping_negative(self):self.assertNotEqual(PRG[0x11CF:0x11D0+len(SNIPPET)],SNIPPET)
    def test_decode_a4(self):self.assertEqual(FIX[0xF1A5-0xC000:0xF1A9-0xC000],b'\x20\x37\xf7\x90')
    def test_decode_51_load_B(self):self.assertEqual(FIX[0xF169-0xC000:0xF171-0xC000],bytes.fromhex('8a 29 0f 85 0c 84 0d 4c c9')[:8])
    def test_decode_be_logical_shift(self):self.assertEqual(FIX[0xF550-0xC000:0xF560-0xC000],bytes.fromhex('a6 0c d0 07 c6 0d 10 03 4c c9 ef 46 09 66 08 ca'))
    def test_decode_b5_mul(self):self.assertEqual(FIX[0xF452-0xC000:0xF45A-0xC000],bytes.fromhex('a0 00 84 00 84 01 a0 10'))
    def test_rate_replay_at_four_original_sample_points(self):
        for r in (0,69,123,190):
            with self.subTest(r=r):
                v=replay_rate(r)
                self.assertEqual((v['after_read_A'],v['after_setB_B'],v['after_shift_A']),(r,1,r//2))
    def test_rate_replay_exhaustive_8bit(self):
        for r in range(256):self.assertEqual(replay_rate(r)['after_shift_A'],r//2)
    def test_rate_negative_control_input_unchanged(self):
        self.assertEqual(replay_rate(123,adjacent=0)['after_shift_A'],replay_rate(123,adjacent=255)['after_shift_A'])
    def test_rate_negative_control_mutate_operand(self):
        # Change A4 operand to $7A8C instead of $7A8A. $7A8C=200, not 123.
        m={START+1:0x8C};c=Cpu(rate=123,script_mutation=m,changed_adjacent=200)
        self.assertEqual(c.advance_vm(START+5)['reg_A'],100)
        self.assertNotEqual(replay_rate(123)['after_shift_A'],100)
    def test_secondary_replay(self):
        for r,x in [(123,1),(123,5),(69,11),(101,100),(0,199)]:
            v=replay_secondary(r,x)
            self.assertEqual(v[f'{START+9:04X}']['reg_A'],x)
            self.assertEqual(v[f'{START+10:04X}']['reg_B'],5)
            self.assertEqual(v[f'{START+11:04X}']['reg_A'],x*5)
    def test_secondary_mul_exhaustive_8bit(self):
        for x in range(256):self.assertEqual(replay_secondary(123,x)[f'{START+11:04X}']['reg_A'],x*5)
    def test_mul_wrap_boundary(self):
        for x in [13107,13108,32768,65535]:
            self.assertEqual(replay_secondary(123,x)[f'{START+11:04X}']['reg_A'],(x*5)&65535)
    def test_no_private_rom_in_output(self):self.assertFalse(any(x.suffix.lower() in ('.nes','.sfc','.smc') for x in OUT.iterdir()))

if __name__=='__main__':
    sample=[replay_rate(r) for r in (0,69,101,123,190,255)]
    (OUT/'rate_shift_6502_replay.json').write_text(json.dumps(sample,indent=2))
    ds=[]
    for r,x in [(123,1),(123,5),(69,11),(101,100),(123,13108)]:
        ds.append({'market_rate':r,'secondary':x,'vm':replay_secondary(r,x)})
    (OUT/'secondary_multiply_6502_replay.json').write_text(json.dumps(ds,indent=2))
    suite=unittest.defaultTestLoader.loadTestsFromTestCase(StaticCpuProof)
    res=unittest.TextTestRunner(stream=sys.stdout,verbosity=2).run(suite)
    sys.exit(0 if res.wasSuccessful() else 1)
