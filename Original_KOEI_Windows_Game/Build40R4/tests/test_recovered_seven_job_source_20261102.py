"""Recovered 8-part Fork4 Build07 job-specific recruitment plus later automatic policy."""
from third_branch.historical_s1 import new_campaign_historical_s1
from third_branch.orders_ui import SPECS, params_for_order
from third_branch.integrated import save_bytes, load_bytes
from third_branch import army_cohorts as ac
from third_branch.scenario1_workforce import JOBS
from og.core.fixed import CoreError
import pytest

def actor(c,pid):
    return next(oid for oid,off in sorted(c.state['core']['officers'].items())
                if off['province_id']==pid and ac.capacity_available(c.state,oid,2)
                and c.state['owners'][pid]==c.state['officer_faction'][oid] and off['bp']>0)

def test_optional_job_selector_visible_and_default_backward_compatible():
    assert ('worker_source',('auto',)+tuple(JOBS),'auto') in SPECS['Recruit'][1]
    assert params_for_order('Recruit',{'count':'1'},'p4')==('recruit',{'province_id':'p4','count':1})
    assert params_for_order('Recruit',{'count':'1','worker_source':'auto'},'p4')==('recruit',{'province_id':'p4','count':1})
    assert params_for_order('Recruit',{'count':'2','worker_source':'town'},'p4')==('recruit',{'province_id':'p4','count':2,'worker_source':'town'})

def test_explicit_job_conserves_one_bucket_and_persists():
    c=new_campaign_historical_s1(37,balance_policy='OG_1000_DIRECTOR_V2');a=actor(c,'p4')
    before=dict(c.state['core']['provinces']['p4']['workers']);before_fp=c.fingerprint
    q=c.preview('recruit',a,{'province_id':'p4','count':2,'worker_source':'town'},'recover:town')
    assert c.fingerprint==before_fp and q['result']['civilians_converted']==2
    assert q['result']['worker_sources']=={'town':2}
    c.execute('recruit',a,{'province_id':'p4','count':2,'worker_source':'town'},'recover:town',q['quote_id'],q['revision'])
    after=c.state['core']['provinces']['p4']['workers']
    assert after['town']==before['town']-2
    assert all(after[j]==before[j] for j in JOBS if j!='town')
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint

def test_job_source_invalid_and_overdraw_are_atomic():
    c=new_campaign_historical_s1(37,balance_policy='OG_1000_DIRECTOR_V2');a=actor(c,'p4');before=c.fingerprint
    n=c.state['core']['provinces']['p4']['workers']['town']
    for src,qty in [('unassigned',1),('town',n+1)]:
        with pytest.raises(CoreError):c.preview('recruit',a,{'province_id':'p4','count':qty,'worker_source':src},'recover:bad:'+src)
        assert c.fingerprint==before
