import gk, nav, random
from traintest import rng_next, get_state, set_state
c=gk.Core()
def w16(a,v): c.wr(a,v&255); c.wr(a+1,v>>8)
CR=0x6DCD+83*1
random.seed(11); ok=n=0
for t in range(20):
    nav.loadstate(c,'state_day2_u0.bin')
    c.wr(0x6062,10); c.wr(0x6063,6)
    F=random.randint(0,999); M=random.randint(0,300); food=random.randint(0,9990)
    w16(CR+0x3C,F); w16(CR+0x42,M); w16(CR+0x46,0); w16(0x6007,food)
    S=random.getrandbits(48); set_state(c,S)
    for i in range(3): c.press(gk.DOWN,3,12)
    c.press(gk.A,3,80); c.press(gk.DOWN,3,25); c.press(gk.DOWN,3,25)
    c.press(gk.A,3,90)
    S1,o1=rng_next(S); S2,o2=rng_next(S1)
    amt=F*(30+o1%11)//100
    if F==0 or amt==0: exp=(F,M,food,0)
    else: exp=(F-amt, M*(55+o2%21)//100, min(9999,food+10*amt), 1)
    got=(c.rd16(CR+0x3C),c.rd16(CR+0x42),c.rd16(0x6007),c.rd16(CR+0x46))
    n+=1; ok+=exp==got
    print(t,'F',F,'M',M,'food',food,'exp',exp,'got',got,'OK' if exp==got else 'FAIL')
print(ok,'/',n)
