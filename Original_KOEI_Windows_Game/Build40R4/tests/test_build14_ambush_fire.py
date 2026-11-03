"""Build14 source-based regression: saved burn-cell lists, Q07 ambush and Q08 wind/rain."""
import copy
from unittest.mock import patch
from third_branch.integrated import new_campaign, validate, save_bytes, load_bytes
from third_branch import officer_battle_adapter as ba
from third_branch import rotk2_battle_core as core


def battle():
    c=new_campaign(126)
    c.do('attack','A3',{'target':'p5','troops':150},'build14:start')
    return c


def test_saved_burning_lists_do_not_crash_reachable_and_block_temporary_fire_cells():
    c=battle();b=c.state['battle']
    b['burning']=[[1,5]]
    assert (1,5) not in core.reachable(b['terrain'],(0,6),6,(),b['burning'])
    assert load_bytes(save_bytes(c)).state['battle']['burning']==[[1,5]]
    r=c.do('battle_move','A3',{'officer_id':'A3','q':1,'r':6},'build14:move')
    assert r['moved']==[1,6]
    assert validate(c.state)


def test_forest_ambush_is_passive_and_damages_officer_after_move():
    c=battle(); b=c.state['battle']; u=ba.find(b,'A','A3');target=ba.find(b,'D','B2')
    assert u is not None and target is not None
    target['q']=2;target['r']=6
    row=list(b['terrain'][6]);row[2]='F';b['terrain'][6]=''.join(row)
    before=u['troops100']
    c.do('battle_move','A3',{'officer_id':'A3','q':1,'r':6},'build14:ambush')
    assert ba.find(c.state['battle'],'A','A3')['troops100']<before
    assert any(e['kind']=='forest_ambush' for e in c.state['battle'].get('combat_events',[]))
    assert validate(c.state)


def test_high_intelligence_mover_immune_to_passive_ambush():
    c=battle();b=c.state['battle'];u=ba.find(b,'A','A3');target=ba.find(b,'D','B2')
    target['q']=2;target['r']=6
    row=list(b['terrain'][6]);row[2]='F';b['terrain'][6]=''.join(row)
    u['intel']=90
    before=u['troops100']
    c.do('battle_move','A3',{'officer_id':'A3','q':1,'r':6},'build14:immune')
    assert ba.find(c.state['battle'],'A','A3')['troops100']==before


def test_wind_spread_replaces_old_burning_and_can_damage_unit():
    c=battle();b=c.state['battle'];b['burning']=[[4,6]];b['wind']=0;b['weather']=0
    t=ba.find(b,'D','B2');t['q']=4;t['r']=5
    before=t['troops100']
    with patch.object(core,'ignite',return_value=True):
        ba._spread_fire(c.state,b)
    assert [4,6] not in b['burning']
    assert [4,5] in b['burning']
    assert t['troops100']<=before
    assert validate(c.state)


def test_rain_and_no_wind_extinguish_old_fires():
    for weather,wind in [(3,0),(0,6)]:
        c=battle();b=c.state['battle'];b['burning']=[[4,6]]
        b['weather']=weather;b['wind']=wind
        ba._spread_fire(c.state,b)
        assert not b['burning']
