"""Q09 modal Flee must commit the SAME selected source officer.

The D905 selected-officer dispatch is source-backed.  These are Enhanced
PORT/controller regressions, not newly observed original emulator goldens.
"""
from types import SimpleNamespace
from unittest.mock import Mock
import pytest
from third_branch.integrated import validate, save_bytes, load_bytes
from third_branch.tactical_view import TacticalWindow
from test_run022_q09_primary_commander_retreat import commissioned


class Message:
    def __init__(self): self.value=''
    def set(self, msg): self.value=msg


def stub(c, selected):
    w=TacticalWindow.__new__(TacticalWindow)
    w.parent=SimpleNamespace(c=c,counter=0)
    w.selected_officer=selected
    w.pending_withdraw=None
    w.withdraw_chosen={}
    w.withdraw_position=0
    w.withdraw_index=0
    w.message=Message()
    w.redraw=lambda:None
    w._issue=Mock()
    return w


def test_individual_flee_changed_selection_after_prompt_never_retargets():
    c=commissioned(2401);w=stub(c,'B9')
    w._withdraw_prompt(False)
    assert w.pending_withdraw['officers']==['B9']
    before=c.fingerprint
    w.selected_officer='B2'  # another *eligible* friendly officer
    w._withdraw_confirm()
    assert w.pending_withdraw is None
    assert not w._issue.called
    assert 'selected' in w.message.value.lower()
    assert before==c.fingerprint and validate(c.state)


def test_individual_flee_selected_actor_no_longer_present_after_prompt():
    c=commissioned(2402);w=stub(c,'B9')
    w._withdraw_prompt(False)
    c.do('battle_withdraw','B2',{'officer_id':'B9','destination':'p6'},
         'r024:2402:earlier-flee')
    before=c.fingerprint
    w._withdraw_confirm()
    assert w.pending_withdraw is None and not w._issue.called
    assert before==c.fingerprint and validate(c.state)


def test_whole_army_pending_list_cannot_reuse_a_departed_subordinate():
    c=commissioned(2403);w=stub(c,'B2')
    w._withdraw_prompt(True)
    assert w.pending_withdraw['officers']==['B2','B9']
    c.do('battle_withdraw','B2',{'officer_id':'B9','destination':'p6'},
         'r024:2403:subordinate-departed')
    before=c.fingerprint
    w._withdraw_confirm()
    assert w.pending_withdraw is None and not w._issue.called
    assert before==c.fingerprint and validate(c.state)


def test_low_level_dispatch_rejects_explicit_officer_override():
    c=commissioned(2404);w=stub(c,'B2')
    # Run023 _issue silently overwrites B9 with B2. Fail closed instead.
    w._issue=TacticalWindow._issue.__get__(w,TacticalWindow)
    before=c.fingerprint
    w._issue('battle_withdraw',{'whole_army':False,'officer_id':'B9',
                               'destination':'p6'})
    assert before==c.fingerprint and w.parent.counter==0
    assert 'selected' in w.message.value.lower() and validate(c.state)


def test_intact_selected_actor_and_cancellation_still_work():
    c=commissioned(2405);w=stub(c,'B9')
    w._withdraw_prompt(False)
    before=c.fingerprint
    w._withdraw_confirm()
    w._issue.assert_called_once()
    args=w._issue.call_args.args
    assert args[0]=='battle_withdraw'
    assert args[1]['officer_id']=='B9'
    assert c.fingerprint==before
    assert load_bytes(save_bytes(c)).fingerprint==before


@pytest.mark.skipif(not __import__('os').environ.get('DISPLAY'),reason='requires Xvfb Tk')
def test_real_tk_modal_stale_actor_rejected_without_disk_or_counter(tmp_path):
    import tkinter as tk
    c=commissioned(2406)
    root=tk.Tk();root.withdraw()
    parent=SimpleNamespace(root=root,c=c,path=tmp_path/'r024.ro2',counter=0,
        audio=SimpleNamespace(effect=Mock(),play=Mock()),selected='p5',
        redraw=Mock(),pause=Mock())
    w=TacticalWindow(parent)
    try:
        w.selected_officer='B9'
        before=c.fingerprint
        w.on_nes_button('down');w.on_nes_button('down')
        assert w.menu.labels[w.menu.index]=='Flee'
        w.on_nes_button('a');w.on_nes_button('a')  # Officer then destination
        assert w.pending_withdraw is not None
        w.selected_officer='B2'  # stale modal selection at time of commit
        w.on_nes_button('a')
        assert w.pending_withdraw is None
        assert 'selected' in w.message.get().lower()
        assert parent.counter==0 and parent.c.fingerprint==before
        assert not parent.path.exists() and validate(parent.c.state)
    finally:
        w.close();root.destroy()