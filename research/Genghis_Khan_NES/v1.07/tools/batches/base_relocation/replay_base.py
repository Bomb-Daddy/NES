"""Original-instruction Move Base / country scheduler boundary replay.

Author: Codex trade/logistics source subagent, 2026-11-04.
VERIFIED_STATIC only: selected original CPU/VM regions in synthetic state.
No screen, controller, full scheduling, battle, or Windows execution claim.
"""
from pathlib import Path
import sys,json
ROOT=Path(__file__).resolve().parents[2]
sys.dont_write_bytecode=True
sys.path.insert(0,str(ROOT/'batches/logistics_source'))
import replay_logistics as l
prepare,run,word,put=l.prepare,l.run,l.word,l.put
SRC,DST,FRAME,STACK=l.SRC,l.DST,l.FRAME,l.STACK

def state(c):
 return {'officers':[word(c,p+81) for p in (SRC,DST)],
         'abilities':[word(c,0x65cd+2*i) for i in range(6)],
         'orders':word(c,0x7b36),'active':word(c,0x7b34),'slot':word(c,0x7b38),
         'record_words':[word(c,a) for a in (0x77d2,0x77e5)],
         'gold':[word(c,p+64) for p in (SRC,DST)],
         'jewels':[word(c,p+38) for p in (SRC,DST)],
         'roles':[c.m[word(c,p+81)+30] for p in (SRC,DST)]}

def relocate(orders=3,body=170,role=101,mutation=None):
 patches={0x65fb:0,0x65fc:role};patches.update(mutation or {})
 c=prepare(0x967a,[SRC],baseline=True,locals={-2:DST},
     words={0x7b34:0,0x7b36:orders,0x65d5:body,0x77d2:321,0x77e5:456},byteset=patches)
 before=bytes(c.m[0x6000:0x8000]);a=state(c)
 r=run(c,0x968a)
 return c,{'before':a,'after':state(c),'steps':r['steps'],
  'changed_addresses':[hex(0x6000+i) for i,(x,y) in enumerate(zip(before,c.m[0x6000:0x8000])) if x!=y]}

def resume(c,start,args=(),bank=0,locals=None):
 c.m[0x8000:0xc000]=l.t.PRG[bank*16384:(bank+1)*16384]
 put(c,2,STACK);put(c,4,FRAME);put(c,6,start);c.pc=0xefc9
 for i,v in enumerate(args):put(c,FRAME+11+2*i,v)
 for a,v in (locals or {}).items():put(c,FRAME+a,v)

def dispatch(c,country):
 # Starts after pre-turn random events and display, before ownership/role tests.
 resume(c,0x8735,locals={-6:country})
 r=run(c,(0x8748,0x8750));return {'player':r['stop']==0x8748,'human':word(c,0x7b70),'steps':r['steps']}

def fresh_orders(c,country):
 # Starts after initial country display. Original role branch and order write.
 resume(c,0x8c02,[country]);put(c,0x63ec,0)
 r=run(c,0x8ced);return word(c,0x7b36)

def schedule_select(c,slot):
 resume(c,0x86df);put(c,0x7b38,slot)
 run(c,0x86f3);return word(c,0x7b34),word(c,FRAME-6)

def quote(c,merchant,item):
 resume(c,0xdd06,[merchant,item],bank=3)
 return run(c,0xdd19)['A']

def sale_at(c,country,item,q,seller_quote):
 resume(c,0x8c20,[country],bank=3,locals={-4:q,-12:country+34+2*item,-8:seller_quote,-10:country+64})
 run(c,0x8c33)
 return {'stock':word(c,country+34+2*item),'gold':word(c,country+64)}

def example():
 c,r=relocate();schedule=[word(c,0x7b3a+2*i)+1 for i in range(word(c,0x65b9))]
 r['schedule']=schedule;r['old_country_dispatch']=dispatch(c,SRC)
 r['later_slot_selected']=schedule_select(c,schedule.index(2))
 r['new_country_dispatch']=dispatch(c,DST);r['new_orders']=fresh_orders(c,DST)
 # Explicit test setup: model a prior legal transfer of one Jewels unit. No
 # game state setter is substituted for Move Base or its cost/scheduling logic.
 put(c,DST+38,word(c,DST+38)+1)
 r['seller_quote']=quote(c,0,2);r['sale_before']={'stock':word(c,DST+38),'gold':word(c,DST+64)}
 r['sale_after']=sale_at(c,DST,2,1,r['seller_quote'])
 r['steps_total']=c.total
 return r

def transfer_relocate_sell(q=1):
 """Same persistent synthetic state through actual transfer, costs, move, sale.

 Bounded regions omit all UI and intervening other-country turns/events.
 Baseline schedule is selected by the original country-selection code; no
 assignment to the active-country field is used after initial state setup.
 """
 c=prepare(0x961c,[SRC+38,DST+38,29],baseline=True,
     locals={-2:q},byteset={0x65fb:0,0x65fc:101})
 r={'initial':state(c)};run(c,0x962b)
 resume(c,0x8c95);put(c,8,23);run(c,0x8ca9);r['after_transfer']=state(c)
 resume(c,0x967a,[SRC],bank=3,locals={-2:DST});run(c,0x968a);r['after_move_base']=state(c)
 r['selected']=schedule_select(c,10);r['dispatch']=dispatch(c,DST);fresh_orders(c,DST);r['before_trade']=state(c)
 r['seller_quote']=quote(c,0,2);sale_at(c,DST,2,q,r['seller_quote'])
 resume(c,0x8e38,[DST],bank=3);put(c,8,9);run(c,0x8e4d);r['after_trade']=state(c);r['steps']=c.total
 return r

if __name__=='__main__':
 print(json.dumps({'relocation':example(),'transfer_relocate_sell':transfer_relocate_sell()},indent=2))
