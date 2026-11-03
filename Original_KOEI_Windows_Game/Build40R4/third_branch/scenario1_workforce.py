"""Director Scenario1 workforce candidate; no NES/DOS source changes.

Seven named civilian jobs; historical initial civilian allocation is distinct
from the pre-existing 100-men-per-group military ledger. Dynamic inflows use
legacy growth/demobilization quantities, whose troop conversion remains OPEN.
"""
from __future__ import annotations
import json
from pathlib import Path
from og.core.fixed import CoreError

POLICY='OG_S1_ROM_CIVILIAN_DIV10_EQUAL7_V1'
JOBS=('farmers','town','artisans','metalworkers','resources','land','flood')
DATA=Path(__file__).with_name('data')/'scenario1_rom_civilian_baseline.json'


def fail(message):
    raise CoreError('BAD_CIVILIAN_WORKFORCE',message)


def baseline():
    d=json.loads(DATA.read_text(encoding='utf8'))
    rows=d.get('rows')
    if d.get('scenario')!=1 or not isinstance(rows,list) or len(rows)!=41:
        fail('41 original provinces required')
    if [r.get('province_id') for r in rows]!=list(range(1,42)):
        fail('province sequence invalid')
    if any(type(r.get('rom_displayed_civilian_population_hundreds')) is not int
           or r['rom_displayed_civilian_population_hundreds']<0
           or r['rom_displayed_civilian_population_hundreds']%10
           or r.get('assignment_units')!=r['rom_displayed_civilian_population_hundreds']//10
           for r in rows):fail('bad source units or conversion')
    if sum(r['rom_displayed_civilian_population_hundreds'] for r in rows)!=104900:
        fail('original ROM civilian total changed')
    if sum(r['assignment_units'] for r in rows)!=10490:
        fail('assignment total changed')
    return rows


def even_workers(n):
    if type(n) is not int or n<0:fail('invalid civilian allocation count')
    q,r=divmod(n,len(JOBS))
    return {**{k:q+(i<r) for i,k in enumerate(JOBS)},'unassigned':0}


def _spread_inflow(w,n):
    # Provisional deterministic treatment of legacy monthly growth/returns.
    # Distribute into the least-staffed approved categories, not a new UI job.
    q,r=divmod(n,len(JOBS))
    for k in JOBS:w[k]+=q
    for k in sorted(JOBS,key=lambda key:(w[key],JOBS.index(key)))[:r]:w[k]+=1
    w['unassigned']=0


def sync(s,origin):
    if s.get('workforce_policy')!=POLICY:return
    ledger=s['civilian_workforce_ledger']
    for pid,p in sorted(s['core']['provinces'].items(),key=lambda t:int(t[0][1:])):
        w=p['workers'];n=w['unassigned']
        if n:_spread_inflow(w,n)
        new=sum(w[k] for k in JOBS)
        old=s['civilian_workforce_current'][pid]
        if new!=old or n:
            ledger.append({'origin':origin,'province_id':pid,'before':old,'after':new,
                           'delta':new-old,'legacy_unassigned_inflow':n})
        s['civilian_workforce_current'][pid]=new


def validate(s):
    if s.get('workforce_policy')!=POLICY:return True
    rows=baseline()
    expected={f'p{r["province_id"]}':r['assignment_units'] for r in rows}
    b=s.get('civilian_workforce_baseline');current=s.get('civilian_workforce_current')
    if not isinstance(b,dict) or set(b)!=set(expected):fail('baseline keys')
    if not isinstance(current,dict) or set(current)!=set(expected):fail('current keys')
    for r in rows:
        pid=f'p{r["province_id"]}'
        if b[pid]!={'rom_displayed_population':r['rom_displayed_civilian_population_hundreds'],
                    'initial_assignment_units':r['assignment_units']}:
            fail('ROM baseline tampered: '+pid)
    totals=dict(expected)
    for event in s.get('civilian_workforce_ledger',[]):
        pid=event.get('province_id')
        if pid not in totals or event.get('before')!=totals[pid] or type(event.get('delta')) is not int:
            fail('workforce event chain')
        if event.get('after')!=event['before']+event['delta']:
            fail('workforce event delta')
        totals[pid]=event['after']
    for pid,p in s['core']['provinces'].items():
        w=p['workers']
        if w.get('unassigned')!=0:fail('unassigned category resurrected: '+pid)
        if current[pid]!=sum(w[k] for k in JOBS) or current[pid]!=totals[pid]:
            fail('workforce sum/ledger mismatch: '+pid)
    return True
