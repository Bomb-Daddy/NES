"""Director v1.3 Scenario1 workforce, historical-only; old synthetic fixtures unchanged.

Original NES population is in displayed hundreds. One civilian assignment unit
is displayed_population/10, not a 100-man army group. Army baseline stays 2670.
"""
import copy
import pytest
from og.core.fixed import CoreError
from third_branch.historical_s1 import new_campaign_historical_s1
from third_branch.integrated import save_bytes, load_bytes, validate
from third_branch import army_cohorts as ac
from third_branch.province_stage import snapshot

JOBS=('farmers','town','artisans','metalworkers','resources','land','flood')
POP=(650,700,1000,1000,700,5000,1200,800,3500,8000,4500,2500,2400,2300,800,1500,2000,2500,9000,8000,6000,5000,6500,3500,1500,700,800,1500,3000,3000,3000,2500,3500,1500,800,850,600,550,600,650,800)


def test_exact_41_rom_displayed_population_div10_seven_equal_buckets():
    c=new_campaign_historical_s1(104, balance_policy='OG_1000_DIRECTOR_V2')
    s=c.state
    assert len(POP)==41 and sum(POP)==104900
    assert sum(ac.army_total(a) for a in s['armies'].values())==2670
    assert len(s['core']['officers'])==107
    assert s['starting_army_evidence']['initial_troop_groups']==2670
    for i,pop in enumerate(POP,1):
        p=s['core']['provinces'][f'p{i}'];w=p['workers'];total=pop//10
        assert pop%10==0
        assert sum(w[k] for k in JOBS)==total
        assert w['unassigned']==0
        assert max(w[k] for k in JOBS)-min(w[k] for k in JOBS)<=1
        assert s['civilian_workforce_baseline'][f'p{i}']['rom_displayed_population']==pop
        assert s['civilian_workforce_baseline'][f'p{i}']['initial_assignment_units']==total
    assert sum(sum(p['workers'][k] for k in JOBS) for p in s['core']['provinces'].values())==10490
    assert [s['core']['provinces'][p]['workers'][JOBS[0]] for p in ('p9','p19','p38')]==[50,129,8]
    assert 'Unassigned' not in dict(snapshot(c,9).workers)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_population_tamper_rejected_atomically():
    c=new_campaign_historical_s1(105);before=c.fingerprint
    bad=copy.deepcopy(c.state)
    bad['core']['provinces']['p9']['workers']['unassigned']=1
    with pytest.raises(CoreError):validate(bad)
    bad=copy.deepcopy(c.state)
    bad['civilian_workforce_baseline']['p19']['initial_assignment_units']=90
    with pytest.raises(CoreError):validate(bad)
    assert c.fingerprint==before


def test_monthly_12_months_preserve_seven_job_categories_and_coldload():
    c=new_campaign_historical_s1(106,balance_policy='OG_1000_DIRECTOR_V2')
    for month in range(1,13):
        c.advance_month()
        assert c.state['month']==month+1
        assert all(p['workers']['unassigned']==0 for p in c.state['core']['provinces'].values())
        assert all(sum(p['workers'][k] for k in JOBS)>=0 for p in c.state['core']['provinces'].values())
        assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
