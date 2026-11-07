"""RUN032 Q10 source-backed controller Switch Sides (hsovl1 $D5DD).

The original allows an offer against an opposing noncommander list member,
1..min(side gold, 99). The port used an inaccessible Next Target control.
These are PORT and live Tk fixtures, not an original NES event golden.
"""
import copy
import os
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import Mock
import pytest
from third_branch.integrated import save_bytes, load_file, validate
from third_branch.tactical_view import TacticalWindow
from test_build16_q10_bribe import make_battle
from test_run030_q06_move_enemy_visible_target import Message


def fixture(seed=32):
    c=make_battle()
    c.state['human_faction']='A'
    assert validate(c.state)
    w=TacticalWindow.__new__(TacticalWindow)
    w.parent=SimpleNamespace(c=c,counter=0)
    w.selected_officer='A3'
    w.selected_target='B1' # stale commander target from old side-panel route
    w.targeting=False
    w.target_mode='battle_move'
    w.target_phase=None; w.target_actor=None
    w.target_enemy=None; w.target_enemy_hex=None
    w.target_q=0;w.target_r=0
    w.bribe_gold=1
    from third_branch.nes_battle_menu import NESBattleMenu
    w.menu=NESBattleMenu();w.controller_index=0;w.action_buttons=[]
    w.pending_withdraw=None;w.pending_prisoner=False
    w.message=Message(); w.redraw=Mock(); w._issue=Mock()
    return c,w


def point(w,q,r):
    w.target_q=q;w.target_r=r


def choose_b2(w):
    w._menu_action('Tactics','Switch sides')
    assert w.targeting and w.target_phase=='enemy'
    point(w,10,5)
    w.on_nes_button('a')
    assert w.target_phase=='gold' and w.target_enemy=='B2'


def test_switch_sides_controller_selects_real_enemy_not_stale_commander():
    c,w=fixture();before=save_bytes(c)
    choose_b2(w)
    w.on_nes_button('a')
    w._issue.assert_called_once_with('battle_bribe',{'gold':1,'target_officer_id':'B2'})
    assert w.selected_target=='B1' # cursor has not been commandeered
    assert not w.targeting and save_bytes(c)==before


def test_enemy_list_head_cannot_be_bribed_before_spending_or_rng():
    c,w=fixture(); before=save_bytes(c);rng=copy.deepcopy(c.state['core']['rng_state'])
    w._menu_action('Tactics','Switch sides');point(w,10,6);w.on_nes_button('a')
    assert w.targeting and w.target_phase=='enemy'
    assert 'commander' in w.message.value.lower()
    assert not w._issue.called and save_bytes(c)==before
    assert c.state['core']['rng_state']==rng


def test_empty_hex_and_cancel_are_no_action():
    c,w=fixture();before=save_bytes(c)
    w._menu_action('Tactics','Switch sides');point(w,0,5);w.on_nes_button('a')
    assert w.targeting and w.target_phase=='enemy' and not w._issue.called
    w.on_nes_button('b')
    assert not w.targeting and w.target_enemy is None
    assert not w._issue.called and save_bytes(c)==before


def test_stale_moved_enemy_or_changed_player_actor_atomic_refusal():
    c,w=fixture();choose_b2(w)
    target=next(u for u in c.state['battle']['officer_units'] if u['id']=='B2')
    target['q']=9
    before=save_bytes(c)
    w.on_nes_button('a')
    assert not w._issue.called and not w.targeting and save_bytes(c)==before
    c,w=fixture();choose_b2(w);before=save_bytes(c)
    w.selected_officer='A4'
    w.on_nes_button('a')
    assert not w._issue.called and not w.targeting and save_bytes(c)==before


def test_offer_gold_limited_to_1_to_min_99_and_current_available_pool():
    c,w=fixture();choose_b2(w)
    old=save_bytes(c)
    for _ in range(110):w.on_nes_button('right')
    assert w.bribe_gold==99
    for _ in range(110):w.on_nes_button('left')
    assert w.bribe_gold==1
    c.state['battle']['gold_a_cents']=300
    w.on_nes_button('right')
    assert w.bribe_gold==2
    w.on_nes_button('right')
    assert w.bribe_gold==3
    w.on_nes_button('right')
    assert w.bribe_gold==3
    w.on_nes_button('a')
    w._issue.assert_called_once_with('battle_bribe',{'gold':3,'target_officer_id':'B2'})
    assert validate(c.state)
    assert old!=save_bytes(c) # intentional fixture pool change only


def test_real_engine_chosen_B2_refusal_spends_offer_and_conserves_officers():
    c,w=fixture(); choose_b2(w);w.on_nes_button('a')
    kind,params=w._issue.call_args.args
    before_total=sum(sum(a['stations'].values())+a['engaged'] for a in c.state['armies'].values())
    before_gold=c.state['battle']['gold_a_cents']
    result=c.do(kind,'A3',dict(params,officer_id='A3'),'run032:actual_bribe')
    assert result['target']=='B2' and result['gold_spent']==1
    assert c.state['battle']['gold_a_cents']==before_gold-100
    assert sum(sum(a['stations'].values())+a['engaged'] for a in c.state['armies'].values())==before_total
    assert validate(c.state)


@pytest.mark.skipif(not os.environ.get('DISPLAY'),reason='real Linux Tk/Xvfb required')
def test_real_tk_controller_switch_sides_and_process_cold_load(tmp_path):
    import tkinter as tk
    c=make_battle();c.state['human_faction']='A'
    root=tk.Tk();root.withdraw()
    parent=SimpleNamespace(root=root,c=c,path=tmp_path/'run032.ro2',counter=0,
        audio=SimpleNamespace(effect=Mock(),play=Mock()),selected='p3',
        redraw=Mock(),pause=Mock())
    w=TacticalWindow(parent)
    try:
        w.selected_officer='A3'; w.selected_target='B1'
        before=save_bytes(parent.c)
        w._menu_action('Tactics','Switch sides')
        w.target_q=10;w.target_r=6;w.on_nes_button('a')
        assert w.target_phase=='enemy' and not parent.path.exists()
        w.target_r=5;w.on_nes_button('a')
        assert w.target_enemy=='B2' and w.target_phase=='gold'
        w.on_nes_button('a')
        assert parent.path.exists() and parent.counter==1
        assert parent.c.state['battle']['gold_a_cents']==9800
        assert validate(parent.c.state)
        # Disk bytes are a real Campaign object and a separate process can read them.
        import subprocess,sys
        script=('import sys;from third_branch.integrated import load_file;'
                'c=load_file(sys.argv[1]);print(c.fingerprint)')
        result=subprocess.run([sys.executable,'-c',script,str(parent.path)],
            cwd=Path(__file__).parents[1],capture_output=True,text=True,check=True)
        assert result.stdout.strip()==parent.c.fingerprint
        assert save_bytes(parent.c)!=before
    finally:
        w.close();root.destroy()
