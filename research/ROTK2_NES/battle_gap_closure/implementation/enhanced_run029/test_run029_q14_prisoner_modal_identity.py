"""Run029: Q14 prisoner fate binds the prisoner and decision shown to the human.

Source basis: Q14 original officer-by-officer postbattle disposition.  The
exact NES warning string for a stale UI is NOT attested.  These are real
Campaign/controller parity invariants, not original-emulator 30-trace goldens.
"""
from types import SimpleNamespace
from unittest.mock import Mock
import copy
import os
import pytest
from og.core.world import add_officer
from third_branch import army_cohorts as ac
from third_branch.integrated import new_campaign, validate, save_bytes, load_file, load_bytes
from third_branch.tactical_view import TacticalWindow

class Message:
    def __init__(self):self.value=''
    def set(self,v):self.value=v

def awaiting_two(seed=29001):
    c=new_campaign(seed);s=c.state
    add_officer(s['core'],'B3','p5',(40,50,55,60,60))
    s['officer_faction']['B3']='B';s['officer_loyalty']['B3']=45
    ac.add(s,'B3','p5',30);ac.refresh(s)
    assert validate(s)
    c.do('attack','A3',{'target':'p5','troops':120,'gold':1},f'r029:{seed}:attack')
    c.state['battle']['rice_d']=0
    c.do('battle_end_turn','A3',{},f'r029:{seed}:win')
    assert c.state['battle']['prisoner_queue']==['B2','B3']
    return c

def stub(c):
    w=TacticalWindow.__new__(TacticalWindow)
    w.parent=SimpleNamespace(c=c,counter=0)
    w.message=Message();w.redraw=Mock()
    w.prisoner_index=0;w.prisoner_decision_index=0
    w.pending_prisoner=False;w.pending_withdraw=None;w.targeting=False
    sent=[];w._issue=lambda k,p:sent.append((k,p))
    return w,sent

def test_reordered_prisoner_queue_does_not_silently_select_next_officer():
    c=awaiting_two();w,sent=stub(c);w._prisoner_prompt()
    assert w._prisoner_choice()=='B2'
    c.state['battle']['prisoner_queue']=['B3','B2']
    before=save_bytes(c); rng=copy.deepcopy(c.state['core']['rng_state'])
    w._prisoner_confirm()
    assert sent==[] and save_bytes(c)==before
    assert c.state['core']['rng_state']==rng
    assert not w.pending_prisoner
    assert 'prisoner' in w.message.value.lower()
    assert validate(c.state)

def test_external_disposition_during_modal_never_substitutes_remaining_prisoner():
    c=awaiting_two(29002);w,sent=stub(c);w._prisoner_prompt()
    c.do('battle_prisoner','A3',{'officer_id':'B2','decision':'recruit'},'r029:external')
    assert c.state['battle']['prisoner_queue']==['B2','B3']
    assert c.state['battle']['prisoner_decisions']=={'B2':'recruit'}
    before=save_bytes(c)
    w._prisoner_confirm()
    assert sent==[] and save_bytes(c)==before
    assert not w.pending_prisoner
    assert validate(c.state)

def test_decision_reorder_does_not_silently_change_release_to_behead():
    c=awaiting_two(29003);w,sent=stub(c);w._prisoner_prompt()
    # Human selects the second visible action, Release.
    w.prisoner_decision_index=1
    original=w._prisoner_options
    w._prisoner_options=lambda: (original()[1],original()[2],original()[0])
    w._prisoner_confirm()
    assert sent==[('battle_prisoner',{'officer_id':'B2','decision':'release'})]
    assert validate(c.state)

def test_select_other_prisoner_then_confirm_correct_officer_and_cold_load(tmp_path):
    c=awaiting_two(29004);w,sent=stub(c)
    w._prisoner_prompt()
    w.on_nes_button('select')
    assert w._prisoner_choice()=='B3'
    assert w.pending_prisoner
    w.prisoner_decision_index=1
    w.on_nes_button('a')
    assert sent==[('battle_prisoner',{'officer_id':'B3','decision':'release'})]
    c.do('battle_prisoner','A3',sent[0][1],'r029:correct')
    assert c.state['battle']['prisoner_decisions']=={'B3':'release'}
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
    assert validate(c.state)

def test_cancel_does_not_dispatch_or_mutate_campaign():
    c=awaiting_two(29005);w,sent=stub(c);w._prisoner_prompt()
    before=save_bytes(c)
    w.on_nes_button('b')
    assert sent==[] and not w.pending_prisoner and save_bytes(c)==before

@pytest.mark.skipif(not os.environ.get('DISPLAY'),reason='real Tk/Xvfb required')
def test_real_tk_controller_reorder_rejects_then_valid_choice_commits_disk(tmp_path):
    import tkinter as tk
    root=tk.Tk();root.withdraw()
    c=awaiting_two(29006)
    parent=SimpleNamespace(root=root,c=c,path=tmp_path/'r029.ro2',counter=0,
        audio=SimpleNamespace(effect=Mock(),play=Mock()),selected='p5',
        redraw=Mock(),pause=Mock())
    w=TacticalWindow(parent)
    try:
        for _ in range(3):w.on_nes_button('down')
        w.on_nes_button('a');w.on_nes_button('down');w.on_nes_button('down');w.on_nes_button('a')
        assert w.pending_prisoner and w._prisoner_choice()=='B2'
        c.state['battle']['prisoner_queue']=['B3','B2']
        before=save_bytes(c)
        w.on_nes_button('a')
        assert parent.counter==0 and not parent.path.exists()
        assert not w.pending_prisoner and save_bytes(c)==before
        assert 'prisoner' in w.message.get().lower()
        # Re-open with newly observed order: the B3 choice must stay B3.
        w._prisoner_prompt()
        assert w._prisoner_choice()=='B3'
        w.on_nes_button('right');w.on_nes_button('a')
        assert parent.counter==1 and parent.path.exists()
        assert parent.c.state['battle']['prisoner_decisions']=={'B3':'release'}
        assert load_file(parent.path).fingerprint==parent.c.fingerprint
        assert validate(parent.c.state)
    finally:w.close();root.destroy()
