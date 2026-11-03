"""Fork Four: deterministic, read-only skill-sensitive Court Advisor scoring.

The engine's province data is NOT changed or misread. Low INT misinterprets
accurate trends and risks; high INT interprets them more appropriately.
This is an EXPERIMENTAL, explicitly switchable scoring layer for batch QA.
No direct orders, resources, BP changes, hidden enemy province access or RNG.
"""
from __future__ import annotations
from dataclasses import dataclass, asdict
import hashlib
import json
import math

DEFAULT_WEIGHT_BPS = 2000


def _clamp(v, lo=0.0, hi=100.0):
    return max(lo, min(hi, float(v)))


def _hash(*values):
    return int(hashlib.sha256(json.dumps(values, sort_keys=True, separators=(',', ':')).encode()).hexdigest()[:16], 16)


def select_advisor(state: dict, faction: str) -> dict | None:
    """Qualified serving officer with greatest Intelligence, stable ties.

    Avoid appointing the ruler to counsel himself if another officer exists.
    Source has 107 initial active officers, 352 roster records (most inactive).
    """
    if not state['factions'].get(faction, {}).get('alive'):
        return None
    active = []
    for oid, employer in sorted(state['officer_faction'].items()):
        if employer != faction or oid not in state['core']['officers']:
            continue
        officer = state['core']['officers'][oid]
        if state['owners'].get(officer['province_id']) != faction:
            continue
        active.append((oid, officer))
    if not active:
        return None
    own_ruler = state.get('ruler_slots', {}).get(faction)
    nonrulers = [(oid, officer) for oid, officer in active if oid != own_ruler]
    roster = nonrulers or active
    oid, officer = min(roster, key=lambda item: (-item[1]['intelligence'], item[0]))
    return {'officer_id': oid, 'intelligence': int(officer['intelligence']),
            'politics': int(officer['politics']), 'faction': faction}


def observe_faction(state: dict, faction: str) -> dict:
    """True, immutable plain-value projection, with NO foreign province facts.

    Historic access is limited to own province observations at that time;
    data about a province previously held by someone else is not revealed.
    """
    visible = {}
    provinces = state['core']['provinces']
    for pid in sorted(provinces):
        if state['owners'].get(pid) != faction:
            continue
        p = provinces[pid]
        extra = state['extras'][pid]
        workers = dict(p['workers'])
        history = []
        for snap in state.get('advisor_snapshots', [])[-12:]:
            if snap.get('owners', {}).get(pid) != faction:
                continue
            h = snap.get('provinces', {}).get(pid, {})
            if h:
                history.append({'month': int(snap['month']),
                                'food': int(h['food']), 'gold': int(h['gold']),
                                'troops': int(h['troops']), 'support': int(h['support'])})
        visible[pid] = {'food': int(p['food_units']), 'gold': int(p['treasury_gold_cents']),
                        'troops': int(p['troops']), 'cultivation': int(p['cultivation']),
                        'support': int(p['support_ticks']), 'arms': int(extra['arms']),
                        'metal': int(extra['metal']), 'workers': workers,
                        'history': tuple(history)}
    return {'faction': faction, 'month': int(state['month']), 'provinces': visible}


def _risk_features(province):
    """Interpret correct facts, do not introduce fabricated observations."""
    food = province['food']; troops = province['troops']
    # Simple reserve horizon is an ADVISOR HEURISTIC, not a game-rule formula.
    reserve = max(30, (troops + 59) // 60 * 9)
    shortage = _clamp((reserve + 85 - food) / max(1, reserve + 85), 0, 1)
    history = province['history']
    food_delta = (food - history[0]['food']) if history else 0
    falling = _clamp(-food_delta / max(40, history[0]['food'] if history else 200), 0, 1)
    support = province['support'] / 50000
    discontent = _clamp((60-support) / 60, 0, 1)
    surplus = _clamp((food - reserve - 50) / 250, 0, 1)
    return {'shortage': shortage, 'falling': falling, 'discontent': discontent,
            'surplus': surplus, 'reserve': reserve, 'food_change': food_delta}


def _true_expert_adjustment(choice, features):
    """Bounded interpretation of an action's projected risk/benefit.

    Not a future-result oracle. The game will determine actual outcome.
    """
    shortage = features['shortage']; falling = features['falling']
    risk = min(1, 0.78*shortage + 0.70*falling)
    kind = choice.kind
    param = choice.params
    if kind == 'recruit':
        return 12 - 72*risk
    if kind == 'attack':
        return 6 - 60*risk
    if kind == 'relocate_army':
        return 17 - 30*risk
    if kind == 'allocate_workers':
        target = param.get('target')
        if target == 'farmers': return 10 + 43*risk
        if target in ('metalworkers','resources','artisans'): return 7 - 35*risk
    if kind == 'market':
        commodity = param.get('commodity')
        side = param.get('side')
        if commodity == 'food':
            return (18 + 48*risk) if side == 'buy' else (14 - 58*risk)
    if kind == 'transport' and param.get('commodity') == 'food':
        return 8 - 28*risk
    if kind == 'distribute_food':
        return 7 + 17*features['discontent'] - 44*risk
    if kind == 'train':
        return 0 - 18*risk
    if kind == 'appoint_governor':
        return 5 + 9*risk
    return 0


def _decision_difficulty(choice):
    """Complex forecasts are harder than obvious observations."""
    if choice.kind in ('attack', 'recruit','market', 'transport', 'relocate_army'):
        return 1.0
    if choice.kind in ('allocate_workers','appoint_governor'):
        return .75
    return .5


def evaluate_advisor(state, faction, rank, *, weight_bps=DEFAULT_WEIGHT_BPS):
    """Return sorted final (score, choice) plus a reproducible audit summary.

    Ruler and advisor scores are independently normalized to 0..100 on the
    same scoring scale. INT 50 is a provisional break-even pivot, not a
    historical KOEI rule. Low INT reverses difficult economic interpretations.
    """
    if not isinstance(weight_bps, int) or not 0 <= weight_bps <= 10000:
        raise ValueError('Advisor influence must be integer basis points 0..10000')
    advisor = select_advisor(state, faction)
    evidence = observe_faction(state, faction)
    if advisor is None or weight_bps == 0 or not rank:
        return rank, {'advisor': advisor, 'weight_bps': 0, 'assessments': [], 'changed': False}
    intel = advisor['intelligence']
    values = []
    for raw_score, choice in rank:
        # Stable sigmoid prevents scores >100 from silently saturating.
        ruler_score = 100/(1+math.exp(-(_clamp(raw_score,-400,400)-50)/25))
        home = state['core']['officers'][choice.actor]['province_id']
        # Candidate is already generated by engine for a selected legal faction
        # officer; a missing visibility projection is a hard failure, not a leak.
        if home not in evidence['provinces']:
            raise ValueError('Advisor candidate references invisible province')
        feat = _risk_features(evidence['provinces'][home])
        adjustment = _true_expert_adjustment(choice,feat)
        difficulty = _decision_difficulty(choice)
        skill = (intel-50)/50
        # Systematic misinterpretation below 50: correct facts, wrong meaning.
        advisor_score = _clamp(ruler_score + adjustment*skill*difficulty)
        # Deterministic residual interpretation uncertainty, not RNG. Both
        # scores use the *same* available facts, and only conclusions vary.
        uncertainty = (1.0-intel/100)*difficulty*7.0
        noise = (_hash(state['month'],faction,advisor['officer_id'],choice.kind,
                       choice.actor,choice.params)%2001-1000)/1000
        advisor_score = _clamp(advisor_score + uncertainty*noise)
        final = (ruler_score*(10000-weight_bps) + advisor_score*weight_bps)/10000
        values.append((round(final,6), choice, {'actor':choice.actor,'kind':choice.kind,
                       'params':choice.params,'raw_ruler_score':raw_score,
                       'ruler_score':round(ruler_score,4), 'advisor_score':round(advisor_score,4),
                       'final_score':round(final,4),'expert_signal':round(adjustment,4),
                       'intelligence':intel,'food_change_observed':feat['food_change']}))
    values.sort(key=lambda r: (-r[0],r[1].kind,r[1].actor,json.dumps(r[1].params,sort_keys=True)))
    output=[(score,choice) for score,choice,_ in values]
    audit=[record for _,_,record in values]
    initial = rank[0][1]
    changed = (output[0][1] != initial) if output else False
    return output, {'advisor':advisor, 'weight_bps':weight_bps, 'changed':changed,
                    'assessments': audit, 'visible_province_count':len(evidence['provinces']),
                    'month':state['month'], 'faction':faction}
