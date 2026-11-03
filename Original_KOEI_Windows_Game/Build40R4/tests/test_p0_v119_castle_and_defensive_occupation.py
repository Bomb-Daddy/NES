"""P0 adversarial: 50-v-240 occupied castle and no suicidal neutral occupation.
Director source-defense-first decision: these are third-branch policy tests, not NES fidelity.
"""
import pytest
from third_branch.historical_s1 import new_campaign_historical_s1
from third_branch.historical_ai import STYLES, take_turn
from third_branch import historical_ai_v3 as v3
rank_choices=v3.rank_choices
from third_branch.integrated import load_bytes, save_bytes
from third_branch import army_cohorts as ac


def test_v119_occupied_castle_must_defeat_defenders_not_auto_teleport():
    c = new_campaign_historical_s1(7)
    initial_defenders = c.state['core']['provinces']['p10']['troops']
    assert initial_defenders >= 50
    result = c.do('attack', 'nes:000', {'target':'p10','troops':50},'p0:castle:start')
    assert result['defenders'] == initial_defenders
    assert c.state['battle']['d']['troops'] == initial_defenders
    if c.state['battle'].get('duel_pending'):
        c.do('battle_duel_response','nes:000',{'accepted':False},'p0:castle:cpu-challenge')
    c.do('battle_auto','nes:000',{},'p0:castle:auto')
    result = c.state['history'][-1]
    assert result['winner'] == 'DEFENDER'
    assert result['defender_survivors'] > 0
    assert c.state['owners']['p10'] == 'R8'
    assert load_bytes(save_bytes(c)).fingerprint == c.fingerprint


def test_v119_occupied_castle_does_not_move_onto_defender_before_resolution():
    c = new_campaign_historical_s1(7)
    c.do('attack','nes:000',{'target':'p10','troops':50},'p0:castle:begin')
    if c.state['battle'].get('duel_pending'):
        c.do('battle_duel_response','nes:000',{'accepted':False},'p0:castle:cpu-challenge')
    for i in range(5):
        if c.state['battle'] is None:break
        c.do('battle_end_turn','nes:000',{},'p0:castle:round:'+str(i))
        if c.state['battle'] is not None:
            b=c.state['battle']
            assert b['d']['troops'] == 0 or (b['a']['q'],b['a']['r']) != (b['d']['q'],b['d']['r'])


def test_v119_defense_guard_rejects_exposed_border_and_accepts_defensible_source():
    c = new_campaign_historical_s1(17)
    s=c.state
    p='p9'; faction=s['owners'][p]
    # The test fixture changes a *policy observation*, not a game command.
    for neighbor in s['neighbors'][p]:
        if s['owners'][neighbor] not in (None,faction):
            hostile=neighbor;break
    s['core']['provinces'][p]['troops']=260
    s['core']['provinces'][hostile]['troops']=240
    assert not v3._source_safe(s,p,faction,240)
    s['core']['provinces'][p]['troops']=600
    assert v3._source_safe(s,p,faction,240)
    assert v3._source_reserve(s,p,faction,240) > 0


def test_v119_real_garrison_transfer_then_legal_neutral_occupation():
    c = new_campaign_historical_s1(17)
    s=c.state
    src='p14';f=s['owners'][src]
    style=STYLES[int(f[1:])]
    initial=sum(v is not None for v in s['owners'].values())
    assert any(s['owners'][n] is None for n in s['neighbors'][src])
    n=s['core']['provinces'][src]['troops']
    before=c.fingerprint
    choices=rank_choices(c,f,style)
    # With authentic separate per-general starting armies, the old artificial
    # transfer is no longer required: a legal, source-safe vacant move exists.
    assert any(v.kind=='relocate_army' for _,v in choices)
    assert c.fingerprint==before  # preview/rank must not mutate state
    result=take_turn(c,f,style,prefix='p0:defense')
    assert result['kind']=='relocate_army'
    assert c.state['core']['provinces'][src]['troops']==n-result['result']['troops']
    assert sum(v is not None for v in c.state['owners'].values())==initial+1
    assert c.state['core']['provinces'][src]['troops']>=15
    assert ac.totals(c.state)=={k:v['troops'] for k,v in c.state['core']['provinces'].items()}
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_v119_four_month_rotation_improves_legal_political_map():
    c=new_campaign_historical_s1(17)
    start=sum(v is not None for v in c.state['owners'].values())
    for month in range(4):
        for i,style in enumerate(STYLES):
            take_turn(c,f'R{i}',style,prefix='p0:four-month')
        c.advance_month()
    assert sum(v is not None for v in c.state['owners'].values())>start
    assert load_bytes(save_bytes(c)).fingerprint == c.fingerprint
