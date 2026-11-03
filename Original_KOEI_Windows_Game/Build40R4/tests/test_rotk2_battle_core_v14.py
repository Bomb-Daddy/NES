"""Independent tests for the approved SNES/NES mechanics in the Enhanced adapter.

These execute actual shipping functions. They do not claim ROM trace parity.
"""
import copy
import pytest
from third_branch import rotk2_battle_core as core
from third_branch.integrated import new_campaign, load_bytes, save_bytes, validate
from og.core.fixed import CoreError


def test_all_936_neighbor_attempts_and_reciprocity():
    assert core.W*core.H*6==936
    directions=(lambda q: ((-1,1),(0,1),(1,1),(-1,0),(0,-1),(1,0)) if q%2 else
                        ((-1,0),(0,1),(1,0),(-1,-1),(0,-1),(1,-1)))
    count=0
    for q in range(13):
        for r in range(12):
            expected=set()
            for dq,dr in directions(q):
                x,y=q+dq,r+dr
                if 0<=x<13 and 0<=y<12:expected.add((x,y));count+=1
            assert set(core.neighbors(q,r))==expected
            for v in expected:
                assert (q,r) in core.neighbors(*v)
                assert core.distance((q,r),v)==1
    assert count==838  # 13 x 12 grid: interior six edges, perimeter clamped


def test_reference_mobility_and_move_costs_and_burning():
    assert core.COST=={'.':2,'F':3,'H':3,'M':7,'W':5,'T':3,'C':3,'S':0}
    assert [core.refill(t) for t in (0,1,10,20,21,40,41,60,61,80,81,100)] == [2,2,2,2,3,3,4,4,5,5,6,6]
    assert core.refill(5,6)==6
    g=['.'*13 for _ in range(12)]
    g[5]='..F..H..M.W.C'
    choices=core.reachable(g,(1,5),3)
    assert choices[(2,5)]==3
    assert (3,5) not in choices
    assert (2,5) not in core.reachable(g,(1,5),3,burning={(2,5)})


def test_simultaneous_loss_terrain_difficulty_and_determinism():
    a={'troops100':10000,'war':90,'morale':85,'troop_ratio':100}
    d={'troops100':10000,'war':40,'morale':70,'troop_ratio':100}
    def zero(n):return 0
    att,dfn=copy.deepcopy(a),copy.deepcopy(d)
    loss=core.exchange(att,dfn,'.','C',zero,difficulty=2,att_ai=False,dfn_ai=True)
    assert loss[0]>0 and loss[1]>0
    assert att['troops100']==10000-loss[0] and dfn['troops100']==10000-loss[1]
    att2,dfn2=copy.deepcopy(a),copy.deepcopy(d)
    assert core.exchange(att2,dfn2,'.','C',zero,difficulty=2,att_ai=False,dfn_ai=True)==loss
    att3,dfn3=copy.deepcopy(a),copy.deepcopy(d)
    other=core.exchange(att3,dfn3,'.','C',zero,difficulty=1,att_ai=False,dfn_ai=True)
    assert loss[1]<=other[1]


def test_rice_unit_scaling_and_fire_weather():
    assert core.rice_tick(45000,19000)==44367
    assert core.rice_tick(44367,19000)==43734
    assert core.rice_tick(30000,12000)==29600
    assert core.rice_tick(1,0)==0
    assert not core.ignite('F',100,0,True,3,lambda n:0)
    assert not core.ignite('W',100,0,False,1,lambda n:0)
    assert core.ignite('F',100,100,False,1,lambda n:0)


def test_real_campaign_source_loader_modes_and_cold_save():
    c=new_campaign(91)
    p=c.state['core']['provinces']['p3'];p['food_units']=500
    before=c.fingerprint
    c.do('attack','A3',{'target':'p5','troops':150},'combat:1')
    assert c.state['battle']['combat_rule']=='ROTK2_UNIFIED_V1_4_OFFICER_LEDGER'
    assert c.state['battle']['officer_units'] and c.state['battle']['officer_units'][0]['id']=='A3'
    assert c.state['battle']['rice_a']==15000
    assert c.state['battle']['day']==1
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
    c.do('battle_end_turn','A3',{},'combat:2')
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
    # Late battle after 30 days cannot result in invader victory by owning castle.
    a=load_bytes(save_bytes(c));b=load_bytes(save_bytes(c))
    for game in (a,b):game.do('battle_auto','A3',{},'combat:3')
    assert a.fingerprint==b.fingerprint
    assert a.state['battle'] is None
    assert a.state['history'][-1]['winner'] in ('ATTACKER','DEFENDER')


def test_player_move_rejects_teleport_without_state_mutation():
    c=new_campaign(24)
    c.do('attack','A3',{'target':'p5','troops':100},'combat:1')
    before=c.fingerprint
    with pytest.raises(CoreError) as err:c.do('battle_move','A3',{'q':12,'r':11},'combat:bad')
    assert err.value.code=='ILLEGAL_MOVE'
    assert c.fingerprint==before


def test_controller_methods_are_actual_game_commands():
    from third_branch.tactical_view import TacticalWindow
    assert all(callable(getattr(TacticalWindow,k,None)) for k in ('attack','charge','simult','fire','end_turn'))


def test_day_thirty_timeout_is_defensive_and_castle_not_instant_victory():
    c=new_campaign(88)
    c.do('attack','A3',{'target':'p5','troops':150},'timeout:start')
    b=c.state['battle']
    castle=next((q,r) for r,line in enumerate(b['terrain']) for q,t in enumerate(line) if t=='C')
    b['a']['q'],b['a']['r']=castle
    b['d']['q'],b['d']['r']=2,2
    b['day']=30
    assert c.state['battle'] is not None
    result=c.do('battle_end_turn','A3',{},'timeout:tick')
    assert result['resolved'] is True
    assert c.state['battle'] is None
    assert c.state['history'][-1]['winner']=='DEFENDER'
    assert validate(c.state)
