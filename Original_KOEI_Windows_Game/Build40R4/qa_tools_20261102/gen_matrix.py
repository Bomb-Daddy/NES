"""Generate ALL_COMMANDS_VERIFICATION_MATRIX (E1) from LIVE source. usage: gen_matrix.py TREE OUT.csv"""
import sys, csv, json, hashlib
tree, out = sys.argv[1], sys.argv[2]
sys.path.insert(0, tree)
from third_branch import province_stage as P, orders_ui as O, integrated as I
VAR = ['a_reach_UI','b_legal_commit','c_refuse_no_mutation','d_cancel','e_double_stale','f_boundary','g_interrupt_return','h_exit_coldload','i_mode_presence']
rows = []
def add(cid, surface, name, route, authority, notes='', status='NOT RUN', src='live source'):
    rows.append(dict(id=cid, surface=surface, command=name, route=route, authority=authority, source_state=src,
                     **{v:'NOT RUN' for v in VAR}, status=status, evidence_class='VERIFIED_STATIC (registry only)', notes=notes))
n = 0
def nid():
    global n; n += 1; return f'E1-{n:03d}'
# 1. 12 top-level categories
for i, c in enumerate(P.NAV, 1):
    add(nid(), 'Province nav', c, 'ProvinceStage.choose', 'UI0092 nav order', f'position {i} of {len(P.NAV)}')
# 2. children of categories, one row per command (+ branch rows for enumerated choice fields)
cat_of = {o: c for c, os_ in P.CATEGORY_ORDERS.items() for o in os_}
for name, (cmd, fields) in O.SPECS.items():
    cat = cat_of.get(name, 'Troop Assignments (not in CATEGORY_ORDERS)')
    route = f'{cat} > {name} -> OrdersWindow/engine:{cmd}'
    enum = [(k, ch) for k, ch, _ in fields if ch]
    if name == 'Market':
        for com in I.PRICE_CENTS:
            for side in ('buy', 'sell'):
                add(nid(), cat, f'{name}: {side} {com}', route, 'UI0092 + DEV-0095 v1.2', 'quote/commit branch')
    elif enum:
        k, ch = enum[0]
        add(nid(), cat, name, route, 'UI0092', 'main path')
        for v in ch:
            add(nid(), cat, f'{name}: {k}={v}', route, 'UI0092', 'enumerated branch')
    else:
        add(nid(), cat, name, route, 'UI0092')
# 3. battle-only orders (TacticalWindow)
for b in sorted(O.BATTLE_ONLY):
    add(nid(), 'Tactical', b, f'TacticalWindow -> engine:{b}', 'DEV-0095 / combat reference')
# 4. non-order nav targets and launcher
for name, route in [('Save / Load pause menu','parent.pause'),('Options / Help','parent.show_context_help'),('End Month','ProvinceStage.choose -> end month'),('Information journal','parent.journal'),('Advisor','parent.advisor'),('Back to Strategic Map','ProvinceStage.choose'),('Troop Assignments window','TroopAssignmentsWindow')]:
    add(nid(), 'Province nav target', name, route, 'UI0092')
import importlib.util
from launcher import app as L
for m in L.MODES: add(nid(), 'Launcher', m, 'launcher.app', 'plan C1 exact label')
# 5. plan-required (C-CMD) items vs source
required = {'Army->Assign': 'Assign Troops', 'Train Troops': 'Train Troops', 'Buy Arms via Market': None,
            'Equip Arms': 'Equip Arms', 'Forge Arms from Metal': None, 'Transport of Metal': 'Transport', 'Train Officer': 'Train Officer'}
gap = []
for req, spec in required.items():
    if req == 'Buy Arms via Market':
        ok = 'arms' in I.PRICE_CENTS
    elif req == 'Transport of Metal':
        ok = 'metal' in dict(O.SPECS['Transport'][1])['commodity'] if False else 'metal' in [c for k,ch,_ in O.SPECS['Transport'][1] if ch for c in ch]
    elif spec:
        ok = spec in O.SPECS
    else:
        ok = False
    if not ok and req == 'Forge Arms from Metal':
        add(nid(), 'Plan C-CMD required', req, 'REMOVED BY DIRECTOR RULING 2026-11-02', 'Director ruling 2026-11-02: no Forge Arms command', 'not required; excluded from denominator', status='RULED_OUT')
    elif not ok:
        gap.append(req)
        add(nid(), 'Plan C-CMD required', req, 'NO ROUTE FOUND IN SOURCE', 'DEV-0095 v1.2 (via plan v2.32 C-CMD)', 'approved but not present in registry', status='MISSING_IN_SOURCE')
cols = ['id','surface','command','route','authority','source_state'] + VAR + ['status','evidence_class','notes']
with open(out, 'w', newline='') as f:
    w = csv.DictWriter(f, fieldnames=cols); w.writeheader(); w.writerows(rows)
summary = dict(rows=len(rows), nav=len(P.NAV), specs=len(O.SPECS), engine_commands=len(I.COMMANDS), battle_only=len(O.BATTLE_ONLY), c_cmd_gaps=gap)
print(json.dumps(summary))
