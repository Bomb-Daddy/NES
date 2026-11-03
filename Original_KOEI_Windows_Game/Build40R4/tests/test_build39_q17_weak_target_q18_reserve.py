"""Q17 SNES ai_select_target weak-adjacent rule, and Q18 pending-reserve guard.

Q17 decoded source prioritizes an adjacent target below 1000 battle-scaled
soldiers when the actor holds >3000; no unsupported claim about other target
ranking. Q18 must not silently run Q09 queued reserves through abstract combat.
"""
from unittest.mock import patch
import pytest
from third_branch.integrated import new_campaign, validate
from third_branch import officer_battle_adapter as ba, offscreen_q18, army_cohorts as ac


def _battle():
    c=new_campaign(39017);s=c.state
    s['core']['officers']['B1']['province_id']='p5'
    s['armies']['army:B1']['stations']={'p5':300}
    ac.refresh(s);assert validate(s)
    c.do('attack','A3',{'target':'p5','troops':120},'q17:weak:begin')
    b=c.state['battle']
    assert [u['id'] for u in b['officer_units']]==['A3','B1','B2']
    b['terrain']=['.'*13 for _ in range(12)]
    b['burning']=[]
    actor=ba.find(b,'A','A3');actor['q'],actor['r']=5,5;actor['mp']=6;actor['acted']=False
    b1=ba.find(b,'D','B1');b1['q'],b1['r']=6,5;b1['troops100']=1500;b1['troops']=15
    b2=ba.find(b,'D','B2');b2['q'],b2['r']=5,6;b2['troops100']=900;b2['troops']=9
    return c,b,actor,b1,b2


def test_cpu_prioritizes_weak_adjacent_enemy_over_list_order():
    c,b,actor,b1,b2=_battle()
    hits=[]
    with patch.object(ba,'_exchange',side_effect=lambda s,b,u,v:hits.append(v['id'])):
        ba._auto_ai(c.state,b,'A')
    assert hits==['B2'], 'Q17 source: weak B2 should beat list-first B1'
    assert actor['acted']


def test_q18_pending_reinforcement_rejected_without_mutation():
    c,b,actor,b1,b2=_battle();c.state['human_faction']='C'
    b2['reserve']=True
    before=repr(b)
    with pytest.raises(ValueError,match='Q18_PENDING_REINFORCEMENTS_UNSUPPORTED'):
        offscreen_q18.resolve(c.state,b)
    assert repr(b)==before


def test_wounded_cpu_commander_holds_instead_of_suicidal_adjacent_attack():
    c,b,actor,b1,b2=_battle()
    actor['troops100']=2900;actor['troops']=29
    hits=[]
    with patch.object(ba,'_exchange',side_effect=lambda s,b,u,v:hits.append(u['id'])):
        ba._auto_ai(c.state,b,'A')
    assert not hits, 'Q17: commander below 3000 should retreat/hold, not attack'


def test_exact_3000_boundary_remains_attack_eligible():
    c,b,actor,b1,b2=_battle()
    actor['troops100']=3000;actor['troops']=30
    hits=[]
    with patch.object(ba,'_exchange',side_effect=lambda s,b,u,v:hits.append(u['id'])):
        ba._auto_ai(c.state,b,'A')
    assert hits==['A3']
