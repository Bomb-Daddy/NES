import st
s2=open('/home/claude/cap/p72/state_000700.b64').read()
steps=[]
for i in range(7):
    steps.append('a')
    if i<6: steps.append('down')
steps+=['right','a']
st.run_ad(steps,'p75',s2,stop=9000)
