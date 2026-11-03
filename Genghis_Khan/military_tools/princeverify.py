import gk, nav, random, numpy as np
from traintest import rng_next, set_state
c=gk.Core()
def w16(a,v): c.wr(a,v&255); c.wr(a+1,v>>8)
R0=0x65CD
random.seed(8); ok=n=0
for t in range(12):
    nav.loadstate(c,'state_orders.bin')
    st=[random.randint(20,400) for _ in range(6)]
    for i,v in enumerate(st): w16(R0+2*i,v)
    lvl=random.randint(1,5); w16(0x7A88,lvl); k=t%6
    S=random.getrandbits(48); set_state(c,S)
    nav.goto(c,4); c.press(gk.A,3,60); c.press(gk.DOWN,3,20); c.press(gk.A,3,200)
    c.press(gk.A,3,200)
    w0=c.wram().copy()
    for i in range(k): c.press(gk.DOWN,3,25)
    c.press(gk.A,3,100); c.run(60)
    w1=c.wram().copy()
    d=[i for i in np.nonzero(w0!=w1)[0] if 0x0500<=i<0x2000]
    S1,o=rng_next(S)
    base=(st[0]+st[5]+st[3])//40 + o%11 + 15
    g=base*(100-4*min(lvl-1,5))//100
    words={}
    for i in d:
        a=i&~1 if False else i
    ch=[]
    for i in sorted(set(i - (i%2) for i in d)):
        ch.append((hex(0x6000+i), int(w0[i])|int(w0[i+1])<<8, int(w1[i])|int(w1[i+1])<<8))
    deltas=[b-a for _,a,b in ch]
    good= g in deltas or any(b==999 for _,a,b in ch)
    n+=1; ok+=good
    print(t,'k',k,'lvl',lvl,'pred gain',g,'changed',ch,'OK' if good else 'CHECK')
print(ok,'/',n)
