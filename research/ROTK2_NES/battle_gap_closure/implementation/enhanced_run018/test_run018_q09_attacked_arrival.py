"""Source-backed D648/D69A partial fidelity: attacked arrivals are live and
retain battle-scale troops.  Recipient tactical activation remains UNVERIFIED.
Fixtures: Classic Run017 source tests (NOT original emulator golden)."""
import copy
from types import SimpleNamespace
import pytest
from og.core.world import add_officer
from og.core.fixed import CoreError
from third_branch import army_cohorts as ac
from third_branch.integrated import new_campaign, validate, save_bytes, load_bytes
from third_branch.tactical_view import TacticalWindow


def ready(seed=1801, *, attacked=True, known=True):
    c=new_campaign(seed);s=c.state
    add_officer(s['core'],'B9','p5',(60,70,55,70,60))
    s['officer_faction']['B9']='B';s['officer_loyalty']['B9']=70
    ac.add(s,'B9','p5',50);ac.refresh(s)
    s['human_faction']='B';s['owners']['p6']='B';s['officer_faction']['C1']='B'
    if attacked:s['q14_source_attacked']={'p6':True}
    if known:
        s['q09_source_officer_bytes']={'B9':{'flag2':0x40}}
        s['q09_source_byte_provenance']='SOURCE_FIXTURE_NOT_EMULATOR'
    assert validate(s)
    c.do('attack','A3',{'target':'p5','troops':120,'gold':1},f'r018:{seed}:start')
    return c


def flee(c):
    return c.do('battle_withdraw','B2',{'officer_id':'B9','destination':'p6'},'r018:flee')


def test_source_attacked_arrival_commits_live_officer_and_preserves_battle_scale():
    c=ready()
    original=next(u for u in c.state['battle']['officer_units'] if u['id']=='B9')['troops100']
    assert original==5000
    result=flee(c)
    assert result['withdrawn']==['B9'] and not result['resolved']
    s=c.state;b=s['battle']
    u=next(u for u in b['officer_units'] if u['id']=='B9')
    assert u['fled'] and u['q09_live_inserted']
    assert s['core']['officers']['B9']['province_id']=='p6'
    assert ac.strength(s,'B9','p6')==50 and ac.strength(s,'B9','p5')==0
    arriving=s['q09_pending_attacked_arrivals']['p6']
    assert len(arriving)==1
    assert arriving[0]['officer']=='B9' and arriving[0]['troops100']==original
    assert arriving[0]['recipient_status']=='TACTICAL_ACTIVATION_UNVERIFIED'
    assert any(e.get('kind')=='q09_attacked_destination_arrival' for e in b['combat_events'])
    assert validate(s)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_attacked_arrival_survives_unrelated_battle_settlement_and_cold_load():
    c=ready(1802);flee(c)
    before=copy.deepcopy(c.state['q09_pending_attacked_arrivals'])
    # Resolve the remaining original battle; attacked recipient isn't normalized away.
    c.state['battle']['rice_a']=0
    c.do('battle_end_turn','B2',{},'r018:finish')
    assert c.state['battle'] is None
    assert c.state['q09_pending_attacked_arrivals']==before
    assert ac.strength(c.state,'B9','p6')==50
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_peaceful_source_arrival_keeps_existing_behavior_and_no_secondary_record():
    c=ready(1803,attacked=False);flee(c)
    assert c.state.get('q09_pending_attacked_arrivals',{})=={}
    assert ac.strength(c.state,'B9','p6')==50
    assert validate(c.state)


def test_source_unknown_compatibility_not_misreported_as_original():
    c=ready(1804,known=False);flee(c)
    assert c.state.get('q09_pending_attacked_arrivals',{})=={}
    assert c.state['core']['officers']['B9']['province_id']=='p5'
    assert any(e['kind']=='q09_unknown_source_fallback' for e in c.state['battle']['combat_events'])
    assert validate(c.state)


def test_controller_nes_flee_destination_routes_real_campaign(monkeypatch):
    c=ready(1805)
    w=TacticalWindow.__new__(TacticalWindow)
    w.parent=SimpleNamespace(c=c)
    class Message:
        def set(self,value):self.value=value
    w.message=Message();w.selected_officer='B9';w.pending_withdraw=None
    w.targeting=False;w.redraw=lambda:None
    events=[];w._issue=lambda k,p:events.append((k,p))
    w._withdraw_prompt(False)
    assert w.pending_withdraw is not None
    for _ in range(5):
        if w._withdraw_legal_destinations()[w.withdraw_index]=='p6':break
        w.on_nes_button('right')
    w.on_nes_button('a')
    assert events==[('battle_withdraw',{'whole_army':False,'destination':'p6','officer_id':'B9'})]
    c.do(events[0][0],'B2',events[0][1],'r018:controller')
    assert c.state['core']['officers']['B9']['province_id']=='p6'
    assert validate(c.state)


def test_invalid_attacked_recipient_record_rejected_on_cold_load():
    c=ready(1806);flee(c)
    s=copy.deepcopy(c.state)
    s['q09_pending_attacked_arrivals']['p6'][0]['troops100']=-1
    with pytest.raises(CoreError,match='BAD_Q09_ATTACKED_ARRIVAL'):
        validate(s)


def test_two_source_tagged_whole_army_arrivals_keep_new_head_order_and_no_rng(monkeypatch):
    c=ready(1807)
    s=c.state
    s['q09_source_officer_bytes']['B2']={'flag2':0x40}
    s['battle']['q09_source_officer_bytes']['B2']={'flag2':0x40}
    from third_branch import officer_battle_adapter as ba
    monkeypatch.setattr(ba,'randrange',lambda *_: (_ for _ in ()).throw(AssertionError('NO_RNG_FLAG40')))
    result=c.do('battle_withdraw','B2',{'whole_army':True,'destination':'p6',
         'officer_destinations':{'B2':'p6','B9':'p6'}},'r018:whole')
    assert result['resolved'] and not result['captured']
    s=c.state
    assert [v['officer'] for v in s['q09_pending_attacked_arrivals']['p6']]==['B9','B2']
    assert all(v['recipient_status']=='TACTICAL_ACTIVATION_UNVERIFIED'
               for v in s['q09_pending_attacked_arrivals']['p6'])
    assert c.state['battle'] is None and validate(s)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
