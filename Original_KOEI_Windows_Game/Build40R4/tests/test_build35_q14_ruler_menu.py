"""Q14 v1.4: no fabricated remote refuge for a defeated ruler; controller options match rules."""
import pytest
from types import SimpleNamespace
from og.core.fixed import CoreError
from third_branch.integrated import new_campaign, save_bytes, load_bytes, validate
from third_branch import officer_battle_adapter as adapter
from third_branch.tactical_view import TacticalWindow


def awaiting_ruler():
    c=new_campaign(3501)
    s=c.state
    s['factions']['B']['ruler_officer']='B2'
    c.do('attack','A3',{'target':'p5','troops':120,'gold':1},'b35:begin')
    c.state['battle']['rice_d']=0
    c.do('battle_end_turn','A3',{},'b35:win')
    assert c.state['battle']['pending_outcome']=='ATTACKER'
    assert 'B2' in c.state['battle']['prisoner_queue']
    assert s['owners']['p4']=='B' and 'p4' not in s['neighbors']['p5']
    return c


def test_remote_only_ruler_home_cannot_be_used_as_release_refuge():
    c=awaiting_ruler();before=c.fingerprint
    assert adapter.prisoner_options(c.state,c.state['battle'],'B2')==('behead',)
    with pytest.raises(CoreError) as exc:
        c.do('battle_prisoner','A3',{'officer_id':'B2','decision':'release'},'b35:bad')
    assert exc.value.code=='NO_RELEASE_DESTINATION'
    assert c.fingerprint==before
    assert load_bytes(save_bytes(c)).fingerprint==before
    out=c.do('battle_prisoner','A3',{'officer_id':'B2','decision':'behead'},'b35:behead')
    assert out['resolved'] and c.state['factions']['B']['ruler_officer'] is None
    assert validate(c.state)


def test_controller_only_shows_legal_ruler_choice_and_executes():
    c=awaiting_ruler();w=TacticalWindow.__new__(TacticalWindow)
    w.parent=SimpleNamespace(c=c)
    class Message:
        def set(self,value):self.value=value
    w.message=Message();w.prisoner_index=0;w.prisoner_decision_index=0;w.targeting=False
    w.controller_actions=[('Prisoner Fate',w.prisoner)];w.controller_index=0
    issued=[];w._issue=lambda kind,p:issued.append((kind,p))
    w.on_nes_button('right');w.on_nes_button('a')
    assert issued==[('battle_prisoner',{'officer_id':'B2','decision':'behead'})]
    c.do('battle_prisoner','A3',issued[0][1],'b35:controller')
    assert validate(c.state)


def test_nonruler_still_has_all_three_choices():
    c=new_campaign(3502)
    assert adapter.prisoner_options(c.state,{'defender':'B','dst':'p5'},'B2')==('recruit','release','behead')


def test_cpu_ruler_no_adjacent_refuge_beheaded_without_modal():
    c=new_campaign(3504)
    c.state['factions']['B']['ruler_officer']='B2'
    c.do('attack','A3',{'target':'p5','troops':120,'gold':1},'b35:cpu:start')
    c.state['human_faction']='C'
    c.state['battle']['rice_d']=0
    out=c.do('battle_end_turn','A3',{},'b35:cpu:win')
    assert out['resolved']
    assert c.state['history'][-1]['prisoner_decisions']=={'B2':'behead'}
    assert 'B2' not in c.state['core']['officers']
    assert c.state['factions']['B']['ruler_officer'] is None
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
