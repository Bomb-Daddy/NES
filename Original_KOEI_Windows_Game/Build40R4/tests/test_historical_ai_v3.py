"""v3 regression, deterministic profiles, legal previews, save/reload and negative probes."""
import copy, json
import pytest
from og.core.fixed import CoreError
from third_branch.historical_s1 import new_campaign_historical_s1
from third_branch.historical_ai_v3 import PROFILES, STYLE_TO_ID, CONF, rank_choices
from third_branch.historical_ai import STYLES,take_turn
from third_branch.integrated import load_bytes,save_bytes


def test_v3_han_names_and_nonnaval_schemas():
    assert len(PROFILES)==12
    assert len(set(x['name'] for x in PROFILES.values()))==12
    assert len(STYLE_TO_ID)==12 and set(STYLE_TO_ID)==set(STYLES)
    assert all('naval' not in p['priority_weights'] for p in PROFILES.values())
    assert all(sum(p['priority_weights'].values())==100 for p in PROFILES.values())
    assert all(p['policy_v3']['no_privileged_actions'] for p in PROFILES.values())
    assert PROFILES['P03']['name']=='Sun Ce'
    assert PROFILES['P09']['name']=='Lu Su'
    assert PROFILES['P12']['name']=='Yang Xiu'


def test_v3_all_twelve_separate_real_command_turns():
    c=new_campaign_historical_s1(17)
    old=c.fingerprint;out=[]
    for i,style in enumerate(STYLES):
        f='R'+str(i)
        r=take_turn(c,f,style,prefix='v3-qa')
        assert r['ai_name']==PROFILES[f'P{i+1:02}']['name']
        assert r['kind'] in ('inactive','delegated_idle','idle') or r.get('actor') in c.state['core']['officers']
        out.append(r)
    assert len(out)==12 and c.fingerprint!=old
    assert not c.state['battle']
    assert all('naval' not in x['kind'] and 'ship' not in x['kind'] for x in out)
    assert c.state['ai_profiles']['R2']['name']=='Sun Ce'


def test_v3_replay_identical_save_roundtrip_three_months():
    a=new_campaign_historical_s1(23)
    b=load_bytes(save_bytes(a))
    for _ in range(3):
        for c in (a,b):
            for i,style in enumerate(STYLES):
                take_turn(c,'R'+str(i),style,prefix='v3-replay')
            c.advance_month()
        b=load_bytes(save_bytes(b))
        assert a.fingerprint==b.fingerprint


def test_v3_nonmutating_rank_and_boundary_probes():
    c=new_campaign_historical_s1(17)
    orig=c.fingerprint
    choices=rank_choices(c,'R11','rules_lawyer')
    assert isinstance(choices,list)
    assert c.fingerprint==orig
    r=take_turn(c,'R11','rules_lawyer',prefix='v3-bounds')
    assert len(r['probe_results'])==3 if choices else not r['probe_results']
    assert all('result' in x for x in r['probe_results'])
    assert c.state['revision']==len(c.state['log'])
    assert len(c.state['processed'])==len(c.state['log'])


def test_v3_legal_negative_cases_preserve_state():
    c=new_campaign_historical_s1(17)
    actor=next(x for x,f in c.state['officer_faction'].items() if f=='R0')
    pid=c.state['core']['officers'][actor]['province_id']
    old=c.fingerprint
    with pytest.raises(CoreError):c.preview('market',actor,{'commodity':'furs','side':'sell','units':0},'ai3:invalid')
    assert c.fingerprint==old
    r=take_turn(c,'R0','curious',prefix='v3-safety')
    if r.get('actor'):
        current=c.fingerprint
        with pytest.raises(CoreError):c.do(r['kind'],r['actor'],r['params'],c.state['log'][-1]['id'])
        assert c.fingerprint==current


def test_v31_nonmilitary_profiles_do_not_open_with_opportunistic_attack():
    c=new_campaign_historical_s1(17)
    for i in (3,4,5,6,7,8,9,10,11):
        choices=rank_choices(c,'R'+str(i),STYLES[i])
        assert not any(z.kind=='attack' for _,z in choices), STYLES[i]


def test_v32_no_governor_churn_or_plan_spam_after_commit():
    c=new_campaign_historical_s1(17)
    for _ in range(4):
        for i,style in enumerate(STYLES):
            take_turn(c,'R'+str(i),style,prefix='qa-v32')
        c.advance_month()
    r2=[x for x in c.state['log'] if ':R2:' in x['id'] and x['kind']=='plan_attack']
    r10=[x for x in c.state['log'] if ':R10:' in x['id'] and x['kind']=='appoint_governor']
    assert r2==[]
    assert len(r10)<=2

def test_v32_lu_su_legal_supply_redistribution():
    c=new_campaign_historical_s1(17)
    s=c.state
    assert s['owners']['p10']==s['owners']['p11']=='R8'
    ranked=rank_choices(c,'R8','logistician')
    assert any(x.kind=='transport' for _,x in ranked)
    before=c.fingerprint
    result=take_turn(c,'R8','logistician',prefix='v32-logi')
    assert result['kind']=='transport'
    assert c.fingerprint!=before
