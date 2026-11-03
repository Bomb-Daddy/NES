import sys, os; sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import gk, nav
from traintest import rng_next, get_state, set_state
lib=gk.lib
def tbl(c): 
    return ([lib.gk_peek_bank(5,0xBBCE+i) for i in range(24)],[lib.gk_peek_bank(5,0xBBE0+i) for i in range(9)])
def side_of(ptr): return 0 if ptr<0x60CE else 1
def stance_code(c,side):
    b=c.rd(0x6001+27*side+2); return 2 if b==100 else (1 if b==101 else 0)
def strength(c,ptr):
    s=side_of(ptr); size=c.rd16(ptr)
    sk=c.rd16(0x6037+2*s)//10+1; ar=c.rd16(0x603B+2*s)//5+1
    base=(size*(100+sk)//100 + size*(100+ar)//100)>>1
    if c.rd(0x6001+27*s+2)==102: f=1090
    else:
        p=c.rd16(0x6001+27*s)+14; W=lambda i:c.rd16(p+2*i)
        f=W(5)+(W(0)>>1)+(W(4)>>2)+(W(1)>>1)+1000
    return (base+size*f//1000)>>1
def terrain(c,ptr): return c.rd(0x615C+c.rd(ptr+3)*16+c.rd(ptr+2))&0x7F
def defense_factor(c,ptr):
    t=terrain(c,ptr); st=stance_code(c,side_of(ptr)); T,_=tbl(c)
    return T[c.rd(ptr+4)*6+t]+st
def casualties(size,p,r):
    L10=p*p; L11=100-p; L9=L11*L11
    L8=size*(87+r)//100
    L7=L8*L9//(L10+L9)
    return min(size,(L7>>1)+L11//p+1)
def predict(c,att,dfn,S):
    _,M=tbl(c)
    k=M[c.rd(att+4)*3+c.rd(dfn+4)]+stance_code(c,side_of(att))
    A=strength(c,att)*k; D=strength(c,dfn)*defense_factor(c,dfn)
    p=100*A//(A+D) if A+D else 50
    p=max(1,min(99,p))
    S1,o1=rng_next(S); S2,o2=rng_next(S1)
    ca=casualties(c.rd16(att),p,o1%24); cd=casualties(c.rd16(dfn),100-p,o2%24)
    return p,ca,cd
