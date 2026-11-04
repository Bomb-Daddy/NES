"""Transparent original-6502 replay. ROM loaded privately; no console/PPU emulation."""
from inspect_source import prg,O
class Cpu:
 def __init__(self,bank=1):
  self.m=bytearray(65536);self.m[0x8000:0xc000]=prg[bank*16384:(bank+1)*16384];self.m[0xc000:]=prg[-16384:]
  self.pc=0xefc9;self.a=self.x=self.y=0;self.sp=255;self.c=self.z=self.n=self.v=self.i=self.d=0;self.count=0
  self.vmtrace=[];self.writes=[]
 def word(self,p):return self.m[p]|self.m[(p+1)&65535]<<8
 def setword(self,p,v):self.m[p]=v&255;self.m[p+1]=(v>>8)&255
 def stream(self):return self.word(6)
 def get(self):
  b=self.m[self.pc];self.pc=(self.pc+1)&65535;return b
 def nz(self,v):v&=255;self.z=int(v==0);self.n=v>>7;return v
 def push(self,v):self.m[256+self.sp]=v&255;self.sp=(self.sp-1)&255
 def pop(self):self.sp=(self.sp+1)&255;return self.m[256+self.sp]
 def adc(self,v):
  t=self.a+v+self.c;self.v=int(bool((~(self.a^v)&(self.a^t))&128));self.c=int(t>255);self.a=self.nz(t)
 def flags(self):return self.c|(self.z<<1)|(self.i<<2)|(self.d<<3)|0x30|(self.v<<6)|(self.n<<7)
 def setflags(self,p):self.c=p&1;self.z=(p>>1)&1;self.i=(p>>2)&1;self.d=(p>>3)&1;self.v=(p>>6)&1;self.n=p>>7
 def step(self):
  pc=self.pc;op=self.get();self.count+=1
  if op not in O:raise RuntimeError(f'Illegal opcode {op:02X}@{pc:04X}')
  name,mode=O[op];addr=None;v=None
  if mode in ('zp','zpx','zpy','indx','indy'):
   addr=self.get()
   if mode=='zpx':addr=(addr+self.x)&255
   if mode=='zpy':addr=(addr+self.y)&255
   if mode=='indx':q=(addr+self.x)&255;addr=self.m[q]|self.m[(q+1)&255]<<8
   if mode=='indy':addr=((self.m[addr]|self.m[(addr+1)&255]<<8)+self.y)&65535
  elif mode in ('abs','absx','absy','ind'):
   addr=self.get()|(self.get()<<8)
   if mode=='absx':addr=(addr+self.x)&65535
   if mode=='absy':addr=(addr+self.y)&65535
   if mode=='ind':addr=self.m[addr]|self.m[(addr&0xff00)|((addr+1)&255)]<<8
  elif mode in ('imm','rel'):v=self.get()
  elif mode=='acc':v=self.a
  if addr is not None:v=self.m[addr]
  write=None
  if name in ('LDA','LDX','LDY'):setattr(self,name[-1].lower(),self.nz(v))
  elif name in ('STA','STX','STY'):write=getattr(self,name[-1].lower())
  elif name=='ADC':self.adc(v)
  elif name=='SBC':self.adc(v^255)
  elif name=='AND':self.a=self.nz(self.a&v)
  elif name=='ORA':self.a=self.nz(self.a|v)
  elif name=='EOR':self.a=self.nz(self.a^v)
  elif name in ('CMP','CPX','CPY'):
   t=getattr(self,{'CMP':'a','CPX':'x','CPY':'y'}[name]);self.c=int(t>=v);self.nz(t-v)
  elif name=='BIT':self.z=int((self.a&v)==0);self.n=v>>7;self.v=(v>>6)&1
  elif name in ('ASL','LSR','ROL','ROR'):
   oldc=self.c
   if name in ('ASL','ROL'):self.c=v>>7;write=self.nz((v<<1)|(oldc if name=='ROL' else 0))
   else:self.c=v&1;write=self.nz((v>>1)|(oldc<<7 if name=='ROR' else 0))
   if mode=='acc':self.a=write;write=None
  elif name in ('INC','DEC'):write=self.nz(v+(1 if name=='INC' else -1))
  elif name in ('INX','INY','DEX','DEY'):
   reg=name[-1].lower();setattr(self,reg,self.nz(getattr(self,reg)+(1 if name[0]=='I' else -1)))
  elif name in ('TAX','TAY','TXA','TYA','TSX','TXS'):
   src,dst={'TAX':('a','x'),'TAY':('a','y'),'TXA':('x','a'),'TYA':('y','a'),'TSX':('sp','x'),'TXS':('x','sp')}[name]
   val=getattr(self,src);setattr(self,dst,val if name=='TXS' else self.nz(val))
  elif name in ('PHA','PHP'):self.push(self.a if name=='PHA' else self.flags())
  elif name=='PLA':self.a=self.nz(self.pop())
  elif name=='PLP':self.setflags(self.pop())
  elif name=='JMP':self.pc=addr
  elif name=='JSR':q=(self.pc-1)&65535;self.push(q>>8);self.push(q);self.pc=addr
  elif name=='RTS':self.pc=((self.pop()|(self.pop()<<8))+1)&65535
  elif name=='RTI':self.setflags(self.pop());self.pc=self.pop()|(self.pop()<<8)
  elif name.startswith('B') and name!='BRK':
   cond={'BCC':not self.c,'BCS':self.c,'BEQ':self.z,'BNE':not self.z,'BMI':self.n,'BPL':not self.n,'BVC':not self.v,'BVS':self.v}[name]
   if cond:self.pc=(self.pc+(v-256 if v&128 else v))&65535
  elif name in ('CLC','SEC','CLI','SEI','CLD','SED','CLV'):
   setattr(self,{'CLC':'c','SEC':'c','CLI':'i','SEI':'i','CLD':'d','SED':'d','CLV':'v'}[name],int(name.startswith('SE')))
  elif name=='NOP':pass
  else:raise RuntimeError(f'Unsupported {name} @{pc:04x}')
  if write is not None:
   if 0x6000<=addr<0x8000 and self.m[addr]!=write:self.writes.append({'step':self.count,'pc':f'{pc:04X}','address':f'{addr:04X}','old':self.m[addr],'new':write})
   if addr>=0x8000:raise RuntimeError(f'Unmodelled mapper write @{pc:04x} ->{addr:04x}')
   self.m[addr]=write
 def setup(self,start,args=()):
  self.setword(2,0x7800);self.setword(4,0x7200);self.setword(6,start)
  for i,v in enumerate(args):self.setword(0x720b+i*2,v)
 def run(self,stop,maxsteps=1000000,trace=True):
  while self.count<maxsteps:
   if self.pc==0xefc9:
    p=self.stream()
    if trace:self.vmtrace.append({'pc':f'{p:04X}','opcode':f'{self.m[p]:02X}','a':self.word(8),'b':self.word(12),'stack':f'{self.word(2):04X}','frame':f'{self.word(4):04X}','step':self.count})
    if p==stop:return self.word(8)
   self.step()
  raise RuntimeError(f'Budget exhausted CPU{self.pc:04X} VM{self.stream():04X}')
