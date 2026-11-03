# Recovered from original eight-part Fork4 Build07; adapted solely for renamed
# modern error-contract identifiers. Original bytes retained in recovery provenance.
"""RED-before/green-after: Scenario1 recruitment uses seven real civilian jobs."""
import pytest
from og.core.fixed import CoreError
from third_branch import army_cohorts as ac
from third_branch.historical_s1 import new_campaign_historical_s1
from third_branch.integrated import save_bytes,load_bytes,hash_state
from third_branch.scenario1_workforce import JOBS


def start(pid):
    c=new_campaign_historical_s1(37,balance_policy='OG_1000_DIRECTOR_V2')
    actor=next(oid for oid,o in sorted(c.state['core']['officers'].items())
        if o['province_id']==pid and c.state['owners'][pid]==c.state['officer_faction'][oid]
        and ac.capacity_available(c.state,oid,1) and o['bp']>=1)
    return c,actor


@pytest.mark.parametrize('pid',['p4','p6','p21'])
def test_historical_recruit_preview_commit_coldload_conserves_civilians_and_military(pid):
    c,actor=start(pid)
    s=c.state;p=s['core']['provinces'][pid]
    before_jobs=sum(p['workers'][k] for k in JOBS)
    before_troops=p['troops'];before_fingerprint=c.fingerprint
    q=c.preview('recruit',actor,{'province_id':pid,'count':1},'recruit-test')
    assert c.fingerprint==before_fingerprint
    assert q['result']['civilians_converted']==1
    assert sum(q['result']['worker_sources'].values())==1
    assert set(q['result']['worker_sources']).issubset(JOBS)
    c.execute('recruit',actor,{'province_id':pid,'count':1},'recruit-test',q['quote_id'],q['revision'])
    p=c.state['core']['provinces'][pid]
    assert sum(p['workers'][k] for k in JOBS)==before_jobs-1
    assert p['troops']==before_troops+1
    assert p['workers']['unassigned']==0
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
    assert c.state['civilian_workforce_current'][pid]==before_jobs-1


def test_explicit_job_selection_and_all_or_nothing_errors():
    c,actor=start('p4');p=c.state['core']['provinces']['p4']
    n=p['workers']['town'];before=c.fingerprint
    for params,code in [({'province_id':'p4','count':n+1,'worker_source':'town'},'NOT_ENOUGH_WORKERS'),
                        ({'province_id':'p4','count':1,'worker_source':'unassigned'},'BAD_PARAMS'),
                        ({'province_id':'p4','count':True},'NOT_INT'),
                        ({'province_id':'p4','count':999999},'GENERAL_TROOP_CAP')]:
        with pytest.raises(CoreError) as e:c.preview('recruit',actor,params,'invalid')
        assert e.value.code==code
        assert c.fingerprint==before
    c.do('recruit',actor,{'province_id':'p4','count':2,'worker_source':'town'},'town-two')
    assert c.state['core']['provinces']['p4']['workers']['town']==n-2
    assert c.state['core']['provinces']['p4']['workers']['unassigned']==0


def test_stale_quote_cancel_and_troop_cap_atomic():
    c,actor=start('p6');before=c.fingerprint
    params={'province_id':'p6','count':1}
    q=c.preview('recruit',actor,params,'stale')
    assert c.fingerprint==before  # cancellation is simply no commit
    with pytest.raises(CoreError) as e:c.execute('recruit',actor,params,'stale','bad-quote',q['revision'])
    assert e.value.code=='STALE_QUOTE' and c.fingerprint==before
    s=c.state;aid=ac.army_id(actor)
    a=s['armies'].get(aid)
    if a:
        # Use the genuine cap, not a mocked handler. Make room then fill to 100.
        n=100-ac.army_total(a)
        if n and n<=s['core']['provinces']['p6']['workers']['town']:
            c.do('recruit',actor,{'province_id':'p6','count':n,'worker_source':'town'},'fill-cap')
            assert ac.army_total(c.state['armies'][aid])==100
            before=c.fingerprint
            with pytest.raises(CoreError) as e:c.preview('recruit',actor,params,'over-cap')
            assert e.value.code=='GENERAL_TROOP_CAP' and c.fingerprint==before


def test_disband_returns_to_real_jobs_not_phantom_pool():
    c,actor=start('p6');pid='p6';p=c.state['core']['provinces'][pid]
    n=sum(p['workers'][k] for k in JOBS)
    c.do('recruit',actor,{'province_id':pid,'count':1},'recruit-before-disband')
    c.do('disband_troops',actor,{'count':1},'disband-after-recruit')
    p=c.state['core']['provinces'][pid]
    assert sum(p['workers'][k] for k in JOBS)==n
    assert p['workers']['unassigned']==0
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
