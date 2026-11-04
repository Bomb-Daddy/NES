from pathlib import Path
import json

def restore(name='spring1175'):
 for bp in call('breakpoint.list')['breakpoints']:call('breakpoint.delete',handle=bp['handle'])
 call('input.set',buttons={})
 call('state.restore',path=str(OUT/(name+'.mss')));step(2)
 if call('state.status')['operation']!='loaded':
  # Native message waits can delay the next permitted VM callback.
  # Acknowledge that old state, then restore again with released input.
  step(180)
  for _ in range(3):
   if call('state.status')['operation']=='loaded':break
   press('a',5,60)
  assert call('state.status')['operation']=='loaded'
  call('input.set',buttons={})
  call('state.restore',path=str(OUT/(name+'.mss')));step(2)
 assert call('state.status')['operation']=='loaded'
def memory(a,n):return bytes(call('cpu.read_memory',memoryType='nesMemory',address=a,length=n)['bytes'])
def word(a):return int.from_bytes(memory(a,2),'little')
def write(a,data):return call('cpu.write_memory',memoryType='nesMemory',address=a,bytes=list(data))
def wword(a,v):return write(a,int(v).to_bytes(2,'little'))
def keys(seq,pause=60):
 for k in seq:press(k,5,pause)
def menu(n):keys(['down']*n);press('a',5,180)
def all_offers():
 write(0x7a8c,bytes([1,0])*84)
 for a in [0x6e18,0x6e1a,0x6e1c]:wword(a,1)
def shop(m=0,mode=1):
 menu(4);keys(['down']*m);press('a',5,180);keys(['down']*mode);press('a',5,600)
def item(i):
 col,row=(0,i) if i<6 else ((1,i-6) if i<11 else (2,i-11))
 keys(['right']*col+['down']*row,100);press('a',5,600)

def number(n):
 # Numeric prompt starts0. Right multiplies the entered value by10; Up increments the units digit.
 s=str(n)
 for j,d in enumerate(s):
  if j:press('right',5,60)
  keys(['up']*int(d))
def completed(label):
 press('a',5,180)
 return snap(label)
