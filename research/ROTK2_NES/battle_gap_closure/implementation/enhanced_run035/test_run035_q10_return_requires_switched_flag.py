"""Enhanced Run035: original English NES Q10 automatic return requires the
previously-switched officer flag *and* matching original liege.

Grounded in answer matrix v1.4 Q10, hsovl1 $D5DD: original liege match
alone does not authorize automatic bribery. These exercise the Enhanced port,
not an original emulator execution or pixel-perfect Windows/Xbox release.
"""
from __future__ import annotations
import copy
import os
from pathlib import Path
import subprocess
import sys
from types import SimpleNamespace
from unittest.mock import Mock

import pytest
from og.core.fixed import CoreError
from third_branch import officer_battle_adapter as ba
from third_branch import army_cohorts as ac
from third_branch.integrated import load_bytes, save_bytes, validate
from third_branch.tactical_view import TacticalWindow
from test_run034_q10_returning_switched_officer import prior_switch


def _unswitched_original_liege():
    """An opposing officer with matching historical liege but no +2 switch bit."""
    c = prior_switch()
    b = c.state['battle']
    unit = ba.find(b,'A','B2')
    assert unit['original_faction']=='B' and unit['switched']
    # Represents actual source-byte flag = 0. A forged historical-liege label
    # must never skip source Q10 success roll. Campaign validate permits it.
    unit['switched']=False
    assert validate(c.state)
    return c


def _troop_total(s):
    return sum(ac.army_total(a) for a in s['armies'].values())


def test_matching_original_liege_without_switched_flag_is_not_auto_success():
    c=_unswitched_original_liege()
    s=c.state; old_gold=s['battle']['gold_d_cents']; total=_troop_total(s)
    old_rng=copy.deepcopy(s['core']['rng_state'])
    out=c.do('battle_bribe','B1',{'officer_id':'B1',
        'target_officer_id':'B2','gold':1},'r035:unswitched-refusal')
    assert out['success'] is False
    s=c.state
    assert s['battle']['gold_d_cents']==old_gold-100  # gold spent on refusal
    assert ba.find(s['battle'],'A','B2') is not None
    assert ba.find(s['battle'],'D','B2') is None
    assert s['officer_faction']['B2']=='A'
    assert _troop_total(s)==total
    assert s['core']['rng_state']!=old_rng  # normal Q10 roll was performed
    assert validate(s)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_actual_switched_bit_original_liege_remains_guaranteed_return_without_roll():
    c=prior_switch();s=c.state
    before=copy.deepcopy(s['core']['rng_state']); total=_troop_total(s)
    gold=s['battle']['gold_d_cents']
    out=c.do('battle_bribe','B1',{'officer_id':'B1',
        'target_officer_id':'B2','gold':1},'r035:genuine-return')
    s=c.state
    s=c.state
    assert out['success'] and s['core']['rng_state']==before
    assert s['battle']['gold_d_cents']==gold-100
    assert ba.find(s['battle'],'D','B2') is not None
    assert s['officer_loyalty']['B2']==100
    assert _troop_total(s)==total and validate(s)


def test_source_protected_bit_remains_guaranteed_even_when_not_switched():
    c=_unswitched_original_liege();s=c.state
    u=ba.find(s['battle'],'A','B2')
    u['protected']=True
    before=copy.deepcopy(s['core']['rng_state']); total=_troop_total(s)
    out=c.do('battle_bribe','B1',{'officer_id':'B1',
        'target_officer_id':'B2','gold':1},'r035:protected')
    s=c.state
    s=c.state
    assert out['success']
    # Protected return without matching switched bit earns regular loyalty
    # 80 + rand(20), so the Q10 loyalty draw still advances the RNG.
    assert s['core']['rng_state']!=before
    assert ba.find(s['battle'],'D','B2') is not None
    assert 80<=s['officer_loyalty']['B2']<=99
    assert _troop_total(s)==total and validate(s)


def test_nonreturning_switched_officer_remains_eligible_and_refuses_one_gold():
    c=prior_switch();s=c.state
    u=ba.find(s['battle'],'A','B2');u['original_faction']='A'
    before=s['battle']['gold_d_cents'];total=_troop_total(s)
    out=c.do('battle_bribe','B1',{'officer_id':'B1',
        'target_officer_id':'B2','gold':1},'r035:switched-other-liege')
    s=c.state
    assert not out['success']
    assert s['battle']['gold_d_cents']==before-100
    assert ba.find(s['battle'],'A','B2') is not None
    assert _troop_total(s)==total and validate(s)


def test_illegal_commander_target_cannot_spend_gold_or_rng():
    c=_unswitched_original_liege();s=c.state
    old=save_bytes(c)
    with pytest.raises(CoreError,match='ILLEGAL_BRIBE_TARGET'):
        c.do('battle_bribe','B1',{'officer_id':'B1',
            'target_officer_id':'A3','gold':1},'r035:illegal-commander')
    assert save_bytes(c)==old and validate(s)


def test_controller_commits_exact_unprotected_officer_and_gold_without_mutation():
    c=_unswitched_original_liege()
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
    u=ba.find(c.state['battle'],'A','B2')
    w.target_q,w.target_r=u['q'],u['r']
    w.on_nes_button('a')
    assert w.target_phase=='gold' and w.target_enemy=='B2'
    w.on_nes_button('a')
    w._issue.assert_called_once_with('battle_bribe',{'gold':1,'target_officer_id':'B2'})
    assert save_bytes(c)==before


@pytest.mark.skipif(not os.environ.get('DISPLAY'),reason='actual Tk needs Xvfb')
def test_real_1280x720_tk_controller_refusal_saved_and_cold_loaded(tmp_path):
    import tkinter as tk
    c=_unswitched_original_liege()
    root=tk.Tk();root.withdraw()
    parent=SimpleNamespace(root=root,c=c,path=tmp_path/'run035.ro2',counter=0,
        audio=SimpleNamespace(effect=Mock(),play=Mock()),selected='p5',
        redraw=Mock(),pause=Mock())
    w=TacticalWindow(parent)
    try:
        assert tuple(w.menu.labels)==('Move','Attack','Flee','View','Tactics','Wait')
        w.selected_officer='B1'
        total=_troop_total(c.state)
        initial_gold=c.state['battle']['gold_d_cents']
        w._menu_action('Tactics','Switch sides')
        u=ba.find(c.state['battle'],'A','B2')
        w.target_q,w.target_r=u['q'],u['r']
        w.on_nes_button('a')
        assert w.target_phase=='gold' and w.target_enemy=='B2'
        w.on_nes_button('a')
        assert parent.counter==1 and parent.path.is_file()
        assert ba.find(parent.c.state['battle'],'A','B2') is not None
        assert parent.c.state['battle']['gold_d_cents']==initial_gold-100
        assert _troop_total(parent.c.state)==total and validate(parent.c.state)
        script=('import sys;from third_branch.integrated import load_file;'
                'print(load_file(sys.argv[1]).fingerprint)')
        r=subprocess.run([sys.executable,'-c',script,str(parent.path)],
            cwd=str(Path(__file__).parents[1]),check=True,text=True,capture_output=True)
        assert r.stdout.strip()==parent.c.fingerprint
    finally:
        w.close();root.destroy()
