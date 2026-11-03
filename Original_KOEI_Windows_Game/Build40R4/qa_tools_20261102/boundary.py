"""E1 variant f (boundary values) at the command bus for every numeric order field, using the layer-2/3 recipes.
For each (order, numeric field): probe -1,0,1,2,100,101,1000,10**6,2**31,2**63 on a fresh campaign (never a non-CoreError, never a mutation on refusal),
then find the largest legal value by doubling+bisection and confirm legal(max) and refused(max+1) with no mutation.
usage: boundary.py TREE OUT.json"""
import sys, os, json, copy
HERE = os.path.dirname(os.path.abspath(__file__))
tree, out = sys.argv[1], sys.argv[2]
sys.argv = ['layer23.py', tree, '/tmp/_unused']
src = open(os.path.join(HERE, 'layer23.py')).read().split("if __name__ == '__main__':")[0]
ns = {'__file__': os.path.join(HERE, 'layer23.py'), '__name__': 'layer23'}; exec(compile(src, 'layer23.py', 'exec'), ns)
SPECS, build, recipes, params_for_order, CoreError, code = (ns[k] for k in ('SPECS', 'build', 'recipes', 'params_for_order', 'CoreError', 'code'))
NUMERIC = {'count', 'units', 'effort', 'troops', 'promised_troops', 'gold_cents', 'gold'}
PROBES = [-1, 0, 1, 2, 100, 101, 1000, 10**6, 2**31, 2**63]
rs = recipes(); res = {}
def form_params(name, rec):
    """The params the recipe commits with (typed), via the same converter the form uses."""
    kind = SPECS[name][0]
    c = build(rec['scenario'], rec['seed'], rec['steps']); pid = c.state['core']['officers'][rec['actor']]['province_id']
    if rec['params'] is None:
        vals = {k: d for k, _, d in SPECS[name][1]}
        # the real form would pre-fill neighbours etc.; use engine-typed params from a UI-default probe
        try: return params_for_order(name, vals, pid)[1]
        except Exception: return None
    vals = {k: str(v) for k, v in rec['params'].items()}
    for k, _, d in SPECS[name][1]: vals.setdefault(k, d)
    return params_for_order(name, vals, pid)[1]

def attempt(name, rec, params):
    kind = SPECS[name][0]
    c = build(rec['scenario'], rec['seed'], rec['steps']); fp = c.fingerprint
    try:
        c.preview(kind, rec['actor'], params, 'bp')
        c.execute(kind, rec['actor'], params, 'bp'); return 'commit', None
    except CoreError as e:
        return 'refused:' + code(e), ('MUTATED' if c.fingerprint != fp else None)
    except Exception as e:
        return 'NON_CORE_ERROR:' + type(e).__name__ + ':' + str(e)[:50], 'ERR'

for name, rec in rs.items():
    r = {'fields': {}}; kind = SPECS[name][0]
    base = form_params(name, rec)
    if base is None: r['status'] = 'NO_BASE_PARAMS'; res[name] = r; continue
    # make the base legal first (recipe params are the proven legal set)
    base_out, _ = attempt(name, rec, base); r['base'] = {'params': base, 'outcome': base_out}
    nf = [k for k in base if k in NUMERIC]
    if not nf: r['status'] = 'NOT_APPLICABLE: no numeric field'; res[name] = r; continue
    bad = []
    for k in nf:
        probes = {}
        for v in PROBES:
            p = dict(base); p[k] = v; o, flag = attempt(name, rec, p); probes[str(v)] = o
            if flag: bad.append((k, v, o, flag))
        # largest legal value
        lo = None
        for v in PROBES:
            if probes[str(v)] == 'commit': lo = v if lo is None or v > lo else lo
        top = None
        if lo is not None:
            hi = lo
            while hi < 2**40:
                o, flag = attempt(name, rec, {**base, k: hi * 2})
                if flag: bad.append((k, hi * 2, o, flag))
                if o != 'commit': break
                hi *= 2
            a, b = hi, hi * 2          # a legal, b refused (or cap)
            while b - a > 1:
                m = (a + b) // 2; o, flag = attempt(name, rec, {**base, k: m})
                if flag: bad.append((k, m, o, flag))
                if o == 'commit': a = m
                else: b = m
            oa, _ = attempt(name, rec, {**base, k: a}); ob, fb = attempt(name, rec, {**base, k: a + 1})
            top = {'max_legal': a, 'max_legal_outcome': oa, 'max_plus_1_outcome': ob, 'mutation_flag': fb}
        r['fields'][k] = {'probes': probes, 'upper_boundary': top}
    r['status'] = 'PASS' if not bad else f'FAIL {bad[:3]}'
    r['violations'] = bad
    res[name] = r
    print(name, r['status'][:80], {k: (v['upper_boundary'] or {}).get('max_legal') for k, v in r['fields'].items()}, flush=True)
json.dump(res, open(out, 'w'), indent=1, default=str)
print('boundary done; orders', len(res), 'failing', [n for n, r in res.items() if str(r.get('status', '')).startswith('FAIL')])
