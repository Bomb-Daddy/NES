"""Run a single real 15-ruler historical scenario, 12 distinct AI-controlled rulers,
120 months, 120 atomic cold-load-verified game saves plus per-ruler metrics.
The other 3 original rulers are baseline control policies. No synthetic factions.
"""
from __future__ import annotations
import argparse,csv,json,hashlib,sys
from collections import Counter
from pathlib import Path
from third_branch.historical_s1 import new_campaign_historical_s1, source
from third_branch.historical_ai import STYLES,take_turn
from third_branch.integrated import save_file,load_file
from og.core.world2 import calendar_of

def metrics(s,style_by):
    rows=[]
    for fid,info in sorted(s['factions'].items(),key=lambda v:int(v[0][1:])):
        provinces=[p for p,v in s['owners'].items() if v==fid]
        officers=[o for o,v in s['officer_faction'].items() if v==fid]
        pp=[s['core']['provinces'][p] for p in provinces]
        logs=[e for e in s['log'] if e.get('actor') in officers and e['kind']!='month']
        typ=Counter(e['kind'] for e in logs)
        rows.append({'faction':fid,'ruler':info['name'],'policy':style_by[fid],
            'provinces':len(provinces),'officers':len(officers),'troops':sum(p['troops'] for p in pp),
            'gold_cents':sum(p['treasury_gold_cents'] for p in pp),
            'food_units':sum(p['food_units'] for p in pp),
            'support_avg_ticks':round(sum(p['support_ticks'] for p in pp)/max(1,len(pp))),
            'cultivation_avg':round(sum(p['cultivation'] for p in pp)/max(1,len(pp)),2),
            'committed_orders':len(logs),'diplomacy':sum(v for k,v in typ.items() if k in ('Ally','Joint','Give','Marry','Threat','Cancel')),
            'military_orders':sum(v for k,v in typ.items() if k in ('recruit','attack','plan_attack','train')),
            'industry_orders':typ['allocate_workers'],'market_orders':typ['market'],
            'governor_orders':typ['appoint_governor'],'recruited_officers':typ['recruit_officer'],
            'civilian_workers':sum(sum(p['workers'].values()) for p in pp),
            'alive':int(info['alive'])})
    return rows

def run(out,seed=17,months=120):
    out=Path(out);out.mkdir(parents=True,exist_ok=True)
    saves=out/'saves';saves.mkdir(exist_ok=True)
    c=new_campaign_historical_s1(seed)
    rulers=sorted(c.state['factions'],key=lambda f:int(f[1:]))
    style_by={f:(STYLES[i] if i<12 else 'cautious') for i,f in enumerate(rulers)}
    allmetrics=[];actions=[];timeline=[];prevhash=c.fingerprint
    for t in range(1,months+1):
        summary=[]
        for faction in rulers:
            action=take_turn(c,faction,style_by[faction],prefix=f'S1-seed{seed}')
            summary.append(action)
        calendar_before=calendar_of(c.state['core'],c.state['month'])
        completed=c.advance_month()
        save=saves/f'month_{t:03}.ogsave'
        save_file(c,save)
        # Cold-load each and every month; canonical hash must match.
        cold=load_file(save)
        if cold.fingerprint!=c.fingerprint:raise AssertionError('Cold-load mismatch '+str(t))
        cold_year,cold_month=calendar_of(cold.state['core'],cold.state['month'])
        if cold.state['month']!=t+1:raise AssertionError('Wrong next month')
        rows=metrics(cold.state,style_by)
        for row in rows:
            row.update(month_index=t,completed_year=calendar_before[0],completed_month=calendar_before[1],save_sha256=hashlib.sha256(save.read_bytes()).hexdigest(),state_sha256=cold.fingerprint)
        allmetrics+=rows
        kinds=Counter(a['kind'] for a in summary)
        timeline.append({'month':t,'calendar':calendar_before,'state_sha256':cold.fingerprint,
                         'previous_sha256':prevhash,'save':save.name,'save_size':save.stat().st_size,
                         'action_types':dict(kinds),'total_provinces':sum(v is not None for v in cold.state['owners'].values()),
                         'active_officers':len(cold.state['core']['officers'])})
        actions.extend({'month':t,**a} for a in summary)
        c=cold;prevhash=c.fingerprint
        if t%12==0: print('COMPLETE_YEAR',calendar_before[0],'MONTH',t,'HASH',cold.fingerprint[:16],flush=True)
    with (out/'monthly_faction_metrics.csv').open('w',newline='') as f:
        w=csv.DictWriter(f,fieldnames=list(allmetrics[0]));w.writeheader();w.writerows(allmetrics)
    (out/'monthly_timeline.json').write_text(json.dumps(timeline,indent=2)+'\n')
    with (out/'actions.jsonl').open('w') as f:
        for a in actions:f.write(json.dumps(a,sort_keys=True)+'\n')
    start={r['faction']:r for r in allmetrics[:15]};end={r['faction']:r for r in allmetrics[-15:]}
    delta={fid:{key:end[fid][key]-start[fid][key] for key in ('provinces','officers','troops','gold_cents','food_units','committed_orders','diplomacy','military_orders','industry_orders','market_orders','governor_orders','recruited_officers')} for fid in start}
    identities={fid:{'name':end[fid]['ruler'],'style':end[fid]['policy'],'months':months,**delta[fid]} for fid in delta}
    type_counter={f:Counter(a['kind'] for a in actions if a['faction']==f) for f in rulers}
    unique_signatures=len(set(tuple(sorted(type_counter[f].items())) for f in rulers[:12]))
    report={'scenario':'NES-derived 189 S1','seed':seed,'months':months,'save_count':len(list(saves.glob('*.ogsave'))),
       'monthly_metrics_rows':len(allmetrics),'policy_count':len(STYLES),'historical_factions':len(rulers),
       'unique_first_12_action_signatures':unique_signatures,'final_hash':c.fingerprint,
       'profile_deltas':identities,'action_mix':{f:dict(type_counter[f]) for f in rulers},
       'disclaimer':'Linux headless test of integrated proposed original-game engine; not Windows, not independent NES parity, not historical AI acceptance.'}
    (out/'balance_report.json').write_text(json.dumps(report,indent=2)+'\n')
    return report

if __name__=='__main__':
    a=argparse.ArgumentParser();a.add_argument('--out',default='historical_ten_year_qa');a.add_argument('--seed',type=int,default=17);a.add_argument('--months',type=int,default=120);v=a.parse_args()
    report=run(v.out,v.seed,v.months)
    print('FINAL',json.dumps({k:report[k] for k in ('months','save_count','monthly_metrics_rows','historical_factions','unique_first_12_action_signatures','final_hash')}))
