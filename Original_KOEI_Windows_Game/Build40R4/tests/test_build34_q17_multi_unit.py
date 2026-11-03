"""Q17 multi-unit fire evacuation through the real preview/execute/settle bus.

The zero-enemy state is deliberately injected at the boundary before terminal
settlement; ordinary combat may settle earlier. This is a defensive branch QA,
not evidence that the historical game routinely reaches this exact state.
"""
from unittest.mock import patch
import pytest
from third_branch.integrated import new_campaign,save_bytes,load_bytes,validate
from third_branch import officer_battle_adapter as ba,army_cohorts as ac
from og.core.fixed import CoreError


def _two_defenders_no_enemy():
    c=new_campaign(34017);s=c.state
    # Legal two-officer garrison: move B1's station and officer into p5.
    s['core']['officers']['B1']['province_id']='p5'
    s['armies']['army:B1']['stations']={'p5':300}
    ac.refresh(s);assert validate(s)
    c.do('attack','A3',{'target':'p5','troops':120},'q17:begin')
    b=c.state['battle']
    assert {u['id'] for u in ba._all(b,'D')}=={'B1','B2'}
    for u in b['officer_units']:
        if u['side']=='D':u['mp']=6;u['acted']=False
        else:u['troops100']=0;u['troops']=0
    b['terrain']=['.'*13 for _ in range(12)]
    b['burning']=[[u['q'],u['r']] for u in ba._all(b,'D')]
    assert ba._all(b,'A')==[]
    return c


def test_both_burning_cpu_defenders_evacuate_before_terminal_settlement_and_cold_load():
    c=_two_defenders_no_enemy();moves=[];original=ba._move
    def track(s,b,u,to):
        moves.append((u['id'],(u['q'],u['r']),to))
        return original(s,b,u,to)
    with patch.object(ba,'_move',side_effect=track):
        out=c.do('battle_end_turn','A3',{},'q17:round')
    assert out['resolved'] and c.state['battle'] is None
    # Preview and execute are independent transactional runs; each must move both.
    assert [id for id,*_ in moves]==['B1','B2','B1','B2']
    assert all(before!=after for _,before,after in moves)
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
    fingerprint=c.fingerprint
    with pytest.raises(CoreError):
        c.do('battle_end_turn','A3',{},'q17:round')
    assert c.fingerprint==fingerprint


def test_zero_mobility_defender_stays_while_other_evacuates():
    c=_two_defenders_no_enemy();b=c.state['battle']
    unit=ba.find(b,'D','B2');initial=(unit['q'],unit['r']);unit['mp']=0
    moved=[];original=ba._move
    def track(s,b,u,to):
        moved.append(u['id']);return original(s,b,u,to)
    with patch.object(ba,'_move',side_effect=track):
        out=c.do('battle_end_turn','A3',{},'q17:round')
    assert out['resolved'] and moved==['B1','B1']
    assert unit['q']==initial[0] and unit['r']==initial[1]
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
