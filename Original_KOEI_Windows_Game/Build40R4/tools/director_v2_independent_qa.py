"""Independent persisted-state analysis/replay, not the campaign generator."""
from pathlib import Path
from collections import Counter
import argparse,hashlib,csv,json
from third_branch.integrated import load_file,save_bytes
from third_branch.historical_s1 import new_campaign_historical_s1
from third_branch.historical_ai import STYLES
from third_branch.balance_ai_candidate_v2 import take_turn
from og.core.world2 import calendar_of
from og.core.fixed import CoreError

POL='OG_1000_DIRECTOR_V2'
def replay(c,month):
    factions=sorted(c.state['factions'],key=lambda x:int(x[1:]))
    for i,faction in enumerate(factions):
        take_turn(c,faction,STYLES[i] if i<12 else 'cautious',prefix='balance-candidate-v2')
    c.advance_month()
    return c.fingerprint

def run(path):
    path=Path(path);saves=path/'saves';files=sorted(saves.glob('month_*.ogsave'))
    assert len(files)==120 and files==[saves/f'month_{i:03}.ogsave' for i in range(1,121)]
    rows=[];lastlog=0;actions=Counter();factions={};prev=None
    for i,p in enumerate(files,1):
        c=load_file(p);s=c.state
        assert c.state.get('balance_policy')==POL and s['month']==i+1
        assert len(s['officer_catalog'])==352 and len(s['owners'])==41
        assert set(s['metal_deposits'])==set(s['owners'])
        assert len(s['family_relations'])==23
        data=p.read_bytes();
        newlog=s['log'][lastlog:];lastlog=len(s['log']);actions.update(e['kind'] for e in newlog)
        occupied={pid:f for pid,f in s['owners'].items() if f is not None}
        core=s['core']['provinces'];ex=s['extras'];owned=list(occupied)
        row={'month':i,'year':calendar_of(s['core'],i)[0],'month_of_year':calendar_of(s['core'],i)[1],
             'state_sha256':c.fingerprint,'save_sha256':hashlib.sha256(data).hexdigest(),
             'occupied':len(owned),'unowned':41-len(owned),'occupation_orders':actions['occupy_vacant'],
             'government_gold':sum(core[x]['treasury_gold_cents'] for x in owned),
             'neutral_gold':sum(core[x]['treasury_gold_cents'] for x in core if x not in owned),
             'food':sum(core[x]['food_units'] for x in owned),
             'metal':sum(ex[x]['metal'] for x in owned),'arms':sum(ex[x]['arms'] for x in owned),
             'troops':sum(core[x]['troops'] for x in owned),
             'cultivation_avg':round(sum(core[x]['cultivation'] for x in owned)/len(owned),3),
             'wealth_avg':round(sum(core[x]['wealth_milli']/1000 for x in owned)/len(owned),3),
             'source_ore_sites_verified':sum(s['metal_deposits'][x] is True for x in core),
             'source_ore_sites_unknown':sum(s['metal_deposits'][x] is None for x in core)}
        rows.append(row);prev=c
    assert rows[-1]['occupied']==sum(v is not None for v in prev.state['owners'].values())
    with (path/'numeric_monthly.csv').open('w',newline='') as f:
        w=csv.DictWriter(f,fieldnames=list(rows[0]));w.writeheader();w.writerows(rows)
    before=new_campaign_historical_s1(seed=17,balance_policy=POL)
    replay_first=replay(before,1)==rows[0]['state_sha256']
    middle=load_file(saves/'month_060.ogsave')
    replay_61=replay(middle,61)==rows[60]['state_sha256']
    # Tamper must reject without writing to original file.
    tampered=bytearray((saves/'month_060.ogsave').read_bytes());tampered[-55]^=1
    from third_branch.integrated import load_bytes
    try:load_bytes(bytes(tampered)); tamper_rejected=False
    except (CoreError,ValueError,UnicodeError,json.JSONDecodeError):tamper_rejected=True
    assert replay_first and replay_61 and tamper_rejected
    report={'months':len(files),'replay_month1':replay_first,'replay_month61':replay_61,'corruption_rejected':tamper_rejected,
            'all_saves_cold_loaded':True,'factions':len(prev.state['factions']),'profiles':12,
            'starting':rows[0],'end':rows[-1],
            'accepted_commands':dict(actions),'total_accepted_actions':sum(v for k,v in actions.items() if k!='month'),
            'per_faction_provinces':{fid:sum(x==fid for x in prev.state['owners'].values()) for fid in prev.state['factions']},
            'mine_provenance':'BLOCKED_SOURCE: no verified 41-province deposit map; all location flags unknown',
            'notes':'Linux deterministic simulation, historical scenario S1 and original-game source; not Windows physical controller testing.'}
    (path/'independent_report.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:report[k] for k in ('months','all_saves_cold_loaded','replay_month1','replay_month61','corruption_rejected','total_accepted_actions')},indent=2),flush=True)
    return report

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--out',required=True);a=p.parse_args();run(a.out)
