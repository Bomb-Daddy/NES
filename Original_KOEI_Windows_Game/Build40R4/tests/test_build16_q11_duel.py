"""Q11 deterministic duel state/ledger tests; no claim of NES frame parity."""
import pytest
from third_branch.integrated import new_campaign,save_bytes,load_bytes,validate
from third_branch import officer_battle_adapter as ba,army_cohorts as ac
from og.core.fixed import CoreError


def battle():
    c=new_campaign(331)
    ac.add(c.state,'B1','p5',50);ac.refresh(c.state)
    c.do('attack','A3',{'target':'p5','troops':150},'q11:start')
    return c


def test_refusal_deserts_each_defender_and_is_once_only():
    c=battle();b=c.state['battle'];old={u['id']:u['troops100'] for u in b['officer_units'] if u['side']=='D'}
    out=c.do('battle_duel','A3',{'officer_id':'A3','accepted':False},'q11:refuse')
    assert out['duel_result']=='REFUSED'
    assert c.state['battle']['duel_done']
    for u in c.state['battle']['officer_units']:
        if u['side']=='D':assert u['troops100']<old[u['id']]
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
    before=c.fingerprint
    with pytest.raises(CoreError):c.do('battle_duel','A3',{'officer_id':'A3','accepted':False},'q11:twice')
    assert c.fingerprint==before


def test_accepted_duel_replays_identically_from_cold_save():
    c=battle()
    # Obsolete player-selected CPU opponent corrected to source Q11 scan.
    ba.find(c.state['battle'],'A','A3')['war']=40
    expected=ba.choose_ai_duel_acceptor(c.state['battle'],ba.find(c.state['battle'],'A','A3'))
    assert expected is not None
    original=save_bytes(c)
    args={'officer_id':'A3','target_officer_id':expected['id'],'accepted':True}
    out=c.do('battle_duel','A3',args,'q11:accept')
    replay=load_bytes(original)
    other=replay.do('battle_duel','A3',args,'q11:accept')
    assert out==other and c.fingerprint==replay.fingerprint
    assert out['duel_result'] in ('ATTACKER','DEFENDER','DRAW')
    assert validate(c.state)


def test_invalid_duel_target_rolls_back():
    c=battle();before=c.fingerprint
    with pytest.raises(CoreError):
        c.do('battle_duel','A3',{'officer_id':'A3','target_officer_id':'A3','accepted':True},'q11:bad')
    assert c.fingerprint==before


def test_duel_killing_last_attacker_settles_without_orphan_engagement():
    from unittest.mock import patch
    c=battle()
    # A War-90 B2 is the original non-head-first CPU acceptor. Preserve the
    # independently tested death/ledger outcome without an illegal forced pick.
    ba.find(c.state['battle'],'D','B2')['war']=90
    expected=ba.choose_ai_duel_acceptor(c.state['battle'],ba.find(c.state['battle'],'A','A3'))
    assert expected is not None and expected['id']=='B2'
    with patch.object(ba,'randrange',side_effect=lambda state,n:max(0,n-1)):
        out=c.do('battle_duel','A3',{'officer_id':'A3','target_officer_id':expected['id'],'accepted':True},'q11:dead')
    assert out['duel_result']=='DEFENDER'
    assert out['resolved'] and c.state['battle'] is None
    assert not any(a['engaged'] for a in c.state['armies'].values())
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
