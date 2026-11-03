import gk, nav
c=gk.Core()
def w16(a,v): c.wr(a,v&255); c.wr(a+1,v>>8)
def run_day():
    c.press(gk.DOWN,3,15);c.press(gk.DOWN,3,15);c.press(gk.DOWN,3,15); c.press(gk.A,3,10)
    for f in range(4000):
        c.run(1)
        if c.rd16(0x63D4) or c.rd16(0x6045)!=1: break
        if f%300==299: c.press(gk.A,2,2)
    c.run(5)
def load():
    nav.loadstate(c,'state_afterpass.bin'); c.press(gk.B,3,30)
# (a) side 0 food runs out
load(); w16(0x6007,3); run_day()
print('a: food0=3 -> over',c.rd16(0x63D4),'winner',c.rd16(0x603F),'food0',c.rd16(0x6007),'expect over=1 winner=1')
# (b) side 1 food runs out
load(); w16(0x6022,3); run_day()
print('b: food1=3 -> over',c.rd16(0x63D4),'winner',c.rd16(0x603F),'expect over=1 winner=0')
# (c) both out: side 0 checked first
load(); w16(0x6007,3); w16(0x6022,3); run_day()
print('c: both -> over',c.rd16(0x63D4),'winner',c.rd16(0x603F),'expect winner=1 (side 0 recorded first)')
# (d) defense command unit size 0 before day end
load(); w16(0x6060+110,0); run_day()
print('d: def cmd unit=0 -> over',c.rd16(0x63D4),'winner',c.rd16(0x603F),'expect over=1 winner=0')
# (e) Pass mobility: unit 7 mobility before/after
load(); m0=c.rd(0x6060+7*11+5); c.press(gk.DOWN,3,15);c.press(gk.DOWN,3,15);c.press(gk.DOWN,3,15); c.press(gk.A,3,10); c.run(20)
print('e: pass mobility',m0,'->',c.rd(0x6060+7*11+5),'type',c.rd(0x6060+7*11+4),'expect min(m+1,15/13/13)')
