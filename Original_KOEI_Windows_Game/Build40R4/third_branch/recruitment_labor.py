"""Enhanced Scenario1: atomic recruitment from the SEVEN approved civilian jobs.

Only the original-game Enhanced Historical S1 Campaign opts into this adapter.
Synthetic and old saved core worlds retain their legacy historical semantics.
"""
from og.core.commands import _check_amount, _officer_in
from og.core.world2 import FIXTURE_V1
from og.core.fixed import CoreError
from .scenario1_workforce import JOBS, _spread_inflow


def available(workers, *, exclude=()):
    return sum(workers[k] for k in JOBS if k not in exclude)


def draw(workers, count, *, exclude=()):
    """Remove exactly count real workers, treating the jobs fairly and deterministically.

    Largest remaining pool first, stable JOBS order on ties; caller stages atomic
    deepcopy. Never debit the internal retired `unassigned` field.
    """
    if type(count) is not int or count < 1:
        raise CoreError('BAD_PARAMS','count must be a positive integer')
    allowed=tuple(k for k in JOBS if k not in exclude)
    if sum(workers[k] for k in allowed)<count:
        raise CoreError('NOT_ENOUGH_WORKERS','approved civilian jobs lack %d workers'%count)
    out={}
    # Distribute removals evenly to preserve the province's diverse workforce.
    for _ in range(count):
        k=max(allowed,key=lambda k:(workers[k],-JOBS.index(k)))
        workers[k]-=1
        out[k]=out.get(k,0)+1
    assert sum(out.values())==count
    return out


def restore(workers,count):
    if type(count) is not int or count<1:
        raise CoreError('BAD_PARAMS','bad demobilized group count')
    _spread_inflow(workers,count)


def recruit_from_jobs(d,actor,params):
    # Recover the original Build07 explicit seven-job selection. The later
    # default automatic balancing algorithm remains unchanged.
    if not isinstance(params,dict) or set(params) not in (
            {'province_id','count'}, {'province_id','count','worker_source'}):
        raise CoreError('BAD_PARAMS','recruit requires province_id/count and optional worker_source')
    source=params.get('worker_source')
    if source is not None and (type(source) is not str or source not in JOBS):
        raise CoreError('BAD_PARAMS','worker_source must be one of the seven actual civilian jobs')
    _check_amount(d,{'province_id':params.get('province_id'),'count':params.get('count')},
                  ('province_id','count'),'count')
    officer,bp_cost=_officer_in(d,actor,params['province_id'])
    prov=d['provinces'][params['province_id']]
    jobs=prov['workers']
    if jobs['unassigned'] != 0:
        raise CoreError('BAD_CIVILIAN_WORKFORCE','legacy unassigned balance must be migrated first')
    if source is None:
        taken=draw(jobs,params['count'])
    else:
        if jobs[source] < params['count']:
            raise CoreError('NOT_ENOUGH_WORKERS','requested source job has insufficient civilian workers')
        jobs[source]-=params['count']
        taken={source:params['count']}
    officer['bp']-=bp_cost
    prov['troops']+=params['count']
    prov['support_ticks']=max(0,prov['support_ticks']-params['count']*FIXTURE_V1['recruit_support_loss_ticks_per_troop'])
    return {'province_id':params['province_id'],'troops_after':prov['troops'],
            'support_ticks_after':prov['support_ticks'],'bp_after':officer['bp'],
            'civilian_workers_recruited':params['count'],'civilians_converted':params['count'],'worker_sources':taken}
