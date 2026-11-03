import gk, nav, random
from traintest import rng_next, set_state
c=gk.Core(); C0=0x6DCD
def w16(a,v): c.wr(a,v&255); c.wr(a+1,v>>8)
rd=lambda o: c.rd16(C0+o)
def enter(n):
    for i,ch in enumerate(str(n)):
        if i>0: c.press(gk.RIGHT,3,15)
        for k in range(int(ch)): c.press(gk.UP,3,12)
    c.press(gk.A,3,60)
def DB48(X,P,w,N): return 0 if P==0 and N==0 else min(999,(X*P+w*N)//(P+N))
random.seed(9); ok=n=0
for t in range(20):
    nav.loadstate(c,'state_orders.bin')
    G=random.randint(10,900); A=random.randint(0,300); S=random.randint(0,300); M=random.randint(0,200); T=random.randint(0,300)
    rate=random.randint(0,200)
    w16(C0+0x40,G); w16(C0+0x14,A); w16(C0+0x16,S); w16(C0+0x42,M); w16(C0+0x18,T); w16(0x7A8A,rate)
    pop=rd(0x18)+rd(0x1C)+rd(0x20)+rd(0x3A)+rd(0x3E)
    price=60*(rate+100)//100
    mx=min(999-pop, G*10//price)
    if mx<1: continue
    N=random.randint(1,min(mx,99))
    R=random.getrandbits(48); set_state(c,R)
    nav.goto(c,5); c.press(gk.A,3,60); c.press(gk.DOWN,3,20); c.press(gk.DOWN,3,20); c.press(gk.A,3,80)
    c.press(gk.DOWN,3,20); c.press(gk.DOWN,3,20); c.press(gk.A,3,80)
    c.press(gk.A,3,40)  # dismiss rate message if any
    enter(N); c.run(60)
    S1,o=rng_next(R)
    exp=(DB48(A,pop,100,N),DB48(S,pop,100,N),DB48(M,pop,50+o%11,N),min(999,T+N),(G*10-price*N)//10)
    got=(rd(0x14),rd(0x16),rd(0x42),rd(0x18),rd(0x40))
    good=exp==got; n+=1; ok+=good
    print(t,'N',N,'max',mx,'price',price,'exp',exp,'got',got,'OK' if good else 'FAIL')
print(ok,'/',n)
