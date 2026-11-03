import st
s=open('/home/claude/cap/p66/state_006900.b64').read()
ev=[(60,'right',6),(260,'a',6),(900,'b',6),(1200,'right',6),(1500,'a',6),(2000,'down',6),(2150,'down',6),(2300,'down',6),(2500,'a',6)]
st.run(ev, [1900,2450,2800], [2800], 2810, 'p70', state=s)
