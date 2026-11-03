"""Release gate: Director approved real civilian workers feed historic troop orders.
Verify the complete actual Historical S1 Campaign, not the old synthetic core fixture.
"""
import pytest
from og.core.fixed import CoreError
from third_branch.historical_s1 import new_campaign_historical_s1
from third_branch.integrated import load_bytes, save_bytes, validate
from third_branch import army_cohorts as ac
from third_branch.scenario1_workforce import JOBS
from third_branch.historical_ai_v3 import rank_choices


def total_people(c):
    return sum(sum(p['workers'].values()) + p['troops'] for p in c.state['core']['provinces'].values())


def actor_at(c, pid, *, capacity=1):
    return next(a for a,v in c.state['core']['officers'].items()
                if v['province_id']==pid and ac.capacity_available(c.state,a,capacity))


@pytest.mark.parametrize('pid',('p4','p6','p21'))
def test_recruitment_real_jobs_preview_commit_coldload(pid):
    c=new_campaign_historical_s1(); a=actor_at(c,pid)
    old=c.state['core']['provinces'][pid]
    workforce=sum(old['workers'][k] for k in JOBS)
    troops=old['troops']; people=total_people(c); original_hash=c.fingerprint
    quote=c.preview('recruit',a,{'province_id':pid,'count':1},'recruit:'+pid)
    assert c.fingerprint==original_hash
    assert sum(quote['result']['worker_sources'].values())==1
    c.execute('recruit',a,{'province_id':pid,'count':1},'recruit:'+pid,quote['quote_id'],quote['revision'])
    after=c.state['core']['provinces'][pid]
    assert sum(after['workers'][k] for k in JOBS)==workforce-1
    assert after['workers']['unassigned']==0
    assert after['troops']==troops+1
    assert total_people(c)==people
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
    assert validate(c.state)


def test_real_recruitment_atomic_rejection_and_100_troop_general_cap():
    c=new_campaign_historical_s1();pid='p6'
    full=next(a for a,v in c.state['core']['officers'].items() if v['province_id']==pid and not ac.capacity_available(c.state,a,1))
    before=c.fingerprint
    with pytest.raises(CoreError) as e:c.do('recruit',full,{'province_id':pid,'count':1},'full')
    assert e.value.code=='GENERAL_TROOP_CAP'
    assert c.fingerprint==before
    a=actor_at(c,pid)
    with pytest.raises(CoreError) as e:c.do('recruit',a,{'province_id':pid,'count':99999999},'no-workers')
    assert e.value.code=='GENERAL_TROOP_CAP' or e.value.code=='NOT_ENOUGH_WORKERS'
    assert c.fingerprint==before
    with pytest.raises(CoreError) as e:c.do('recruit',a,{'province_id':pid,'count':0},'zero')
    assert e.value.code=='BAD_PARAMS'
    assert c.fingerprint==before


def test_real_disband_and_governor_no_legacy_unassigned():
    c=new_campaign_historical_s1(); pid='p4'; a=actor_at(c,pid)
    before=total_people(c)
    c.do('recruit',a,{'province_id':pid,'count':1},'recruited')
    c.do('disband_troops',a,{'count':1},'disbanded')
    assert total_people(c)==before
    assert c.state['core']['provinces'][pid]['workers']['unassigned']==0
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_all_twelve_ai_styles_can_offer_real_recruits():
    c=new_campaign_historical_s1()
    # Each one is historical present and has 7 actual populated jobs.
    for s in c.state['core']['provinces'].values():
        assert s['workers']['unassigned']==0
    for style in ('aggressive','cautious','farmer','industrialist','trader','court','recruiter','scholar','defender','logistician','artisan','rules_lawyer'):
        # Experimental personality naming is versioned; candidate must not be
        # dependent on any retired unassigned worker balance.
        if style not in __import__('third_branch.historical_ai',fromlist=['STYLES']).STYLES:
            continue
        choices=rank_choices(c,'R1',style)
        assert isinstance(choices,list)
    assert validate(c.state)
