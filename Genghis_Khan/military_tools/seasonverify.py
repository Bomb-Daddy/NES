import gk, nav, random
c=gk.Core(); C0=0x6DCD
def w16(a,v): c.wr(a,v&255); c.wr(a+1,v>>8)
rd=lambda o: c.rd16(C0+o)
def passseason(target):
    nav.goto(c,13); c.press(gk.A,3,60)
    for i in range(80):
        c.run(100); c.press(gk.A,2,2)
        if c.rd16(0x65BD)==target and i>3:
            for k in range(12): c.run(100); c.press(gk.A,2,2)
            break
random.seed(2); out=[]
# Summer: skill*93%, Winter: arms*97%
for season,field,pct in ((0,0x16,93),(2,0x14,97)):
    for t in range(4):
        nav.loadstate(c,'state_orders.bin'); w16(0x65BD,season)
        v=random.randint(50,999); w16(C0+field,v)
        passseason((season+1)&3)
        got=rd(field); exp=v*pct//100
        out.append((season+1,hex(field),v,exp,got,exp==got)); print(out[-1])
# Spring upkeep: food -= pop/2, starvation scaling
for t in range(6):
    nav.loadstate(c,'state_orders.bin'); w16(0x65BD,3)
    F=random.randint(0,400); w16(C0+0x36,F)
    pop=rd(0x18)+rd(0x1C)+rd(0x20)+rd(0x3A)+rd(0x3E)
    T0=rd(0x18)
    passseason(0)
    print('spring food',F,'pop',pop,'troops',T0,'->',rd(0x18),'food after',rd(0x36),'pred(no starve)',max(0,F-pop//2) if pop<=2*F else 'starve')
