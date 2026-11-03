import st
s=open('/home/claude/cap/p61/state_005850.b64').read()
old=[(6300,'left',6),(6450,'a',6),(6700,'up',6),(6900,'a',6),(7300,'right',6),(7500,'a',6),(8000,'left',6),(8150,'a',6)]+[(8500+i*250,'a',6) for i in range(3)]+[(9400,'right',6),(9600,'a',6),(10300,'a',6)]
ev=[(30,'up',12),(130,'up',12),(230,'up',12),(500,'a',6)]+[(t-5450,b,h) for t,b,h in old]
st.run(ev, list(range(5700,8101,300)), list(range(5700,8101,300)), 8110, 'p66', state=s)
