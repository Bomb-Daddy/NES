"""Q09 D648/D69A recipient attachment: SOURCE-FIXTURE/PORT PARTIAL.

Tests do NOT certify simultaneous second tactical activation or original RNG/maps.
They assert the source's newest-HEAD order and original battle 100x strength
when the existing Campaign explicitly starts the recipient tactical encounter.
"""
import copy
import pytest
from og.core.fixed import CoreError
from third_branch.integrated import validate, save_bytes, load_bytes
from third_branch import army_cohorts as ac
from test_run018_q09_attacked_arrival import ready


def pending_two(seed=1901):
    c=ready(seed)
    c.state['q09_source_officer_bytes']['B2']={'flag2':0x40}
    c.state['battle']['q09_source_officer_bytes']['B2']={'flag2':0x40}
    r=c.do('battle_withdraw','B2',{
        'whole_army':True,'destination':'p6',
        'officer_destinations':{'B2':'p6','B9':'p6'}},f'r019:{seed}:withdraw')
    assert r['resolved'] and not c.state['battle']
    assert [x['officer'] for x in c.state['q09_pending_attacked_arrivals']['p6']]==['B9','B2']
    return c


def attack_recipient(c, token='attack'):
    return c.do('attack','C2',{'target':'p6','troops':110,'gold':1},'r019:'+token)


def test_new_recipient_battle_promotes_source_head_order_and_consumes_pending_once():
    c=pending_two(1901)
    assert attack_recipient(c)['battle_started']
    b=c.state['battle']
    defenders=[u for u in b['officer_units'] if u['side']=='D']
    assert [u['id'] for u in defenders]==['B9','B2','C1']
    assert [u['troops100'] for u in defenders]==[5000,24000,24000]
    assert c.state.get('q09_pending_attacked_arrivals',{})=={}
    assert any(e['kind']=='q09_recipient_source_attached_port_partial' for e in b['combat_events'])
    assert ac.strength(c.state,'B9','p6')==50
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_attacked_recipient_original_fractional_battle_scale_is_preserved():
    c=ready(1902)
    u=next(x for x in c.state['battle']['officer_units'] if x['id']=='B9')
    u['troops100']=5075  # Original battle hundredths not exactly strategic * 100.
    c.do('battle_withdraw','B2',{'officer_id':'B9','destination':'p6'},'r019:1902:withdraw')
    assert c.state['q09_pending_attacked_arrivals']['p6'][0]['troops100']==5075
    assert ac.strength(c.state,'B9','p6')==50
    c.state['battle']['rice_a']=0
    c.do('battle_end_turn','B2',{},'r019:1902:settle')
    assert c.state['battle'] is None
    attack_recipient(c,'1902:attack')
    unit=next(u for u in c.state['battle']['officer_units'] if u['id']=='B9')
    assert unit['troops100']==5075
    assert ac.strength(c.state,'B9','p6')==50
    assert validate(c.state)


def test_pending_cold_load_then_new_tactical_battle_uses_source_order():
    c=pending_two(1903)
    cold=load_bytes(save_bytes(c))
    assert cold.fingerprint==c.fingerprint
    attack_recipient(cold,'1903:attack')
    assert [u['id'] for u in cold.state['battle']['officer_units'] if u['side']=='D']==['B9','B2','C1']
    assert cold.state.get('q09_pending_attacked_arrivals',{})=={}
    assert load_bytes(save_bytes(cold)).fingerprint==cold.fingerprint


def test_unrelated_pending_destination_is_not_consumed():
    c=pending_two(1904)
    # A separately attacked province cannot eat the recipient's army.
    c.state['q09_pending_attacked_arrivals']['p7']=copy.deepcopy(
        c.state['q09_pending_attacked_arrivals']['p6'])
    del c.state['q09_pending_attacked_arrivals']['p6']
    # An invalid moved record is rejected rather than silently attached.
    with pytest.raises(CoreError,match='BAD_Q09_ATTACKED_ARRIVAL'):
        validate(c.state)


def test_no_pending_attacked_recipient_retains_existing_ordinary_defender_path():
    c=ready(1905,attacked=False)
    c.do('battle_withdraw','B2',{'officer_id':'B9','destination':'p6'},'r019:1905:withdraw')
    c.state['battle']['rice_a']=0
    c.do('battle_end_turn','B2',{},'r019:1905:settle')
    assert c.state.get('q09_pending_attacked_arrivals',{})=={}
    attack_recipient(c,'1905:attack')
    assert c.state.get('q09_pending_attacked_arrivals',{})=={}
    assert not any(e['kind']=='q09_recipient_source_attached_port_partial' for e in c.state['battle'].get('combat_events',[]))
    assert validate(c.state)

@pytest.mark.skipif(not __import__('os').environ.get('DISPLAY'),reason='requires Xvfb Tk')
def test_real_tk_controller_six_commands_after_recipient_activation(tmp_path):
    import tkinter as tk
    from types import SimpleNamespace
    from unittest.mock import Mock
    from third_branch.tactical_view import TacticalWindow
    from third_branch.nes_battle_menu import ROOT
    root=tk.Tk();root.withdraw()
    c=pending_two(1906)
    attack_recipient(c,'1906:attack')
    parent=SimpleNamespace(root=root,c=c,path=tmp_path/'q09-recipient.ro2',
        counter=0,audio=SimpleNamespace(effect=Mock(),play=Mock()),
        selected='p6',redraw=Mock(),pause=Mock())
    w=TacticalWindow(parent)
    try:
        assert [title for title,_ in w.controller_actions]==list(ROOT)
        root.update_idletasks()
        assert w.root.geometry().startswith('1280x720')
        assert w.canvas.find_withtag('nes_menu')
        w.on_nes_button('b')
        assert 'Day' in w.message.get()
        assert [u['id'] for u in c.state['battle']['officer_units'] if u['side']=='D']==[
            'B9','B2','C1']
        assert validate(c.state)
    finally:
        w.close();root.destroy()


def test_source_head_unit_can_take_real_wait_and_cold_save_without_duplication():
    c=pending_two(1907)
    attack_recipient(c,'1907:attack')
    before=ac.strength(c.state,'B9','p6')
    reply=c.do('battle_wait','B2',{'officer_id':'B9'},'r019:1907:wait')
    assert reply['officer']=='B9' and reply['rested']
    assert [u['id'] for u in c.state['battle']['officer_units'] if u['side']=='D']==[
        'B9','B2','C1']
    assert ac.strength(c.state,'B9','p6')==before
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint