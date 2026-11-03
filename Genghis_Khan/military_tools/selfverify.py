import gk, nav, random
from traintest import rng_next, set_state
c=gk.Core()
def w16(a,v): c.wr(a,v&255); c.wr(a+1,v>>8)
R0=0x65CD
random.seed(4); ok=n=0
for t in range(18):
    nav.loadstate(c,'state_orders.bin')
    st=[random.randint(20,400) for _ in range(6)]
    for i,v in enumerate(st): w16(R0+2*i,v)
    lvl=random.randint(1,5); w16(0x7A88,lvl)
    k=t%6
    S=random.getrandbits(48); set_state(c,S)
    nav.goto(c,4); c.press(gk.A,3,60)
    c.press(gk.DOWN,3,20); c.press(gk.DOWN,3,20); c.press(gk.A,3,200)
    for i in range(k): c.press(gk.DOWN,3,25)
    c.press(gk.A,3,100); c.run(60)
    S1,o=rng_next(S)
    base=(st[1]+st[2]+st[4])//40 + o%11 + 15
    g=base*(100-4*min(lvl-1,5))//100
    exp=list(st); exp[k]=min(999,st[k]+g)
    got=[c.rd16(R0+2*i) for i in range(6)]
    good=exp==got; n+=1; ok+=good
    print(t,'k',k,'lvl',lvl,'gain',g,'exp',exp,'got',got,'OK' if good else 'FAIL')
print(ok,'/',n)
