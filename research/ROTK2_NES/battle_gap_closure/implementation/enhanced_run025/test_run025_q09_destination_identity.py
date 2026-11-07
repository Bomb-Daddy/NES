"""Run025 Q09 controller destination identity preserved from modal choice to commit.

English NES hsovl2 D89D uses an explicit province destination. These verify
Enhanced UI/controller port behavior, not a new English NES emulator golden.
"""
from types import SimpleNamespace
from unittest.mock import Mock
import pytest
from third_branch.integrated import validate, save_bytes, load_bytes
from third_branch.tactical_view import TacticalWindow
from test_run022_q09_primary_commander_retreat import commissioned

class Message:
    def __init__(self): self.value = ''
    def set(self, v): self.value = v

def fixture(seed, selected='B9'):
    c=commissioned(seed)
    # Both p6 and p7 are now legal destinations for faction B.
    c.state['owners']['p7']='B'
    c.state['officer_faction']['C2']='B'
    assert validate(c.state)
    w=TacticalWindow.__new__(TacticalWindow)
    w.parent=SimpleNamespace(c=c,counter=0)
    w.selected_officer=selected
    w.pending_withdraw=None
    w.withdraw_chosen={};w.withdraw_position=0;w.withdraw_index=0
    w.message=Message();w.redraw=lambda:None;w._issue=Mock()
    return c,w

def select_p7(w):
    # Exercise the same NES directional input that changes the destination.
    for _ in range(5):
        if 'p7' in (w.message.get() if hasattr(w.message,'get') else w.message.value): return
        w.on_nes_button('right')
    raise AssertionError('No destination p7')

def test_individual_selected_province_identity_survives_reordering():
    c,w=fixture(2501)
    w._withdraw_prompt(False);select_p7(w)
    before=c.fingerprint
    # Same legal destinations, changed iteration order after modal opened.
    c.state['neighbors']['p5']=list(reversed(c.state['neighbors']['p5']))
    assert validate(c.state)
    w._withdraw_confirm()
    w._issue.assert_called_once()
    assert w._issue.call_args.args[1]=={'whole_army':False,'destination':'p7','officer_id':'B9'}
    assert c.fingerprint!=before  # deliberate source-state reordering, no command execution

def test_individual_destination_becomes_illegal_no_retarget():
    c,w=fixture(2502)
    w._withdraw_prompt(False);select_p7(w)
    c.state['owners']['p7']='C';c.state['officer_faction']['C2']='C'
    assert validate(c.state)
    before=c.fingerprint
    w._withdraw_confirm()
    assert w.pending_withdraw is None and not w._issue.called
    assert c.fingerprint==before and 'destination' in w.message.value.lower()

def test_whole_army_destinations_are_locked_per_officer():
    c,w=fixture(2503,'B2')
    w._withdraw_prompt(True)
    assert w.pending_withdraw['officers']==['B2','B9']
    w._withdraw_confirm() # B2 p6
    assert w.withdraw_position==1
    select_p7(w)
    c.state['neighbors']['p5']=list(reversed(c.state['neighbors']['p5']))
    assert validate(c.state)
    w._withdraw_confirm()
    w._issue.assert_called_once()
    assert w._issue.call_args.args[1]['officer_destinations']=={'B2':'p6','B9':'p7'}

def test_whole_army_staged_destination_invalidates_atomically():
    c,w=fixture(2504,'B2')
    w._withdraw_prompt(True)
    select_p7(w)
    w._withdraw_confirm() # B2 destination p7
    assert w.withdraw_position==1
    c.state['owners']['p7']='C';c.state['officer_faction']['C2']='C'
    assert validate(c.state)
    before=c.fingerprint
    w._withdraw_confirm()
    assert w.pending_withdraw is None and not w._issue.called
    assert c.fingerprint==before

def test_valid_unchanged_destination_and_cancel_still_work():
    c,w=fixture(2505)
    w._withdraw_prompt(False)
    w._withdraw_confirm()
    assert w._issue.call_args.args[1]['destination']=='p6'
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
    w._issue.reset_mock();w._withdraw_prompt(False)
    w.on_nes_button('b')
    assert w.pending_withdraw is None and not w._issue.called

@pytest.mark.skipif(not __import__('os').environ.get('DISPLAY'),reason='requires Linux Xvfb/Tk')
def test_real_six_command_tk_controller_destination_stays_p7(tmp_path):
    import tkinter as tk
    c=commissioned(2506)
    c.state['owners']['p7']='B';c.state['officer_faction']['C2']='B'
    assert validate(c.state)
    root=tk.Tk();root.withdraw()
    parent=SimpleNamespace(root=root,c=c,path=tmp_path/'r025.ro2',counter=0,
         audio=SimpleNamespace(effect=Mock(),play=Mock()),selected='p5',
         redraw=Mock(),pause=Mock())
    w=TacticalWindow(parent)
    try:
        w.selected_officer='B9'
        w.on_nes_button('down');w.on_nes_button('down')
        assert w.menu.labels[w.menu.index]=='Flee'
        w.on_nes_button('a');w.on_nes_button('a')
        assert w.pending_withdraw is not None
        select_p7(w)
        parent.c.state['neighbors']['p5']=list(reversed(parent.c.state['neighbors']['p5']))
        assert validate(parent.c.state)
        w.on_nes_button('a')
        assert w.pending_withdraw is None
        assert parent.c.state['core']['officers']['B9']['province_id']=='p7'
        assert parent.path.exists() and validate(parent.c.state)
        from third_branch.integrated import load_file
        assert load_file(parent.path).fingerprint==parent.c.fingerprint
    finally:
        w.close();root.destroy()
