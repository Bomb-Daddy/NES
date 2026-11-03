"""E1 layer-1 sequenced setups for the 7 commands not committed by layer1.py / layer1_s1.py.
Setups use only real engine commands (c.do). usage: layer1_seq.py TREE OUT.json"""
import sys, json, itertools, copy
tree, out = sys.argv[1], sys.argv[2]; sys.path.insert(0, tree)
from og.core.fixed import CoreError
from third_branch.integrated import new_campaign_41, save_bytes, load_bytes
from third_branch.historical_s1 import new_campaign_historical_s1, list_recruitable
def code(e): return getattr(e, 'code', type(e).__name__)
def ruler_actor(c):
    s = c.state; f = s['officer_faction']; fac = f[next(iter(f))]
    return [o for o in f if f[o] == fac]
def variants(base, kind, actor, params, setup_steps):
    r = {}
    c = copy.deepcopy(base)
    for st in setup_steps: c.do(*st)
    fp = c.fingerprint
    q = c.preview(kind, actor, params, 'seq1'); r['d_preview'] = 'ok_no_mutation' if c.fingerprint == fp else 'MUTATED'
    c.execute(kind, actor, params, 'seq1', q['quote_id'], q['revision'])
    r['b_commit'] = 'committed' if c.fingerprint != fp else 'NO_CHANGE'
    fp1 = c.fingerprint
    for lab, rid, qid in (('e_dup', 'seq1', None), ('e_stale', 'seq2', 'bad')):
        try: c.execute(kind, actor, params, rid, qid); r[lab] = 'ACCEPTED'
        except CoreError as e: r[lab] = ('rejected:' + code(e)) if c.fingerprint == fp1 else 'MUTATED_ON_REJECT'
    c2 = copy.deepcopy(base)
    for st in setup_steps: c2.do(*st)
    q2 = c2.preview(kind, actor, params, 'seq3'); fp2 = c2.fingerprint
    try: c2.execute(kind, actor, params, 'seq3', q2['quote_id'], q2['revision'] + 5); r['e_badrev'] = 'ACCEPTED'
    except CoreError as e: r['e_badrev'] = ('rejected:' + code(e)) if c2.fingerprint == fp2 else 'MUTATED_ON_REJECT'
    r['h_saveload'] = 'identical' if load_bytes(save_bytes(c)).fingerprint == c.fingerprint else 'DIFFERENT'
    return r
res = {}
def attempt(name, base, kind, cands, steps_fn=lambda a, p: []):
    tried = 0; refusals = {}
    for actor, params in cands:
        tried += 1
        try:
            r = variants(base, kind, actor, params, steps_fn(actor, params))
            r.update(actor=actor, params=params, tried=tried, scenario=base.state['source_label']); res[name] = r; return
        except CoreError as e: refusals[code(e)] = refusals.get(code(e), 0) + 1
        except Exception as e: refusals['ERR:' + type(e).__name__ + ':' + str(e)[:60]] = refusals.get('ERR', 0) + 1
    res[name] = {'b_commit': 'NO_VALID_SETUP_FOUND', 'tried': tried, 'refusals': refusals}
for label, base in (('S41', new_campaign_41(7)), ('S1', new_campaign_historical_s1())):
    s = base.state; f = s['officer_faction']; offs = list(f)
    def loc(o): return s['core']['officers'][o]['province_id']
    facs = list(s['factions'])
    def sfx(n): return f'{n}@{label}'
    # Cancel Alliance: Ally first (real command), then Cancel
    cands = [(a, {'target': t}) for a in offs for t in facs if t != f[a]]
    if sfx('Cancel Alliance') not in res or res.get(sfx('Cancel Alliance'), {}).get('b_commit') != 'committed':
        for name, kind, extra in (('Cancel Alliance', 'Cancel', None),):
            tried = 0; refusals = {}
            for a, p in cands:
                tried += 1
                try:
                    r = variants(base, 'Cancel', a, p, [('Ally', a, p, 'setup-ally')]); r.update(actor=a, params=p, tried=tried); res[sfx(name)] = r; break
                except CoreError as e: refusals[code(e)] = refusals.get(code(e), 0) + 1
                except Exception as e: refusals['ERR:' + type(e).__name__] = refusals.get('ERR:' + type(e).__name__, 0) + 1
            else: res[sfx(name)] = {'b_commit': 'NO_VALID_SETUP_FOUND', 'tried': tried, 'refusals': refusals}
    # Joint: Ally then Joint with objective province
    tried = 0; refusals = {}; done = False
    for a in offs:
        for t in facs:
            if t == f[a] or done: continue
            for obj in s['owners']:
                tried += 1
                p = {'target': t, 'objective': obj, 'promised_troops': 20}
                try:
                    r = variants(base, 'Joint', a, p, [('Ally', a, {'target': t}, 'setup-ally')]); r.update(actor=a, params=p, tried=tried); res[sfx('Joint')] = r; done = True; break
                except CoreError as e: refusals[code(e)] = refusals.get(code(e), 0) + 1
                except Exception as e: refusals['ERR:' + type(e).__name__] = refusals.get('ERR:' + type(e).__name__, 0) + 1
                if tried > 3000: done = True; break
    if not done or sfx('Joint') not in res: res[sfx('Joint')] = {'b_commit': 'NO_VALID_SETUP_FOUND', 'tried': tried, 'refusals': refusals}
    # Transport: friendly adjacent target, actor = any officer, commodity food
    c_t = [(a, {'target': t, 'commodity': 'food', 'units': 10}) for a in offs for t in s['neighbors'].get(loc(a), [])]
    attempt(sfx('Transport'), base, 'transport', c_t)
    # Move Army: assign troops to a commander first, then relocate to adjacent friendly/vacant
    c_m = []
    for a in offs:
        for b in offs:
            if a != b and f[a] == f[b] and loc(a) == loc(b):
                for t in s['neighbors'].get(loc(a), []): c_m.append((b, {'target': t}, a))
    done = False; tried = 0; refusals = {}
    for b, p, a in c_m:
        tried += 1
        try:
            r = variants(base, 'relocate_army', b, p, [('assign_troops', a, {'target_officer': b, 'count': 20}, 'setup-assign')]); r.update(actor=b, params=p, tried=tried); res[sfx('Move Army')] = r; done = True; break
        except CoreError as e: refusals[code(e)] = refusals.get(code(e), 0) + 1
        except Exception as e: refusals['ERR:' + type(e).__name__] = refusals.get('ERR:' + type(e).__name__, 0) + 1
    if not done: res[sfx('Move Army')] = {'b_commit': 'NO_VALID_SETUP_FOUND', 'tried': tried, 'refusals': refusals}
    # Reward Governor: appoint governor first (real command) then reward by ruler
    done = False; tried = 0; refusals = {}
    for a in offs:
        for g in offs:
            if a != g and f[a] == f[g] and loc(a) == loc(g):
                tried += 1
                p = {'target_governor': g, 'method': 'gold', 'gold': 20}
                try:
                    r = variants(base, 'reward_governor', a, p, [('appoint_governor', a, {'officer': g, 'focus': 'farms'}, 'setup-gov')]); r.update(actor=a, params=p, tried=tried); res[sfx('Reward Governor')] = r; done = True; break
                except CoreError as e: refusals[code(e)] = refusals.get(code(e), 0) + 1
                except Exception as e: refusals['ERR:' + type(e).__name__] = refusals.get('ERR:' + type(e).__name__, 0) + 1
        if done: break
    if not done: res[sfx('Reward Governor')] = {'b_commit': 'NO_VALID_SETUP_FOUND', 'tried': tried, 'refusals': refusals}
    # Swear Brotherhood: same-province same-faction pairs, any actor
    cs = [(a, {'target_officer': b}) for a in offs for b in offs if a != b and f[a] == f[b]]
    attempt(sfx('Swear Brotherhood'), base, 'swear_brotherhood', cs)
    # Recruit Officer (S1 only): recruitable officers per province, actor in that province
    if label == 'S1':
        cr = []
        for a in offs:
            for t in list_recruitable(s, loc(a)): cr.append((a, {'officer_id': t}))
        attempt(sfx('Recruit Officer'), base, 'recruit_officer', cr)
json.dump(res, open(out, 'w'), indent=1, default=str)
for n, r in res.items(): print(n, '|', {k: v for k, v in r.items() if k != 'params'})
