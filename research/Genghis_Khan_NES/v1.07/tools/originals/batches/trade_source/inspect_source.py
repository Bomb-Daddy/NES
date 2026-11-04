from pathlib import Path
import os,importlib.util
ROOT=Path(__file__).resolve().parents[2]
os.environ.setdefault('GK_ROM',str(ROOT/'private/Genghis Khan (USA).nes'))
os.environ.setdefault('GK_SRAM',str(ROOT/'evidence/roundtrip/baseline.sram'))
spec=importlib.util.spec_from_file_location('h',ROOT/'output/research/Genghis_Khan_NES/v1.06/tools/replay_b7_arithmetic.py');h=importlib.util.module_from_spec(spec);spec.loader.exec_module(h)
PRG=h.PRG
mem=bytearray(65536);mem[0x8000:0xc000]=PRG[3*16384:4*16384];mem[0xc000:]=PRG[15*16384:]
def fixed(pc):return mem[pc]
def fixedword(pc):return fixed(pc)|(fixed(pc+1)<<8)
def hx(a):return f'${a:04X}'
def offset(a):return a-0x8000+3*16384+16 if a<0xc000 else a-0xc000+15*16384+16
src=(ROOT/'recovery/static_dependencies/gk_static_forensics.py').read_text();exec(src[src.index('# Standard, legal'):src.index('# Capture handler')])
VM={0xb0:'A = word[A]',0xb1:'word[pop] = A',0xb3:'push A',0xb4:'B = pop',0xb5:'A *= B (u16)',0xb8:'A /= B (u16)',0xba:'A %= B',0xbb:'A += B',0xbc:'A -= B',0xbd:'A <<= B',0xbe:'A >>= B',0xc0:'A = (A==B)',0xc1:'A = (A!=B)',0xc6:'A = (A < B unsigned)',0xc8:'A = (A > B unsigned)',0xcd:'swap A B',0xcf:'return',0xd0:'A++',0xd1:'A--',0xd2:'A *= 2',0xd3:'A = byte[A]',0xd4:'byte[pop] = A',0xd6:'jump',0xd7:'jump if A!=0',0xd8:'jump if A==0',0xe9:'call',0xac:'native call',0xa4:'A = word[absolute]',0xa8:'word[absolute] = A',0x89:'A = signed immediate',0x8a:'A = immediate',0x8b:'B = signed immediate',0x8c:'B = immediate',0x8d:'push signed immediate',0x8e:'push immediate',0x8f:'A += signed immediate',0x90:'A += immediate',0xb7:'native helper',0xde:'A = frame+offset'}
def vm(a):
 op=mem[a];sz=1;desc=VM.get(op,f'UNKNOWN {op:02X}')
 if op<0x40:
  nib=op&15;off=(nib*2-24 if nib<12 else (nib-12)*2+11);desc=['A = ','B = ','store A to ','push '][op>>4]+f'frame[{off:+}]'
 elif op<0x50:desc=f'A = {op&15}'
 elif op<0x60:desc=f'B = {op&15}'
 elif op<0x70:desc=f'push {op&15}'
 elif op<0x80:desc=f'A += {op&15}'
 elif op in (0x89,0x8b,0x8d,0x8f,0xb7):
  sz=2;desc+=' '+str(mem[a+1] if mem[a+1]<128 or op==0xb7 else mem[a+1]-256)
 elif op in (0x8a,0x8c,0x8e,0x90,0xa4,0xa8,0xac,0xde,0xd6,0xd7,0xd8):
  sz=3;desc+=f' ${fixedword(a+1):04X}'
 elif op==0xe9:
  sz=4;desc+=f' ${fixedword(a+1):04X}; popargs={mem[a+3]}'
 return sz,f'{a:04X}: {desc}'
def vmlines(a,b):
 out=[]
 while a<b:
  sz,t=vm(a);out.append(t);a+=sz
 return '\n'.join(out)
if __name__=='__main__':
 import sys
 kind,a,b=sys.argv[1],int(sys.argv[2],16),int(sys.argv[3],16)
 print(vmlines(a,b) if kind=='vm' else lines(a,b))
