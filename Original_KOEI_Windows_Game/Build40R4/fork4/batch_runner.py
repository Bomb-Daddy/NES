"""Fork Four matched-case batch runner: actual historical Scenario 1 engine.

Each (seed, weight) has a single .f4save with every province, general, faction
and global engine metric for every month plus advisor decision evidence.
"""
from __future__ import annotations
import argparse,csv,json,hashlib
from pathlib import Path
from third_branch.historical_s1 import new_campaign_historical_s1
from third_branch.historical_ai import STYLES,take_turn
from .batch_archive import CampaignArchive


def run_case(out,seed,months,weight,*,audit_every=12):
    if not 0<=weight<=10000:raise ValueError('Weight out of range')
    out=Path(out);out.mkdir(parents=True,exist_ok=True)
    name=f'seed{seed}_w{weight:04}.f4save';path=out/name
    with CampaignArchive(path) as archive:
        count=archive.db.execute('SELECT COUNT(*) FROM turn_index').fetchone()[0]
        if count:
            c=archive.cold_load(); completed=c.state['month']-1
            if (c.state.get('fork4_advisor_config') or {}).get('weight_bps')!=weight:
                raise RuntimeError('Scenario weight changed across resumed save')
            if (c.state.get('fork4_advisor_config') or {}).get('seed')!=seed:
                raise RuntimeError('Scenario seed changed across resumed save')
        else:
            c=new_campaign_historical_s1(seed,balance_policy='OG_1000_DIRECTOR_V2')
            c.state['fork4_advisor_config']={'enabled': True,'weight_bps':weight,'seed':seed,'revision':'fork4-batch-1'}
            archive.record(c,0,());completed=0
        factions=sorted(c.state['factions'],key=lambda f:int(f[1:]))
        styles={f:(STYLES[i] if i<len(STYLES) else 'cautious') for i,f in enumerate(factions)}
        if completed>months:raise RuntimeError('Existing run longer than request')
        for m in range(completed+1,months+1):
            actions=[take_turn(c,f,styles[f],prefix=f'F4-seed{seed}') for f in factions]
            c.advance_month()
            archive.record(c,m,actions)
            c=archive.cold_load()
            if m in (1,months) or m%audit_every==0:
                print('FORK4',seed,weight,m,'of',months,'checksum',c.fingerprint[:12],flush=True)
        archive.verify_counts(months)
        events=archive.db.execute('SELECT COUNT(*) FROM advisor_decisions').fetchone()[0]
        changed=archive.db.execute('SELECT COUNT(*) FROM advisor_decisions WHERE changed=1').fetchone()[0]
        owned=sum(x is not None for x in c.state['owners'].values())
        return {'seed':seed,'advisor_weight_bps':weight,'months':months,
                'province_count':len(c.state['owners']), 'general_count':len(c.state['officer_catalog']),
                'faction_count':len(factions),'advisor_decisions':events,
                'advisor_changes':changed,'governed_provinces':owned,
                'game_state_sha256':c.fingerprint,'archive':str(path),
                'sha256':hashlib.sha256(path.read_bytes()).hexdigest()}


def main():
    p=argparse.ArgumentParser()
    p.add_argument('--out',default='fork4_out')
    p.add_argument('--months',type=int,default=120)
    p.add_argument('--seeds',default='17,23,41')
    p.add_argument('--weights',default='0,1000,2000,3000')
    x=p.parse_args();out=Path(x.out)
    results=[]
    for seed in (int(v) for v in x.seeds.split(',')):
        for weight in (int(v) for v in x.weights.split(',')):
            results.append(run_case(out,seed,x.months,weight))
    (out/'summary.json').write_text(json.dumps(results,indent=2),encoding='utf-8')
    with (out/'summary.csv').open('w',newline='',encoding='utf-8') as f:
        w=csv.DictWriter(f,results[0]);w.writeheader();w.writerows(results)
    print('COMPLETE',len(results),'cases; exact files in',out)

if __name__=='__main__':main()
