"""Q14 real campaign prisoner choices: red-before/green-after, not ROM acceptance."""
import pytest
from og.core.fixed import CoreError
from third_branch.integrated import new_campaign, save_bytes, load_bytes, validate


def awaiting():
    c = new_campaign(1814)
    c.do('attack', 'A3', {'target':'p5','troops':120,'gold':1}, 'q14:begin')
    c.state['battle']['rice_d'] = 0
    c.do('battle_end_turn','A3',{},'q14:win')
    return c


def test_recruit_explicit_and_persistent_not_auto_join():
    c=awaiting()
    assert c.state['battle'] is not None
    assert c.state['battle']['pending_outcome']=='ATTACKER'
    assert 'B2' in c.state['battle']['prisoner_queue']
    assert c.state['officer_faction']['B2']=='B'
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
    out=c.do('battle_prisoner','A3',{'officer_id':'B2','decision':'recruit'},'q14:recruit')
    assert out['decision']=='recruit' and out['resolved']
    assert c.state['owners']['p5']=='A'
    assert c.state['officer_faction']['B2']=='A'
    assert c.state['core']['officers']['B2']['province_id']=='p5'
    assert c.state['history'][-1]['prisoner_decisions']=={'B2':'recruit'}
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_release_uses_existing_home_without_fabricated_garrison():
    c=awaiting()
    out=c.do('battle_prisoner','A3',{'officer_id':'B2','decision':'release'},'q14:release')
    assert out['resolved']
    assert c.state['officer_faction']['B2']=='B'
    assert c.state['core']['officers']['B2']['province_id']=='p4'
    assert c.state['owners']['p5']=='A'
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_behead_removes_officer_and_preserves_campaign_validity():
    c=awaiting()
    out=c.do('battle_prisoner','A3',{'officer_id':'B2','decision':'behead'},'q14:behead')
    assert out['resolved']
    assert 'B2' not in c.state['core']['officers']
    assert 'B2' not in c.state['officer_faction']
    assert 'army:B2' not in c.state['armies']
    assert c.state['history'][-1]['prisoner_decisions']=={'B2':'behead'}
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_invalid_decision_and_other_orders_are_atomic():
    c=awaiting();h=c.fingerprint
    for idx,(kind,params) in enumerate([
        ('battle_prisoner',{'officer_id':'B2','decision':'invalid'}),
        ('battle_prisoner',{'officer_id':'A3','decision':'behead'}),
        ('battle_prisoner',{'officer_id':'B2','decision':'recruit','extra':'bogus'}),
        ('battle_end_turn',{}),
    ]):
        with pytest.raises(CoreError):c.do(kind,'A3',params,f'q14:bad:{idx}')
        assert c.fingerprint==h


def test_controller_prisoner_menu_dispatches_real_persisted_command():
    from types import SimpleNamespace
    from third_branch.tactical_view import TacticalWindow
    class Message:
        def set(self,value):self.value=value
    c=awaiting();w=TacticalWindow.__new__(TacticalWindow)
    w.parent=SimpleNamespace(c=c);w.message=Message()
    w.prisoner_index=0;w.prisoner_decision_index=0;w.targeting=False
    issued=[];w._issue=lambda kind,params:issued.append((kind,params))
    w.controller_actions=[('Prisoner Fate',w.prisoner)];w.controller_index=0
    w.on_nes_button('right');w.on_nes_button('a')
    assert issued==[('battle_prisoner',{'officer_id':'B2','decision':'release'})]
    out=c.do('battle_prisoner','A3',issued[0][1],'q14:controller')
    assert out['resolved'] and validate(c.state)


def test_two_prisoners_require_two_choices_and_no_early_conquest():
    from og.core.world import add_officer
    from third_branch import army_cohorts as ac
    c=new_campaign(1815);s=c.state
    add_officer(s['core'],'B3','p5',(40,50,55,60,60))
    s['officer_faction']['B3']='B';s['officer_loyalty']['B3']=45
    ac.add(s,'B3','p5',30);ac.refresh(s)
    assert validate(s)
    c.do('attack','A3',{'target':'p5','troops':120,'gold':1},'q14:multi:start')
    c.state['battle']['rice_d']=0
    c.do('battle_end_turn','A3',{},'q14:multi:win')
    assert set(c.state['battle']['prisoner_queue'])=={'B2','B3'}
    first=c.do('battle_prisoner','A3',{'officer_id':'B2','decision':'recruit'},'q14:multi:1')
    assert not first['resolved'] and c.state['owners']['p5']=='B'
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
    second=c.do('battle_prisoner','A3',{'officer_id':'B3','decision':'release'},'q14:multi:2')
    assert second['resolved'] and c.state['owners']['p5']=='A'
    assert c.state['officer_faction']['B2']=='A' and c.state['officer_faction']['B3']=='B'
    assert validate(c.state)


def test_behead_remote_army_reassigned_to_real_same_faction_commander():
    from third_branch import army_cohorts as ac
    c=new_campaign(1816);s=c.state
    ac.add(s,'B2','p4',10);ac.refresh(s)
    assert validate(s)
    c.do('attack','A3',{'target':'p5','troops':120,'gold':1},'q14:remote:start')
    c.state['battle']['rice_d']=0
    c.do('battle_end_turn','A3',{},'q14:remote:win')
    assert 'B2' in c.state['battle']['prisoner_queue']
    before=ac.strength(c.state,'B1','p4')+ac.strength(c.state,'B2','p4')
    c.do('battle_prisoner','A3',{'officer_id':'B2','decision':'behead'},'q14:remote:behead')
    assert ac.strength(c.state,'B1','p4')==before
    assert 'B2' not in c.state['core']['officers']
    assert validate(c.state)


def test_ruler_not_recruitable_and_invalid_choice_is_atomic():
    c=new_campaign(1817)
    c.state['factions']['B']['ruler_officer']='B2'
    c.do('attack','A3',{'target':'p5','troops':120,'gold':1},'q14:ruler:start')
    c.state['battle']['rice_d']=0
    c.do('battle_end_turn','A3',{},'q14:ruler:win')
    before=c.fingerprint
    with pytest.raises(CoreError) as err:
        c.do('battle_prisoner','A3',{'officer_id':'B2','decision':'recruit'},'q14:ruler:invalid')
    assert err.value.code=='RULER_CANNOT_BE_RECRUITED'
    assert c.fingerprint==before
    # Q14 v1.4 correction (Build35): p4 is a remote B province, not a
    # neighbor of p5. The original ruler menu cannot offer Release here.
    with pytest.raises(CoreError) as err:
        c.do('battle_prisoner','A3',{'officer_id':'B2','decision':'release'},'q14:ruler:remote-invalid')
    assert err.value.code=='NO_RELEASE_DESTINATION'
    assert c.fingerprint==before
    c.do('battle_prisoner','A3',{'officer_id':'B2','decision':'behead'},'q14:ruler:behead')
    assert validate(c.state)


def test_beheaded_ruler_pointer_does_not_dangle():
    c=new_campaign(1818)
    c.state['factions']['B']['ruler_officer']='B2'
    c.do('attack','A3',{'target':'p5','troops':120,'gold':1},'q14:ruler2:start')
    c.state['battle']['rice_d']=0
    c.do('battle_end_turn','A3',{},'q14:ruler2:win')
    c.do('battle_prisoner','A3',{'officer_id':'B2','decision':'behead'},'q14:ruler2:behead')
    assert c.state['factions']['B']['ruler_officer'] is None
    assert validate(c.state)
