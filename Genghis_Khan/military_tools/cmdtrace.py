import sys, os; sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import gk, nav, vmdis as v, pickle
lib=gk.lib
lib.gk_vm_alloc(4000000)
def vm_on():
    lib.gk_vm_reset(); lib.gk_vm(1)
def vm_off():
    lib.gk_vm(0)
    n=lib.gk_vm_count(); b=lib.gk_vmbuf()
    return [(b[i].op,b[i].vpc,b[i].bank,b[i].acc,b[i].fp,b[i].gp) for i in range(n)]
def show(t,a,b,out=sys.stdout):
    for i in range(a,min(b,len(t))):
        op,vpc,bank,acc,fp,gp=t[i]
        out.write('%d acc=%5d %04X %s\n'%(i,acc,vpc-1,v.fmt(bank,vpc,op)))
