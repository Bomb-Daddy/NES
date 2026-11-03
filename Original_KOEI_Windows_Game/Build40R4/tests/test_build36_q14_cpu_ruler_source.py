"""Q14 SNES source-byte CPU ruler fate with persistent campaign RNG/ledger."""
import pytest
from og.core.fixed import CoreError
from third_branch import officer_battle_adapter as adapter
from third_branch.integrated import new_campaign,save_bytes,load_bytes,validate


def source_state():
    c=new_campaign(3601);s=c.state
    s['factions']['B']['ruler_officer']='B2'
    # Synthetic fixture makes p4 adjacent to p5; not a claim about ROM topology.
    s['neighbors']['p5'].append('p4')
    s['neighbors']['p4'].append('p5')
    s['factions']['A']['q14_source_relation']={'B':80}
    s['factions']['A']['q14_source_guard']=0
    s['factions']['B']['q14_source_stat6']=100
    assert validate(s)
    return c


def test_cpu_ruler_source_roll_releases_and_decrements_relation(monkeypatch):
    c=source_state();s=c.state;b={'defender':'B','attacker':'A','dst':'p5'}
    calls=[]
    monkeypatch.setattr(adapter,'randrange',lambda core,n:(calls.append(n) or 0))
    assert adapter.cpu_ruler_disposition(s,b,'A','B')=='release'
    assert s['factions']['A']['q14_source_relation']=={'B':30}
    assert calls==[100]


def test_cpu_ruler_source_roll_failure_beheads_without_relation_change(monkeypatch):
    c=source_state();s=c.state;b={'defender':'B','attacker':'A','dst':'p5'}
    calls=[]
    monkeypatch.setattr(adapter,'randrange',lambda core,n:(calls.append(n) or 99))
    assert adapter.cpu_ruler_disposition(s,b,'A','B')=='behead'
    assert s['factions']['A']['q14_source_relation']=={'B':80}
    assert calls==[100]


def test_guard_and_no_refuge_do_not_consume_rng(monkeypatch):
    c=source_state();s=c.state;b={'defender':'B','attacker':'A','dst':'p5'}
    calls=[];monkeypatch.setattr(adapter,'randrange',lambda core,n:calls.append(n))
    s['factions']['A']['q14_source_guard']=200
    assert adapter.cpu_ruler_disposition(s,b,'A','B')=='behead'
    s['factions']['A']['q14_source_guard']=0
    s['neighbors']['p5'].remove('p4')
    assert adapter.cpu_ruler_disposition(s,b,'A','B')=='behead'
    assert not calls


def test_invalid_source_field_rolls_back_actual_campaign_transaction():
    c=source_state()
    c.do('attack','A3',{'target':'p5','troops':120,'gold':1},'b36:begin')
    c.state['human_faction']='C'
    c.state['factions']['A']['q14_source_relation']['B']=256
    ruler=next(u for u in c.state['battle']['officer_units'] if u['id']=='B2')
    ruler['troops100']=0;ruler['troops']=0
    adapter._sync(c.state,c.state['battle'])
    before=c.fingerprint
    with pytest.raises(CoreError):c.do('battle_end_turn','A3',{},'b36:invalid')
    assert c.fingerprint==before
    assert load_bytes(save_bytes(c)).fingerprint==before


def test_real_cpu_campaign_ruler_fate_persists_and_cold_load(monkeypatch):
    c=source_state()
    c.do('attack','A3',{'target':'p5','troops':120,'gold':1},'b36:begin')
    c.state['human_faction']='C'
    # The ruler's field army was defeated while other defenders could retreat.
    # This must still produce a Q14 capture-menu entry, not a free teleport.
    ruler=next(u for u in c.state['battle']['officer_units'] if u['id']=='B2')
    ruler['troops100']=0;ruler['troops']=0
    adapter._sync(c.state,c.state['battle'])
    monkeypatch.setattr(adapter,'randrange',lambda core,n:0)
    out=c.do('battle_end_turn','A3',{},'b36:resolve')
    assert out['resolved']
    assert c.state['history'][-1]['prisoner_decisions']=={'B2':'release'}
    assert c.state['factions']['A']['q14_source_relation']=={'B':30}
    assert c.state['core']['officers']['B2']['province_id']=='p4'
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_captured_ruler_and_surviving_defender_retreat_are_distinct(monkeypatch):
    from og.core.world import add_officer
    from third_branch import army_cohorts as ac
    c=source_state();s=c.state
    add_officer(s['core'],'B3','p5',(42,54,61,64,50))
    s['officer_faction']['B3']='B';s['officer_loyalty']['B3']=45
    ac.add(s,'B3','p5',30);ac.refresh(s)
    assert validate(s)
    c.do('attack','A3',{'target':'p5','troops':120,'gold':1},'b36:multi:begin')
    c.state['human_faction']='C'
    b=c.state['battle']
    ruler=next(u for u in b['officer_units'] if u['id']=='B2')
    ruler['troops100']=0;ruler['troops']=0
    adapter._sync(c.state,b)
    b['rice_d']=0
    monkeypatch.setattr(adapter,'randrange',lambda core,n:0)
    out=c.do('battle_end_turn','A3',{},'b36:multi:resolve')
    assert out['resolved']
    assert c.state['history'][-1]['prisoner_decisions']=={'B2':'release'}
    dispositions=c.state['history'][-1]['defender_officer_dispositions']
    assert {'officer':'B2','status':'released','province':'p4'} in dispositions
    assert {'officer':'B3','status':'retreated','province':'p4'} in dispositions
    assert c.state['core']['officers']['B2']['province_id']=='p4'
    assert c.state['core']['officers']['B3']['province_id']=='p4'
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_uninjured_ruler_with_retreat_is_not_captured(monkeypatch):
    c=source_state()
    c.do('attack','A3',{'target':'p5','troops':120,'gold':1},'b36:healthy:begin')
    c.state['human_faction']='C'
    c.state['battle']['rice_d']=0
    calls=[]
    monkeypatch.setattr(adapter,'randrange',lambda core,n:(calls.append(n) or 0))
    out=c.do('battle_end_turn','A3',{},'b36:healthy:resolve')
    assert out['resolved']
    assert c.state['history'][-1]['prisoner_decisions']=={}
    assert c.state['core']['officers']['B2']['province_id']=='p4'
    assert c.state['factions']['A']['q14_source_relation']=={'B':80}
    assert not calls
    assert validate(c.state)


def test_missing_source_bytes_are_explicitly_provisional_no_rng(monkeypatch):
    c=source_state();s=c.state;b={'defender':'B','attacker':'A','dst':'p5'}
    del s['factions']['A']['q14_source_relation']
    calls=[]
    monkeypatch.setattr(adapter,'randrange',lambda core,n:calls.append(n))
    assert adapter.cpu_ruler_disposition(s,b,'A','B')=='release'
    assert not calls


@pytest.mark.parametrize('r,stat6,roll,expected',[
    (0,100,99,'release'),(100,0,0,'behead'),(100,0,1,'behead'),
    (99,1,0,'release'),(99,1,1,'behead'),(80,100,59,'release'),
    (80,100,60,'behead'),(200,0,0,'behead'),(0,0,49,'release'),
    (0,0,50,'behead')])
def test_original_cpu_release_threshold_edges(monkeypatch,r,stat6,roll,expected):
    c=source_state();s=c.state;b={'defender':'B','attacker':'A','dst':'p5'}
    s['factions']['A']['q14_source_relation']['B']=r
    s['factions']['B']['q14_source_stat6']=stat6
    monkeypatch.setattr(adapter,'randrange',lambda core,n:roll)
    assert adapter.cpu_ruler_disposition(s,b,'A','B')==expected
