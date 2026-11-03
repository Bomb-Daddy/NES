"""Director-required remote ruler -> governor reward; never a province-permission change."""
import pytest
from og.core.fixed import CoreError
from third_branch.integrated import new_campaign, save_bytes, load_bytes
from third_branch.orders_ui import eligible_governors, SPECS, params_for_order

@pytest.fixture
def c():
    x=new_campaign(20261031)
    # A2 is stationed in a different faction-A province than the ruler A1.
    assert x.state['core']['officers']['A1']['province_id']=='p1'
    assert x.state['core']['officers']['A2']['province_id']=='p2'
    x.state['governors']['p2']={'officer':'A2','focus':'farms'}
    return x

def cmd(c, method='gold', gold=15,actor='A1',target='A2'):
    return c.do('reward_governor',actor,{'target_governor':target,'method':method,'gold':gold},'reward-governor-'+str(c.state['revision']))

def bad(c, code, **kwargs):
    before=c.fingerprint
    with pytest.raises(CoreError) as e: cmd(c,**kwargs)
    assert e.value.code==code
    assert c.fingerprint==before

def test_roster_shows_all_appointed_owned_governors_and_loyalty(c):
    assert eligible_governors(c.state,'A')==[('A2','p2',65)]
    assert eligible_governors(c.state,'B')==[]
    assert 'Reward Governor' in SPECS
    kind,p=params_for_order('Reward Governor',{'target_governor':'A2','method':'gold','gold':'15'},'p1')
    assert (kind,p)==('reward_governor',{'target_governor':'A2','method':'gold','gold':15})

def test_ruler_rewards_remote_governor_gold_from_ruler_province_atomic_and_saved(c):
    home=c.state['core']['provinces']['p1'];remote=c.state['core']['provinces']['p2']
    pre_home=home['treasury_gold_cents'];pre_remote=remote['treasury_gold_cents'];pre_bp=c.state['core']['officers']['A1']['bp']
    before_owners=dict(c.state['owners']); before_governors=dict(c.state['governors'])
    out=cmd(c)
    assert out['command']=='reward_governor' and out['governor_province']=='p2' and out['source_province']=='p1'
    assert c.state['core']['provinces']['p1']['treasury_gold_cents']==pre_home-1500
    assert c.state['core']['provinces']['p2']['treasury_gold_cents']==pre_remote
    assert c.state['core']['officers']['A1']['bp']==pre_bp-6
    assert c.state['officer_loyalty']['A2']>65
    assert c.state['owners']==before_owners and c.state['governors']==before_governors
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint

def test_only_ruler_may_reward_governor(c):
    bad(c,'REQUIRES_RULER',actor='A2')

def test_not_appointed_is_rejected(c):
    bad(c,'NOT_APPOINTED_GOVERNOR',target='A3')
    c.state['governors'].clear()
    bad(c,'NOT_APPOINTED_GOVERNOR')

def test_horse_from_ruler_home_inventory_not_governor(c):
    bad(c,'NO_HORSE',method='horse')
    c.state['reward_horses']={'p1':1,'p2':3}
    out=cmd(c,method='horse')
    assert out['cost_cents']==0
    assert c.state['reward_horses']=={'p1':0,'p2':3}
    assert out['loyalty_after']>65

def test_not_enough_gold_bad_method_bad_quantity_and_no_bp_are_atomic(c):
    c.state['core']['provinces']['p1']['treasury_gold_cents']=50
    bad(c,'NO_GOLD',gold=15)
    bad(c,'BAD_QUANTITY',gold=0)
    bad(c,'BAD_REWARD_METHOD',method='writings')
    c.state['core']['provinces']['p1']['treasury_gold_cents']=300
    c.state['core']['officers']['A1']['bp']=0
    bad(c,'NO_BP',gold=1)

def test_no_foreign_governor(c):
    c.state['governors']['p4']={'officer':'B1','focus':'farms'}
    assert ('B1','p4',65) not in eligible_governors(c.state,'A')
    bad(c,'NOT_APPOINTED_GOVERNOR',target='B1')

def test_brotherhood_does_not_control_governor_permissions(c):
    c.state['sworn_brothers']={'A':['A2']}
    c.state['officer_loyalty']['A2']=100
    assert eligible_governors(c.state,'A')==[('A2','p2',100)]
    cmd(c, gold=1)
    assert c.state['owners']['p2']=='A' and c.state['governors']['p2']['officer']=='A2'

def test_real_game_ui_issue_permits_ruler_from_other_owned_province_and_saves(c,tmp_path):
    """Contract test of actual StrategyGame.issue without needing a display server."""
    from third_branch.strategic_game import StrategicWindow
    class Audio:
        def effect(self,*_): pass
    class Message:
        def set(self,*_): pass
    class Dummy:
        pass
    fake=Dummy()
    fake.c=c
    fake.player='A'
    fake.selected=2 # viewing governor's province, ruler located p1
    fake.counter=0
    fake.path=tmp_path/'campaign.sav'
    fake.audio=Audio();fake.msg=Message()
    fake.redraw=lambda:None
    fake._show_committed_events=lambda events:None
    before=c.state['officer_loyalty']['A2']
    assert StrategicWindow.issue(fake,'reward_governor',{'target_governor':'A2','method':'gold','gold':1},actor='A1',confirm=False)
    assert fake.path.exists()
    assert fake.c.state['officer_loyalty']['A2']>before
    assert fake.c.state['owners']['p2']=='A'
