"""Bank-aware original NES source replay for cross-country economic transfers.
Synthetic original-6502 execution only; no Mesen, UI or port acceptance claims.
"""
from pathlib import Path
import sys,json,hashlib,os
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'batches/trade_source'))
import replay_trade as t
FRAME=0x5a00;STACK=0x5f00;SRC=0x6dcd;DST=SRC+83
BASELINE=Path(os.environ['GK_SRAM']).read_bytes()
word=t.word;put=t.put
class Cpu(t.Cpu):
 def step(self):
  if self.m[self.pc]!=0xf1:return super().step()
  # Official 6502 SBC (zero-page),Y, reached by original VM switch handler.
  self.get();z=self.get();self.count+=1;self.total+=1;self.opcounts[0xf1]=self.opcounts.get(0xf1,0)+1
  self.adc(self.m[(self.readind(z)+self.y)&65535]^255)

def prepare(start,args=(),bank=3,words=None,locals=None,byteset=None,baseline=False):
 c=Cpu(bank)
 if baseline:c.m[0x6000:0x8000]=BASELINE
 put(c,2,STACK);put(c,4,FRAME);put(c,6,start);c.pc=0xefc9
 for i,v in enumerate(args):put(c,FRAME+11+2*i,v)
 for a,v in (words or {}).items():put(c,a,v)
 for a,v in (locals or {}).items():put(c,FRAME+a,v)
 for a,v in (byteset or {}).items():c.m[a]=v
 return c

def run(c,stops,budget=500000):
 stops={stops} if isinstance(stops,int) else set(stops)
 for _ in range(budget):
  if c.pc==0xefc9 and c.stream() in stops:return {'A':word(c,8),'stop':c.stream(),'steps':c.total,'stack':word(c,2),'frame':word(c,4)}
  c.step()
 raise RuntimeError(f'Logistics source boundary not reached cpu={c.pc:04X} vm={c.stream():04X}')

def scalar_cap(stock,dest_used):
 c=prepare(0x95fc,[SRC+64,DST+64,dest_used],words={SRC+64:stock});r=run(c,0x960a);return word(c,FRAME-2)
def scalar_commit(src,dst,q,offset=64,mutation=None):
 c=prepare(0x961c,[SRC+offset,DST+offset,dst],words={SRC+offset:src,DST+offset:dst},locals={-2:q},byteset=mutation)
 before=bytes(c.m[0x6000:0x8000]);r=run(c,0x962b)
 return {'source':word(c,SRC+offset),'destination':word(c,DST+offset),'amount':r['A'],'steps':r['steps'],'changed_addresses':[a+0x6000 for a,(x,y) in enumerate(zip(before,c.m[0x6000:0x8000])) if x!=y]}
def scalar_cancel(src,dst):
 c=prepare(0x9615,[SRC+64,DST+64,dst],words={SRC+64:src,DST+64:dst});put(c,8,65535);before=bytes(c.m[0x6000:0x8000]);r=run(c,0x9634);return r['A'],before==bytes(c.m[0x6000:0x8000])
def country_state(c,p):
 return {'counts':[word(c,p+k) for k in (24,28,32,58,62)],'quality':[word(c,p+k) for k in (20,22,66)],'eligibility':c.m[p+72],'civilians':word(c,p+73),'production_history':[word(c,p+56),word(c,p+60)]}
def people(source_counts,dest_counts,source_quality,dest_quality,category,q,eligibility=(32,27),civilians=(34,43),mutation=None):
 words={};off=(24,28,32,58,62)[category]
 for p,counts,qual,civ in ((SRC,source_counts,source_quality,civilians[0]),(DST,dest_counts,dest_quality,civilians[1])):
  words.update({p+k:v for k,v in zip((24,28,32,58,62),counts)})
  words.update({p+k:v for k,v in zip((20,22,66),qual)});words[p+73]=civ;words[p+56]=17;words[p+60]=23
 c=prepare(0x961c,[SRC+off,DST+off,sum(dest_counts)],words=words,locals={-2:q},byteset=mutation)
 c.m[SRC+72],c.m[DST+72]=eligibility
 before=bytes(c.m[0x6000:0x8000]);run(c,0x962b)
 # Original people command resumes after UI-only E123 at $96F7. Preserve game state,
 # install its actual caller frame variables and original arguments; no arithmetic stub.
 put(c,6,0x96f7);put(c,FRAME+11,SRC);put(c,FRAME-8,SRC);put(c,FRAME-10,DST);put(c,FRAME-6,q)
 r=run(c,0x9703)
 return {'source':country_state(c,SRC),'destination':country_state(c,DST),'steps':r['steps'],'changed_addresses':[a+0x6000 for a,(x,y) in enumerate(zip(before,c.m[0x6000:0x8000])) if x!=y]}
def owned_source(candidate_index,selected=0,owner_mutations=None):
 c=prepare(0x84bb,[SRC+83*candidate_index],baseline=True,words={0x7b34:selected},byteset=owner_mutations);return run(c,0x84c9)['A']
def destination_gate(source_index,dest_index,owner_mutations=None,adjacency_mutations=None):
 patches=dict(owner_mutations or {});patches.update(adjacency_mutations or {})
 c=prepare(0x84cf,[SRC+83*dest_index],baseline=True,words={0x7b34:source_index},byteset=patches);return run(c,0x84ee)['A']
def any_owned_neighbor(source_index,owner_mutations=None,adjacency_mutations=None):
 patches=dict(owner_mutations or {});patches.update(adjacency_mutations or {})
 c=prepare(0x841c,[SRC+83*source_index],baseline=True,byteset=patches);return run(c,(0x843e,0x844c))['A']
def owned_count(owner_mutations=None):
 c=prepare(0x85b7,[SRC],baseline=True,byteset=owner_mutations);return run(c,0x85df)['A']
def cost(code,abilities,orders=3,success=True):
 # Executes original parent command result processing in bank0, including nested
 # original ability pointer resolution and all six source stat deductions.
 words={0x7b66:0,0x7b34:0,0x7b36:orders}
 words.update({0x65bf+14+2*i:v for i,v in enumerate(abilities)})
 c=prepare(0x8c95,bank=0,words=words,baseline=True);put(c,8,code if success else 255);run(c,0x8ca9)
 return {'abilities':[word(c,0x65bf+14+2*i) for i in range(6)],'orders':word(c,0x7b36)}
def ability_gate(code,abilities):
 words={0x7b66:0,0x7b34:0};words.update({0x65bf+14+2*i:v for i,v in enumerate(abilities)})
 c=prepare(0x8280,[code],words=words,baseline=True);r=run(c,(0x829d,0x82ba));return r['stop']==0x82ba

if __name__=='__main__':
 print(json.dumps({'gold':scalar_commit(439,66,10),'food':scalar_commit(208,90,8,54),'people':people([61,5,4,6,27],[63,5,4,6,27],[110,124,118],[150,80,100],0,8),'empty_source':people([8,0,0,0,0],[10,0,0,0,0],[110,124,118],[150,80,100],0,8),'friendly_gate':destination_gate(0,1,{0x65fb:0}),'cost22':cost(22,[100]*6),'cost23':cost(23,[100]*6)},indent=2))
