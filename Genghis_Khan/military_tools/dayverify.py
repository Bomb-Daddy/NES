import gk, nav, random
c=gk.Core()
def army(s):
    t=0
    for k in range(10):
        p=0x6060+110*s+11*k
        if c.rd(p+9)==0: t+=c.rd16(p)
    return t
def w16(a,v): c.wr(a,v&255); c.wr(a+1,v>>8)
ok=0; n=0
random.seed(7)
for trial in range(16):
    nav.loadstate(c,'state_afterpass.bin'); c.press(gk.B,3,30)
    for s in (0,1):
        for k in range(10):
            p=0x6060+110*s+11*k
            if c.rd(p+9)==0 and c.rd16(p)>0 and k!=0 or (k==0 and c.rd16(p)>0): w16(p,random.randint(30,900))
        w16(0x6007+27*s,random.randint(400,3000)); c.wr(0x6001+27*s+8,random.randint(0,29))
    f0=[c.rd16(0x6007+27*s) for s in (0,1)]; b0=[c.rd(0x6001+27*s+8) for s in (0,1)]
    c.press(gk.DOWN,3,15);c.press(gk.DOWN,3,15);c.press(gk.DOWN,3,15); c.press(gk.A,3,10)
    res=None
    for f in range(4000):
        c.run(1)
        if c.rd16(0x6045)!=1 or c.rd16(0x63D4): res=True; break
        if f%300==299: c.press(gk.A,2,2)
    if not res or c.rd16(0x63D4): print('trial',trial,'ended/over',c.rd16(0x63D4)); continue
    T=[army(0),army(1)]; exp=[]
    for s in (0,1):
        use=T[s]//30; L9=T[s]%30+b0[s]
        if L9>=30: use+=1; L9-=30
        exp.append((max(0,f0[s]-use),L9))
    got=[(c.rd16(0x6007+27*s),c.rd(0x6001+27*s+8)) for s in (0,1)]
    n+=1; good = exp==got; ok+=good
    print(trial,'T',T,'food0',f0,'b8',b0,'exp',exp,'got',got,'OK' if good else 'FAIL')
print(ok,'/',n)
