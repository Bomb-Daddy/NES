import gk, nav, random
from traintest import set_state
c=gk.Core()
def w16(a,v): c.wr(a,v&255); c.wr(a+1,v>>8)
def army(s):
    return sum(c.rd16(0x6060+110*s+11*k) for k in range(10) if c.rd(0x6060+110*s+11*k+9)==0)
def gainset(size,level):
    out=set()
    for r1 in range(19):
        x=size*(2+r1)//100+1
        for r2 in range(5): out.add(x*(101-level*20//3+r2)//100)
    return out
random.seed(5); ok=n=0
for t in range(10):
    nav.loadstate(c,'state_day2_u0.bin')
    c.wr(0x6062,2); c.wr(0x6063,0)
    for k in range(1,10): c.wr(0x6060+11*k+9,200)
    S=random.randint(50,2000); w16(0x6060,S); w16(0x6007,5000)
    lvl=c.rd16(0x7A88); set_state(c,random.getrandbits(48))
    for i in range(3): c.press(gk.DOWN,3,12)
    c.press(gk.A,3,80)
    for i in range(3): c.press(gk.DOWN,3,25)
    c.press(gk.A,3,90)
    h0=c.rd(0x6060+8)
    gains=[]; day=c.rd16(0x6045); food=c.rd16(0x6007); T=army(0); carry=c.rd(0x6001+8)
    for f in range(30000):
        c.run(1)
        if c.rd16(0x63D4): break
        if c.rd16(0x6045)!=day:
            nf=c.rd16(0x6007); use=T//30+(1 if T%30+carry>=30 else 0)
            gains.append(nf-food+use)
            day=c.rd16(0x6045); food=nf; T=army(0); carry=c.rd(0x6001+8)
            if len(gains)==3: break
        if f%300==299: c.press(gk.A,2,2)
    gs=gainset(S,lvl); pos=[g for g in gains if g>0]
    good = len(pos)==1 and pos[0] in gs
    n+=1; ok+=good
    print(t,'idx',sorted(gs).index(pos[0]) if pos and pos[0] in gs else None,'size',S,'lvl',lvl,'hunt byte',h0,'daily extra food',gains,'possible',min(gs),'-',max(gs),'OK' if good else 'CHECK')
print(ok,'/',n)
