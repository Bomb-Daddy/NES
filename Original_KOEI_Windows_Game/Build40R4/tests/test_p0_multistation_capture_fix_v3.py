"""P0 regression: a captured field officer may command off-field detachments.

These are campaign-ledger invariants, not verified original KOEI combat formulas.
"""
import copy
from third_branch.integrated import new_campaign, save_bytes, load_bytes, validate
from third_branch import army_cohorts as ac


def setup_split_capture(seed=77, *, surviving_peer=True):
    c=new_campaign(seed)
    s=c.state
    if surviving_peer:
        ac.add(s,'B2','p4',10)
    else:
        # Legitimate state with just one B commander, B2, and a remote p4
        # garrison. Move B1 to the A side and keep every soldier counted.
        b1=s['armies'].pop('army:B1')
        n=b1['stations']['p4']
        s['core']['officers']['B1']['province_id']='p1'
        s['officer_faction']['B1']='A'
        ac.ensure(s,'B2')['stations']['p4']=n+10
    ac.refresh(s)
    ac.lose_at(s,'p5',239)
    assert validate(s)
    return c


def total_military(s):
    return sum(v['troops'] for v in s['core']['provinces'].values()) + sum(v['prisoners'] for v in s['extras'].values()) + (s['battle']['a']['troops'] if s['battle'] else 0)


def do_capture(c):
    c.do('attack','A3',{'target':'p5','troops':200},'test:attack')
    # New Q11 source rule: resolve an actual opening CPU duel challenge via
    # the public campaign command before automated subsequent battle turns.
    if c.state['battle'].get('duel_pending'):
        c.do('battle_duel_response','A3',{'accepted':False},'test:opening-cpu-challenge')
    out=c.do('battle_auto','A3',{},'test:resolve')
    # Q14 prisoner decisions must be explicitly made by the human player;
    # the battle must NOT auto-recruit captured generals silently.
    if c.state['battle'] is not None and c.state['battle'].get('pending_outcome'):
        for prisoner in tuple(c.state['battle'].get('prisoner_queue',())):
            out=c.do('battle_prisoner','A3',
                {'officer_id':prisoner,'decision':'recruit'},'test:prisoner:'+prisoner)
    assert out['resolved'] and c.state['battle'] is None
    assert validate(c.state)
    return c.state['history'][-1]


def test_remote_garrison_gets_surviving_defender_commander_instead_of_switching_sides():
    c=setup_split_capture()
    s=c.state;before=total_military(s)
    hist=do_capture(c)
    assert hist['winner']=='ATTACKER'
    assert c.state['owners']['p4']=='B' and c.state['owners']['p5']=='A'
    assert c.state['officer_faction']['B2']=='A'
    assert ac.strength(c.state,'B1','p4')==310
    assert ac.strength(c.state,'B2','p4')==0
    casualties=(hist['start_attacker']-hist['attacker_survivors'])+(hist['start_defender']-hist['defender_survivors'])
    assert 0 <= casualties <= hist['start_attacker']+hist['start_defender']
    assert total_military(c.state)==before-casualties
    assert {'officer':'B2','status':'remote_troops_reassigned','province':'p4','troops':10,'new_commander':'B1'} in hist['defender_officer_dispositions']
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
    result=c.advance_month()
    assert result['next_month']==2 and validate(c.state)


def test_no_defending_replacement_officer_commander_withdraws_to_remote_post():
    c=setup_split_capture(surviving_peer=False)
    s=c.state;before=total_military(s)
    hist=do_capture(c)
    assert s is not c.state
    assert c.state['owners']['p4']=='B' and c.state['owners']['p5']=='A'
    assert c.state['officer_faction']['B2']=='B'
    assert c.state['core']['officers']['B2']['province_id']=='p4'
    assert ac.strength(c.state,'B2','p4')==310
    assert any(x['status']=='escaped_to_distant_garrison' for x in hist['defender_officer_dispositions'])
    casualties=(hist['start_attacker']-hist['attacker_survivors'])+(hist['start_defender']-hist['defender_survivors'])
    assert 0 <= casualties <= hist['start_attacker']+hist['start_defender']
    assert total_military(c.state)==before-casualties
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
    c.advance_month()
    assert validate(c.state)


def test_unchanged_start_deterministic_repeatability_and_no_prebattle_mutation():
    original=setup_split_capture(117)
    before=original.fingerprint
    a=load_bytes(save_bytes(original));b=load_bytes(save_bytes(original))
    assert original.fingerprint==before
    for c in (a,b):
        do_capture(c)
        c.advance_month()
    assert a.fingerprint==b.fingerprint
    assert a.state['history']==b.state['history']


def test_standard_battle_without_remote_detachment_still_completes():
    c=new_campaign(77)
    s=c.state
    ac.lose_at(s,'p5',239)
    assert validate(s)
    h=do_capture(c)
    assert h['winner']=='ATTACKER'
    assert c.state['officer_faction']['B2']=='A'
    c.advance_month()
    assert validate(c.state)
