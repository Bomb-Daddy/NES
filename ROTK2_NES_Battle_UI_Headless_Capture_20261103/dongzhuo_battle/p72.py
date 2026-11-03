import st
s=open('/home/claude/cap/p70/state_002800.b64').read()
ev=[(60,'right',12),(200,'right',12),(340,'a',6)]
st.run(ev, [700], [700], 710, 'p72', state=s)
s2=open('/home/claude/cap/p72/state_000700.b64').read()
ev=[(60,'a',6),(250,'down',6),(450,'a',6),(650,'down',6),(850,'a',6)]
st.run(ev, [40,200,400,600,800,1100], [], 1120, 'p73', state=s2)
