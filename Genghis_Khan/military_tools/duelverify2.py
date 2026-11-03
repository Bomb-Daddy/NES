import sys, os; sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from wartools import *
from traintest import set_state
import random
c=gk.Core(); nav.loadstate(c,'state_duel1.bin'); c.run(3); st=c.save()
random.seed(2024); res=[]
slots=(0,1,2,4,7)
for t in range(24):
    c.load(st); c.run(2)
    for i in slots:
        for base in (0x6060,0x60CE):
            v=random.randrange(10,400); c.wr(base+11*i,v&255); c.wr(base+11*i+1,v>>8)
    for p in (0x65D3,):   # challenger stat3 >= defender stat3 -> always accepted
        c.wr(p,200&255); c.wr(p+1,0)
    c.wr(0x65F2,100); c.wr(0x65F3,0)
    for a in (0x65BF+14+2*i for i in range(6)):
        pass
    set_state(c,random.getrandbits(48))
    b0=[c.rd16(0x6060+11*i) for i in slots]; b1=[c.rd16(0x60CE+11*i) for i in slots]
    c.press(gk.A,3,100); c.run(900)
    a0=[c.rd16(0x6060+11*i) for i in slots]; a1=[c.rd16(0x60CE+11*i) for i in slots]
    res.append((b0,a0,b1,a1))
ok=0;info=[]
for b0,a0,b1,a1 in res:
    # loser = side whose units shrank
    for (bl,al,bw,aw,name) in ((b0,a0,b1,a1,'we lost'),(b1,a1,b0,a0,'they lost')):
        if all(x<=y for x,y in zip(al,bl)) and sum(al)<sum(bl) and all(x>=y for x,y in zip(aw,bw)):
            T=sum(y-x for x,y in zip(al,bl)); n=len([x for x in bw if x>0])
            pct=[ (y-x)*100/y for x,y in zip(al,bl)]
            gain=set(x-y for x,y in zip(aw,bw))
            good=all(any(y-(y*(20+r)//100)==x for r in range(10)) for x,y in zip(al,bl)) and gain=={T//n}
            ok+=good; info.append((name,good,T,n,gain,[round(p) for p in pct]))
print(ok,'matching duel outcomes of',len(res),'trials'); other=[(sum(b0),sum(a0),sum(b1),sum(a1)) for b0,a0,b1,a1 in res]; print('totals before/after (ours,theirs):',other)
for i in info[:24]: print(i)
