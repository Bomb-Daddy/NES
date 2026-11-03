"""E1 layer-1 engine check: drive every province order through Campaign preview/execute on a fresh campaign.
usage: layer1.py TREE OUT.json   Variants recorded: b commit, c wrong-owner refusal (no mutation), d preview-no-mutation,
e duplicate id + stale quote, h save/load round trip."""
import sys, json, copy
tree, out = sys.argv[1], sys.argv[2]
sys.path.insert(0, tree)
from og.core.fixed import CoreError
from third_branch.integrated import new_campaign_41, save_bytes, load_bytes
from third_branch.orders_ui import SPECS, params_for_order
c0 = new_campaign_41(7)
s0 = c0.state
player_fac = s0['officer_faction']['O14']
other = next(o for o, f in s0['officer_faction'].items() if f != player_fac)
res = {}
def code(e): return getattr(e, 'code', type(e).__name__)
for name, (kind, fields) in SPECS.items():
    r = {'kind': kind}
    vals = {k: d for k, _, d in fields}
    try:
        _, params = params_for_order(name, vals, 'p14')
    except Exception as e:
        r['form'] = f'form_error:{e}'; res[name] = r; continue
    for actor in ('O14', 'O01'):
        c = new_campaign_41(7); fp = c.fingerprint
        try:
            q = c.preview(kind, actor, params, 'r1')
            r[f'd_preview_{actor}'] = 'ok_no_mutation' if c.fingerprint == fp else 'MUTATED'
            c.execute(kind, actor, params, 'r1', q['quote_id'], q['revision'])
            r[f'b_commit_{actor}'] = 'committed' if c.fingerprint != fp and c.state['revision'] == 1 else 'NO_CHANGE'
            fp1 = c.fingerprint
            try: c.execute(kind, actor, params, 'r1'); r[f'e_dup_{actor}'] = 'DUP_ACCEPTED'
            except CoreError as e: r[f'e_dup_{actor}'] = f'rejected:{code(e)}' if c.fingerprint == fp1 else 'MUTATED_ON_REJECT'
            try: c.execute(kind, actor, params, 'r2', quote_id='bad'); r[f'e_stale_{actor}'] = 'STALE_ACCEPTED'
            except CoreError as e: r[f'e_stale_{actor}'] = f'rejected:{code(e)}' if c.fingerprint == fp1 else 'MUTATED_ON_REJECT'
            r[f'h_saveload_{actor}'] = 'identical' if load_bytes(save_bytes(c)).fingerprint == c.fingerprint else 'DIFFERENT'
            break
        except CoreError as e:
            r[f'b_commit_{actor}'] = f'rejected_with_defaults:{code(e)}'
        except Exception as e:
            r[f'b_commit_{actor}'] = f'ERROR:{type(e).__name__}:{e}'
    # c: wrong-owner actor must be refused, no mutation
    c = new_campaign_41(7); fp = c.fingerprint
    try:
        c.do(kind, other, params, 'w1'); r['c_wrong_owner'] = 'ACCEPTED'
    except CoreError as e:
        r['c_wrong_owner'] = f'rejected:{code(e)}' if c.fingerprint == fp else 'MUTATED_ON_REJECT'
    except Exception as e:
        r['c_wrong_owner'] = f'ERROR:{type(e).__name__}'
    res[name] = r
json.dump(res, open(out, 'w'), indent=1)
ok = lambda v: isinstance(v, str) and (v in ('committed', 'identical', 'ok_no_mutation') or v.startswith('rejected:'))
for n, r in res.items():
    print(n, '|', {k: v for k, v in r.items() if k != 'kind'})
