import csv,json,time
from pathlib import Path
from persona_campaigns import PROFILES,run,state,build,OUT
from og.core import scheduler
from og.core.world import world_hash
from collections import Counter
st=time.monotonic();out=[];rows=[];errors=[]
for p in PROFILES:
    b,shot,o,trace=run(p,7,60)
    rec={'profile':p['id'],'seed':7,'months':60,'hash':world_hash(b.data),'accepted':o['accepted'],'invalid_rejected':o['invalid_rejected'],'negative_mutations':o['unexpected_mutation'],'preview_mutations':o['preview_mutation'],'state':shot[-1]}
    out.append(rec);rows.extend(shot)
    print('60_MONTH',p['id'],'accepted',o['accepted'],'food',shot[-1]['food'],'gold',shot[-1]['gold_cents'],'bp',shot[-1]['bp'],flush=True)
for name in ('P03','P04','P05','P08','P12'):
    p=next(x for x in PROFILES if x['id']==name)
    b,shot,o,_=run(p,7,120)
    b2,_,o2,_=run(p,7,120,reload=True)
    match=world_hash(b.data)==world_hash(b2.data)
    out.append({'profile':name,'seed':7,'months':120,'hash':world_hash(b.data),'accepted':o['accepted'],'invalid_rejected':o['invalid_rejected'],'cold_replay_match':match,'state':shot[-1]})
    if not match:errors.append({'profile':name,'failure':'cold_replay'})
    rows.extend(shot)
    print('120_MONTH',name,'food',shot[-1]['food'],'gold',shot[-1]['gold_cents'],'bp',shot[-1]['bp'],'same',match,flush=True)
for months in (60,120):
    b=build(7)
    for m in range(months):scheduler.run_month(b)
    out.append({'profile':'PASSIVE','seed':7,'months':months,'hash':world_hash(b.data),'state':state(b,'PASSIVE',7,months,Counter())})
(OUT/'longhorizon.json').write_text(json.dumps({'results':out,'errors':errors,'elapsed_seconds':round(time.monotonic()-st,2)},indent=2))
with (OUT/'longhorizon_milestones.csv').open('w',newline='') as f:
    w=csv.DictWriter(f,fieldnames=rows[0]);w.writeheader();w.writerows(rows)
print('LONG_DONE',len(out),'errors',len(errors),'seconds',round(time.monotonic()-st,2),flush=True)
