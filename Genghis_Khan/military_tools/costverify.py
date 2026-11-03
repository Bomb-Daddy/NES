import sys, os; sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from wartools import *
c=gk.Core(); nav.loadstate(c,'state_s5.bin'); c.run(30); st=c.save()
K={'D':gk.DOWN,'U':gk.UP,'L':gk.LEFT,'R':gk.RIGHT,'A':gk.A}
COST=[8,6,3,4,5,4,5,255,7,5,255,2,5,255,255,5]
rows=[]
for code in range(16):
    c.load(st); c.run(2); c.wr(0x6062,5); c.wr(0x6063,5)
    for r in range(10):
        for q in range(16): c.wr(0x615C+r*16+q,2)
    c.wr(0x615C+6*16+5,code)           # cell (5,6) = dir1 (down) from (5,5)
    for k in 'ALA': c.press(K[k],3,50)
    u=units(c)[0]; moved=(u[2],u[3])==(5,6); m=u[5]
    rows.append((code,COST[code],moved,10-m if moved else None))
print(rows)
print('match:',all((cst<=10)==mv and (not mv or spent==cst) for _,cst,mv,spent in rows))
