"""Run015 Q14: ordinary Release with no eligible refuge.

Original sengo $84F6/root $4EB0: free/unaffiliated officer persists in
contested province, with zero soldiers and training 100. Campaign tests run
against real state/ledger/commands, not an isolated Q14 helper.
"""
import copy
import pytest
from og.core.fixed import CoreError
from third_branch.integrated import new_campaign, validate, save_bytes, load_bytes
from third_branch import army_cohorts as ac
from third_branch.historical_s1 import list_recruitable


def captured_no_refuge(seed=15001):
    c=new_campaign(seed)
    s=c.state
    # Legitimate one-province B: move its ruler/army from p4 into its only
    # surviving province p5; p4 becomes neutral. All old soldiers preserved.
    s['core']['officers']['B1']['province_id']='p5'
    army=ac.ensure(s,'B1')
    n=army['stations'].pop('p4')
    army['stations']['p5']=n
    s['owners']['p4']=None
    ac.refresh(s)
    assert validate(s)
    c.do('attack','A3',{'target':'p5','troops':120,'gold':1},f'run015:{seed}:attack')
    c.state['battle']['rice_d']=0
    c.do('battle_end_turn','A3',{},f'run015:{seed}:win')
    assert c.state['battle']['pending_outcome']=='ATTACKER'
    assert {'B1','B2'}<=set(c.state['battle']['prisoner_queue'])
    return c


def settle_release(seed=15001):
    c=captured_no_refuge(seed)
    c.do('battle_prisoner','A3',{'officer_id':'B2','decision':'release'},f'run015:{seed}:release')
    c.do('battle_prisoner','A3',{'officer_id':'B1','decision':'behead'},f'run015:{seed}:behead')
    return c


def test_free_officer_zero_troops_training100_and_no_phantom_faction():
    c=settle_release()
    s=c.state
    assert s['battle'] is None
    assert s['core']['officers']['B2']['province_id']=='p5'
    assert s['free_officers']['B2']['province_id']=='p5'
    assert 'B2' not in s['officer_faction']
    assert 'B2' not in s['officer_loyalty']
    assert s['unit_training']['B2']==100
    assert 'army:B2' not in s['armies']
    assert s['owners']['p5']=='A'
    assert validate(s)


def test_free_officer_is_eligible_for_actual_recruit_officer_at_location():
    c=settle_release(15002)
    assert c.state['free_officers']['B2']['province_id']=='p5'
    assert 'B2' in list_recruitable(c.state,'p5')
    assert 'B2' not in list_recruitable(c.state,'p3')
    # Position an existing victorious A commander at his occupied p5 garrison
    # to exercise real strategic recruitment; never teleport the free officer.
    c.state['core']['officers']['A3']['province_id']='p5'
    assert validate(c.state)
    result=c.do('recruit_officer','A3',{'officer_id':'B2'},'run015:recruit')
    assert result['officer_id']=='B2'
    assert c.state['officer_faction']['B2']=='A'
    assert 'B2' not in c.state['free_officers']
    assert 'B2' not in list_recruitable(c.state,'p5')
    assert c.state['unit_training']['B2']==100
    assert validate(c.state)


def test_save_and_cold_load_free_officer_and_conserved_troops():
    c=settle_release(15003)
    before=c.fingerprint
    a=load_bytes(save_bytes(c))
    assert a.fingerprint==before
    assert a.state['free_officers']['B2']['province_id']=='p5'
    assert ac.totals(a.state)=={pid:p['troops'] for pid,p in a.state['core']['provinces'].items()}


def test_invalid_remote_recruit_has_byte_identical_rollback():
    c=settle_release(15004)
    before=save_bytes(c)
    with pytest.raises(CoreError):
        c.do('recruit_officer','A1',{'officer_id':'B2'},'run015:remote')
    assert save_bytes(c)==before


def test_corrupt_free_owner_or_army_rejected():
    c=settle_release(15005)
    bad=copy.deepcopy(c.state)
    bad['officer_faction']['B2']='B'
    with pytest.raises(CoreError):validate(bad)
    bad=copy.deepcopy(c.state)
    bad['armies']['army:B2']={'commander':'B2','stations':{'p5':1},'engaged':0}
    with pytest.raises(CoreError):validate(bad)

@pytest.mark.skipif(not __import__('os').environ.get('DISPLAY'),reason='requires Linux Xvfb Tk')
def test_real_six_command_tk_prisoner_release_to_free_list(tmp_path):
    import tkinter as tk
    from types import SimpleNamespace
    from unittest.mock import Mock
    from third_branch.tactical_view import TacticalWindow
    from third_branch.integrated import load_file
    root=tk.Tk();root.withdraw()
    c=captured_no_refuge(15006)
    parent=SimpleNamespace(root=root,c=c,path=tmp_path/'q14-free.ro2',
        counter=0,audio=SimpleNamespace(effect=Mock(),play=Mock()),
        selected='p5',redraw=Mock(),pause=Mock())
    w=TacticalWindow(parent)
    try:
        assert [label for label,_ in w.controller_actions]==['Move','Attack','Flee','View','Tactics','Wait']
        # Navigate real controller View->Prisoner fate, then use SELECT to
        # choose the ordinary B2 rather than the ruler B1.
        for _ in range(3):w.on_nes_button('down')
        w.on_nes_button('a')
        w.on_nes_button('down');w.on_nes_button('down');w.on_nes_button('a')
        assert w.pending_prisoner
        for _ in range(3):
            if w._prisoner_choice()=='B2':break
            w.on_nes_button('select')
        assert w._prisoner_choice()=='B2'
        assert 'release' in w._prisoner_options()
        for _ in range(len(w._prisoner_options())):
            if w._prisoner_options()[w.prisoner_decision_index]=='release':break
            w.on_nes_button('right')
        w.on_nes_button('a')
        assert parent.c.state['battle'] is not None  # Ruler still awaiting fate
        assert parent.c.state['battle']['prisoner_decisions']['B2']=='release'
        # Follow up with B1 Behead, restoring the separate free-list state.
        w._prisoner_prompt()
        assert w._prisoner_choice()=='B1'
        for _ in range(len(w._prisoner_options())):
            if w._prisoner_options()[w.prisoner_decision_index]=='behead':break
            w.on_nes_button('right')
        w.on_nes_button('a')
        assert parent.c.state['battle'] is None
        assert 'B2' in parent.c.state['free_officers']
        assert parent.path.is_file()
        assert load_file(parent.path).fingerprint==parent.c.fingerprint
        assert validate(parent.c.state)
    finally:
        w.close();root.destroy()
