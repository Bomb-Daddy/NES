import sys, os; sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from combat import *
from wartools import *
import random
c=gk.Core(); random.seed(11)
st=None
nav.loadstate(c,'state_preatk.bin'); c.run(2); st=c.save()
att=0x60AD; dfn=0x60CE
ok=0;n=0;bad=[]
for trial in range(40):
    c.load(st); c.run(2)
    sa=random.choice([5,20,40,90,200,400]); sd=random.choice([5,30,126,250,500])
    c.wr(att,sa&255); c.wr(att+1,sa>>8); c.wr(dfn,sd&255); c.wr(dfn+1,sd>>8)
    c.wr(att+4,random.randrange(3)); c.wr(dfn+4,random.randrange(3))
    c.wr(0x615C+1*16+11,random.randrange(6))          # terrain under defender (11,1)
    c.wr(0x6003,random.choice([100,101,102,0])); c.wr(0x601E,random.choice([100,101,102,0]))
    for a in (0x6037,0x6039,0x603B,0x603D):
        v=random.randrange(1000); c.wr(a,v&255); c.wr(a+1,v>>8)
    for base in (c.rd16(0x6001),c.rd16(0x601C)):
        for i in range(6):
            v=random.randrange(40,260); c.wr(base+14+2*i,v&255); c.wr(base+15+2*i,v>>8)
    set_state(c,random.getrandbits(48))
    S=get_state(c); pred=predict(c,att,dfn,S)
    a0,d0=c.rd16(att),c.rd16(dfn)
    c.press(gk.A,3,60); c.run(600)
    a1,d1=c.rd16(att),c.rd16(dfn)
    got=(a0-a1,d0-d1); exp=(min(a0,pred[1]),min(d0,pred[2]))
    n+=1
    if got==exp: ok+=1
    else: bad.append((trial,sa,sd,pred,got))
print('combat model',ok,'/',n); print(bad[:10])
