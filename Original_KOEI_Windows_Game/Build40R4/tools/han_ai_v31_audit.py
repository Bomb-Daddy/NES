"""Independent action-attribution repair for v3 request IDs (legacy QA parser misread them)."""
from collections import Counter
from pathlib import Path
import argparse,hashlib,json
from third_branch.integrated import load_file
from third_branch.historical_ai_v3 import PROFILES


def audit(root, expected_months=120):
    root=Path(root);saves=root/'saves';names=[saves/f'month_{n:03d}.ogsave' for n in range(1,expected_months+1)]
    assert all(f.is_file() for f in names),'missing or nonconsecutive save'
    states=[load_file(p) for p in names]
    assert all(c.state['month']==i+2 for i,c in enumerate(states))
    last=states[-1].state
    counts={f'R{i}':Counter() for i in range(15)}
    commits={f'R{i}':0 for i in range(15)}
    errors=[]
    for e in last['log']:
        if e['kind']=='month':continue
        parts=e['id'].split(':')
        if len(parts)<6 or parts[1]!='ai3' or not parts[3].startswith('R'):
            errors.append(e['id']);continue
        fid=parts[3];assert fid in counts
        if e['kind']!='battle_auto':
            counts[fid][e['kind']]+=1;commits[fid]+=1
    mix={f:dict(v) for f,v in counts.items()}
    unique=len(set(tuple(sorted(mix[f].items())) for f in list(counts)[:12]))
    first=states[0].state
    def nprovs(s,f):return sum(v==f for v in s['owners'].values())
    summary={'run':'Han AI v3.1 final source corrected attribution','months':expected_months,
      'save_count':len(names),'verified_cold_loads':len(states),'final_hash':states[-1].fingerprint,
      'monthly_faction_rows':expected_months*15,
      'unique_profiles_by_accepted_action_mix':unique,
      'accepted_nonbattle_ai_commands':sum(commits.values()),
      'total_completed_battles':len(last['history']),
      'battle_attacker_wins':sum(x.get('winner')=='ATTACKER' for x in last['history']),
      'unclassified_journal_ids':errors,
      'factions':{f:{'profile':f'P{int(f[1:])+1:02}' if int(f[1:])<12 else 'CAUTIOUS_CONTROL',
                      'ai_name':PROFILES.get(f'P{int(f[1:])+1:02}',{}).get('name','Control'),
                      'ruler':last['factions'][f]['name'],
                      'provinces_month1':nprovs(first,f),
                      'provinces_month120':nprovs(last,f),
                      'committed_nonbattle':commits[f],
                      'action_mix':mix[f]} for f in counts},
      'save_sha256':{p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in names},
      'provenance':'Read every real cold-save state. Pre-refinement v3.0 and v3.1 source may not be mixed.'}
    (root/'han_ai_v31_corrected_audit.json').write_text(json.dumps(summary,indent=2)+'\n')
    return summary

if __name__=='__main__':
    ap=argparse.ArgumentParser();ap.add_argument('--root',required=True);ap.add_argument('--months',type=int,default=120)
    q=ap.parse_args();d=audit(q.root,q.months)
    print('VERIFIED_AUDIT',json.dumps({k:d[k] for k in ('months','save_count','verified_cold_loads','unique_profiles_by_accepted_action_mix','accepted_nonbattle_ai_commands','total_completed_battles','battle_attacker_wins','final_hash')}))
    for fid,r in d['factions'].items():print(fid,r['ai_name'],r['action_mix'],r['provinces_month1'],r['provinces_month120'])
