"""Original-instruction, bank-aware synthetic replay. No UI/emulator claims.
Private ROM and optional prior baseline SRAM required; never writes ROM bytes.
"""
from inspect_source import *
import json,hashlib
class Cpu(h.HelperCpu):
 def __init__(self,bank=3):
  super().__init__();self.m[0x8000:0xc000]=PRG[bank*16384:(bank+1)*16384];self.total=0
 def step(self):
  self.total+=1
  if self.count>90000:self.count=0 # inherited diagnostic counter only; CPU state untouched
  op=self.m[self.pc]
  extra={0xad,0xae,0xac,0x8d,0x8e,0x8c,0x9d,0x6d,0x7d,0x79,0xce,0xee,0x0e,0x3e,0x2e,0x08,0x28,0x40,0xba,0x25,0x35,0x2a,0x6a,0x4d,0xed,0x0d}
  if op not in extra:return super().step()
  self.get();self.count+=1;self.opcounts[op]=self.opcounts.get(op,0)+1
  if op==0xad:self.a=self.nz(self.m[self.word()])
  elif op==0xae:self.x=self.nz(self.m[self.word()])
  elif op==0xac:self.y=self.nz(self.m[self.word()])
  elif op==0x8d:self.m[self.word()]=self.a
  elif op==0x8e:self.m[self.word()]=self.x
  elif op==0x8c:self.m[self.word()]=self.y
  elif op==0x9d:self.m[(self.word()+self.x)&65535]=self.a
  elif op==0x6d:self.adc(self.m[self.word()])
  elif op==0x7d:self.adc(self.m[(self.word()+self.x)&65535])
  elif op==0x79:self.adc(self.m[(self.word()+self.y)&65535])
  elif op==0xed:self.adc(self.m[self.word()]^255)
  elif op in (0xce,0xee):
   a=self.word();self.m[a]=self.nz(self.m[a]+(1 if op==0xee else -1))
  elif op in (0x0e,0x3e,0x2e):
   a=(self.word()+(self.x if op==0x3e else 0))&65535;t=self.m[a];self.m[a]=self.nz((t<<1)|(self.c if op!=0x0e else 0));self.c=t>>7
  elif op==0x08:self.push(self.c|(self.z<<1)|(1<<4)|(1<<5)|(self.v<<6)|(self.n<<7))
  elif op in (0x28,0x40):
   p=self.pop();self.c=p&1;self.z=(p>>1)&1;self.v=(p>>6)&1;self.n=(p>>7)&1
   if op==0x40:self.pc=self.pop()|(self.pop()<<8)
  elif op==0xba:self.x=self.nz(self.sp)
  elif op==0x25:self.a=self.nz(self.a&self.m[self.get()])
  elif op==0x35:self.a=self.nz(self.a&self.m[(self.get()+self.x)&255])
  elif op==0x2a:t=self.a;self.a=self.nz((t<<1)|self.c);self.c=t>>7
  elif op==0x6a:t=self.a;self.a=self.nz((t>>1)|(self.c<<7));self.c=t&1
  elif op==0x4d:self.a=self.nz(self.a^self.m[self.word()])
  elif op==0x0d:self.a=self.nz(self.a|self.m[self.word()])
def word(c,a):return c.m[a]|c.m[a+1]<<8
def put(c,a,v):c.m[a]=v&255;c.m[a+1]=(v>>8)&255
def prepare(start,args=(),bank=3,words=None,locals=None,mutation=None,seed=0x010203040506):
 c=Cpu(bank);put(c,2,0x7800);put(c,4,0x7200);put(c,6,start);c.pc=0xefc9
 for i,v in enumerate(args):put(c,0x720b+2*i,v)
 for a,v in (words or {}).items():put(c,a,v)
 for a,v in (locals or {}).items():put(c,0x7200+a,v)
 for a,v in (mutation or {}).items():c.m[a]=v
 c.m[0x84:0x8a]=seed.to_bytes(6,'big')
 return c

def run(c,stop,budget=2000000,trace=False):
 out=[];old=None
 for _ in range(budget):
  if c.pc==0xefc9:
   s=c.stream()
   if trace and s!=old:out.append({'pc':f'{s:04X}','A':word(c,8),'B':word(c,12),'sp':word(c,2),'fp':word(c,4)});old=s
   if s==stop:return {'A':word(c,8),'steps':c.total,'trace':out,'stack':word(c,2),'frame':word(c,4)}
  c.step()
 raise RuntimeError(f'Budget exhausted: cpu={c.pc:04X} VM={c.stream():04X}')

COUNTRY=0x6dcd

def rng(seed,n=11):
 c=prepare(0xdb98,[n],seed=seed);r=run(c,0xdba6);return r['A'],int.from_bytes(c.m[0x84:0x8a],'big'),r['steps']
def avg(old,count,new,n):
 c=prepare(0xdb4d,[old,count,new,n]);r=run(c,0xdb92 if count or n else 0xdb56);return r['A']
def sale(gold,stock,quantity,quote,mutation=None):
 c=prepare(0x8c20,[COUNTRY],words={COUNTRY+64:gold,COUNTRY+34:stock},locals={-4:quantity,-12:COUNTRY+34,-8:quote,-10:COUNTRY+64},mutation=mutation)
 r=run(c,0x8c33);return {'gold':word(c,COUNTRY+64),'stock':word(c,COUNTRY+34),'steps':r['steps']}
def sale_cap(gold,stock,quote):
 c=prepare(0x8bf1,[COUNTRY],words={COUNTRY+64:gold,COUNTRY+34:stock},locals={-12:COUNTRY+34,-8:quote,-10:COUNTRY+64});run(c,0x8c06);return word(c,0x71fa)
def buy_cap(gold,quote,capacity):
 c=prepare(0x8662,[COUNTRY,quote,capacity],words={COUNTRY+64:gold});return run(c,0x8670)['A']
def hire(arms,skill,morale,counts,n,seed=0x010203040506,mutation=None):
 words={COUNTRY+20:arms,COUNTRY+22:skill,COUNTRY+66:morale};words.update({COUNTRY+k:v for k,v in zip((24,28,32,58,62),counts)})
 c=prepare(0x8688,[COUNTRY,n],words=words,seed=seed,mutation=mutation);r=run(c,0x86ca)
 return {'arms':word(c,COUNTRY+20),'skill':word(c,COUNTRY+22),'morale':word(c,COUNTRY+66),'counts':[word(c,COUNTRY+k) for k in (24,28,32,58,62)],'steps':r['steps'],'seed_after':int.from_bytes(c.m[0x84:0x8a],'big')}
def weapon_gain(arms,total,quantity,index,base,mutation=None):
 c=prepare(0x8d06,[COUNTRY],words={COUNTRY+20:arms,COUNTRY+24:total},locals={-4:quantity,-2:index},mutation=mutation);c.m[0x7de5+index]=base;run(c,0x8d25);return word(c,COUNTRY+20)
def weapon_cap(arms,total,index,base,gold=999,quote=1):
 c=prepare(0x8ca8,[COUNTRY],words={COUNTRY+20:arms,COUNTRY+24:total,COUNTRY+64:gold},locals={-2:index,-6:quote});c.m[0x7de5+index]=base;run(c,0x8cd5);return word(c,0x71fc)
def offers(seed,mutation=None):
 c=prepare(0x8331,bank=1,seed=seed,mutation=mutation);r=run(c,0x83c4)
 return {'sell':[[word(c,0x7a8c+22*j+2*i) for i in range(11)] for j in range(3)],'buy':[[word(c,0x7ace+34*j+2*i) for i in range(17)] for j in range(3)],'seed':int.from_bytes(c.m[0x84:0x8a],'big'),'steps':r['steps']}
if __name__=='__main__':
 print(json.dumps({'rng':rng(0x010203040506),'avg':avg(110,103,100,8),'sale':sale(432,27,1,50),'hire':hire(110,124,118,[61,5,4,6,27],8),'weapon':weapon_gain(110,103,1,11,20),'offers':offers(0x010203040506)},indent=2))

def presence(economy,morale,scenario_count=14,countdown=14,seed=0x010203040506):
 c=prepare(0x907a,bank=1,words={COUNTRY+26:economy,COUNTRY+66:morale,0x65b9:scenario_count},locals={-12:COUNTRY,-2:countdown},seed=seed);r=run(c,0x90cb)
 return {'flags':[word(c,COUNTRY+75+2*i) for i in range(3)],'steps':r['steps'],'seed':int.from_bytes(c.m[0x84:0x8a],'big')}
def ability(value,mode='debit'):
 c=prepare(0x8247 if mode=='debit' else 0x8280,[9],words={0x7b66:0,COUNTRY+81:0x6500,0x6510:value})
 for i in range(6):put(c,0x650e+2*i,value)
 c.m[0x651e]=100;c.m[0x7e18+9*6:0x7e18+10*6]=bytes([0,5,0,0,0,0])
 if mode=='debit':run(c,0x827a);return [word(c,0x650e+2*i) for i in range(6)]
 for _ in range(100000):
  if c.pc==0xefc9 and c.stream() in (0x829d,0x82ba):return c.stream()==0x82ba
  c.step()
 raise RuntimeError('ability eligibility boundary not reached')
def hire_commit(arms,skill,morale,counts,n,gold,quote,seed=0x010203040506):
 words={COUNTRY+20:arms,COUNTRY+22:skill,COUNTRY+66:morale,COUNTRY+64:gold};words.update({COUNTRY+k:v for k,v in zip((24,28,32,58,62),counts)})
 c=prepare(0x8d84,[COUNTRY],words=words,locals={-6:n,-4:quote},seed=seed);r=run(c,0x8d9a)
 return {'arms':word(c,COUNTRY+20),'skill':word(c,COUNTRY+22),'morale':word(c,COUNTRY+66),'troops':word(c,COUNTRY+24),'gold':word(c,COUNTRY+64),'steps':r['steps']}


def buy_commit(gold,stock,n,quote,index=0,arms=110,total=103,base=20):
 c=prepare(0x8cfb,[COUNTRY],words={COUNTRY+64:gold,COUNTRY+34+2*index:stock,COUNTRY+20:arms,COUNTRY+24:total},locals={-2:index,-4:n,-6:quote,-10:COUNTRY+34+2*index})
 c.m[0x7de5+index]=base;c.m[0x7a8c:0x7b34]=bytes([1,0])*84;before=bytes(c.m[0x7a8c:0x7b34]);r=run(c,0x8d2d)
 return {'gold':word(c,COUNTRY+64),'stock':word(c,COUNTRY+34+2*index),'arms':word(c,COUNTRY+20),'merchant_masks_unchanged':bytes(c.m[0x7a8c:0x7b34])==before,'steps':r['steps']}
