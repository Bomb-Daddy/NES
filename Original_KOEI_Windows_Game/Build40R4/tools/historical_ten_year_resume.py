"""Resumable 120-month historical S1 QA runner. Safely resumes only from the
consecutive save sequence, always cold-load-verifies before counting completion.
Aggregate evidence is rebuilt entirely from the saved states on disk.
"""
import argparse,csv,hashlib,json
from collections import Counter
from pathlib import Path
from third_branch.historical_s1 import new_campaign_historical_s1
from third_branch.historical_ai import STYLES,take_turn
from third_branch.integrated import load_file,save_file
from og.core.world2 import calendar_of
from tools.historical_ten_year_qa import metrics

def run(out, stop=120, seed=17):
    out=Path(out);out.mkdir(parents=True,exist_ok=True)
    saves=out/'saves';saves.mkdir(exist_ok=True)
    files=sorted(saves.glob('month_*.ogsave'))
    completed=len(files)
    if files!=[saves/f'month_{i:03d}.ogsave' for i in range(1,completed+1)]:
        raise RuntimeError('Nonconsecutive archive; refusing a non-auditable resume')
    if completed:
        c=load_file(files[-1]);assert c.state['month']==completed+1
    else:c=new_campaign_historical_s1(seed)
    factions=sorted(c.state['factions'],key=lambda x:int(x[1:]))
    styles={f:(STYLES[i] if i<12 else 'cautious') for i,f in enumerate(factions)}
    for month in range(completed+1,stop+1):
        for faction in factions:
            take_turn(c,faction,styles[faction],prefix=f'S1-seed{seed}')
        c.advance_month()
        save=saves/f'month_{month:03}.ogsave'
        save_file(c,save)
        cold=load_file(save)
        if cold.fingerprint!=c.fingerprint or cold.state['month']!=month+1:
            raise RuntimeError('Cold load mismatch month '+str(month))
        c=cold
        if month%12==0 or month==stop:
            print('COMPLETE',month,'of',stop,'year',calendar_of(c.state['core'],c.state['month'])[0],flush=True)
    if stop==120:finalize(out,seed)

def finalize(out,seed):
    out=Path(out);saves=out/'saves';files=sorted(saves.glob('month_*.ogsave'))
    if len(files)!=120:raise RuntimeError(f'Not all 120 saves: {len(files)}')
    rows=[];timeline=[];count=Counter();prevlog=0;last=None
    for i,p in enumerate(files,1):
        c=load_file(p);s=c.state;fingerprint=c.fingerprint
        fids=sorted(s['factions'],key=lambda x:int(x[1:]))
        styles={fid:(STYLES[k] if k<12 else 'cautious') for k,fid in enumerate(fids)}
        mm=metrics(s,styles)
        year,calmonth=calendar_of(s['core'],i)
        savehash=hashlib.sha256(p.read_bytes()).hexdigest()
        for r in mm:r.update(month_index=i,calendar_year=year,calendar_month=calmonth,
                             save_sha256=savehash,state_sha256=fingerprint)
        rows+=mm
        events=s['log'][prevlog:];prevlog=len(s['log'])
        mix=Counter(x['kind'] for x in events)
        for x in events:
            rid=x.get('id','')
            if rid.startswith('S1-seed') and ':R' in rid:
                try:
                    parts=rid.split(':');f=(parts[3] if len(parts)>3 and parts[1]=='ai3' else parts[2]);count[(f,x['kind'])]+=1
                except IndexError:pass
        timeline.append({'month':i,'calendar_year':year,'calendar_month':calmonth,
                         'state_sha256':fingerprint,'save_sha256':savehash,
                         'save_file':p.name,'new_events':len(events),'action_mix':dict(mix),
                         'active_officers':len(s['core']['officers']),
                         'occupied_provinces':sum(v is not None for v in s['owners'].values())})
        last=c
    with (out/'monthly_faction_metrics.csv').open('w',newline='',encoding='utf-8') as f:
        w=csv.DictWriter(f,fieldnames=list(rows[0]));w.writeheader();w.writerows(rows)
    (out/'monthly_timeline.json').write_text(json.dumps(timeline,indent=2)+'\n')
    identities={};start={x['faction']:x for x in rows[:15]};end={x['faction']:x for x in rows[-15:]}
    for f in start:
        a=start[f];b=end[f]
        identities[f]={'ruler':a['ruler'],'policy':a['policy'],
                       'action_mix':{k:v for (fid,k),v in count.items() if fid==f},
                       'starting_provinces':a['provinces'],'final_provinces':b['provinces'],
                       'starting_officers':a['officers'],'final_officers':b['officers'],
                       'starting_troops':a['troops'],'final_troops':b['troops'],
                       'starting_gold_cents':a['gold_cents'],'final_gold_cents':b['gold_cents'],
                       'starting_support_ticks':a['support_avg_ticks'],'final_support_ticks':b['support_avg_ticks'],
                       'starting_cultivation':a['cultivation_avg'],'final_cultivation':b['cultivation_avg']}
    signatures={f:tuple(sorted(identities[f]['action_mix'].items())) for f in list(start)[:12]}
    r={'scenario':'NES-derived original-game 189 S1','seed':seed,'months':120,
       'historical_factions':len(start),'distinct_ai_profiles':12,
       'monthly_saves_verified':len(files),'monthly_faction_rows':len(rows),
       'unique_first_12_action_signatures':len(set(signatures.values())),
       'total_accepted_ai_actions':sum(count.values()),'final_hash':last.fingerprint,
       'rulers':identities,'scope':'Headless integrated third-branch engine, Linux; not authentic NES combat/AI or Windows acceptance.'}
    (out/'balance_report.json').write_text(json.dumps(r,indent=2)+'\n')
    print('FINAL_VERIFIED',json.dumps({k:r[k] for k in ('months','historical_factions','distinct_ai_profiles','monthly_saves_verified','monthly_faction_rows','unique_first_12_action_signatures','total_accepted_ai_actions','final_hash')}),flush=True)
    return r

if __name__=='__main__':
    a=argparse.ArgumentParser();a.add_argument('--out',default='historical_ten_year_qa');a.add_argument('--stop',type=int,default=120);a.add_argument('--seed',type=int,default=17);b=a.parse_args();run(b.out,b.stop,b.seed)
