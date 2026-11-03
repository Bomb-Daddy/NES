"""Q07 source-matrix regression: passive forest reaction on Charge advance."""
from unittest.mock import patch
from third_branch.integrated import new_campaign, validate, load_bytes, save_bytes
from third_branch import officer_battle_adapter as ba
from third_branch import army_cohorts as ac


def make_battle():
    c = new_campaign(104)
    ac.add(c.state, 'B1', 'p5', 50)
    ac.refresh(c.state)
    c.do('attack', 'A3', {'target':'p5','troops':150}, 'q07:start')
    b=c.state['battle']
    a=ba.find(b,'A','A3'); d=ba.find(b,'D','B2'); forest=ba.find(b,'D','B1')
    a['q'],a['r'],a['mp'],a['intel']=9,6,6,45
    d['q'],d['r']=10,6
    forest['q'],forest['r']=11,6
    row=list(b['terrain'][6]);row[11]='F';b['terrain'][6]=''.join(row)
    return c


def fake_exchange(s,b,u,v):
    v['troops100']=0;v['troops']=0
    return {'officer':u['id'],'target':v['id'],'actor_loss100':0,'target_loss100':1}


def test_charge_advance_triggers_passive_forest_ambush_and_cold_load():
    c=make_battle()
    before=ba.find(c.state['battle'],'A','A3')['troops100']
    with patch.object(ba,'_exchange',side_effect=fake_exchange):
        c.do('battle_attack','A3',{'officer_id':'A3','target_officer_id':'B2','mode':'charge'},'q07:charge')
    b=c.state['battle']
    assert b is not None
    assert (ba.find(b,'A','A3')['q'],ba.find(b,'A','A3')['r'])==(10,6)
    assert ba.find(b,'A','A3')['troops100']<before
    assert any(e['kind']=='forest_ambush' for e in b.get('combat_events',[]))
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
    assert validate(c.state)


def test_charge_advance_int90_immune_negative_control():
    c=make_battle()
    ba.find(c.state['battle'],'A','A3')['intel']=90
    before=ba.find(c.state['battle'],'A','A3')['troops100']
    with patch.object(ba,'_exchange',side_effect=fake_exchange):
        c.do('battle_attack','A3',{'officer_id':'A3','target_officer_id':'B2','mode':'charge'},'q07:immune')
    assert ba.find(c.state['battle'],'A','A3')['troops100']==before
    assert not any(e['kind']=='forest_ambush' for e in c.state['battle'].get('combat_events',[]))
