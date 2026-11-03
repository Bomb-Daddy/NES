import gk, nav, random
c=gk.Core(); C0=0x6DCD
def w16(a,v): c.wr(a,v&255); c.wr(a+1,v>>8)
rd=lambda o: c.rd16(C0+o)
F=[0x18,0x1C,0x20,0x3A,0x3E]
random.seed(13); ok=n=0
for t in range(12):
    nav.loadstate(c,'state_orders.bin'); w16(0x65BD,3)
    v=[random.randint(0,180) for _ in F]
    for o,x in zip(F,v): w16(C0+o,x)
    pool=random.randint(0,900); w16(C0+0x49,pool)
    w16(C0+0x36,999); w16(C0+0x40,999)   # avoid starvation and unpaid troops
    P=min(999,sum(v))
    # expected
    e=list(v)
    if P<20: e[0]+=1
    else:
        L=min(999,P*103//100)-P; b=L//5; r=L%5
        e=[x+b for x in e]
        inc={1:[0],2:[2,0],3:[1,2,0],4:[3,1,2,0]}.get(r,[])
        for i in inc: e[i]+=1
    epool=min(999,pool+pool*5//100+1)
    nav.goto(c,13); c.press(gk.A,3,60)
    for i in range(80):
        c.run(100); c.press(gk.A,2,2)
        if c.rd16(0x65BD)==0 and i>3:
            for k in range(12): c.run(100); c.press(gk.A,2,2)
            break
    got=[rd(o) for o in F]
    # spring also pays troops from gold: troops unaffected while gold>=troops
    good=(got==e and rd(0x49)==epool); n+=1; ok+=good
    print(t,'P',P,'exp',e,epool,'got',got,rd(0x49),'OK' if good else 'FAIL')
print(ok,'/',n)
