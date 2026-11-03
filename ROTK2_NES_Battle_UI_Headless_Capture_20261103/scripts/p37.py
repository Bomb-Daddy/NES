from common import *
import sys
which=sys.argv[1]
ev=BASE+PRE8[:]
s=21800
if which=='move':
    ev+=[(s,'a',8),(s+300,'a',8),(s+800,'right',8),(s+1100,'right',8),(s+1400,'a',8),(s+2000,'right',8),(s+2300,'a',8)]
    run(ev, list(range(s,s+3600,60)), s+3600, 'p37move')
elif which=='flee':
    ev+=[(s,'down',8),(s+200,'down',8),(s+500,'a',8),(s+1100,'a',8),(s+1700,'left',8),(s+1900,'a',8)]
    run(ev, list(range(s,s+3600,60)), s+3600, 'p37flee')
elif which=='longwait':
    t=s
    for i in range(24):
        for k in range(5): ev.append((t+k*150,'down',8))
        ev.append((t+900,'a',8)); t+=2200
    run(ev, list(range(s,t,150)), t, 'p37longwait')
if which=='dirs':
    ev+=[(s,'a',8),(s+300,'a',8)]
    t=s+900
    for d in ['down','down','up','left','left','right','right','right']:
        ev.append((t,d,8)); t+=450
    run(ev, list(range(s,t+300,45)), t+300, 'p37dirs')
if which=='wait40':
    t=s
    for i in range(40):
        for k in range(5): ev.append((t+k*150,'down',8))
        ev.append((t+900,'a',8)); t+=2200
    run(ev, list(range(s,t,150)), t, 'p37wait40')
if which=='dirs2':
    ev+=[(s,'a',8),(s+300,'a',8)]
    t=s+900
    for d in ['down','up','left','right']:
        ev+=[(t,d,8),(t+350,'a',8)]; t+=900
    run(ev, list(range(s,t+300,45)), t+300, 'p37dirs2')
if which=='movenemy':
    ev+=[(s,'a',8),(s+300,'down',8),(s+600,'a',8)]
    t=s+1200
    for d in ['a','a','right','a']:
        ev+=[(t,d,8)]; t+=500
    run(ev, list(range(s,t+300,45)), t+300, 'p37movenemy')
if which in ('adv1','adv2'):
    pats={'adv1':['right','right'],'adv2':['right','down']}[which]
    t=s
    for i in range(14):
        ev+=[(t,'a',8),(t+300,'a',8)]
        u=t+800
        for d in pats:
            ev+=[(u,d,8),(u+350,'a',8)]; u+=800
        ev+=[(u+300,'b',8)]
        for k in range(5): ev.append((u+800+k*150,'down',8))
        ev.append((u+1700,'a',8)); t=u+3600
    run(ev, list(range(s,t,120)), t, 'p37'+which)
