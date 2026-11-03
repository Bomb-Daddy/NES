"""Q08 day-boundary parity: legacy aggregate and default officer adapter."""
from unittest.mock import patch
from third_branch.integrated import new_campaign,save_bytes,load_bytes,validate
from third_branch import integrated
from third_branch import rotk2_battle_core as core


def aggregate(seed):
    c=new_campaign(seed)
    c.do('attack','A3',{'target':'p5','troops':150},f'q08:begin:{seed}')
    b=c.state['battle']
    b['combat_rule']='ROTK2_UNIFIED_V1_4_AGGREGATE_ADAPTER'
    b.pop('officer_units',None)  # exercise legacy aggregate adapter, not default officer route
    b['day']=3;b['wind']=0;b['weather']=0;b['burning']=[[4,6]]
    b['a']['q']=0;b['a']['r']=0
    b['d']['q']=10;b['d']['r']=10
    return c


def test_aggregate_day_four_rain_prevents_spread_instead_of_igniting_then_raining():
    c=aggregate(2201);b=c.state['battle']
    # Q08 Markov table allows cloudy (1) -> rain (3), but forbids clear (0) -> rain.
    b['weather']=1
    calls=[]
    def roll(_state,n):return {7:0,100:0,4:3}.get(n,0)
    def ignite(*args,**kwargs):
        calls.append((args,kwargs));return True
    with patch.object(integrated,'randrange',side_effect=roll),patch.object(core,'ignite',side_effect=ignite):
        c.do('battle_end_turn','A3',{},'q08:rain')
    b=c.state['battle']
    assert b is not None
    assert b['day']==4 and b['weather']==3
    assert b['burning']==[]
    assert calls==[]
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_aggregate_spread_damages_new_fire_hex_not_extinguished_old_hex():
    c=aggregate(2202);b=c.state['battle']
    b['d']['q']=4;b['d']['r']=5
    b['d']['mp']=0  # isolate fire from the defender's AI movement phase
    before=b['d']['troops100']
    def roll(_state,n):
        if n==100:return 99  # no weather change
        if n==7:return 0
        return max(0,n-1)
    with patch.object(integrated,'randrange',side_effect=roll),patch.object(core,'ignite',return_value=True):
        c.do('battle_end_turn','A3',{},'q08:spread')
    b=c.state['battle']
    assert b is not None
    assert [4,6] not in b['burning'] and [4,5] in b['burning']
    assert b['d']['troops100']<before
    assert validate(c.state)


def test_default_officer_route_already_uses_new_day_weather_and_wind():
    from third_branch import officer_battle_adapter as officer
    c=new_campaign(2203)
    c.do('attack','A3',{'target':'p5','troops':150},'q08:officer:start')
    b=c.state['battle']
    assert b['combat_rule']=='ROTK2_UNIFIED_V1_4_OFFICER_LEDGER'
    b['day']=3;b['wind']=6;b['weather']=0
    observed=[]
    def roll(_state,n):return {7:0,100:0,4:3}.get(n,0)
    with patch.object(officer,'randrange',side_effect=roll), \
         patch.object(officer,'weather_transition',wraps=officer.weather_transition) as transition, \
         patch.object(officer,'_auto_ai'), \
         patch.object(officer,'_spontaneous_switches'), \
         patch.object(officer,'_spread_fire',side_effect=lambda state,battle: observed.append((battle['day'],battle['wind'],battle['weather']))):
        c.do('battle_end_turn','A3',{},'q08:officer:day4')
    # Source correction: the NES Markov table at 0x1DCAC forbids 0 -> 3.
    # Old test asserted the obsolete direct rand(4) assignment.
    assert observed and all(x==(4,0,0) for x in observed)
    assert transition.call_count==2  # preview + execute
    assert all(c.args==(0,3,0) for c in transition.call_args_list)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
