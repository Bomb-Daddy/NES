"""Hostile fuzz: random kinds/actors/params through Campaign. Fail conditions:
 non-CoreError exception; state mutated on rejection; validate() failing after commits; replay nondeterminism; revision not +1 per commit."""
import sys, json, random, copy, collections
tree, out = sys.argv[1], sys.argv[2]; sys.path.insert(0, tree)
from og.core.fixed import CoreError
from third_branch.integrated import new_campaign_41, save_bytes, load_bytes, validate
from third_branch.historical_s1 import new_campaign_historical_s1
from third_branch.orders_ui import SPECS
KINDS = sorted({k for k, _ in SPECS.values()})
JUNK = [None, -1, 0, 1, 10**9, 2**63, -2**63, 'x', '', 'p1', 'p99', 'A', 'B', 'O01', 'nes:000', 'nes:999', 1.5, float('nan'), True, [], {}, [1], {'a': 1}, 'food', 'gold', 'buy', 'sell', '../../etc', 'ß' * 50]
def mkparams(rng, spec_fields, c):
    s = c.state; pool = list(s['owners']) + list(s['factions']) + list(s['core']['officers']) + JUNK
    p = {}
    for k, ch, d in spec_fields:
        p[k] = rng.choice(list(ch)) if ch and rng.random() < .5 else rng.choice(pool)
    for _ in range(rng.randint(0, 2)): p[rng.choice(['x', 'count', 'units', 'target', 'gold'])] = rng.choice(JUNK)
    return p
stats = collections.Counter(); bad = []
for label, mk in (('S41', new_campaign_41), ('S1', new_campaign_historical_s1)):
    for seed in range(1, 7):
        rng = random.Random(seed * 1000 + (label == 'S1')); c = mk(seed); log = []
        for step in range(400):
            kind = rng.choice(KINDS)
            fields = next(f for k, f in SPECS.values() if k == kind)
            actor = rng.choice(list(c.state['core']['officers']) + ['', None, 'zzz'])
            params = mkparams(rng, fields, c); rid = f'f{step}'
            fp = c.fingerprint; rev = c.state['revision']
            try:
                c.do(kind, actor, params, rid)
                stats['commit'] += 1; log.append((kind, actor, params, rid))
                if c.state['revision'] != rev + 1: bad.append((label, seed, step, 'REV_NOT_PLUS_1', kind))
            except CoreError:
                stats['reject'] += 1
                if c.fingerprint != fp: bad.append((label, seed, step, 'MUTATED_ON_REJECT', kind, str(params)[:120]))
            except BaseException as e:
                stats['CRASH'] += 1; bad.append((label, seed, step, 'CRASH', kind, type(e).__name__, str(e)[:100], str(params)[:120]))
                if c.fingerprint != fp: bad.append((label, seed, step, 'MUTATED_ON_CRASH', kind))
        try: validate(c.state)
        except BaseException as e: bad.append((label, seed, 'END', 'VALIDATE_FAIL', str(e)[:100]))
        # save/load and deterministic replay
        try:
            if load_bytes(save_bytes(c)).fingerprint != c.fingerprint: bad.append((label, seed, 'END', 'SAVELOAD_DIFF'))
            r = mk(seed)
            for kind, actor, params, rid in log: r.do(kind, actor, params, rid)
            if r.fingerprint != c.fingerprint: bad.append((label, seed, 'END', 'REPLAY_DIVERGED', len(log)))
        except BaseException as e: bad.append((label, seed, 'END', 'REPLAY_ERR', type(e).__name__, str(e)[:100]))
json.dump({'stats': stats, 'bad': bad}, open(out, 'w'), indent=1, default=str)
print(dict(stats)); print(len(bad), 'problems')
for b in collections.OrderedDict((str(x[3:5]), x) for x in bad).values(): print(b)
