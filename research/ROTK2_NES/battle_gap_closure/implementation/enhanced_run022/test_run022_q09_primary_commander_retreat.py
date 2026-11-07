"""Run022 Q09 primary commander whole-army authority, partial source-backed parity.

Reference: original shared hsovl2 D905/D89D primary-head choice, independently
captured as Classic Run007 and Run009 source fixtures. This is a PORT test,
not a new NES emulator replay, Windows run, or full four-slot scheduler proof.
"""
import copy
from types import SimpleNamespace
from unittest.mock import Mock
import pytest
from og.core.fixed import CoreError
from third_branch.integrated import validate, save_bytes, load_bytes
from third_branch.tactical_view import TacticalWindow
from test_run018_q09_attacked_arrival import ready


def commissioned(seed):
    c=ready(seed)
    s=c.state
    s['q09_source_officer_bytes']['B2']={'flag2':0x40}
    s['battle']['q09_source_officer_bytes']['B2']={'flag2':0x40}
    return c


def whole_params():
    return {'whole_army':True,'destination':'p6',
            'officer_destinations':{'B2':'p6','B9':'p6'}}


def test_nonprimary_B9_cannot_order_whole_army_retreat_and_rng_unchanged():
    c=commissioned(2201)
    before=c.fingerprint
    rng=copy.deepcopy(c.state['core']['rng_state'])
    with pytest.raises(CoreError,match='ONLY_COMMANDER_WHOLE_WITHDRAW'):
        c.do('battle_withdraw','B9',whole_params(),'r022:nonprimary')
    assert c.fingerprint==before
    assert c.state['core']['rng_state']==rng
    assert validate(c.state)


def test_nonprimary_selected_officer_cannot_be_forged_as_commander():
    c=commissioned(2202)
    before=c.fingerprint
    params=whole_params();params['officer_id']='B9'
    with pytest.raises(CoreError,match='ONLY_COMMANDER_WHOLE_WITHDRAW'):
        c.do('battle_withdraw','B2',params,'r022:forged')
    assert c.fingerprint==before and validate(c.state)


def test_source_primary_B2_can_still_withdraw_and_cold_load():
    c=commissioned(2203)
    result=c.do('battle_withdraw','B2',whole_params(),'r022:commander')
    assert set(result['withdrawn'])=={'B2','B9'}
    assert c.state['battle'] is None
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_gui_noncommander_cannot_open_whole_army_prompt():
    c=commissioned(2204)
    w=TacticalWindow.__new__(TacticalWindow)
    class Message:
        def set(self,value):self.value=value
    w.parent=SimpleNamespace(c=c)
    w.selected_officer='B9'
    w.pending_withdraw=None
    w.message=Message();w.redraw=lambda:None
    before=c.fingerprint
    w._withdraw_prompt(True)
    assert w.pending_withdraw is None
    assert 'commander' in w.message.value.lower()
    assert c.fingerprint==before


def test_gui_after_primary_flees_rejects_whole_army_without_keyerror():
    c=commissioned(2205)
    c.do('battle_withdraw','B2',{'officer_id':'B2','whole_army':False,
        'destination':'p6'},'r022:commander-flee')
    assert c.state['battle'] is not None
    w=TacticalWindow.__new__(TacticalWindow)
    class Message:
        def set(self,value):self.value=value
    w.parent=SimpleNamespace(c=c)
    w.selected_officer='B9'
    w.pending_withdraw=None;w.message=Message();w.redraw=lambda:None
    before=c.fingerprint
    w._withdraw_prompt(True)
    assert w.pending_withdraw is None
    assert c.fingerprint==before
    assert validate(c.state)


@pytest.mark.skipif(not __import__('os').environ.get('DISPLAY'),reason='Tk requires Xvfb')
def test_real_controller_noncommander_cannot_issue_whole_army_and_cancel(tmp_path):
    import tkinter as tk
    c=commissioned(2206)
    root=tk.Tk();root.withdraw()
    parent=SimpleNamespace(root=root,c=c,path=tmp_path/'run022.ro2',counter=0,
        audio=SimpleNamespace(effect=Mock(),play=Mock()),selected='p5',
        redraw=Mock(),pause=Mock())
    w=TacticalWindow(parent)
    try:
        w.selected_officer='B9'
        before=parent.c.fingerprint
        w.on_nes_button('down');w.on_nes_button('down')
        assert w.menu.labels[w.menu.index]=='Flee'
        w.on_nes_button('a')
        assert w.menu.labels==('Officer','Whole army')
        w.on_nes_button('down');w.on_nes_button('a')
        assert w.pending_withdraw is None
        assert 'commander' in w.message.get().lower()
        assert parent.counter==0 and parent.c.fingerprint==before
        assert not parent.path.exists()
        assert validate(parent.c.state)
    finally:
        w.close();root.destroy()


@pytest.mark.skipif(not __import__('os').environ.get('DISPLAY'),reason='Tk requires Xvfb')
def test_real_controller_primary_eligible_prompt_and_B_cancel(tmp_path):
    import tkinter as tk
    c=commissioned(2207)
    root=tk.Tk();root.withdraw()
    parent=SimpleNamespace(root=root,c=c,path=tmp_path/'run022c.ro2',counter=0,
        audio=SimpleNamespace(effect=Mock(),play=Mock()),selected='p5',
        redraw=Mock(),pause=Mock())
    w=TacticalWindow(parent)
    try:
        w.selected_officer='B2'
        before=parent.c.fingerprint
        w._withdraw_prompt(True)
        assert w.pending_withdraw is not None
        assert w.pending_withdraw['officers']==['B2','B9']
        w.on_nes_button('b')
        assert w.pending_withdraw is None
        assert parent.c.fingerprint==before and parent.counter==0
        assert not parent.path.exists()
    finally:
        w.close();root.destroy()
