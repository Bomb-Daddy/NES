import st
s=open('/home/claude/cap/p92/state_000534.b64').read()
st.run_ad(['up','a','x','down','a','x','left','a','x'],'p93',s,stop=20000,maxwait=60,savesteps=True)
