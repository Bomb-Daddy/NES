import st
for tag,sf in (('p94a','/home/claude/cap/p92/state_000534.b64'),('p94b','/home/claude/cap/p93/state_000453.b64')):
    s=open(sf).read()
    st.run([], [], [], 30, tag, state=s, peeks=[(20,0x7000,0x1000),(21,0x6000,0x1000)])
