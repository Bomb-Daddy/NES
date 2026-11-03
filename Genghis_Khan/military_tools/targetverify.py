import gk, nav
c=gk.Core(); lib=gk.lib
nav.loadstate(c,'state_orders.bin')
import sys
if len(sys.argv)>1:
    c.wr(0x65BD,3); c.wr(0x6DCD+0x18,int(sys.argv[1])); c.wr(0x6DCD+0x19,0)

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
        if v==0x8C84: cur={'cands':[]}            # entry (L10=0)
        elif cur is None: continue
        elif v==0x8C9C: cur['cands'].append({'p':r.acc})
        elif v==0x8CA6: cur['cands'][-1]['elig']=1     # passed JZ
        elif v==0x8CB1: cur['cands'][-1]['r5']=r.acc
        elif v==0x8CB9: cur['cands'][-1]['player']=r.acc
        elif v==0x8CC1: cur['cands'][-1]['de0c']=r.acc
        elif v==0x8CCB: cur['cands'][-1]['dn']=r.acc
        elif v==0x8CD1: cur['cands'][-1]['db']=r.acc
        elif v==0x8CC5 or v==0x8CE9:
            cur['ret']=r.acc; calls.append(cur); cur=None
    if len(calls)>=40 or c.rd16(0x65BB)>c.rd16(0x65BB)+5: break
    if f>=59000: break
# check logic
ok=0
for k in calls:
    best=0; ret=None
    for cd in k['cands']:
        if not cd.get('elig'): continue
        if best==0: best=cd['p']
        if cd.get('r5')==0 and cd.get('player') and not cd.get('de0c'): ret=cd['p']; break
        if cd.get('dn') is not None and cd['db']>cd['dn']: best=cd['p']
    if ret is None: ret=best
    good=ret==k['ret']; ok+=good
    if not good: print('MISMATCH',k)
print(len(calls),'calls; logic reproduced',ok)
print('with eligible:',sum(1 for k in calls if any(cd.get('elig') for cd in k['cands'])))
for k in calls:
    if any(cd.get('elig') for cd in k['cands']): print(k)
