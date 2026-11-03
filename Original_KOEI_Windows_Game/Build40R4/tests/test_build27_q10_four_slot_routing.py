"""Q04/Q10: original four lists separate main armies from reinforcement queues."""
from unittest.mock import patch
from third_branch.integrated import new_campaign,save_bytes,load_bytes,validate
from third_branch import officer_battle_adapter as ba,army_cohorts as ac
from og.core.world import add_officer


def test_multiple_original_attackers_are_in_main_slot_with_one_commander():
    c=new_campaign(2701);s=c.state
    add_officer(s['core'],'A4','p3',(55,65,70,60,70))
    s['officer_faction']['A4']='A';s['officer_loyalty']['A4']=72
    ac.add(s,'A4','p3',45);ac.refresh(s)
    c.do('attack','A3',{'target':'p5','troops':120},'q10:two:start')
    s=c.state;b=s['battle']
    ac.take(s,'A4','p3',30)
    ac.ensure(s,'A4')['engaged']+=30
    b['participants'].append({'id':'army:A4','troops':30,'home':'p3'})
    b['a']['troops']+=30
    ba.initialize(s,b)
    ac.refresh(s)
    attackers=[u for u in b['officer_units'] if u['side']=='A']
    assert len(attackers)==2
    assert {u['slot'] for u in attackers}=={2}
    assert [u['id'] for u in attackers if u['commander']]==['A3']
    assert validate(s)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_low_loyalty_reinforcements_never_spontaneously_switch_or_draw_rng():
    c=new_campaign(2702);s=c.state
    add_officer(s['core'],'A4','p3',(55,65,70,60,70))
    s['officer_faction']['A4']='A';s['officer_loyalty']['A4']=0
    ac.add(s,'A4','p3',45);ac.refresh(s)
    c.do('attack','A3',{'target':'p5','troops':120},'q10:queue:start')
    s=c.state;b=s['battle']
    ba._queue_reinforcements(s,b,{'source':'p3','officer_ids':['A4'],
                               'rice':0,'gold':0,'arrival_day':2},side='A')
    assert next(u for u in b['officer_units'] if u['id']=='A4')['slot']==3
    before=c.fingerprint
    with patch.object(ba,'randrange',side_effect=lambda state,n:99) as roll:
        assert ba._spontaneous_switches(s,b)==[]
        assert roll.call_count==0
    assert c.fingerprint==before
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
    assert validate(s)


def test_reinforcement_list_has_one_real_head_and_persists():
    c=new_campaign(2703);s=c.state
    for oid in ('A4','A5'):
        add_officer(s['core'],oid,'p3',(55,65,70,60,70))
        s['officer_faction'][oid]='A';s['officer_loyalty'][oid]=72
        ac.add(s,oid,'p3',25)
    ac.refresh(s)
    c.do('attack','A3',{'target':'p5','troops':120},'q10:head:start')
    c.do('battle_reinforce','A3',{'source':'p3','officer_ids':['A4','A5'],
       'rice':0,'gold':0,'arrival_day':2},'q10:head:reinforce')
    b=c.state['battle'];reserve=[u for u in b['officer_units'] if u['slot']==3]
    assert [u['id'] for u in reserve if u['commander']]==['A4']
    assert [u['id'] for u in reserve if not u['commander']]==['A5']
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
    assert validate(c.state)


def test_cold_loaded_build26_initial_attacker_slot_migrates_on_next_command():
    c=new_campaign(2704);s=c.state
    add_officer(s['core'],'A4','p3',(55,65,70,60,70))
    s['officer_faction']['A4']='A';s['officer_loyalty']['A4']=72
    ac.add(s,'A4','p3',45);ac.refresh(s)
    c.do('attack','A3',{'target':'p5','troops':120},'q10:migrate:start')
    s=c.state;b=s['battle']
    ac.take(s,'A4','p3',30);ac.ensure(s,'A4')['engaged']+=30
    b['participants'].append({'id':'army:A4','troops':30,'home':'p3'})
    b['a']['troops']+=30
    ba.initialize(s,b);ac.refresh(s)
    legacy=next(u for u in b['officer_units'] if u['id']=='A4')
    legacy['slot']=3;legacy['commander']=True  # Build26 on-disk bug
    c=load_bytes(save_bytes(c))
    assert next(u for u in c.state['battle']['officer_units'] if u['id']=='A4')['slot']==3
    c.do('battle_wait','A3',{'officer_id':'A3'},'q10:migrate:wait')
    corrected=next(u for u in c.state['battle']['officer_units'] if u['id']=='A4')
    assert corrected['slot']==2 and not corrected['commander']
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_invalid_order_preserves_historical_save_and_rolls_back_slot_migration():
    from og.core.fixed import CoreError
    import pytest
    c=new_campaign(2705);s=c.state
    add_officer(s['core'],'A4','p3',(55,65,70,60,70))
    s['officer_faction']['A4']='A';s['officer_loyalty']['A4']=72
    ac.add(s,'A4','p3',45);ac.refresh(s)
    c.do('attack','A3',{'target':'p5','troops':120},'q10:rollback:start')
    s=c.state;b=s['battle']
    ac.take(s,'A4','p3',30);ac.ensure(s,'A4')['engaged']+=30
    b['participants'].append({'id':'army:A4','troops':30,'home':'p3'})
    b['a']['troops']+=30
    ba.initialize(s,b);ac.refresh(s)
    legacy=next(u for u in b['officer_units'] if u['id']=='A4')
    legacy['slot']=3;legacy['commander']=True
    c=load_bytes(save_bytes(c));before=c.fingerprint
    with pytest.raises(CoreError):
        c.do('battle_wait','A3',{'officer_id':'DOES_NOT_EXIST'},'q10:rollback:invalid')
    assert c.fingerprint==before
    assert next(u for u in c.state['battle']['officer_units'] if u['id']=='A4')['slot']==3
