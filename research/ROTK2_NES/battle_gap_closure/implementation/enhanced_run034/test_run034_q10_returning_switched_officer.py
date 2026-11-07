"""Run034 English NES Q10 hsovl1 D5DD previously-switched opponent regression.

Source: an opposing non-head unit is bribable even if its +2 switch bit is set.
Returning to its original liege succeeds automatically; gold is spent on
refusal otherwise. Daily spontaneous switches still exclude switched units.
These tests exercise the port and actual Tk, not original emulator goldens.
"""
import copy
import os
import subprocess
import sys
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import Mock
import pytest

from third_branch import officer_battle_adapter as ba
from third_branch import army_cohorts as ac
from third_branch.integrated import save_bytes, load_bytes, validate
from third_branch.tactical_view import TacticalWindow
from test_build16_q10_bribe import make_battle


def prior_switch(seed=3400):
    c = make_battle()
    s = c.state
    b = s['battle']
    u = ba.find(b, 'D', 'B2')
    assert u and u['original_faction'] == 'B'
    ba._switch_side(s, b, u, 'A', 90)  # genuine earlier field switch
    s['human_faction'] = 'B'
    assert u['switched'] and u['side'] == 'A'
    assert validate(s)
    return c


def troop_total(s):
    return sum(ac.army_total(a) for a in s['armies'].values())


def test_original_liege_can_bribe_back_switched_opponent_with_no_rng_draw():
    c = prior_switch()
    old = save_bytes(c)
    b = c.state['battle']
    gold = b['gold_d_cents']
    total = troop_total(c.state)
    before_rng = copy.deepcopy(c.state['core']['rng_state'])
    out = c.do('battle_bribe', 'B1', {'officer_id': 'B1',
        'target_officer_id': 'B2', 'gold': 1}, 'run034:return-original')
    assert out['success'] and out['target'] == 'B2'
    b = c.state['battle']
    assert b['gold_d_cents'] == gold-100
    assert ba.find(b, 'D', 'B2') is not None
    assert ba.find(b, 'A', 'B2') is None
    assert c.state['officer_faction']['B2'] == 'B'
    assert c.state['officer_loyalty']['B2'] == 100
    assert ba.find(b, 'D', 'B2')['switched']  # daily RNG exclusion remains
    assert troop_total(c.state) == total
    assert c.state['core']['rng_state'] == before_rng
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint == c.fingerprint
    assert save_bytes(c) != old


def test_switched_nonreturning_opponent_is_still_eligible_but_may_refuse():
    c = prior_switch()
    u = ba.find(c.state['battle'], 'A', 'B2')
    u['original_faction'] = 'A'  # formerly from A, now not returning to B
    before = troop_total(c.state)
    gold = c.state['battle']['gold_d_cents']
    out = c.do('battle_bribe','B1',{'officer_id':'B1',
        'target_officer_id':'B2','gold':1}, 'run034:switched-refused')
    assert out['success'] is False
    assert c.state['battle']['gold_d_cents'] == gold-100
    assert ba.find(c.state['battle'], 'A', 'B2') is not None
    assert troop_total(c.state) == before
    assert validate(c.state)


def test_commander_is_still_unbribable_with_switched_flag():
    from og.core.fixed import CoreError
    c = make_battle()
    c.state['battle']['officer_units'][1]['switched'] = True
    before = save_bytes(c)
    with pytest.raises(CoreError):
        c.do('battle_bribe','A3',{'officer_id':'A3','target_officer_id':'B1','gold':1},'run034:head')
    assert save_bytes(c)==before


def test_spontaneous_daily_switch_still_excludes_already_switched():
    c = prior_switch()
    s = c.state
    s['officer_loyalty']['B2'] = 0
    before = save_bytes(c)
    with pytest.MonkeyPatch.context() as mp:
        def should_not_draw(state,n):
            raise AssertionError('switched officer must not enter daily RNG trial')
        mp.setattr(ba,'randrange',should_not_draw)
        result=ba._spontaneous_switches(s,s['battle'])
    assert not result
    assert save_bytes(c)==before


def test_controller_targets_switched_opponent_not_rejected_as_ineligible():
    c=prior_switch()
    w=TacticalWindow.__new__(TacticalWindow)
    w.parent=SimpleNamespace(c=c)
    w.selected_officer='B1';w.selected_target=None
    w.targeting=False;w.target_actor=None;w.target_phase=None
    w.target_enemy=None;w.target_enemy_hex=None;w.target_q=0;w.target_r=0
    w.pending_withdraw=None;w.pending_prisoner=False
    w.bribe_gold=1;w.message=SimpleNamespace(value='',set=lambda x:setattr(w.message,'value',x))
    w.redraw=Mock();w._issue=Mock()
    before=save_bytes(c)
    w._menu_action('Tactics','Switch sides')
    assert w.target_phase=='enemy'
    u=ba.find(c.state['battle'],'A','B2')
    w.target_q,w.target_r=u['q'],u['r']
    w.on_nes_button('a')
    assert w.target_phase=='gold' and w.target_enemy=='B2'
    w.on_nes_button('a')
    w._issue.assert_called_once_with('battle_bribe',{'gold':1,'target_officer_id':'B2'})
    assert save_bytes(c)==before


@pytest.mark.skipif(not os.environ.get('DISPLAY'),reason='actual Linux Tk display required')
def test_real_tk_switch_back_and_separate_process_disk_cold_load(tmp_path):
    import tkinter as tk
    c=prior_switch()
    root=tk.Tk();root.withdraw()
    parent=SimpleNamespace(root=root,c=c,path=tmp_path/'q10_return.ro2',counter=0,
        audio=SimpleNamespace(effect=Mock(),play=Mock()),selected='p5',
        redraw=Mock(),pause=Mock())
    w=TacticalWindow(parent)
    try:
        w.selected_officer='B1'
        total=troop_total(parent.c.state)
        w._menu_action('Tactics','Switch sides')
        u=ba.find(parent.c.state['battle'],'A','B2')
        w.target_q,w.target_r=u['q'],u['r']
        w.on_nes_button('a')
        assert w.target_phase=='gold' and w.target_enemy=='B2'
        w.on_nes_button('a')
        assert parent.path.is_file() and parent.counter==1
        assert ba.find(parent.c.state['battle'],'D','B2') is not None
        assert troop_total(parent.c.state)==total
        assert validate(parent.c.state)
        script='import sys;from third_branch.integrated import load_file;print(load_file(sys.argv[1]).fingerprint)'
        proc=subprocess.run([sys.executable,'-c',script,str(parent.path)],
           cwd=str(Path(__file__).parents[1]),check=True,capture_output=True,text=True)
        assert proc.stdout.strip()==parent.c.fingerprint
    finally:
        w.close();root.destroy()
