"""Q09 recipient tactical battle: player-selected first actionable defender.

Source basis: English NES/SNES unified Q04/Q09 player's side and source
HEAD insertion at hsovl2 D648/D69A.  Run019 recipient attachment correctly
inserts B9 as first D unit, but initial selected_officer remains the A head.
This test covers a controller-visible gameplay regression in the actual engine.
No claim of second simultaneous battle scheduling or NES event goldens.
"""
import pytest
from og.core.fixed import CoreError
from third_branch.integrated import validate, save_bytes, load_bytes
from third_branch.tactical_view import render_battle
from test_run019_q09_recipient_source_attach import pending_two, attack_recipient


def recipient(seed):
    c=pending_two(seed)
    attack_recipient(c,token=f'run020:{seed}:attack')
    return c


def test_human_defender_source_head_is_initial_selected_officer():
    c=recipient(202001)
    b=c.state['battle']
    assert c.state['human_faction']==b['defender']=='B'
    assert [u['id'] for u in b['officer_units'] if u['side']=='D']==['B9','B2','C1']
    assert b['selected_officer']=='B9'
    assert validate(c.state)


def test_default_controller_wait_dispatch_targets_actual_defender():
    c=recipient(202002)
    before=c.state['battle']['a']['troops100'],c.state['battle']['d']['troops100']
    reply=c.do('battle_wait','B2',{},'run020:202002:default_wait')
    assert reply['officer']=='B9' and reply['rested']
    b=c.state['battle']
    assert next(u for u in b['officer_units'] if u['id']=='B9')['acted']
    assert not next(u for u in b['officer_units'] if u['id']=='C2')['acted']
    assert before==(b['a']['troops100'],b['d']['troops100'])
    assert validate(c.state)


def test_selected_player_officer_survives_disk_roundtrip_and_can_act():
    c=recipient(202003)
    cold=load_bytes(save_bytes(c))
    assert cold.fingerprint==c.fingerprint
    assert cold.state['battle']['selected_officer']=='B9'
    assert cold.do('battle_wait','B2',{},'run020:202003:cold_wait')['officer']=='B9'
    assert validate(cold.state)
    assert load_bytes(save_bytes(cold)).fingerprint==cold.fingerprint


def test_no_attacked_destination_regular_human_defender_initial_selected():
    # This defect is not limited to the Q09 pending-arrival overlay.
    from test_run018_q09_attacked_arrival import ready
    c=ready(202004,attacked=False)
    b=c.state['battle']
    assert b['defender']=='B' and c.state['human_faction']=='B'
    first_def=next(u['id'] for u in b['officer_units'] if u['side']=='D')
    assert b['selected_officer']==first_def
    assert validate(c.state)


def test_source_head_activates_without_any_attack_troop_transfer():
    c=recipient(202005)
    pre={u['id']:u['troops100'] for u in c.state['battle']['officer_units']}
    assert c.state.get('q09_pending_attacked_arrivals',{})=={}
    # The default selection must be a local officer, not a second transfer.
    assert c.state['battle']['selected_officer']=='B9'
    assert {u['id']:u['troops100'] for u in c.state['battle']['officer_units']}==pre
    assert validate(c.state)


def test_attacker_game_selection_preserved_when_human_is_attacker():
    c=recipient(202006)
    # Freshly start identical already-finished Q09 recipient encounter with
    # the initiating faction as human before the real attack.
    from test_run019_q09_recipient_source_attach import pending_two
    fresh=pending_two(202007)
    fresh.state['human_faction']='C'
    attack_recipient(fresh,token='run020:attacker')
    b=fresh.state['battle']
    first_att=next(u['id'] for u in b['officer_units'] if u['side']=='A')
    assert b['selected_officer']==first_att=='C2'
    assert validate(fresh.state)


@pytest.mark.skipif(not __import__('os').environ.get('DISPLAY'),reason='requires Xvfb')
def test_1280x720_controller_view_focuses_actual_human_defender(tmp_path):
    import tkinter as tk
    from types import SimpleNamespace
    from unittest.mock import Mock
    from third_branch.tactical_view import TacticalWindow
    root=tk.Tk(); root.withdraw()
    c=recipient(202008)
    parent=SimpleNamespace(root=root,c=c,path=tmp_path/'run020.ro2',counter=0,
        audio=SimpleNamespace(effect=Mock(),play=Mock()),selected='p6',
        redraw=Mock(),pause=Mock())
    w=TacticalWindow(parent)
    try:
        assert w.selected_officer==c.state['battle']['selected_officer']=='B9'
        root.update_idletasks()
        assert w.root.geometry().startswith('1280x720')
        assert w.canvas.find_withtag('nes_menu')
        w.on_nes_button('b')
        assert 'Day' in w.message.get()
        frame=render_battle(c)
        assert frame.size==(1280,720)
    finally:
        w.close();root.destroy()