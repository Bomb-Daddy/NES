import sys, os; sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from wartools import *
from hexverify import nb
import itertools
c=gk.Core(); nav.loadstate(c,'state_s5.bin'); c.run(30); st=c.save()
K={'D':gk.DOWN,'U':gk.UP,'L':gk.LEFT,'R':gk.RIGHT,'A':gk.A}
res={}
for n in range(0,5):
    for mid in itertools.product('LR',repeat=n):
        c.load(st); c.run(2); c.wr(0x6062,5); c.wr(0x6063,5)
        for r in range(10):
            for q in range(16): c.wr(0x615C+r*16+q,2)
        for k in 'A'+''.join(mid)+'A': c.press(K[k],3,50)
        u=units(c)[0]; got=(u[2],u[3])
        dirs=[d for d in range(6) if nb(5,5,d)==got]
        res[''.join(mid)]=dirs[0] if dirs else None
print(res)
