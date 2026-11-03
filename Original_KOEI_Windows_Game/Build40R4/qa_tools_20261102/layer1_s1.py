"""E1 layer-1 (historical S1): for each order, search actor/param candidates until one commits, then run variants.
usage: layer1_s1.py TREE OUT.json"""
import sys, json, itertools
tree, out = sys.argv[1], sys.argv[2]
sys.path.insert(0, tree)
from og.core.fixed import CoreError
from third_branch.historical_s1 import new_campaign_historical_s1, list_recruitable
from third_branch.integrated import save_bytes, load_bytes
from third_branch.orders_ui import SPECS, params_for_order
def fresh(): return new_campaign_historical_s1()
c0 = fresh(); s0 = c0.state
offs = list(s0['core']['officers']); of = s0['officer_faction']
fac0 = of[offs[0]]
mine = [o for o in offs if of.get(o) == fac0]
foes = [o for o in offs if of.get(o) not in (None, fac0)]
provs = list(s0['owners']); facs = list(s0['factions'])
def cands(name, fields):
    defaults = {k: d for k, _, d in fields}
    space = {}
    for k, ch, d in fields:
        if k in ('target', 'objective'): space[k] = provs + facs
        elif k in ('target_officer', 'target_governor', 'officer', 'general'): space[k] = mine[:6] + foes[:3]
        elif k == 'officer_id': space[k] = [r for r in list_recruitable(s0)[:4]] if False else [d, 'nes:000','nes:001','nes:002']
        elif ch: space[k] = list(ch)
        else: space[k] = [d]
    keys = list(space)
    # cap combinations
    for combo in itertools.islice(itertools.product(*[space[k] for k in keys]), 250):
        yield dict(zip(keys, combo))
def code(e): return getattr(e, 'code', type(e).__name__)
res = {}
actors = mine[:5] + foes[:1]
for name, (kind, fields) in SPECS.items():
    r = {'kind': kind, 'tried': 0}; found = None; refusals = {}
    for vals in cands(name, fields):
        for actor in actors:
            r['tried'] += 1
            c = fresh(); fp = c.fingerprint
            try:
                _, params = params_for_order(name, {k: str(v) for k, v in vals.items()}, s0['officer_faction'] and 'p0')
            except Exception as e:
                continue
            try:
                q = c.preview(kind, actor, params, 'r1')
                if c.fingerprint != fp: r['d_preview'] = 'MUTATED'
                c.execute(kind, actor, params, 'r1', q['quote_id'], q['revision'])
                if c.fingerprint != fp: found = (actor, params); break
            except CoreError as e:
                refusals[code(e)] = refusals.get(code(e), 0) + 1
                if c.fingerprint != fp: r['MUTATED_ON_REJECT'] = code(e)
            except Exception as e:
                refusals['ERR:' + type(e).__name__] = refusals.get('ERR:' + type(e).__name__, 0) + 1
        if found: break
    if not found:
        r['b_commit'] = 'NO_VALID_PARAMS_FOUND'; r['refusal_codes'] = refusals; res[name] = r; continue
    actor, params = found
    r['actor'] = actor; r['params'] = params; r['b_commit'] = 'committed'
    r.setdefault('d_preview', 'ok_no_mutation')
    c = fresh(); q = c.preview(kind, actor, params, 'r1'); c.execute(kind, actor, params, 'r1', q['quote_id'], q['revision'])
    fp1 = c.fingerprint; r['revision_after'] = c.state['revision']
    for lab, kw in (('e_dup', dict(request_id='r1')), ('e_stale', dict(request_id='r2', quote_id='bad'))):
        try: c.execute(kind, actor, params, kw['request_id'], kw.get('quote_id')); r[lab] = 'ACCEPTED'
        except CoreError as e: r[lab] = ('rejected:' + code(e)) if c.fingerprint == fp1 else 'MUTATED_ON_REJECT'
    r['h_saveload'] = 'identical' if load_bytes(save_bytes(c)).fingerprint == c.fingerprint else 'DIFFERENT'
    # stale revision: commit another no-op-ish order first is order-specific; test expected_revision mismatch directly
    c2 = fresh(); q2 = c2.preview(kind, actor, params, 'r3'); fp2 = c2.fingerprint
    try: c2.execute(kind, actor, params, 'r3', q2['quote_id'], q2['revision'] + 5); r['e_badrev'] = 'ACCEPTED'
    except CoreError as e: r['e_badrev'] = ('rejected:' + code(e)) if c2.fingerprint == fp2 else 'MUTATED_ON_REJECT'
    res[name] = r
json.dump(res, open(out, 'w'), indent=1, default=str)
for n, r in res.items(): print(n, '|', {k: v for k, v in r.items() if k not in ('kind', 'params')})
