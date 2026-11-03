"""Q09 port-level reserve termination regression; no NES hardware parity claim."""
from third_branch import officer_battle_adapter as ba


def battle(fled):
    return {'rice_a':10000,'rice_d':10000,'officer_units':[
        {'side':'A','reserve':True,'fled':fled,'troops100':4000},
        {'side':'D','reserve':False,'fled':False,'troops100':6000},
    ]}


def test_fled_reserve_is_not_a_pending_army_and_defender_wins():
    b=battle(True)
    assert not ba._pending(b,'A')
    assert ba._result(None,b)=='DEFENDER'


def test_live_reserve_remains_pending_and_prevents_premature_defeat():
    b=battle(False)
    assert ba._pending(b,'A')
    assert ba._result(None,b) is None
