"""Run016 source-branch consolidation against actual Enhanced Campaign.
Must FAIL on original Run015A before the merger; no guessed original NES rules.
"""
import copy, json
import pytest
from og.core.fixed import CoreError
from third_branch.integrated import new_campaign, validate, save_bytes, load_bytes, canonical_bytes, hash_state
from third_branch.historical_s1 import list_recruitable
from third_branch import army_cohorts as ac
from test_run015_q14_no_refuge_free_officer import no_refuge


def test_q14_prior_faction_provenance_and_source_uncertainty_recorded():
    c=no_refuge(16101)
    c.do('battle_prisoner','A3',{'officer_id':'B2','decision':'release'},'r016:release')
    s=c.state
    p=s['q14_free_officer_provenance']['B2']
    assert p['province_id']=='p5' and p['prior_faction']=='B'
    assert p['source_rule']=='Q14_NO_REFUGE_FREE'
    assert p['original_special_flag']=='UNKNOWN_NOT_INFERRED'
    assert s['q14_free_officers_by_province']['p5']==['B2']
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_historical_availability_updated_and_provenance_retires_on_recruit():
    c=no_refuge(16102)
    c.state['officer_availability']={'B2':'Serving'}
    c.do('battle_prisoner','A3',{'officer_id':'B2','decision':'release'},'r016:release')
    assert c.state['officer_availability']['B2']=='Free/recruitable'
    assert list_recruitable(c.state,'p5')==['B2']
    c.state['core']['officers']['A3']['province_id']='p5'
    c.do('recruit_officer','A3',{'officer_id':'B2'},'r016:recruit')
    assert c.state['officer_availability']['B2']=='Serving'
    assert 'B2' not in c.state['q14_free_officer_provenance']
    assert 'B2' not in c.state['q14_free_officers_by_province'].get('p5',[])
    assert validate(c.state)


def test_two_free_officers_menu_preserves_original_province_head_order():
    from og.core.world import add_officer
    c=new_campaign(16103);s=c.state
    s['owners']['p4']='A';s['officer_faction']['B1']='A'
    add_officer(s['core'],'B3','p5',(50,60,70,80,90))
    s['officer_faction']['B3']='B';s['officer_loyalty']['B3']=50
    assert validate(s)
    c.do('attack','A3',{'target':'p5','troops':120,'gold':1},'r016:attack')
    c.state['battle']['rice_d']=0
    c.do('battle_end_turn','A3',{},'r016:win')
    c.do('battle_prisoner','A3',{'officer_id':'B2','decision':'release'},'r016:release-b2')
    c.do('battle_prisoner','A3',{'officer_id':'B3','decision':'release'},'r016:release-b3')
    assert c.state['q14_free_officers_by_province']['p5']==['B3','B2']
    assert list_recruitable(c.state,'p5')==['B3','B2']
    assert set(c.state['q14_free_officer_provenance'])=={'B2','B3'}
    assert c.state['q14_free_officer_provenance']['B3']['prior_faction']=='B'
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_bad_provenance_rejected_and_no_army_created():
    c=no_refuge(16104)
    c.do('battle_prisoner','A3',{'officer_id':'B2','decision':'release'},'r016:release')
    before=save_bytes(c)
    wrong=copy.deepcopy(c.state)
    wrong['q14_free_officer_provenance']['B2']['province_id']='p4'
    with pytest.raises(CoreError,match='BAD_Q14_FREE_PROVENANCE'):validate(wrong)
    wrong=copy.deepcopy(c.state)
    wrong['q14_free_officer_provenance']['not-an-officer']={'province_id':'p5','prior_faction':'B','source_rule':'Q14_NO_REFUGE_FREE','original_special_flag':'UNKNOWN_NOT_INFERRED'}
    with pytest.raises(CoreError,match='BAD_Q14_FREE_PROVENANCE'):validate(wrong)
    assert save_bytes(c)==before
    assert ac.totals(c.state)=={pid:p['troops'] for pid,p in c.state['core']['provinces'].items()}


def test_old_run015a_saves_remain_loadable_and_run015b_schema_explicitly_blocked():
    c=no_refuge(16105)
    c.do('battle_prisoner','A3',{'officer_id':'B2','decision':'release'},'r016:release')
    older=copy.deepcopy(c.state)
    older.pop('q14_free_officer_provenance',None)
    raw=canonical_bytes({'format':'OG-INTEGRATED-TEST-1','sha256':hash_state(older),'state':older})
    assert load_bytes(raw).fingerprint==hash_state(older)
    # The B sibling's indexed record has no province linked-list ordering;
    # silently reinterpreting that as original NES ordering would be a lie.
    wrong=copy.deepcopy(older)
    wrong['free_officers']={'B2':{'province_id':'p5','prior_faction':'B','source_rule':'Q14_NO_REFUGE_FREE'}}
    raw=canonical_bytes({'format':'OG-INTEGRATED-TEST-1','sha256':hash_state(wrong),'state':wrong})
    with pytest.raises(CoreError,match='RUN015B_SCHEMA_UNMIGRATED'):load_bytes(raw)


def test_source_special_flag_positively_true_fails_closed_byte_identically():
    c=no_refuge(16106);c.state['q14_source_special_free_flag']={'B2':True}
    before=save_bytes(c)
    with pytest.raises(CoreError,match='Q14_SPECIAL_FREE_RETURN_UNMAPPED'):
        c.do('battle_prisoner','A3',{'officer_id':'B2','decision':'release'},'r016:special')
    assert save_bytes(c)==before
