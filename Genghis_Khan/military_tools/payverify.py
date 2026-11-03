import gk, nav, random
c=gk.Core(); C0=0x6DCD
def w16(a,v): c.wr(a,v&255); c.wr(a+1,v>>8)
rd=lambda o: c.rd16(C0+o)
def spring():
    nav.goto(c,13); c.press(gk.A,3,60)
    for i in range(80):
        c.run(100); c.press(gk.A,2,2)
        if c.rd16(0x65BD)==0 and i>3:
            for k in range(12): c.run(100); c.press(gk.A,2,2)
            return
random.seed(17); ok=n=0
for t in range(10):
    nav.loadstate(c,'state_orders.bin'); w16(0x65BD,3); w16(C0+0x36,999)
    G=random.randint(0,300); T=random.randint(0,300); w16(C0+0x40,G); w16(C0+0x18,T)
    E=rd(0x1A); tax=rd(0x44); M=rd(0x42)
    inc=(E*tax//100)*(min(13,M//9)+12)//25
    g2=G+inc
    T2=min(T,g2); G2=min(999,g2-T2)
    pop=T2+rd(0x1C)+rd(0x20)+rd(0x3A)+rd(0x3E)
    spring()
    gotT=rd(0x18); gotG=rd(0x40)
    # growth adds to troops afterwards: compute growth on pop
    P=min(999,pop); L=min(999,P*103//100)-P; add=L//5+(1 if L%5>=1 else 0) if P>=20 else 1
    good=(gotG==G2 and gotT==T2+add); n+=1; ok+=good
    print(t,'G',G,'T',T,'inc',inc,'exp T',T2+add,'G',G2,'got T',gotT,'G',gotG,'OK' if good else 'FAIL')
print(ok,'/',n)
