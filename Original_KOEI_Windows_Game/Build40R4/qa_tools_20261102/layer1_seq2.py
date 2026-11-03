"""Remaining 3: Cancel Alliance, Joint (need a real alliance; Ally is chance-based, vary seed/request id), Swear Brotherhood (raise loyalty via real reward_officer)."""
import sys, json, copy
tree, out = sys.argv[1], sys.argv[2]; sys.path.insert(0, tree)
sys.path.insert(0, __import__('os').path.dirname(__import__('os').path.abspath(__file__)))
from og.core.fixed import CoreError
from third_branch.integrated import new_campaign_41, save_bytes, load_bytes
from third_branch.historical_s1 import new_campaign_historical_s1
def code(e): return getattr(e, 'code', type(e).__name__)
def run(base, kind, actor, params, steps):
    r = {}; c = copy.deepcopy(base)
    for st in steps: c.do(*st)
    fp = c.fingerprint
    q = c.preview(kind, actor, params, 'z1'); r['d_preview'] = 'ok_no_mutation' if c.fingerprint == fp else 'MUTATED'
    c.execute(kind, actor, params, 'z1', q['quote_id'], q['revision']); r['b_commit'] = 'committed' if c.fingerprint != fp else 'NO_CHANGE'
    fp1 = c.fingerprint
    for lab, rid, qid in (('e_dup', 'z1', None), ('e_stale', 'z2', 'bad')):
        try: c.execute(kind, actor, params, rid, qid); r[lab] = 'ACCEPTED'
        except CoreError as e: r[lab] = ('rejected:' + code(e)) if c.fingerprint == fp1 else 'MUTATED_ON_REJECT'
    c2 = copy.deepcopy(base)
    for st in steps: c2.do(*st)
    q2 = c2.preview(kind, actor, params, 'z3'); fp2 = c2.fingerprint
    try: c2.execute(kind, actor, params, 'z3', q2['quote_id'], q2['revision'] + 5); r['e_badrev'] = 'ACCEPTED'
    except CoreError as e: r['e_badrev'] = ('rejected:' + code(e)) if c2.fingerprint == fp2 else 'MUTATED_ON_REJECT'
    r['h_saveload'] = 'identical' if load_bytes(save_bytes(c)).fingerprint == c.fingerprint else 'DIFFERENT'
    return r
res = {}
for label, mk in (('S41', lambda sd: new_campaign_41(sd)), ('S1', lambda sd: new_campaign_historical_s1(sd))):
    # find (seed, actor, target) where Ally results in a real alliance
    found = None
    for seed in range(1, 40):
        base = mk(seed); s = base.state; f = s['officer_faction']
        for a in f:
            for t in s['factions']:
                if t == f[a]: continue
                c = copy.deepcopy(base)
                try: c.do('Ally', a, {'target': t}, 'sa')
                except Exception: continue
                if c.state['alliances']: found = (seed, a, t); break
            if found: break
        if found: break
    if not found: res['Cancel Alliance@' + label] = res['Joint@' + label] = {'b_commit': 'NO_ALLIANCE_FORMED_IN_SEARCH'}; continue
    seed, a, t = found; base = mk(seed); s = base.state
    steps = [('Ally', a, {'target': t}, 'sa')]
    try:
        r = run(base, 'Cancel', a, {'target': t}, steps); r.update(actor=a, params={'target': t}, seed=seed); res['Cancel Alliance@' + label] = r
    except CoreError as e: res['Cancel Alliance@' + label] = {'b_commit': 'REFUSED:' + code(e), 'seed': seed}
    done = False; refusals = {}
    for obj in s['owners']:
        p = {'target': t, 'objective': obj, 'promised_troops': 20}
        try:
            r = run(base, 'Joint', a, p, steps); r.update(actor=a, params=p, seed=seed); res['Joint@' + label] = r; done = True; break
        except CoreError as e: refusals[code(e)] = refusals.get(code(e), 0) + 1
        except Exception as e: refusals['ERR:' + type(e).__name__] = 1
    if not done: res['Joint@' + label] = {'b_commit': 'NO_VALID_OBJECTIVE', 'refusals': refusals, 'seed': seed}
    # Swear Brotherhood: ruler actor, local same-faction officer; raise loyalty with real reward_officer steps
    f = s['officer_faction']; loc = lambda o: s['core']['officers'][o]['province_id']
    done = False; refusals = {}
    for ra in f:
        for b in f:
            if ra == b or f[ra] != f[b] or loc(ra) != loc(b): continue
            for n in (0, 3, 6, 10):
                steps2 = [('reward_officer', ra, {'target_officer': b, 'method': 'gold', 'gold': 20}, f'sr{i}') for i in range(n)]
                try:
                    r = run(base, 'swear_brotherhood', ra, {'target_officer': b}, steps2); r.update(actor=ra, params={'target_officer': b}, setup_rewards=n); res['Swear Brotherhood@' + label] = r; done = True; break
                except CoreError as e: refusals[code(e)] = refusals.get(code(e), 0) + 1
                except Exception as e: refusals['ERR:' + type(e).__name__] = refusals.get('ERR:' + type(e).__name__, 0) + 1
            if done: break
        if done: break
    if not done: res['Swear Brotherhood@' + label] = {'b_commit': 'NO_VALID_SETUP_FOUND', 'refusals': refusals}
json.dump(res, open(out, 'w'), indent=1, default=str)
for n, r in res.items(): print(n, '|', {k: v for k, v in r.items() if k != 'params'})
