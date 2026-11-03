import gk, nav, random
c=gk.Core(); C0=0x6DCD
def w16(a,v): c.wr(a,v&255); c.wr(a+1,v>>8)
rd=lambda o: c.rd16(C0+o)
def enter(n):
    for i,ch in enumerate(str(n)):
        if i>0: c.press(gk.RIGHT,3,15)
        for k in range(int(ch)): c.press(gk.UP,3,12)
    c.press(gk.A,3,80)
random.seed(21); ok=n=0
for t in range(18):
    nav.loadstate(c,'state_orders.bin')
    for i in range(11,17): w16(0x7ACE+0x22*2+2*i,1)
    r=t%6; i=11+r
    G=random.randint(50,900); A=random.randint(0,900); T=random.randint(1,400)
    w16(C0+0x40,G); w16(C0+0x14,A); w16(C0+0x18,T)
    pop=rd(0x18)+rd(0x1C)+rd(0x20)+rd(0x3A)+rd(0x3E)
    K=c.rd(0x7DE5+i); price=c.rd(0x7DE5+34+i)*(c.rd16(0x7A8A)+100)//100
    mx=min(999, pop*(999-A)//K, G*10//price)
    if mx<1: continue
    N=random.randint(1,min(mx,99))
    nav.goto(c,5); c.press(gk.A,3,60); c.press(gk.DOWN,3,20); c.press(gk.DOWN,3,20); c.press(gk.A,3,80)
    c.press(gk.DOWN,3,20); c.press(gk.A,3,200)
    c.press(gk.RIGHT,3,20); c.press(gk.RIGHT,3,20)
    for k in range(r): c.press(gk.DOWN,3,20)
    c.press(gk.A,3,100); enter(N); c.run(60)
    exp=(min(999,A+9*K*(N+1)//pop),(G*10-price*N)//10)
    got=(rd(0x14),rd(0x40))
    good=exp==got; n+=1; ok+=good
    print(t,'item',i,'K',K,'price',price,'N',N,'max',mx,'pop',pop,'exp',exp,'got',got,'OK' if good else 'FAIL')
print(ok,'/',n)
