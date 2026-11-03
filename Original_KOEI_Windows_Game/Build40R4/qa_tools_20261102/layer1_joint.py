import sys, json, copy
tree, out = sys.argv[1], sys.argv[2]; sys.path.insert(0, tree)
from og.core.fixed import CoreError
from third_branch.integrated import new_campaign_41
from third_branch.historical_s1 import new_campaign_historical_s1
import importlib.util
spec = importlib.util.spec_from_file_location('l2', '/tmp/claude-0/-home-claude/48069ccc-e05e-5b3a-8e88-00ed8e45cef2/scratchpad/e1_tools/layer1_seq2.py')
src = open('/tmp/claude-0/-home-claude/48069ccc-e05e-5b3a-8e88-00ed8e45cef2/scratchpad/e1_tools/layer1_seq2.py').read().split('res = {}')[0].replace('tree, out = sys.argv[1], sys.argv[2]; sys.path.insert(0, tree)', '')
ns = {'sys': sys, 'json': json, 'copy': copy, '__file__': '/tmp/claude-0/-home-claude/48069ccc-e05e-5b3a-8e88-00ed8e45cef2/scratchpad/e1_tools/layer1_seq2.py'}; exec(src, ns); run = ns['run']; code = ns['code']
res = {}
for label, mk in (('S41', new_campaign_41), ('S1', new_campaign_historical_s1)):
    base = mk(2); s = base.state; f = s['officer_faction']; done = False; refusals = {}
    for a in f:
        for t in s['factions']:
            if t == f[a]: continue
            c = copy.deepcopy(base)
            try: c.do('Ally', a, {'target': t}, 'sa')
            except Exception: continue
            if not c.state['alliances']: continue
            for obj in s['owners']:
                steps = [('Ally', a, {'target': t}, 'sa'), ('plan_attack', a, {'target': obj}, 'sp')]
                p = {'target': t, 'objective': obj, 'promised_troops': 20}
                try:
                    r = run(base, 'Joint', a, p, steps); r.update(actor=a, params=p); res['Joint@' + label] = r; done = True; break
                except CoreError as e: refusals[code(e)] = refusals.get(code(e), 0) + 1
                except Exception as e: refusals['ERR:' + type(e).__name__ + str(e)[:40]] = 1
            if done: break
        if done: break
    if not done: res['Joint@' + label] = {'b_commit': 'NO_VALID_SETUP_FOUND', 'refusals': refusals}
json.dump(res, open(out, 'w'), indent=1, default=str)
for n, r in res.items(): print(n, '|', {k: v for k, v in r.items() if k != 'params'})
