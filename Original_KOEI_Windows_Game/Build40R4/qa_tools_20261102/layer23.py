"""E1 layers 2 and 3 for every order, driven through the REAL launched UI classes (StrategicWindow + OrdersWindow)
under xvfb, then a FRESH PROCESS cold-load and an independent engine replay.

usage: layer23.py TREE OUTDIR            (parent: runs everything)
       layer23.py --child load FILE      (child: cold-load a save in a new process, print fingerprint + validate)
       layer23.py --child replay TREE JSON   (child: rebuild the base campaign, replay recorded commands, print fingerprint)

Layer 2 variants per order: a reach (order selectable, all spec fields rendered), c refusal (bad field / bad actor,
no mutation), d cancel (confirm dialog answered No, no mutation), b commit (confirm Yes, exactly one commit),
e controller-only (D-pad focus walk to Confirm, A=preview only, A=commit once, A=preview again).
Layer 3: h cold-load in a new process == in-GUI fingerprint; independent replay in a new process == in-GUI fingerprint.
"""
import sys, os, json, copy, subprocess, tempfile, traceback, itertools

def code(e): return getattr(e, 'code', type(e).__name__)

# ------------------------------------------------------------------ child modes
if len(sys.argv) > 1 and sys.argv[1] == '--child':
    mode = sys.argv[2]
    if mode == 'load':
        sys.path.insert(0, sys.argv[3]); from third_branch.integrated import load_file, validate
        c = load_file(sys.argv[4]); validate(c.state)
        print(json.dumps({'fingerprint': c.fingerprint, 'revision': c.state['revision'], 'valid': True}))
    elif mode == 'replay':
        sys.path.insert(0, sys.argv[3]); rec = json.load(open(sys.argv[4]))
        from third_branch.integrated import new_campaign_41, validate
        from third_branch.historical_s1 import new_campaign_historical_s1
        mk = new_campaign_41 if rec['scenario'] == 'S41' else new_campaign_historical_s1
        c = mk(rec['seed']) if rec['seed'] is not None else mk()
        for st in rec['steps']: c.do(*st)
        for st in rec['gui_commands']:
            kind, actor, params, rid = st
            c.execute(kind, actor, params, rid)
        validate(c.state)
        print(json.dumps({'fingerprint': c.fingerprint, 'revision': c.state['revision']}))
    sys.exit(0)

tree, outdir = sys.argv[1], sys.argv[2]; os.makedirs(outdir, exist_ok=True)
sys.path.insert(0, tree)
os.environ.setdefault('KOEI_DISABLE_AUDIO', '1')
import tkinter
from og.core.fixed import CoreError
from third_branch.integrated import new_campaign_41, save_file, load_file
from third_branch.historical_s1 import new_campaign_historical_s1
from third_branch.orders_ui import SPECS, OrdersWindow, params_for_order
from third_branch.strategic_game import StrategicWindow

MK = {'S41': new_campaign_41, 'S1': new_campaign_historical_s1}
def build(scenario, seed, steps):
    mk = MK[scenario]; c = mk(seed) if seed is not None else mk()
    for st in steps: c.do(*st)
    return c

# ------------------------------------------------------------------ recipes: where each order can legally commit
def spec_defaults(name, pid):
    vals = {k: d for k, _, d in SPECS[name][1]}
    return vals

SIMPLE = {   # (scenario, actor, params-or-None=spec defaults) taken from the layer-1 runs that committed
 'Allocate Workers': ('S41', 'O14', None), 'Distribute Food': ('S41', 'O14', None), 'Distribute Goods': ('S41', 'O14', None),
 'Recruit': ('S41', 'O14', None), 'Disband Soldiers': ('S41', 'O14', None), 'Market': ('S41', 'O14', None),
 'Train Officer': ('S41', 'O14', None), 'Buy Horse': ('S41', 'O01', None), 'Feast': ('S41', 'O01', None),
 'Rest': ('S41', 'O01', None), 'Appoint Governor': ('S41', 'O01', None), 'Ally': ('S41', 'O14', None),
 'Marry': ('S41', 'O14', None), 'Give': ('S41', 'O14', None), 'Threat': ('S41', 'O14', None),
 'Transport': ('S41', 'O01', {'target': 'p2', 'commodity': 'food', 'units': 10}),
 'Assign Troops': ('S1', 'nes:000', {'target_officer': 'nes:033', 'count': 10}),
 'Train Troops': ('S1', 'nes:000', {}), 'Equip Arms': ('S1', 'nes:000', {'general': 'nes:000', 'units': 1}),
 'Reward Officer': ('S1', 'nes:000', {'target_officer': 'nes:033', 'method': 'gold', 'gold': 20}),
 'Appoint Advisor': ('S1', 'nes:000', {'target_officer': 'nes:000'}),
 'Plan Attack': ('S1', 'nes:001', {'target': 'p3'}),
 'Attack': ('S1', 'nes:000', {'target': 'p6', 'troops': 80}),
 'Recruit Officer': ('S1', 'nes:000', {'officer_id': 'nes:099'}),
 'Reward Governor': ('S1', 'nes:000', {'target_governor': 'nes:033', 'method': 'gold', 'gold': 20}),
 'Move Army': ('S1', 'nes:054', {'target': 'p22'}),
}

def find_ally_seed(scenario):
    mk = MK[scenario]
    for seed in range(1, 40):
        base = mk(seed); s = base.state; f = s['officer_faction']
        for a in f:
            for t in s['factions']:
                if t == f[a]: continue
                c = copy.deepcopy(base)
                try: c.do('Ally', a, {'target': t}, 'sa')
                except Exception: continue
                if c.state['alliances']: return seed, a, t
    return None

def recipes():
    out = {}
    for name, (sc, actor, params) in SIMPLE.items():
        out[name] = dict(scenario=sc, seed=None, steps=[], actor=actor, params=params)
    for sc in ('S41',):
        f = find_ally_seed(sc)
        if f:
            seed, a, t = f; steps = [('Ally', a, {'target': t}, 'sa')]
            out['Cancel Alliance'] = dict(scenario=sc, seed=seed, steps=steps, actor=a, params={'target': t})
            base = MK[sc](seed)
            for obj in base.state['owners']:
                p = {'target': t, 'objective': obj, 'promised_troops': '20'}
                try:
                    c = copy.deepcopy(base)
                    for st in steps: c.do(*st)
                    c.preview('Joint', a, {'target': t, 'objective': obj, 'promised_troops': 20}, 'jz')
                    out['Joint'] = dict(scenario=sc, seed=seed, steps=steps, actor=a, params=p); break
                except Exception: continue
    # Joint needs a scheduled attack; reuse the layer-1 proven recipe if the first search found none
    if 'Joint' not in out:
        base = new_campaign_41(2); s = base.state; f = s['officer_faction']; done = False
        for a in f:
            for t in s['factions']:
                if t == f[a]: continue
                c = copy.deepcopy(base)
                try: c.do('Ally', a, {'target': t}, 'sa')
                except Exception: continue
                if not c.state['alliances']: continue
                for obj in s['owners']:
                    steps = [('Ally', a, {'target': t}, 'sa'), ('plan_attack', a, {'target': obj}, 'sp')]
                    try:
                        c2 = copy.deepcopy(base)
                        for st in steps: c2.do(*st)
                        c2.preview('Joint', a, {'target': t, 'objective': obj, 'promised_troops': 20}, 'jz')
                        out['Joint'] = dict(scenario='S41', seed=2, steps=steps, actor=a,
                                            params={'target': t, 'objective': obj, 'promised_troops': '20'}); done = True; break
                    except Exception: continue
                if done: break
            if done: break
    # Swear Brotherhood: ruler actor + same-province same-faction officer, loyalty raised with real reward_officer steps
    for sc in ('S1',):
        base = MK[sc](); s = base.state; f = s['officer_faction']; loc = lambda o: s['core']['officers'][o]['province_id']
        found = False
        for ra in f:
            for b in f:
                if ra == b or f[ra] != f[b] or loc(ra) != loc(b): continue
                for n in (0, 3, 6, 10):
                    steps = [('reward_officer', ra, {'target_officer': b, 'method': 'gold', 'gold': 20}, f'sr{i}') for i in range(n)]
                    try:
                        c = copy.deepcopy(base)
                        for st in steps: c.do(*st)
                        c.preview('swear_brotherhood', ra, {'target_officer': b}, 'sbz')
                        out['Swear Brotherhood'] = dict(scenario=sc, seed=None, steps=steps, actor=ra, params={'target_officer': b}); found = True; break
                    except Exception: continue
                if found: break
            if found: break
    # Reward Governor needs an appointed governor: search appoint_governor setup steps
    sc = 'S1'; base = MK[sc](); s0 = base.state; f = s0['officer_faction']; found = False
    for ruler_ in f:
        for gov in f:
            if gov == ruler_ or f[gov] != f[ruler_]: continue
            steps = [('appoint_governor', ruler_, {'officer': gov, 'focus': 'farms'}, 'ag1')]
            try:
                c = copy.deepcopy(base)
                for st in steps: c.do(*st)
                c.preview('reward_governor', ruler_, {'target_governor': gov, 'method': 'gold', 'gold': 20}, 'rgz')
                out['Reward Governor'] = dict(scenario=sc, seed=None, steps=steps, actor=ruler_,
                                              params={'target_governor': gov, 'method': 'gold', 'gold': 20}); found = True; break
            except Exception: continue
        if found: break
    return out

# ------------------------------------------------------------------ UI helpers
def pump(g, n=3):
    for _ in range(n): g.root.update()

def answer_confirm(g, yes):
    """Press Enter (Yes) or Esc (No) on the in-game confirm dialog once it exists. Returns a handle; call stop() after submit."""
    state = {'tries': 0, 'dead': False}
    def go():
        if state['dead']: return
        w = getattr(g, 'confirm_win', None)
        if w is not None:
            try:
                w.update(); w.event_generate('<Return>' if yes else '<Escape>'); return
            except tkinter.TclError: return
        state['tries'] += 1
        if state['tries'] < 80: g.root.after(25, go)
    g.root.after(25, go)
    return state

def submit_with(g, ow, yes):
    st = answer_confirm(g, yes)
    try: ow.submit()
    finally: st['dead'] = True
    pump(g, 6)

def cleanup(g):
    for d in list(g.root.winfo_children()):
        if isinstance(d, tkinter.Toplevel):
            try: d.destroy()
            except Exception: pass
    try: g.root.destroy()
    except Exception: pass

def open_gui(rec, tag, workdir):
    c = build(rec['scenario'], rec['seed'], rec['steps'])
    path = os.path.join(workdir, f'{tag}.json'); save_file(c, path)
    g = StrategicWindow(path); pump(g)
    actor = rec['actor']; o = g.c.state['core']['officers'][actor]
    g.player = g.c.state['officer_faction'][actor]; g.selected = int(o['province_id'][1:]); g.redraw()
    return g, path

def fill_fields(ow, name, params):
    """Write recipe params into the real form widgets (strings, as a player would type/select)."""
    for k, var in ow.fields.items():
        if params is not None and k in params: var.set(str(params[k]))

def run_order(name, rec, workdir):
    r = {'scenario': rec['scenario'], 'actor': rec['actor']}
    kind = SPECS[name][0]
    # ---- mouse/keyboard path
    g, path = open_gui(rec, 'm_' + name.replace(' ', '_'), workdir)
    try:
        ow = OrdersWindow(g, initial_order=name); ow.actor.set(rec['actor']); ow._fields(); pump(g)
        spec_fields = [k for k, _, _ in SPECS[name][1]]
        r['a_reach_UI'] = 'PASS' if (name in ow.allowed_orders and ow.order.get() == name and list(ow.fields) == spec_fields) \
            else f'FAIL fields={list(ow.fields)} expected={spec_fields}'
        params_in = rec['params'] if rec['params'] is not None else None
        try:
            _k, _p = params_for_order(name, {k: v.get() for k, v in ow.fields.items()}, f'p{g.selected}')
            g.c.clone().preview(_k, rec['actor'], _p, 'probe'); r['params_source'] = 'UI_DEFAULTS'
        except Exception:
            fill_fields(ow, name, params_in); r['params_source'] = 'RECIPE_OVERRIDE'
        try:
            gk, gp = params_for_order(name, {k: v.get() for k, v in ow.fields.items()}, f'p{g.selected}')
            r['form_params'] = gp
        except Exception as e:
            r['a_reach_UI'] += f' | form_error:{e}'; gk, gp = kind, {}
        fp0 = g.c.fingerprint; rev0 = g.c.state['revision']
        # c: refusal -> bad field value, then bad actor; neither may mutate
        c_notes = []
        if ow.fields:
            first = next(iter(ow.fields)); keep = ow.fields[first].get()
            ow.fields[first].set('zzz-not-valid'); ow.submit(); pump(g)
            c_notes.append('bad_field:' + ('no_mutation' if g.c.fingerprint == fp0 else 'MUTATED'))
            ow.fields[first].set(keep)
        keep_actor = ow.actor.get(); ow.actor.set('ZZ-NO-SUCH'); submit_with(g, ow, True)
        c_notes.append('bad_actor:' + ('no_mutation' if g.c.fingerprint == fp0 else 'MUTATED'))
        ow.actor.set(keep_actor)
        r['c_refuse_no_mutation'] = 'PASS' if all(x.endswith('no_mutation') for x in c_notes) else 'FAIL ' + ';'.join(c_notes)
        r['c_detail'] = c_notes
        # d: cancel
        submit_with(g, ow, False)
        r['d_cancel'] = 'PASS' if g.c.fingerprint == fp0 and 'cancel' in g.msg.get().lower() else \
            f'FAIL changed={g.c.fingerprint != fp0} msg={g.msg.get()[:60]}'
        # b: commit exactly once
        submit_with(g, ow, True)
        lc = getattr(g.c, 'last_commit', None)
        committed = g.c.fingerprint != fp0
        span_ok = bool(lc) and lc['kind'] == kind and lc['revision_before'] == rev0 and lc['revision_after'] == g.c.state['revision'] \
            and g.c.state['revision'] - rev0 == 1 + lc['event_records']
        r['b_legal_commit'] = 'PASS' if (committed and span_ok) else f'FAIL committed={committed} last_commit={lc} msg={g.msg.get()[:80]}'
        r['commit_msg'] = g.msg.get()[:100]
        # exactly one command entry of this kind with a ui: id
        ui_entries = [e for e in g.c.state['log'] if str(e.get('id', '')).startswith('ui:')]
        r['b_one_ui_entry'] = 'PASS' if len(ui_entries) == 1 and ui_entries[0]['kind'] == kind else f'FAIL ui_entries={len(ui_entries)}'
        gui_rid = ui_entries[0]['id'] if ui_entries else None
        gui_fp = g.c.fingerprint
        # ---- layer 3
        disk = load_file(path).fingerprint
        r['h_disk_matches_memory'] = 'PASS' if disk == gui_fp else 'FAIL'
        cp = subprocess.run([sys.executable, '-I', __file__, '--child', 'load', tree, path], capture_output=True, text=True, timeout=120)
        try:
            cj = json.loads(cp.stdout.strip().splitlines()[-1])
            r['h_coldload_new_process'] = 'PASS' if cj['fingerprint'] == gui_fp and cj['valid'] else 'FAIL fingerprint differs'
        except Exception:
            r['h_coldload_new_process'] = 'FAIL child_error:' + (cp.stderr or cp.stdout)[-200:]
        recjson = os.path.join(workdir, 'rec_' + name.replace(' ', '_') + '.json')
        json.dump(dict(scenario=rec['scenario'], seed=rec['seed'], steps=rec['steps'],
                       gui_commands=[[kind, rec['actor'], gp, gui_rid]]), open(recjson, 'w'), default=str)
        rp = subprocess.run([sys.executable, '-I', __file__, '--child', 'replay', tree, recjson], capture_output=True, text=True, timeout=120)
        try:
            rj = json.loads(rp.stdout.strip().splitlines()[-1])
            r['h_independent_replay'] = 'PASS' if rj['fingerprint'] == gui_fp else 'FAIL replay fingerprint differs'
        except Exception:
            r['h_independent_replay'] = 'FAIL child_error:' + (rp.stderr or rp.stdout)[-200:]
    except Exception:
        r['harness_exception'] = traceback.format_exc()[-400:]
    finally:
        cleanup(g)
    # ---- controller-only path on a fresh game
    g2, p2 = open_gui(rec, 'c_' + name.replace(' ', '_'), workdir)
    try:
        ow = OrdersWindow(g2, initial_order=name); ow.actor.set(rec['actor']); ow._fields(); pump(g2)
        try:
            _k, _p = params_for_order(name, {k: v.get() for k, v in ow.fields.items()}, f'p{g2.selected}')
            g2.c.clone().preview(_k, rec['actor'], _p, 'probe')
        except Exception:
            fill_fields(ow, name, rec['params'])
        fp0 = g2.c.fingerprint; rev0 = g2.c.state['revision']
        names = []
        for _ in range(len(ow._controller_items()) + 2):
            names.append(ow._controller_items()[ow.controller_index % len(ow._controller_items())][0])
            if names[-1] == 'confirm': break
            ow.on_nes_button('down')
        reached = names[-1] == 'confirm'
        steps_seen = []
        if reached:
            ow.on_nes_button('a'); pump(g2); steps_seen.append(g2.c.fingerprint != fp0)   # preview only
            ow.on_nes_button('a'); pump(g2, 6); steps_seen.append(g2.c.fingerprint != fp0)  # commit
            fp1 = g2.c.fingerprint; rev1 = g2.c.state['revision']
            ow.on_nes_button('a'); pump(g2); steps_seen.append(g2.c.fingerprint != fp1)   # preview again, no 2nd commit
        ui_entries = [e for e in g2.c.state['log'] if str(e.get('id', '')).startswith('ui:')]
        ok = reached and steps_seen == [False, True, False] and len(ui_entries) == 1
        r['e_controller_only_preview_then_commit_once'] = 'PASS' if ok else f'FAIL reached={reached} seq={steps_seen} ui_entries={len(ui_entries)} focus_walk={names}'
    except Exception:
        r['e_controller_only_preview_then_commit_once'] = 'FAIL exception ' + traceback.format_exc()[-300:]
    finally:
        cleanup(g2)
    return r

if __name__ == '__main__':
    rs = recipes(); res = {}
    work = tempfile.mkdtemp(prefix='l23_', dir=outdir)
    missing = [n for n in SPECS if n not in rs]
    for name in SPECS:
        if name not in rs:
            res[name] = {'status': 'NO_COMMITTING_RECIPE_FOUND'}; continue
        res[name] = run_order(name, rs[name], work)
        print(name, '|', {k: (v if not isinstance(v, str) else v[:70]) for k, v in res[name].items() if k.startswith(('a_', 'b_l', 'b_o', 'c_r', 'd_', 'e_', 'h_'))}, flush=True)
    json.dump({'recipes': rs, 'results': res}, open(os.path.join(outdir, 'layer23_results.json'), 'w'), indent=1, default=str)
    bad = [(n, k, v) for n, r in res.items() for k, v in r.items() if isinstance(v, str) and v.startswith('FAIL')]
    print('orders', len(res), 'missing_recipe', missing, 'failing_cells', len(bad))
    for b in bad[:30]: print(b)
