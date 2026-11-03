from common import *
import sys
which=sys.argv[1]
ev=BASE+PRE8[:]
s=21800
if which=='attack':
    ev+= [(s,'down',8),(s+300,'a',8),(s+900,'a',8),(s+1500,'a',8),(s+2100,'a',8)]
    run(ev, list(range(s,s+3000,70)), s+3000, 'p36attack')
elif which=='wait':
    for i in range(5): ev.append((s+i*200,'down',8))
    ev.append((s+1300,'a',8))
    run(ev, list(range(s,s+3500,100)), s+3500, 'p36wait')
