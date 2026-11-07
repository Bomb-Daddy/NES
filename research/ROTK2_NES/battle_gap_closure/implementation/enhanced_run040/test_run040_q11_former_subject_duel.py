"""Q11 former-subject duel-damage safety: source v1.4 hsovl52 and ai_eval_odds_b.

Source describes both ceilings War//10 if either officer has the +3 protected
bits OR is a former subject of the opposing ruler. Historical provenance in
this port is explicit ``switched`` plus known ``original_faction``. Unknown
historical provenance MUST NOT be invented. This verifies port behavior, not
an English NES emulator/controller screenshot golden.
"""
import copy
import os
import subprocess
import sys
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import Mock, patch
import pytest

from third_branch import officer_battle_adapter as ba
from third_branch.integrated import save_bytes, load_bytes, validate
from third_branch.tactical_view import TacticalWindow
from test_build16_q11_duel import battle


def _prepared(seed=0, who='A'):
    c=battle()
    b=c.state['battle']
    actor=ba.find(b, 'A', 'A3')
    actor['war']=40
    target=ba.choose_ai_duel_acceptor(b,actor)
    assert target is not None and target['side']=='D'
    actor['switched']=False
    target['switched']=False
    actor['protected']=False
    target['protected']=False
    if who=='A':
        actor['switched']=True;actor['original_faction']=b['defender']
    if who=='D':
        target['switched']=True;target['original_faction']=b['attacker']
    return c, actor, target


def _bounds(c, actor, target, rid):
    bounds=[]
    def controlled(_state,n):
        bounds.append(n)
        return 0
    with patch.object(ba,'randrange',side_effect=controlled):
        result=c.do('battle_duel',actor['id'],{'officer_id':actor['id'],
                     'target_officer_id':target['id'],'accepted':True},rid)
    assert result['duel_result']=='DRAW'
    assert c.state['battle']['duel_done']
    assert validate(c.state)
    return bounds


def test_former_enemy_subject_as_attacker_uses_original_two_ceilings():
    c,u,v=_prepared(who='A')
    assert _bounds(c,u,v,'run040:former:A')[:2]==[4,v['war']//10]


def test_former_enemy_subject_as_defender_uses_original_two_ceilings():
    c,u,v=_prepared(who='D')
    assert _bounds(c,u,v,'run040:former:D')[:2]==[4,v['war']//10]


def test_unknown_switched_flag_does_not_invent_historical_former_subject():
    c,u,v=_prepared(who='A');u['switched']=False
    assert _bounds(c,u,v,'run040:unknown')[:2]==[20,15]


def test_switched_but_unrelated_original_faction_does_not_trigger_limit():
    c,u,v=_prepared(who='A');u['original_faction']='UNRELATED'
    assert _bounds(c,u,v,'run040:unrelated')[:2]==[20,15]


def test_original_special_protection_still_uses_ten_percent_ceiling():
    c,u,v=_prepared(who='none');v['protected']=True
    assert _bounds(c,u,v,'run040:special')[:2]==[4,v['war']//10]


def test_duel_historical_protection_survives_save_cold_load_and_rng():
    c,u,v=_prepared(who='A')
    data=save_bytes(c)
    restored=load_bytes(data)
    assert restored.fingerprint==c.fingerprint
    u2=ba.find(restored.state['battle'],'A',u['id'])
    v2=ba.find(restored.state['battle'],'D',v['id'])
    assert _bounds(c,u,v,'run040:cold')==_bounds(restored,u2,v2,'run040:cold')
    assert c.fingerprint==restored.fingerprint


@pytest.mark.skipif(not os.environ.get('DISPLAY'),reason='requires actual Linux Tk/Xvfb')
def test_actual_six_command_tk_and_two_process_disk_save_cold_load(tmp_path):
    import tkinter as tk
    c,u,v=_prepared(who='A')
    root=tk.Tk();root.withdraw()
    parent=SimpleNamespace(root=root,c=c,path=tmp_path/'run040.ro2',counter=0,
        audio=SimpleNamespace(effect=Mock(),play=Mock()),selected='p5',redraw=Mock(),pause=Mock())
    w=TacticalWindow(parent)
    try:
        assert tuple(w.menu.labels)==('Move','Attack','Flee','View','Tactics','Wait')
        root.update_idletasks()
        assert w.root.geometry().startswith('1280x720')
        w.selected_officer=u['id']
        # The CPU's original response is selected within the actual UI, not
        # a test-only alternative command or a fabricated direct call.
        w.duel()
        assert parent.counter==1
        assert parent.path.is_file()
        assert validate(parent.c.state)
        script=('import sys;from third_branch.integrated import load_file;'
                'print(load_file(sys.argv[1]).fingerprint)')
        r=subprocess.run([sys.executable,'-c',script,str(parent.path)],
            cwd=str(Path(__file__).parents[1]),capture_output=True,text=True,check=True)
        assert r.stdout.strip()==parent.c.fingerprint
    finally:
        w.close();root.destroy()
