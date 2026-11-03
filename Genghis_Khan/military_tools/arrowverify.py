import sys, os; sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from combat import *
from wartools import *
import random
def arrow_pred(c,att,dfn,S):
    s=side_of(att); L9=min(c.rd16(0x6037+2*s)//10+1,30)
    S1,o=rng_next(S); r=o%16
    Y=(c.rd16(att)*L9//100)*(30+r)//100
    df=defense_factor(c,dfn)
    return max(0,Y*(18-df)//19) if 18-df>=0 else 0
c=gk.Core(); random.seed(int(sys.argv[1]) if len(sys.argv)>1 else 3)
nav.loadstate(c,'state_afterpass.bin'); c.run(5); st=c.save()
att=0x60AD; dfn=0x60CE; ok=0;n=0;bad=[]
for trial in range(30):
    c.load(st); c.run(2)
    d=2; c.wr(dfn+2,11); c.wr(dfn+3,d)
    sa=random.choice([100,300,600,900]); sd=random.choice([200,500,999])
    c.wr(att,sa&255); c.wr(att+1,sa>>8); c.wr(dfn,sd&255); c.wr(dfn+1,sd>>8)
    c.wr(dfn+4,random.randrange(3))
    for rr in range(10):
        for q in range(16):
            if (q,rr)!=(11,0): c.wr(0x615C+rr*16+q,2)
    c.wr(0x615C+d*16+11,random.randrange(6))
    sk=random.randrange(1000); c.wr(0x6037,sk&255); c.wr(0x6038,sk>>8)
    c.wr(0x601E,random.choice([100,101,102,5]))
    set_state(c,random.getrandbits(48)); S=get_state(c)
    pred=arrow_pred(c,att,dfn,S); d0=c.rd16(dfn); a0=c.rd(0x6001+9)
    c.press(gk.DOWN,3,40); c.press(gk.A,3,90)
    for k in 'RR': c.press(gk.RIGHT,3,40)
    c.press(gk.A,3,60); c.run(300)
    got=d0-c.rd16(dfn); n+=1
    if got==min(pred,d0): ok+=1
    else: bad.append((trial,d,sa,sd,pred,got,a0,c.rd(0x6001+9)))
print('arrow model',ok,'/',n,bad[:8])
