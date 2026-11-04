#!/usr/bin/env python3
"""Original 6502 instruction replay of bounded domestic routines; no NES emulation."""
import sys, os, importlib.util, json, hashlib
from pathlib import Path
sys.dont_write_bytecode=True
ROOT=Path(__file__).resolve().parent
BASE=ROOT.parents[1]
assert os.environ.get('GK_ROM'), 'Set GK_ROM explicitly'
s=importlib.util.spec_from_file_location('original_helper',BASE/'output/research/Genghis_Khan_NES/v1.06/tools/replay_b7_arithmetic.py');h=importlib.util.module_from_spec(s);s.loader.exec_module(h)
SRAM=Path(os.environ['GK_SRAM'])
class DomesticCpu(h.HelperCpu):
 def step(self):
  if self.m[self.pc] in (0x50,0x70):
   op=self.get();self.count+=1;self.opcounts[op]=self.opcounts.get(op,0)+1;self.branch(self.v==(op==0x70));return
  super().step()
COUNTRY=0x6dcd
FIELDS={'arms':0x14,'skill':0x16,'troops':0x18,'economy':0x1a,'town':0x1c,'building':0x1e,'masons':0x20,'food':0x36,'artisans':0x3a,'food_workers':0x3e,'gold':0x40,'morale':0x42,'tax':0x44,'tax_used':0x46,'eligibility':0x48,'civilians':0x49}
def word(c,p,v=None):
 if v is None:return c.m[p]|(c.m[(p+1)&65535]<<8)
 c.m[p]=v&255;c.m[p+1]=(v>>8)&255

def run(entry,stop,args,bank=3,mutations=None,fields=None,segments=None,rng=None,locals_=None,cap=400000):
 c=DomesticCpu();c.m[0x8000:0xc000]=h.PRG[bank*16384:(bank+1)*16384];c.m[0x6000:0x8000]=SRAM.read_bytes()
 # Synthetic scratch frame/stack at $700/$1800, outside loaded cartridge state.
 word(c,2,0x1800);word(c,4,0x700);word(c,6,entry);c.pc=0xefc9
 for i,v in enumerate(args):word(c,0x70b+2*i,v)
 if fields:
  for k,v in fields.items():
   p=COUNTRY+FIELDS[k]
   if k=='eligibility':c.m[p]=v
   else:word(c,p,v)
 if segments:
  for p,v in segments.items():word(c,p,v)
 if locals_:
  for p,v in locals_.items():word(c,0x700+p,v)
 if mutations:
  for p,v in mutations.items():c.m[p]=v
 before=bytes(c.m);trace=[];rng_events=[];seen=set();rng=list(rng or [])
 for i in range(cap):
  if c.pc==0xefc9:
   p=c.stream()
   if p not in seen:
    seen.add(p);trace.append({'pc':f'{p:04X}','a':word(c,8),'b':word(c,12),'frame':word(c,4),'sp':word(c,2)})
   if p==stop and word(c,4)==0x700:
    out={'entry':f'{entry:04X}','stop':f'{stop:04X}','bank':bank,'args':args,'a':word(c,8),'cpu_steps':i,'frame_restored':word(c,4)==0x700,'stack_restored':word(c,2)==0x1800,'fields':{k:(c.m[COUNTRY+o] if k=='eligibility' else word(c,COUNTRY+o)) for k,o in FIELDS.items()},'changes':[{'address':f'{p:04X}','before':before[p],'after':c.m[p]} for p in range(0x6000,0x8000) if before[p]!=c.m[p]],'rng':rng_events,'trace':trace}
    return out
   # Controlled exogenous random draw boundary. Original DB93 modulus follows.
   # F988 would query native RNG; substitute A at DB9E, skip its call only.
   if p==0xdb9e and rng:
    v=rng.pop(0);word(c,8,v);word(c,6,0xdba4);rng_events.append({'native_random_input':v,'modulus':word(c,word(c,4)+11)});continue
  try:c.step()
  except Exception as e:raise RuntimeError(f'{e}; VM={c.stream():04X}; frame={word(c,4):04X}; trace={trace[-12:]}') from e
 raise RuntimeError(f'budget {cap}; vm={c.stream():04X}')

if __name__=='__main__':
 samples=[]
 for entry,stop,args in [(0x8914,0x892e,[COUNTRY,118,1]),(0x8a85,0x8ad3,[COUNTRY,10,8]),(0x88bd,0x890e,[COUNTRY,35]),(0x9063,0x909c,[COUNTRY,8])]:
  try:
   r=run(entry,stop,args);samples.append(r);print(hex(entry),r['a'],r['fields'],flush=True)
  except Exception as e:samples.append({'entry':hex(entry),'error':repr(e)});print(repr(e),flush=True)
 (ROOT/'initial_replays.json').write_text(json.dumps(samples,indent=2))
