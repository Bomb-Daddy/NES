#!/usr/bin/env python3
"""Real third-game command-path persona matrix; each monthly action hits Campaign.
Supports deterministic paired cold-load after six months. Original persona weights are
not yet linked to a full score search; synthetic three/four style mapping is disclosed.
"""
import argparse,json
from pathlib import Path
from third_branch.ai import profile_campaigns
p=argparse.ArgumentParser();p.add_argument('--profiles',default='config/KOEI_12_Player_Playtesting_Profiles_v1_1.json');p.add_argument('--months',type=int,default=12);p.add_argument('--seeds',default='7,41');p.add_argument('--out',default='out/integrated_personas.json')
a=p.parse_args();res=[]
for seed in (int(i) for i in a.seeds.split(',')):
    first=profile_campaigns(a.profiles,a.months,seed)
    second=profile_campaigns(a.profiles,a.months,seed)
    assert first==second,'same-seed campaign replay mismatch'
    res.extend({'seed':seed,**row} for row in first)
summary={'profile_campaigns':len(res),'monthly_steps':len(res)*a.months,'battle_results':sum(r['battle_results'] for r in res),
         'diplomatic_orders':sum(r['diplomatic_orders'] for r in res),
         'unique_profiles':len(set(r['profile'] for r in res)),'replay_matched':len(res)}
path=Path(a.out);path.parent.mkdir(parents=True,exist_ok=True)
path.write_text(json.dumps({'label':'THIRD_BRANCH_INTEGRATED_TEST_ONLY','summary':summary,'runs':res},indent=2,sort_keys=True)+'\n')
print(json.dumps(summary,sort_keys=True))
