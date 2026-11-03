import copy, tempfile
from pathlib import Path
import pytest
from og.core.fixed import CoreError
from third_branch.historical_s1 import new_campaign_historical_s1
from third_branch.integrated import hash_state,save_file,load_file
from third_branch.balance_policy_1000 import post_month_director_v2
from third_branch.orders_ui import SPECS,params_for_order

POL='OG_1000_DIRECTOR_V2'

def target(c):
    s=c.state
    for oid,off in sorted(s['core']['officers'].items()):
        pid=off['province_id'];army=s['armies'].get('army:'+oid)
        if not army or set(army['stations'])!={pid}:continue
        if s['governors'].get(pid,{}).get('officer')==oid:continue
        for dst in s['neighbors'][pid]:
            if s['owners'][dst] is None:
                try:
                    c.preview('relocate_army',oid,{'target':dst},'target-probe-'+oid)
                except CoreError:continue
                return oid,pid,dst
    raise AssertionError('No playable neutral front')

def test_occupation_rejections_and_atomic_save():
    c=new_campaign_historical_s1(seed=17,balance_policy=POL)
    oid,src,dst=target(c);s=c.state
    assert s['owners'][dst] is None
    before=hash_state(s)
    for params in ({'target':'p404'}, {'target':src}, {'target':'p0'}):
        with pytest.raises(CoreError):c.do('relocate_army',oid,params,'neg:'+str(params))
        assert c.fingerprint==before
    enemy=next(p for p,o in s['owners'].items() if o not in (None,s['officer_faction'][oid]))
    with pytest.raises(CoreError):c.do('relocate_army',oid,{'target':enemy},'owned')
    assert c.fingerprint==before
    rev=c.state['revision'];n=s['armies']['army:'+oid]['stations'][src];food=s['core']['provinces'][src]['food_units']
    r=c.do('relocate_army',oid,{'target':dst},'occupy-one')
    assert r['troops']==n and c.state['owners'][dst]==s['officer_faction'][oid]
    assert c.state['core']['provinces'][dst]['troops']==n
    assert c.state['core']['officers'][oid]['province_id']==dst
    assert c.state['core']['provinces'][src]['food_units']==food-max(1,(n+59)//60)
    assert c.state['revision']==rev+1
    after=c.fingerprint
    with pytest.raises(CoreError):c.do('relocate_army',oid,{'target':dst},'occupy-two')
    with pytest.raises(CoreError):c.do('relocate_army',oid,{'target':dst},'occupy-one')
    with pytest.raises(CoreError):c.execute('relocate_army',oid,{'target':dst},'stale',expected_revision=rev)
    assert c.fingerprint==after
    with tempfile.TemporaryDirectory() as d:
        file=Path(d)/'new.ogsave';save_file(c,file);loaded=load_file(file)
        assert loaded.fingerprint==c.fingerprint
        assert loaded.state['core']['provinces'][dst]['troops']==n
    order,par=params_for_order('Move Army',{'target':dst},src)
    assert order=='relocate_army'

def test_civilians_do_not_eat_and_metal_sites_gate():
    c=new_campaign_historical_s1(seed=17,balance_policy=POL)
    oid,src,dst=target(c)
    s=c.state; p=s['core']['provinces'][src]
    # The NES starting-army fixture assigns troops to EVERY local general,
    # unlike the superseded one-commander synthetic bootstrap. Remove ALL
    # local standing troops for the no-soldiers consumption negative control.
    from third_branch import army_cohorts as ac
    ac.lose_at(s,src,s['core']['provinces'][src]['troops'])
    assert s['core']['provinces'][src]['troops']==0
    p['food_units']=100
    p['workers']['resources']=350
    s['extras'][src]['metal']=10
    post_month_director_v2(s)
    assert p['food_units']==100
    assert s['extras'][src]['metal']<=10
    # Positive sourced-map designation is a unit-test fixture, not a real location claim.
    s['metal_deposits'][src]=True
    start=s['extras'][src]['metal']
    p['workers']['metalworkers']=0
    post_month_director_v2(s)
    assert s['extras'][src]['metal']==start+10
    s['metal_deposits'][src]=False
    post_month_director_v2(s)
    assert s['extras'][src]['metal']==start+10

def test_market_does_not_require_counterparty_stock():
    c=new_campaign_historical_s1(seed=17,balance_policy=POL)
    oid,src,dst=target(c)
    p=c.state['core']['provinces'][src]
    c.state['market_stocks'][src]['furs']=0
    p['treasury_gold_cents']=50000
    c.do('market',oid,{'commodity':'furs','side':'buy','units':2},'market-stock-free')
    assert c.state['market_stocks'][src]['furs']==0
    assert c.state['extras'][src]['furs']==12

def test_legacy_policy_not_mutated():
    a=new_campaign_historical_s1(seed=17,balance_policy='OG_1000_SLOW_GROWTH_V1_CANDIDATE')
    assert 'metal_deposits' not in a.state
    a.advance_month()
    with tempfile.TemporaryDirectory() as d:
        file=Path(d)/'old.ogsave';save_file(a,file)
        assert load_file(file).fingerprint==a.fingerprint
