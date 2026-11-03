"""Q19 CPU help request: real 41-map transaction and Q09 officer ledger."""
import pytest
from third_branch.integrated import new_campaign_41,load_bytes,save_bytes,validate
from third_branch import officer_battle_adapter as ba,army_cohorts as ac
from og.core.fixed import CoreError

@pytest.mark.parametrize('invader,defender,rice,expected',[
    (125,100,400,False),(130,100,200,False),(130,100,202,True),
    (0,100,300,False),(130,0,201,True)
])
def test_q19_strict_source_threshold(invader,defender,rice,expected):
    assert ba.q19_defender_help_threshold(invader,defender,rice) is expected

def test_q19_invalid_inputs():
    for bad in (-1,True,1.5,'150'):
        with pytest.raises(CoreError,match='Q19_INVALID_HELP_INPUT'):
            ba.q19_defender_help_threshold(bad,100,300)

def test_real_41_map_cpu_help_queues_named_army_and_cold_loads():
    c=new_campaign_41(40019);s=c.state
    s['human_faction']='A'
    # p9(A) attacks p16(B), with p17(B) an eligible adjacent relief source.
    ac.ensure(s,'O16')['stations']['p16']=50
    ac.refresh(s)
    assert validate(s)
    source_before=ac.strength(s,'O17','p17')
    c.do('attack','O09',{'target':'p16','troops':190},'q19:real:begin')
    b=c.state['battle']
    assert b['terrain_source']=='NES_ROM_VERIFIED_41'
    assert b['combat_rule']=='ROTK2_UNIFIED_V1_4_OFFICER_LEDGER'
    assert any(ev['kind']=='q19_cpu_defender_help_request' for ev in b['combat_events'])
    queued=[u for u in b['officer_units'] if u['side']=='D' and u.get('reserve')]
    assert len(queued)==1
    reserve=queued[0]
    assert reserve['id']=='O17' and reserve['slot']==1 and reserve['home']=='p17'
    assert reserve['arrive_day']==3 and reserve['troops100']==source_before*100
    assert ac.strength(c.state,'O17','p17')==0
    assert c.state['armies']['army:O17']['engaged']==source_before
    assert not ba.q19_request_cpu_defender_help(c.state,b)
    assert len([u for u in b['officer_units'] if u['side']=='D' and u.get('reserve')])==1
    assert validate(c.state)
    fresh=load_bytes(save_bytes(c))
    assert fresh.fingerprint==c.fingerprint
    assert fresh.state['battle']['officer_units']==b['officer_units']

def test_41_map_no_source_does_not_fabricate_reinforcements():
    c=new_campaign_41(40020);s=c.state;s['human_faction']='A'
    ac.ensure(s,'O16')['stations']['p16']=50
    ac.ensure(s,'O17')['stations']['p17']=0
    ac.ensure(s,'O18')['stations']['p18']=0
    ac.ensure(s,'O24')['stations']['p24']=0
    ac.refresh(s)
    c.do('attack','O09',{'target':'p16','troops':190},'q19:no-source')
    b=c.state['battle']
    assert not any(u['side']=='D' and u.get('reserve') for u in b['officer_units'])
    assert any(ev['kind']=='q19_help_unfulfilled' for ev in b['combat_events'])
    assert validate(c.state)
