"""Matrix verifier: exit 1 if matrix denominator != live-source registry, or a UI command is unreachable/unhooked.
usage: verify_matrix.py TREE MATRIX.csv"""
import sys, csv, re
tree, mpath = sys.argv[1], sys.argv[2]
sys.path.insert(0, tree)
from third_branch import province_stage as P, orders_ui as O, integrated as I
errs = []
rows = list(csv.DictReader(open(mpath)))
names = {r['command'] for r in rows}
# registry vs matrix
for n in P.NAV:
    if n not in names: errs.append(f'nav missing from matrix: {n}')
for n in O.SPECS:
    if not any(x == n or x.startswith(n + ':') for x in names): errs.append(f'command missing from matrix: {n}')
# registry vs UI reachability: every SPECS command must be reachable from a category or Troop Assignments
reach = {o for os_ in P.CATEGORY_ORDERS.values() for o in os_} | {'Assign Troops', 'Disband Soldiers'}
for n in O.SPECS:
    if n not in reach: errs.append(f'UI-unreachable command (not in any category): {n}')
for c, os_ in P.CATEGORY_ORDERS.items():
    for o in os_:
        if o not in O.SPECS: errs.append(f'category lists command with no spec: {c}/{o}')
# handler existence: each engine command name must be dispatched in integrated.py source
src = open(tree + '/third_branch/integrated.py').read() + open(tree + '/third_branch/officer_relations.py').read()
for cmd, _ in O.SPECS.values():
    if not re.search(r'[\'"]%s[\'"]|kind\s*==\s*[\'"]%s|def\s+%s\b' % (cmd, cmd, cmd), src):
        errs.append(f'no handler reference for engine command: {cmd}')
# approved-but-missing rows keep the gate red
for r in rows:
    if r['status'] == 'MISSING_IN_SOURCE': errs.append(f"approved command missing in source: {r['command']}")
print('\n'.join(errs) if errs else 'VERIFIER: clean')
sys.exit(1 if errs else 0)
