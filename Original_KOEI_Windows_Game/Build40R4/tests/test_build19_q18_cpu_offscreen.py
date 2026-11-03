"""Q18 NPC-only six-round integrated transaction and deterministic cold replay."""
import pytest
from third_branch.integrated import new_campaign, save_bytes, load_bytes, validate
from third_branch import army_cohorts as ac


def _npc(seed):
    c=new_campaign(seed)
    c.state['human_faction']='C'  # A attacks B; neither is controlled by the player.
    assert validate(c.state)
    return c


def test_npc_attack_uses_six_round_offscreen_and_commits_once():
    c=_npc(1919)
    original=sum(ac.army_total(a) for a in c.state['armies'].values())
    out=c.do('attack','A3',{'target':'p5','troops':120,'gold':1},'q18:npc')
    assert out['offscreen'] is True
    assert 1<=out['rounds']<=6
    assert c.state['battle'] is None
    assert len(c.state['history'])==1
    assert c.state['history'][0]['combat_rule']=='Q18_NPC_SIX_ROUND_V1_4_PARTIAL'
    assert sum(ac.army_total(a) for a in c.state['armies'].values())<=original
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_npc_battle_replay_and_rollback():
    c=_npc(1920)
    a=load_bytes(save_bytes(c))
    params={'target':'p5','troops':120,'gold':1}
    r1=c.do('attack','A3',params,'q18:replay')
    r2=a.do('attack','A3',params,'q18:replay')
    assert r1==r2 and c.fingerprint==a.fingerprint
    h=c.fingerprint
    with pytest.raises(Exception):
        c.do('attack','A3',{'target':'p3','troops':120,'gold':1},'q18:invalid-friendly-target')
    assert c.fingerprint==h


def test_human_battle_not_auto_resolved():
    c=new_campaign(1921);c.state['human_faction']='A'
    result=c.do('attack','A3',{'target':'p5','troops':120,'gold':1},'q18:human')
    assert result['battle_started'] and c.state['battle'] is not None
    assert not c.state['history']


def test_npc_invasion_uses_original_q19_provision_window_not_player_minimum():
    c=_npc(1922)
    before=c.state['core']['provinces']['p3']['food_units']
    c.do('attack','A3',{'target':'p5','troops':120,'gold':1},'q18:npc:supply')
    spent=before-c.state['core']['provinces']['p3']['food_units']
    assert 150<=spent<=209  # Q19: sent*5/4 + rand(sent/2)


def test_exact_six_round_limit_with_ample_rice_and_no_unearned_conquest():
    from third_branch import offscreen_q18
    c=new_campaign(1988);c.state['human_faction']='A'
    c.do('attack','A3',{'target':'p5','troops':120,'gold':1},'q18:setup')
    s=c.state;s['human_faction']='C'
    s['battle']['rice_a']=s['battle']['rice_d']=1000000
    report=offscreen_q18.resolve(s,s['battle'])
    assert report['rounds']==6 and report['winner']=='DEFENDER'
    assert report['reason']=='SIX_ROUND_STRENGTH_GATE'
    assert len(s['battle']['q18_rounds'])==6
    assert all(row['troops100_a']>=0 and row['troops100_d']>=0 for row in s['battle']['q18_rounds'])
    assert validate(s)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_cpu_capture_decision_does_not_block_transaction():
    c=_npc(1999);s=c.state
    # Source-backed negative case: the defender has no remaining home and
    # cannot be left waiting on a player-only prisoner modal.
    s['owners']['p4']='C';s['officer_faction']['B1']='C'
    n=ac.strength(s,'B2','p5')
    ac.take(s,'B2','p5',n-1);ac.refresh(s)
    assert validate(s)
    out=c.do('attack','A3',{'target':'p5','troops':120,'gold':1},'q18:cpu:prisoner')
    assert out['offscreen'] and out['resolved']
    assert c.state['battle'] is None and c.state['owners']['p5']=='A'
    assert c.state['history'][-1]['prisoner_decisions']=={'B2':'recruit'}
    assert c.state['officer_faction']['B2']=='A'
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
