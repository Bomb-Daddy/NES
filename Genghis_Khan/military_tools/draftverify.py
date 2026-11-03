import gk, nav, random
from traintest import rng_next, set_state
c=gk.Core(); C0=0x6DCD
def w16(a,v): c.wr(a,v&255); c.wr(a+1,v>>8)
rd=lambda o: c.rd16(C0+o)
def enter(n):
    s=str(n)
    for i,ch in enumerate(s):
        if i>0: c.press(gk.RIGHT,3,15)
        for k in range(int(ch)): c.press(gk.UP,3,12)
    c.press(gk.A,3,60)
def DB48(X,P,w,N): return 0 if P==0 and N==0 else min(999,(X*P+w*N)//(P+N))
random.seed(3); ok=n=0
for t in range(20):
    nav.loadstate(c,'state_orders.bin')
    prop=random.randint(5,50); pool=random.randint(20,400)
    A=random.randint(0,300); S=random.randint(0,300); M=random.randint(0,200); T=random.randint(0,300)
    c.wr(C0+0x48,prop); w16(C0+0x49,pool); w16(C0+0x14,A); w16(C0+0x16,S); w16(C0+0x42,M); w16(C0+0x18,T)
    pop=rd(0x18)+rd(0x1C)+rd(0x20)+rd(0x3A)+rd(0x3E)
    mx=min(pool*prop//100, 999-pop)
    if mx<1: continue
    N=random.randint(1,min(mx,99))
    R=random.getrandbits(48); set_state(c,R)
    nav.goto(c,6); c.press(gk.A,3,60)
    for i in range(5): c.press(gk.DOWN,3,20)
    c.press(gk.A,3,80); enter(N); c.run(60)
    S1,o=rng_next(R)
    newprop=max(0,(mx-N)*100//(pool-N) - o%3) if pool-N>0 else None
    exp=(DB48(A,pop,1,N),DB48(S,pop,1,N),DB48(M,pop,1,N),min(999,T+N),pool-N,newprop)
    got=(rd(0x14),rd(0x16),rd(0x42),rd(0x18),rd(0x49),c.rd(C0+0x48))
    good=exp==got; n+=1; ok+=good
    print(t,'N',N,'max',mx,'pop',pop,'exp',exp,'got',got,'OK' if good else 'FAIL')
print(ok,'/',n)
