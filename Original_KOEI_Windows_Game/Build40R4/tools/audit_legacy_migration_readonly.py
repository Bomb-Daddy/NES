#!/usr/bin/env python3
"""Independent read-only audit of Fork4 .f4save migration prerequisites.

Never writes the original SQLite archive, invents training/arms, or relaxes the
100-group active historical cap. Designed to be run outside production game.
"""
import argparse,hashlib,json,sqlite3,zlib
from pathlib import Path


def canonical(s):
    return json.dumps(s,sort_keys=True,separators=(',',':'),ensure_ascii=False).encode()


def audit(path):
    path=Path(path)
    before=hashlib.sha256(path.read_bytes()).hexdigest()
    db=sqlite3.connect(f'file:{path.absolute()}?mode=ro',uri=True)
    try:
        assert db.execute('PRAGMA integrity_check').fetchone()[0]=='ok'
        rows=db.execute('SELECT completed_month,engine_month,state_sha256,whole_state_z FROM turn_index ORDER BY completed_month').fetchall()
        assert rows and [r[0] for r in rows]==list(range(rows[-1][0]+1))
        all_months=[];hash_mismatches=[]
        for month,engine,sha,blob in rows:
            s=json.loads(zlib.decompress(blob));actual=hashlib.sha256(canonical(s)).hexdigest()
            if sha!=actual:hash_mismatches.append(month)
            assert engine==month+1 and s['month']==engine
            armies=s['armies'];over=[(aid,sum(a['stations'].values())+a['engaged']) for aid,a in armies.items() if sum(a['stations'].values())+a['engaged']>100]
            all_months.append({'month':month,'army_count':len(armies),'over_cap_armies':len(over),
                               'peak_army_groups':max((sum(a['stations'].values())+a['engaged'] for a in armies.values()),default=0),
                               'troop_groups':sum(sum(a['stations'].values())+a['engaged'] for a in armies.values()),
                               'missing_training':len(s['core']['officers']) if 'unit_training' not in s else len(set(s['core']['officers'])-set(s['unit_training'])),
                               'missing_arms':len(s['core']['officers']) if 'unit_armed_men' not in s else len(set(s['core']['officers'])-set(s['unit_armed_men'])),
                               'active_officers':len(s['core']['officers']),
                               'battle_active':s['battle'] is not None})
        last=rows[-1][0]
        cur=db.execute('SELECT completed_month,game_save,game_save_sha256 FROM current_save WHERE id=1').fetchone()
        assert cur and cur[0]==last and hashlib.sha256(cur[1]).hexdigest()==cur[2]
        assert not hash_mismatches, f'Historical snapshots corrupt: {hash_mismatches}'
        result={'archive_sha256':before,'snapshot_count':len(rows),'last_completed_month':last,
                'month47':next((r for r in all_months if r['month']==47),None),
                'overcap_army_months':sum(r['over_cap_armies'] for r in all_months),
                'months_with_overcap':sum(r['over_cap_armies']>0 for r in all_months),
                'snapshot_hashes_verified':len(rows),'current_save_sha_verified':True,
                'historical_schema_unknowns':['unit_training','unit_armed_men','workforce_policy',
                   'civilian_workforce_baseline','civilian_workforce_current','civilian_workforce_ledger'],
                'migration_status':'BLOCKED_UNAPPROVED_LEGACY_ARMY_CAP_AND_UNKNOWN_SKILL_ARMS',
                'monthly':all_months}
    finally:db.close()
    after=hashlib.sha256(path.read_bytes()).hexdigest()
    assert after==before, 'Original archive changed unexpectedly'
    result['original_bytes_unchanged']=True
    return result

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('save');p.add_argument('--output')
    a=p.parse_args();result=audit(a.save)
    out=json.dumps(result,indent=2)
    if a.output:Path(a.output).write_text(out+'\n',encoding='utf-8')
    print(json.dumps({k:v for k,v in result.items() if k!='monthly'},indent=2))
