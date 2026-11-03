"""Q18: source-constrained six-round NPC-versus-NPC battlefield resolver.

This is the separate strategic CPU battle path, not 30 tactical days.  The
Battle Answer Matrix v1.4 (excsub6 C97F / score_province_9CAF / C30C) supports
six rounds, score weighting, squared opposing strength, 85..115 random damage,
3..5 divisor, and rice burn (troops/6 + 5).  Exact assembler intermediate
rounding, retreat/capture rolls and result bytes remain parity-open; do not
label this a byte-identical NES/SNES emulator.
"""
from og.core.rng import randrange
from . import officer_battle_adapter as officer

RULE='Q18_NPC_SIX_ROUND_V1_4_PARTIAL'


def _strength(s,b,side):
    total=0
    for u in officer._all(b,side):
        if u.get('reserve') or u.get('fled'):continue
        n=u['troops100']
        o=s['core']['officers'][u['id']]
        term=u.get('troop_ratio',100)//4+s['officer_loyalty'][u['id']]//3+o['intelligence']//2+o['war']
        total+=max(1,term*(n//100+1)//50)
    return total


def _remaining(b,side):
    return sum(u['troops100'] for u in officer._all(b,side))


def resolve(s,b):
    """Mutate only a staged campaign state; caller settles in same transaction."""
    if b.get('pending_outcome') or b.get('result'):
        raise ValueError('Q18_BATTLE_ALREADY_RESOLVED')
    if b.get('combat_rule')!='ROTK2_UNIFIED_V1_4_OFFICER_LEDGER':
        raise ValueError('Q18_OFFICER_LEDGER_REQUIRED')
    if s.get('human_faction') in (None,b['attacker'],b['defender']):
        raise ValueError('Q18_REQUIRES_TWO_CPU_FACTIONS')
    if any(u.get('reserve') for u in b['officer_units']):
        raise ValueError('Q18_PENDING_REINFORCEMENTS_UNSUPPORTED')
    rand=lambda n:randrange(s['core'],n)
    b['combat_rule']=RULE
    rounds=[];winner=None;reason=None
    for day in range(1,7):
        sa=_strength(s,b,'A')*(rand(3)+4)
        sd=_strength(s,b,'D')*(rand(2)+3)
        if sa==0 or sd==0:
            winner='DEFENDER' if sa==0 else 'ATTACKER';reason='NO_ACTIVE_OFFICERS';break
        p=max(1,min(99,sa*100//(sa+sd)));q=100-p
        denominator=p*p+q*q
        for side,opponent_share in (('A',q),('D',p)):
            for u in list(officer._all(b,side)):
                n=u['troops100'];pct=85+rand(31);divisor=3+rand(3)
                # All arithmetic is integer and bounded by the officer's actual troops.
                loss=min(n,(n//100+1)*pct*opponent_share**2//(denominator*divisor))
                u['troops100']=max(0,n-loss)
                u['troops']=u['troops100']//100
        troop_a=_remaining(b,'A');troop_d=_remaining(b,'D')
        b['rice_a']=max(0,b['rice_a']-(troop_a//6+500))
        b['rice_d']=max(0,b['rice_d']-(troop_d//6+500))
        b['day']=day
        officer._sync(s,b)
        rounds.append({'round':day,'score_a':sa,'score_d':sd,'share_a':p,
                       'troops100_a':troop_a,'troops100_d':troop_d,
                       'rice100_a':b['rice_a'],'rice100_d':b['rice_d']})
        if not troop_a or not b['rice_a']:
            winner='DEFENDER';reason='ARMY_OR_RICE_EXHAUSTED';break
        if not troop_d or not b['rice_d']:
            winner='ATTACKER';reason='ARMY_OR_RICE_EXHAUSTED';break
        # Q18 branch_C56C: early retreat is not a generic 6:1 conquest.
        # It requires a real neighboring defender-owned destination; if
        # none exists, continue the original six-round calculation.
        attacking=_strength(s,b,'A');defending=_strength(s,b,'D')
        destinations=sorted(pid for pid in s['neighbors'].get(b['dst'],())
                            if s['owners'].get(pid)==b['defender'])
        if defending>0 and attacking>=6*defending and destinations:
            winner='ATTACKER';reason='SIX_TO_ONE_RETREAT'
            b['q18_retreat_destination']=destinations[0]
            break
    if winner is None:
        # Source Q18 six-round hold: no automatic conquest on a modest power
        # lead. Original's 7:1 threshold / retreat destination remains under
        # parity investigation; the invader fails when not overwhelming.
        a=_strength(s,b,'A');d=_strength(s,b,'D')
        winner='ATTACKER' if d==0 or a>=7*d else 'DEFENDER'
        reason='SIX_ROUND_STRENGTH_GATE'
    b['q18_rounds']=rounds
    b['q18_reason']=reason
    b['q18_result']=winner
    return {'winner':winner,'rounds':len(rounds),'reason':reason,'rule':RULE}
