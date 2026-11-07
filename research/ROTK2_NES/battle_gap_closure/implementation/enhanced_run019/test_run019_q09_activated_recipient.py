"""Run019: a recorded live attacked-recipient must join a real later tactical
battle, not remain forever as an unconsumed second copy.

This is source-backed D648/D69A recipient carry-through, NOT evidence that the
original automatic simultaneous battle scheduling has been implemented.
"""
import copy
import pytest
from og.core.fixed import CoreError
from third_branch import army_cohorts as ac
from third_branch.integrated import validate, load_bytes, save_bytes
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).parent))
from test_run018_q09_attacked_arrival import ready, flee


def returning(seed=1901):
    c=ready(seed)
    flee(c)
    c.state['battle']['rice_a']=0
    c.do('battle_end_turn','B2',{},f'r019:end:{seed}')
    assert c.state['battle'] is None
    return c


def activate(c, request_id='r019:activate'):
    return c.do('attack','C2',{'target':'p6','troops':85,'gold':10},request_id)


def test_live_recipient_is_consumed_only_when_real_matching_tactical_battle_starts():
    c=returning()
    rec=copy.deepcopy(c.state['q09_pending_attacked_arrivals']['p6'][0])
    response=activate(c)
    assert response['battle_started']
    b=c.state['battle']
    assert b['dst']=='p6' and b['defender']=='B'
    assert 'p6' not in c.state.get('q09_pending_attacked_arrivals',{})
    matching=[u for u in b['officer_units'] if u['id']=='B9' and u['side']=='D' and not u.get('reserve')]
    assert len(matching)==1 and matching[0]['troops100']==rec['troops100']
    assert b['q09_activated_attacked_recipient']==[{'officer':'B9','troops100':5000,'province':'p6'}]
    assert any(e.get('kind')=='q09_recipient_tactical_activated' and e['officer']=='B9'
               for e in b['combat_events'])
    assert ac.strength(c.state,'B9','p6')==50
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_unrelated_attack_does_not_consume_p6_recipient():
    c=returning(1902)
    assert c.state['owners']['p4']=='B'
    response=c.do('attack','A3',{'target':'p4','troops':30,'gold':10},'r019:other')
    assert response['battle_started']
    assert c.state['q09_pending_attacked_arrivals']['p6'][0]['officer']=='B9'
    assert not c.state['battle'].get('q09_activated_attacked_recipient')
    assert validate(c.state)


def test_forged_recipient_strength_rejected_atomically_before_second_battle():
    c=returning(1903)
    c.state['q09_pending_attacked_arrivals']['p6'][0]['troops100']=100
    assert validate(c.state)
    before=copy.deepcopy(c.state)
    with pytest.raises(CoreError,match='Q09_RECIPIENT_STRENGTH_MISMATCH'):
        activate(c,'r019:mismatch')
    assert c.state==before


def test_peaceful_arrival_does_not_create_recipient_activation():
    c=ready(1904,attacked=False)
    flee(c)
    c.state['battle']['rice_a']=0
    c.do('battle_end_turn','B2',{},'r019:peace-end')
    activate(c,'r019:peace-attack')
    assert not c.state['battle'].get('q09_activated_attacked_recipient')
    assert validate(c.state)


def test_activated_recipient_is_controllable_as_defending_officer():
    c=returning(1905)
    activate(c,'r019:controller-attack')
    b=c.state['battle'];b['duel_pending']=None
    assert any(u['id']=='B9' and u['side']=='D' for u in b['officer_units'])
    res=c.do('battle_wait','B9',{'officer_id':'B9'},'r019:defender-wait')
    assert res['officer']=='B9' and res['rested'] is True
    assert validate(c.state)


def test_nes_controller_wait_menu_routes_activated_defender_without_mouse():
    from types import SimpleNamespace
    from third_branch.tactical_view import TacticalWindow
    from third_branch.nes_battle_menu import NESBattleMenu
    c=returning(1906)
    activate(c,'r019:ui-attack')
    w=TacticalWindow.__new__(TacticalWindow)
    class Message:
        def set(self,value):self.value=value
    w.parent=SimpleNamespace(c=c)
    w.message=Message()
    w.menu=NESBattleMenu()
    w.selected_officer='B9'
    w.targeting=False
    w.pending_withdraw=None
    w.pending_prisoner=False
    w.controller_index=0
    w._cursor=lambda:None
    w.redraw=lambda:None
    captured=[]
    w._issue=lambda kind,params:captured.append((kind,params))
    for _ in range(5):w.on_nes_button('down')
    assert w.menu.labels[w.menu.index]=='Wait'
    w.on_nes_button('a')
    assert w.menu.branch=='Wait'
    w.on_nes_button('a')
    assert captured==[('battle_wait',{})]
    assert w.selected_officer=='B9'
