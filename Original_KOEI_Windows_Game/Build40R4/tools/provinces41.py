import json, time
from pathlib import Path
from og.core.world import new_world,add_province,add_officer,validate_world,world_hash
from og.core.world2 import migrate_v1_to_v2
from og.core.bus import Bus
from og.core import scheduler
from og.core.save import dump_save,load_save
T=time.monotonic();results=[]
for seed in (7,41):
    w=new_world(seed)
    for i in range(1,42):add_province(w,f'p{i:02d}',30+i%40)
    add_officer(w,'o1','p01',(50,50,50,50,50))
    w=migrate_v1_to_v2(w)
    for pid,p in w['provinces'].items():
        p['workers']={'farmers':140,'town':35,'artisans':20,'metalworkers':10,'resources':10,'unassigned':40,'land':2,'flood':3}
        p['wealth_milli']=35000;p['tax_rate_percent']=25;p['support_ticks']=1500000
        p['warehouse_capacity_units']=150;p['treasury_gold_cents']=2500;p['food_units']=100;p['flood_control']=20
    b=Bus(w)
    for m in range(1,13):
        scheduler.run_month(b)
        if m==9:b=Bus(load_save(dump_save(b.data)))
    validate_world(b.data)
    q=b.data
    counts={'tax':len(q['ledger']),'sept_fall':len([e for e in q['stock_ledger'] if e['kind']=='SeptemberPayout']),
            'overflow':len([e for e in q['stock_ledger'] if e['kind']=='OverflowDiscarded']),
            'events':len(q['event_ledger'])}
    rec={'seed':seed,'months':12,'provinces':len(q['provinces']),'hash':world_hash(q),'counts':counts,
         'min_food':min(p['food_units'] for p in q['provinces'].values()),'min_gold':min(p['treasury_gold_cents'] for p in q['provinces'].values()),
         'valid':counts['tax']==41 and counts['sept_fall']==82}
    results.append(rec);print(rec,flush=True)
path=Path(__file__).resolve().parents[1]/'out'/'personas'/'provinces41.json'
path.write_text(json.dumps({'results':results,'elapsed_seconds':round(time.monotonic()-T,2)},indent=2))
