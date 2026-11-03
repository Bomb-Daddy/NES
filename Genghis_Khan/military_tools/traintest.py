import sys, os; sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import gk, nav, random
K=0xE0900E0187C5
def rng_next(S):
    S|=1; P=(S*K)&((1<<48)-1); S=(-P)&((1<<48)-1); return S,(((S>>40)&0x7F)<<8)|((S>>32)&0xFF)
def get_state(c): return int.from_bytes(bytes(c.rd(0x84+i) for i in range(6)),'big')
def set_state(c,S):
    for i,b in enumerate(S.to_bytes(6,'big')): c.wr(0x84+i,b)
def predict(c, S):
    L=nav.W(c,0x65CD); Bt=nav.W(c,0x65D7); Ch=nav.W(c,0x65D3)
    pop=sum(nav.W(c,a) for a in (0x6DE5,0x6DE9,0x6DED,0x6E07,0x6E0B))
    lvl=c.rd(0x7A88)
    S2,r=rng_next(S)
    T=(L+Bt+Ch)//40 + r%11 + 15
    g=T*999//(999+pop)
    pct=100-4*min(lvl-1,5)
    g=g*pct//100
    return g
def run(c):
    nav.loadstate(c,'state_orders.bin')
def do_train(c):
    nav.goto(c,4); c.press(gk.A,3,60); c.press(gk.A,3,60); c.run(400)
if __name__=='__main__':
    c=gk.Core(); random.seed(5); bad=0;n=0
    for trial in range(24):
        run(c)
        if trial>0:
            set_state(c,random.getrandbits(48))
            c.wr(0x6DE5,random.choice([0,20,61,200,500,999])&255); c.wr(0x6DE6,random.choice([0,0,1,3]))
        S=get_state(c); pred=predict(c,S); s0=c.rd16(0x6DE3)
        do_train(c); s1=c.rd16(0x6DE3)
        n+=1; ok=(s1-s0==pred); bad+= (not ok)
        print(trial,'skill',s0,'->',s1,'gain',s1-s0,'pred',pred,'OK' if ok else 'MISMATCH', 'a8a=',c.rd(0x8A))
    print('mismatches',bad,'of',n)
