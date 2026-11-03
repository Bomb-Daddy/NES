import gk, nav, random
c=gk.Core(); lib=gk.lib; C0=0x6DCD
def w16(a,v): c.wr(a,v&255); c.wr(a+1,v>>8)
def snap(): return [(c.rd16(0x6DCD+83*i+0x18),c.rd16(0x6DCD+83*i+0x36),c.rd16(0x6DCD+83*i+0x40)) for i in range(14)]
nav.loadstate(c,'state_orders.bin'); w16(0x65BD,3); w16(C0+0x18,3)
lib.gk_vm_alloc(200000)
nav.goto(c,13); c.press(gk.A,3,60)
events=[]; prev=snap()
for f in range(20000):
    lib.gk_vm_reset(); lib.gk_vm(1); c.frame(gk.A if f%100==50 else 0); lib.gk_vm(0)
    n=lib.gk_vm_count(); buf=lib.gk_vmbuf()
    for i in range(n):
        r=buf[i]
        if r.bank==2 and r.vpc==0x85C3:  # just after entering $85BC body
            events.append(('enter85BC',f,prev))
        if r.bank==2 and r.vpc==0x86D1:
            events.append(('battle',f,snap(),c.rd16(0x6043)))
    prev=snap()
    if any(e[0]=='battle' for e in events): break
for e in events[:6]: print(e[0],e[1], e[3] if len(e)>3 else '')
b=[e for e in events if e[0]=='battle']
if b:
    before=[e for e in events if e[0]=='enter85BC'][-1][2]
    after=b[0][2]
    for i in range(14):
        if before[i]!=after[i]: print('country',i,'before',before[i],'after',after[i])
    print('days',b[0][3])
    for i,(T,F,G) in enumerate(before): print(i,T,F,G,'cap70',min(T,F,G)*70//100,'cap90',min(T,F,G)*90//100)
