"""B5-0040: the historical aggregate fallback must obey the same Q08 ROM table
as the default officer-level route; no direct random-weather overwrite.
"""
from unittest.mock import patch
from third_branch import integrated, officer_battle_adapter as officer
from third_branch.integrated import new_campaign, save_bytes, load_bytes, validate


def _aggregate(seed, weather):
    c = new_campaign(seed)
    c.do('attack', 'A3', {'target':'p5','troops':150},f'b5:0040:attack:{seed}')
    b = c.state['battle']
    b['combat_rule']='ROTK2_UNIFIED_V1_4_AGGREGATE_ADAPTER'
    b.pop('officer_units', None)
    b['day']=3
    b['wind']=6
    b['weather']=weather
    b['burning']=[]
    b['a']['q']=0;b['a']['r']=0
    b['d']['q']=10;b['d']['r']=10
    return c


def test_aggregate_forbidden_clear_to_rain_rejected_and_persisted():
    c = _aggregate(40401, 0)
    def roll(_state,n):
        return {7:6,100:0,4:3,10:0}.get(n,0)
    with patch.object(integrated,'randrange',side_effect=roll), \
         patch.object(officer,'weather_transition',wraps=officer.weather_transition) as spy:
        c.do('battle_end_turn','A3',{},'b5:0040:forbidden')
    assert c.state['battle']['weather']==0, '0->3 forbidden by verified Q08 table'
    assert spy.call_count==2, 'preview and commit must share transition contract'
    assert all(call.args==(0,3,0) for call in spy.call_args_list)
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_aggregate_allowed_cloudy_to_rain_clears_fire():
    c = _aggregate(40402, 1)
    b=c.state['battle'];b['burning']=[[4,6]]
    def roll(_state,n):return {7:0,100:0,4:3,10:9}.get(n,0)
    with patch.object(integrated,'randrange',side_effect=roll), \
         patch.object(officer,'weather_transition',wraps=officer.weather_transition) as spy:
        c.do('battle_end_turn','A3',{},'b5:0040:allowed')
    b=c.state['battle']
    assert b['weather']==3 and b['burning']==[]
    assert spy.call_count==2
    assert all(call.args==(1,3,9) for call in spy.call_args_list)
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
