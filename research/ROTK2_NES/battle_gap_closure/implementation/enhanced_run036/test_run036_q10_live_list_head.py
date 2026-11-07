"""Run036 original Q10 hsovl1 D5DD and $7AC0: *live linked-list head*
protection after the original commander exits. Source-derived behavior, NOT an
original NES emulator golden. Tests use the actual Enhanced campaign and Tk UI.
"""
import copy
import os
from pathlib import Path
import subprocess
import sys
from types import SimpleNamespace
from unittest.mock import Mock, patch

import pytest
from og.core.fixed import CoreError
from third_branch import officer_battle_adapter as ba
from third_branch import army_cohorts as ac
from third_branch.integrated import save_bytes, load_bytes, validate
from third_branch.tactical_view import TacticalWindow
from third_branch.nes_battle_menu import NESBattleMenu
from test_run018_q09_attacked_arrival import ready


def displaced(seed=3600):
    c = ready(seed, attacked=False, known=False)
    # The Q09 original-source-unknown fallback remains explicitly provisional,
    # but this independently tests the OBSERVABLE Q10 linked-list head after
    # the existing port has legally withdrawn its former head.
    c.state['human_faction']='B'
    out=c.do('battle_withdraw','B2',{'officer_id':'B2','destination':'p6'},f'r36:{seed}:flee')
    assert out['withdrawn']==['B2'] and not out['resolved']
    c.state['human_faction']='A'
    b=c.state['battle']
    assert [(u['id'],u['fled']) for u in b['officer_units'] if u['side']=='D']==[
        ('B2',True),('B9',False)]
    assert not ba.find(b,'D','B2') and ba.find(b,'D','B9')
    assert validate(c.state)
    return c


def total(s):
    return sum(ac.army_total(a) for a in s['armies'].values())


def test_new_live_defender_head_cannot_be_bribed_before_gold_rng_or_turn():
    c=displaced(3601)
    before=save_bytes(c);roll=copy.deepcopy(c.state['core']['rng_state']);men=total(c.state)
    with pytest.raises(CoreError,match='ILLEGAL_BRIBE_TARGET'):
        c.do('battle_bribe','A3',{'officer_id':'A3','target_officer_id':'B9','gold':1},'r36:forbidden-head')
    assert save_bytes(c)==before and c.state['core']['rng_state']==roll
    assert total(c.state)==men and validate(c.state)
    assert load_bytes(before).fingerprint==c.fingerprint


def test_live_head_is_not_daily_spontaneous_switch_candidate_no_rng():
    c=displaced(3602);s=c.state;s['officer_loyalty']['B9']=0
    before=save_bytes(c);men=total(s)
    with patch.object(ba,'randrange',side_effect=AssertionError('NO_RNG_FOR_HEAD')) as random:
        assert ba._spontaneous_switches(s,s['battle'])==[]
    random.assert_not_called()
    assert save_bytes(c)==before and total(s)==men and validate(s)


def test_original_main_head_present_preserves_nonhead_bribe_eligibility():
    c=ready(3603,attacked=False,known=False)
    c.state['human_faction']='A'
    before=total(c.state);gold=c.state['battle']['gold_a_cents']
    result=c.do('battle_bribe','A3',{'officer_id':'A3','target_officer_id':'B9','gold':1},'r36:allowed-tail')
    assert result['target']=='B9' and result['gold_spent']==1
    assert c.state['battle']['gold_a_cents']==gold-100 and total(c.state)==before
    assert validate(c.state)


def test_ordinary_second_main_officer_still_enters_daily_trial():
    c=ready(3604,attacked=False,known=False)
    s=c.state;s['officer_loyalty']['B9']=0
    calls=[]
    seq=iter([0,99,0])
    with patch.object(ba,'randrange',side_effect=lambda st,n: (calls.append(n),next(seq))[1]):
        assert ba._spontaneous_switches(s,s['battle'])==['B9']
    assert calls==[100,100,10]
    assert s['officer_faction']['B9']=='A' and validate(s)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def controller(c):
    w=TacticalWindow.__new__(TacticalWindow)
    w.parent=SimpleNamespace(c=c,counter=0)
    w.selected_officer='A3';w.selected_target=None
    w.targeting=False;w.target_mode='battle_move'
    w.target_actor=None;w.target_phase=None;w.target_enemy=None
    w.target_enemy_hex=None;w.target_q=0;w.target_r=0
    w.bribe_gold=1;w.menu=NESBattleMenu();w.controller_index=0
    w.action_buttons=[];w.pending_withdraw=None;w.pending_prisoner=False
    w.message=SimpleNamespace(value='',set=lambda value:setattr(w.message,'value',value))
    w.redraw=Mock();w._issue=Mock()
    return w


def test_controller_refuses_live_head_at_select_without_spending_or_rng():
    c=displaced(3605);w=controller(c)
    before=save_bytes(c)
    w._menu_action('Tactics','Switch sides')
    unit=ba.find(c.state['battle'],'D','B9')
    w.target_q,w.target_r=unit['q'],unit['r']
    w.on_nes_button('a')
    assert w.targeting and w.target_phase=='enemy'
    assert 'commander' in w.message.value.lower() or 'head' in w.message.value.lower()
    w._issue.assert_not_called()
    assert save_bytes(c)==before


def test_gold_modal_rechecks_head_after_external_commander_flee():
    c=ready(3606,attacked=False,known=False);c.state['human_faction']='A'
    w=controller(c);w._menu_action('Tactics','Switch sides')
    unit=ba.find(c.state['battle'],'D','B9')
    w.target_q,w.target_r=unit['q'],unit['r']
    w.on_nes_button('a')
    assert w.target_phase=='gold'
    c.state['human_faction']='B'
    c.do('battle_withdraw','B2',{'officer_id':'B2','destination':'p6'},'r36:gold:flee')
    c.state['human_faction']='A'
    old=save_bytes(c)
    w.on_nes_button('a')
    assert not w.targeting and not w._issue.called
    assert save_bytes(c)==old and validate(c.state)


@pytest.mark.skipif(not os.environ.get('DISPLAY'),reason='requires actual Linux/Xvfb Tk')
def test_actual_tk_six_command_controller_and_disk_cold_load(tmp_path):
    import tkinter as tk
    c=displaced(3607)
    root=tk.Tk();root.withdraw()
    parent=SimpleNamespace(root=root,c=c,path=tmp_path/'run036.ro2',counter=0,
        audio=SimpleNamespace(effect=Mock(),play=Mock()),selected='p5',redraw=Mock(),pause=Mock())
    w=TacticalWindow(parent)
    try:
        assert tuple(w.menu.labels)==('Move','Attack','Flee','View','Tactics','Wait')
        root.update_idletasks()
        assert w.root.geometry().startswith('1280x720')
        before=save_bytes(c)
        w.selected_officer='A3'
        w._menu_action('Tactics','Switch sides')
        unit=ba.find(c.state['battle'],'D','B9')
        w.target_q,w.target_r=unit['q'],unit['r']
        w.on_nes_button('a')
        assert w.target_phase=='enemy' and not parent.path.exists()
        assert save_bytes(c)==before
        # A normal Wait is still executable and really persisted.
        w.on_nes_button('b')
        w._issue('battle_wait',{'officer_id':'A3'})
        # _issue is the production Tk save path, unlike the test-only Mock.
        assert parent.path.is_file() and parent.counter==1
        script='import sys;from third_branch.integrated import load_file;print(load_file(sys.argv[1]).fingerprint)'
        r=subprocess.run([sys.executable,'-c',script,str(parent.path)],cwd=str(Path(__file__).parents[1]),capture_output=True,text=True,check=True)
        assert r.stdout.strip()==parent.c.fingerprint and validate(parent.c.state)
    finally:
        w.close();root.destroy()
