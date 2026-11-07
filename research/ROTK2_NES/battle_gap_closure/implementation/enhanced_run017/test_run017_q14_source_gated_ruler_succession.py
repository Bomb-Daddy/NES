"""Run017 partial Q14: only a *supplied* authenticated ruler successor may take office.

Original SNES-derived Q14 battle fate enters prisoner treatment at sengo $82A4,
$8749; this test never pretends the next ruler's identity is recovered from the
historical English NES scenario. Source-unknown cases remain unresolved.
"""
import copy
from types import SimpleNamespace
import pytest
from og.core.fixed import CoreError
from third_branch.integrated import new_campaign, validate, save_bytes, load_bytes
from third_branch.tactical_view import TacticalWindow


def captured_ruler(seed=17001, successor='B1'):
    c = new_campaign(seed)
    s = c.state
    s['factions']['B']['ruler_officer'] = 'B2'
    if successor is not None:
        s['q14_source_ruler_successor'] = {'B': successor}
    assert validate(s)
    c.do('attack', 'A3', {'target': 'p5', 'troops': 120, 'gold': 1}, f'run017:{seed}:start')
    c.state['battle']['rice_d'] = 0
    c.do('battle_end_turn', 'A3', {}, f'run017:{seed}:win')
    assert c.state['battle']['pending_outcome'] == 'ATTACKER'
    assert c.state['battle']['prisoner_queue'] == ['B2']
    return c


def test_supplied_surviving_successor_installed_from_actual_q14_prisoner_choice():
    c = captured_ruler()
    before = c.fingerprint
    out = c.do('battle_prisoner','A3',{'officer_id':'B2','decision':'behead'},'run017:successor')
    s = c.state
    assert out['resolved'] and before != c.fingerprint
    assert s['factions']['B']['alive'] is True and s['owners']['p4']=='B'
    assert s['factions']['B']['ruler_officer'] == 'B1'
    assert 'B2' not in s['core']['officers']
    assert s['history'][-1]['prisoner_decisions'] == {'B2':'behead'}
    assert validate(s)
    assert load_bytes(save_bytes(c)).fingerprint == c.fingerprint


def test_source_unknown_does_not_fabricate_a_new_ruler():
    c = captured_ruler(17002, successor=None)
    out = c.do('battle_prisoner','A3',{'officer_id':'B2','decision':'behead'},'run017:unknown')
    assert out['resolved']
    assert c.state['factions']['B']['ruler_officer'] is None
    assert c.state['factions']['B']['alive'] is True
    assert validate(c.state)


def test_wrong_faction_and_captured_successor_rejected_without_mutation():
    for proposed in ('A3', 'B2', 'C1', 'BAD'):
        c = captured_ruler(17010 + len(proposed), successor=None)
        c.state['q14_source_ruler_successor'] = {'B': proposed}
        snapshot=save_bytes(c)
        with pytest.raises(CoreError,match='Q14_SOURCE_SUCCESSOR_INVALID'):
            c.do('battle_prisoner','A3',{'officer_id':'B2','decision':'behead'},
                 f'run017:invalid:{proposed}')
        assert save_bytes(c)==snapshot


def test_controller_visible_beheading_reaches_real_campaign_succession():
    c=captured_ruler(17040)
    w=TacticalWindow.__new__(TacticalWindow)
    w.parent=SimpleNamespace(c=c)
    class Message:
        def set(self, value): self.value=value
    w.message=Message()
    w.prisoner_index=0; w.prisoner_decision_index=0; w.targeting=False
    w.pending_prisoner=False; w.redraw=lambda:None
    issued=[]; w._issue=lambda kind,p:issued.append((kind,p))
    w._prisoner_prompt()
    w.on_nes_button('right'); w.on_nes_button('a')
    assert issued == [('battle_prisoner', {'officer_id':'B2','decision':'behead'})]
    c.do('battle_prisoner','A3',issued[0][1],'run017:controller')
    assert c.state['factions']['B']['ruler_officer']=='B1'
    assert validate(c.state)


def test_save_cold_load_preserves_source_successor_before_the_battle_choice(tmp_path):
    c=captured_ruler(17050)
    path=tmp_path/'prisoner_pending.ro2'
    path.write_bytes(save_bytes(c))
    cold=load_bytes(path.read_bytes())
    assert cold.state['q14_source_ruler_successor']=={'B':'B1'}
    cold.do('battle_prisoner','A3',{'officer_id':'B2','decision':'behead'},'run017:cold')
    persisted=tmp_path/'post.ro2'; persisted.write_bytes(save_bytes(cold))
    assert load_bytes(persisted.read_bytes()).state['factions']['B']['ruler_officer']=='B1'
