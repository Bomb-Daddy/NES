"""Hostile QA: full historical catalog, live source positions, family, cold saves, negative control."""
import copy,pytest
from og.core.fixed import CoreError
from og.core.world2 import calendar_of
from third_branch.historical_s1 import (new_campaign_historical_s1,list_recruitable,oid,source)
from third_branch.integrated import save_bytes,load_bytes,validate
from third_branch.historical_ai import STYLES,take_turn

def test_original_game_exact_roster_and_rulers():
    c=new_campaign_historical_s1(71);s=c.state;d=source()
    assert len(s['officer_catalog'])==352
    assert len(s['factions'])==15
    assert len(s['core']['officers'])==107
    assert sum(owner is not None for owner in s['owners'].values())==19
    assert set(s['factions'])=={'R'+str(i) for i in range(15)}
    assert s['factions']['R0']['name']=='Cao Cao'
    assert s['factions']['R1']['name']=='Liu Bei'
    assert s['owners']['p9']=='R0' and s['owners']['p4']=='R1'
    assert s['owners']['p3']=='R9' and s['owners']['p41'] is None
    for pid,cat in s['officer_catalog'].items():
        assert cat['nes_id']==int(pid[4:])
        assert cat['politics'] in range(101) and cat['leadership'] in range(101)
        if cat['scenario1_status']=='Active at start':
            assert s['core']['officers'][pid]['province_id']=='p'+str(cat['scenario1_start_province'])
            assert s['officer_faction'][pid]==s['owners']['p'+str(cat['scenario1_start_province'])]
        else: assert pid not in s['core']['officers']

def test_family_clans_and_genealogical_links_are_separate():
    c=new_campaign_historical_s1();s=c.state
    assert s['officer_catalog'][oid(0)]['rom_family_mask_candidate']==1
    assert s['officer_catalog'][oid(281)]['rom_family_mask_candidate']==1
    assert s['officer_catalog'][oid(1)]['rom_family_mask_candidate']==2
    assert s['officer_catalog'][oid(220)]['rom_family_mask_candidate']==4
    assert s['officer_catalog'][oid(10)]['rom_family_mask_candidate']==0
    assert any(e['from_id']==oid(0) and e['to_id']==oid(281) and e['type']=='father_child' for e in s['family_relations'])
    assert all(e['evidence'].endswith('NOT_VERIFIED_FROM_ROM_MASK') for e in s['family_relations'])

def test_hidden_officer_recruit_atomic_and_immutable_on_replay():
    c=new_campaign_historical_s1()
    candidates=next((pid,list_recruitable(c.state,pid)) for pid,owner in c.state['owners'].items() if owner is not None and list_recruitable(c.state,pid))
    pid,ids=candidates
    f=c.state['owners'][pid]
    actor=next(k for k,v in c.state['officer_faction'].items() if v==f and c.state['core']['officers'][k]['province_id']==pid and c.state['core']['officers'][k]['bp']>=8)
    target=ids[0];before=c.fingerprint
    q=c.preview('recruit_officer',actor,{'officer_id':target},'test:recruit')
    assert c.fingerprint==before
    c.execute('recruit_officer',actor,{'officer_id':target},'test:recruit',q['quote_id'],q['revision'])
    assert target in c.state['core']['officers']
    assert c.state['officer_faction'][target]==f
    assert c.state['officer_availability'][target]=='Serving'
    post=c.fingerprint
    with pytest.raises(CoreError):c.do('recruit_officer',actor,{'officer_id':target},'test:recruit')
    assert c.fingerprint==post
    loaded=load_bytes(save_bytes(c))
    assert loaded.fingerprint==post and loaded.state['family_relations']==c.state['family_relations']

def test_scheduled_arrivals_appear_as_recruitable_not_forced_joins():
    c=new_campaign_historical_s1()
    start=len(c.state['core']['officers'])
    # Bao Xin ID 341 arrives 190 province 9. No automatic allegiance.
    assert c.state['officer_availability'][oid(341)]=='Arrives later'
    for _ in range(12):c.advance_month()
    assert calendar_of(c.state['core'],c.state['month'])==(190,1)
    assert c.state['officer_availability'][oid(341)]=='Arrived/searchable'
    assert oid(341) not in c.state['core']['officers']
    assert len(c.state['core']['officers'])==start
    assert oid(341) in list_recruitable(c.state,'p9')

def test_family_tamper_rejected_and_original_unmodified():
    c=new_campaign_historical_s1();s=copy.deepcopy(c.state)
    s['family_relations'][0]['to_id']='nes:999'
    with pytest.raises(CoreError):validate(s)
    assert c.state['family_relations'][0]['to_id']!='nes:999'

def test_historical_ai_12_real_ruler_styles():
    c=new_campaign_historical_s1()
    before=c.fingerprint
    kinds=[]
    for i,style in enumerate(STYLES):
        result=take_turn(c,'R'+str(i),style,prefix='qa',month=1)
        kinds.append(result['kind'])
    assert len(kinds)==12
    assert c.fingerprint!=before
    assert any(k not in ('idle','inactive') for k in kinds)
    assert len(c.state['factions'])==15
