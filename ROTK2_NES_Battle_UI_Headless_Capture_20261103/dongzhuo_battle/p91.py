import st
s=open('/home/claude/cap/p90/state_002838.b64').read()
steps=(['down']*5+['a','x','x'])*10
st.run_ad(steps,'p91',s,stop=40000,maxwait=60,savesteps=False)
