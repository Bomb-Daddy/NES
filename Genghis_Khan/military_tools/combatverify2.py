import sys, os; sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from combat import *
from wartools import *
import random
def trials(seed,n,wide,dslot):
    c=gk.Core(); random.seed(seed)
    nav.loadstate(c,'state_preatk.bin'); c.run(2); st=c.save()
    att=0x60AD; dfn=0x60CE+11*dslot
    ok=0;bad=[]
    for trial in range(n):
        c.load(st); c.run(2)
        if dslot!=0:
            c.wr(0x60CE+2,255); c.wr(0x60CE+3,255); c.wr(dfn+2,11); c.wr(dfn+3,1)
        sa=random.randrange(1,999 if wide else 300); sd=random.randrange(1,999 if wide else 300)
        c.wr(att,sa&255); c.wr(att+1,sa>>8); c.wr(dfn,sd&255); c.wr(dfn+1,sd>>8)
        c.wr(att+4,random.randrange(3)); c.wr(dfn+4,random.randrange(3))
        c.wr(0x615C+1*16+11,random.randrange(6))
        c.wr(0x6003,random.choice([100,101,102,7])); c.wr(0x601E,random.choice([100,101,102,7]))
        for a in (0x6037,0x6039,0x603B,0x603D):
            v=random.randrange(1000); c.wr(a,v&255); c.wr(a+1,v>>8)
        for base in (c.rd16(0x6001),c.rd16(0x601C)):
            for i in range(6):
                v=random.randrange(0,300); c.wr(base+14+2*i,v&255); c.wr(base+15+2*i,v>>8)
        set_state(c,random.getrandbits(48))
        S=get_state(c); pred=predict(c,att,dfn,S)
        a0,d0=c.rd16(att),c.rd16(dfn)
        c.press(gk.A,3,60); c.run(600)
        got=(a0-c.rd16(att),d0-c.rd16(dfn)); exp=(min(a0,pred[1]),min(d0,pred[2]))
        if got==exp: ok+=1
        else: bad.append((trial,sa,sd,pred,got))
    return ok,n,bad[:5]
print('pass2 wide ranges, defender slot 0:',trials(99,40,True,0))
print('pass3 defender slot 4 (infantry unit) moved to the target hex:',trials(123,30,False,4))
