import sys, os; sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from cmdtrace import *
def enter(c,n):
    ds=str(n)
    for i,d in enumerate(ds):
        for _ in range(int(d)): c.press(gk.UP,3,12)
        if i<len(ds)-1: c.press(gk.RIGHT,3,12)
def war_setup(c,n,food=None,state='state_warsold.bin'):
    nav.loadstate(c,state); c.run(60)
    enter(c,n); c.press(gk.A,3,200)
    enter(c,food if food is not None else n); c.press(gk.A,3,250)
    c.press(gk.A,3,60)     # commander: self
    c.run(120); c.press(gk.A,3,60)   # yes
    c.run(900)
def units(c,base=0x6060,n=10):
    w=bytes(c.S.wram); out=[]
    for i in range(n):
        o=base-0x6000+11*i; out.append(tuple(w[o:o+11]))
    return out

def cur_xy(c):
    """read cursor cell via a traced A press (harmless unless cell is a free deployment cell)"""
    for _ in range(6):
        c.run(40); vm_on(); c.run(4,gk.A); c.run(40); t=vm_off()
        y=[r[3] for r in t if r[1]-1==0xECD2]; x=[r[3] for r in t if r[1]-1==0xECDE]
        if x and y: return (x[0],y[0])
    return None
