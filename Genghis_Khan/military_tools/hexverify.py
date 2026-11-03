import sys, os; sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from wartools import *
def nb(x,y,d):
    if d==0:
        if x==0: return None
        x-=1
        if x%2==0:
            if y==9: return None
            y+=1
    elif d==1:
        if y==9: return None
        y+=1
    elif d==2:
        if x==15: return None
        x+=1
        if x%2==0:
            if y==9: return None
            y+=1
    elif d==3:
        if x==0: return None
        x-=1
        if x%2==1:
            if y==0: return None
            y-=1
    elif d==4:
        if y==0: return None
        y-=1
    elif d==5:
        if x==15: return None
        x+=1
        if x%2==1:
            if y==0: return None
            y-=1
    return (x,y)
RING={0:'LR',1:'L',2:'LL',5:'LLL',3:'R',4:'RR'}
if __name__=='__main__':
    c=gk.Core(); nav.loadstate(c,'state_s5.bin'); c.run(30); st=c.save()
    K={'D':gk.DOWN,'U':gk.UP,'L':gk.LEFT,'R':gk.RIGHT,'A':gk.A}
    tot=ok=0; bad=[]
    for (sx,sy) in [(5,5),(6,5),(4,3),(7,7),(0,0),(15,9),(0,9),(15,0),(1,4),(14,6)]:
        for d in range(6):
            c.load(st); c.run(2)
            c.wr(0x6062,sx); c.wr(0x6063,sy)
            for r in range(10):
                for q in range(16): c.wr(0x615C+r*16+q,2)
            seq='A'+RING[d]+'A'
            for k in seq: c.press(K[k],3,60)
            u=units(c)[0]; got=(u[2],u[3]); exp=nb(sx,sy,d) or (sx,sy)
            tot+=1
            if got==exp: ok+=1
            else: bad.append(((sx,sy),d,got,exp))
    print('hex-step check',ok,'/',tot,bad)
