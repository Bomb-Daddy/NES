import gk, nav, sys
c=gk.Core(); lib=gk.lib
nav.loadstate(c,'state_orders.bin')
if len(sys.argv)>1: c.wr(0x65BD,3); c.wr(0x6DCD+0x18,int(sys.argv[1])); c.wr(0x6DCD+0x19,0)
lib.gk_vm_alloc(400000)
calls=[]; cur=None
nav.goto(c,13); c.press(gk.A,3,60)
for f in range(60000):
    lib.gk_vm_reset(); lib.gk_vm(1); c.frame(gk.A if f%100==50 else 0); lib.gk_vm(0)
    n=lib.gk_vm_count(); buf=lib.gk_vmbuf()
    for i in range(n):
        r=buf[i]
        if r.bank!=2: continue
        v=r.vpc
        if v==0x83D9: 
            if cur: calls.append(cur)
            cur={}
        elif cur is None: continue
        elif v==0x83E4: cur['avg_lt50']=r.acc          # result of ULT
        elif v==0x83F3: cur['target']=r.acc
        elif v==0x8402: cur['A']=r.acc
        elif v==0x8408: cur['D']=r.acc
        elif v==0x8413: cur['r']=r.acc
        elif v==0x8450: cur['x']=r.acc     # after $8340 returns
        elif v==0x845D: cur['tplayer']=r.acc
        elif v==0x846A: cur['rand']=r.acc
        elif v==0x847D: cur['attack']=1
        elif v==0x8480+1 or v==0x83EC+1 or v==0x83FB+1 or v==0x8432+1: pass
    if f>=59000: break
if cur: calls.append(cur)
ok=n2=0
for k in calls:
    if 'r' not in k: continue
    r=k['r']; x=k.get('x',0) if (110<=r<170) else 0
    if 50<=r<110: pred=False
    else:
        if r>=125 or k.get('tplayer'):
            pred = (x==0) or (k.get('rand',100)<20)
        else: pred=False
    got=bool(k.get('attack'))
    n2+=1; ok+= pred==got
    print('r',r,'A',k.get('A'),'D',k.get('D'),'x',k.get('x'),'tplayer',k.get('tplayer'),'rand',k.get('rand'),'pred',pred,'attack',got,'' if pred==got else 'MISMATCH')
print(ok,'/',n2)
