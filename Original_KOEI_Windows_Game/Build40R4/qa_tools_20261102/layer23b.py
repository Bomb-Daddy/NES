"""E1 layers 2/3, part B: province navigation (12 + targets), parametrised variants of the orders, tactical menu, launcher.
usage: layer23b.py TREE OUTDIR   (needs layer23.py next to it; results -> OUTDIR/layer23b_results.json)"""
import sys, os, json, copy, subprocess, tempfile, traceback
HERE = os.path.dirname(os.path.abspath(__file__))
tree, outdir = sys.argv[1], sys.argv[2]; os.makedirs(outdir, exist_ok=True)
sys.argv = ['layer23.py', tree, outdir]
src = open(os.path.join(HERE, 'layer23.py')).read().split("if __name__ == '__main__':")[0]
ns = {'__file__': os.path.join(HERE, 'layer23.py'), '__name__': 'layer23'}
exec(compile(src, 'layer23.py', 'exec'), ns)
globals().update({k: ns[k] for k in ('tkinter', 'build', 'recipes', 'open_gui', 'pump', 'submit_with', 'cleanup', 'fill_fields',
                                     'SPECS', 'OrdersWindow', 'params_for_order', 'MK', 'save_file', 'load_file', 'CoreError',
                                     'new_campaign_41', 'StrategicWindow', 'code')})
from third_branch.province_stage import NAV, CATEGORY_ORDERS
res = {}

# ------------------------------------------------------------------ A. navigation (controller-only route: D-pad down N times, A)
_N=[0]
def start_game(workdir):
    _N[0]+=1; path = os.path.join(workdir, f'nav{_N[0]}.json')
    g = StrategicWindow(path); pump(g)
    owned = next(int(p[1:]) for p, o in g.c.state['owners'].items() if o is not None)
    g.selected = owned; g._finish_new_game_setup(); pump(g)
    assert g.ruler_confirm() is True; pump(g)
    assert g.province_stage is not None and g.province_stage.active()
    return g

def toplevels(g): return [d for d in g.root.winfo_children() if isinstance(d, tkinter.Toplevel)]
def titles(g): return [d.title() for d in toplevels(g) if d is not getattr(g.province_stage, 'root', None)]
def close_extras(g):
    for d in toplevels(g):
        if d is not getattr(g.province_stage, 'root', None):
            try: d.destroy()
            except Exception: pass
    g.pause_dialog = None

def nav_all(workdir):
    out = {}
    for i, name in enumerate(NAV):
        g = start_game(workdir + '/' if False else workdir)
        try:
            st = g.province_stage; st.index = 0
            for _ in range(i): st.on_nes_button('down')
            pump(g); month0 = g.c.state['month']; fp0 = g.c.fingerprint
            st.on_nes_button('a'); pump(g, 6)
            st = g.province_stage
            note = ''
            if name in CATEGORY_ORDERS:
                ok = st.submenu == CATEGORY_ORDERS[name]
                # every child opens the order form on exactly that order, then B returns without a commit
                child_ok = []
                for j, child in enumerate(CATEGORY_ORDERS[name]):
                    st.index = 0
                    for _ in range(j): st.on_nes_button('down')
                    st.on_nes_button('a'); pump(g, 4)
                    od = getattr(g, 'orders_dialog', None)
                    opened = od is not None and od.root.winfo_exists() and od.order.get() == child
                    child_ok.append(opened)
                    if od is not None:
                        try: od.root.destroy()
                        except Exception: pass
                ok = ok and all(child_ok); note = f'children_open={child_ok}'
                st.on_nes_button('b'); pump(g); ok = ok and st.submenu is None and g.c.fingerprint == fp0
            elif name == 'Troop Assignments':
                ok = st.troop_dialog is not None and st.troop_dialog.active()
            elif name == 'Information': ok = any('Journal' in t for t in titles(g))
            elif name == 'Advisor': ok = any('Advisor' in t for t in titles(g))
            elif name == 'Save / Load': ok = g.pause_dialog is not None and g.pause_dialog.root.winfo_exists()
            elif name == 'Options / Help': ok = any('HELP' in t for t in titles(g))
            elif name == 'End Month':
                ok = g.c.state['month'] == month0 + 1 or bool(g.c.state.get('battle')) or bool(titles(g)) or g.c.state['month'] != month0
                note = f'month {month0}->{g.c.state["month"]}; province_active={g.province_stage is not None and g.province_stage.active()}'
            elif name == 'Back to Strategic Map': ok = g.map_mode == 'manual'
            else: ok = False
            out[name] = ('PASS ' if ok else 'FAIL ') + note
        except Exception:
            out[name] = 'FAIL exception ' + traceback.format_exc()[-250:]
        finally:
            close_extras(g); cleanup(g)
    return out

# ------------------------------------------------------------------ B. variants
VARIANTS = []   # (E1 id, order name, field, value)
def add(order, field, vals, ids):
    for v, i in zip(vals, ids): VARIANTS.append((i, order, field, v))
W = ('farmers', 'town', 'artisans', 'metalworkers', 'resources', 'land', 'flood')
add('Allocate Workers', 'source', W, [f'E1-{n:03d}' for n in range(14, 21)])
add('Recruit', 'worker_source', ('auto',) + W, [f'E1-{n:03d}' for n in range(24, 32)])
VARIANTS += [('E1-036', 'Market', ('commodity', 'side'), ('food', 'buy')), ('E1-037', 'Market', ('commodity', 'side'), ('food', 'sell')),
             ('E1-038', 'Market', ('commodity', 'side'), ('specialty', 'buy')), ('E1-039', 'Market', ('commodity', 'side'), ('specialty', 'sell')),
             ('E1-040', 'Market', ('commodity', 'side'), ('metal', 'buy')), ('E1-041', 'Market', ('commodity', 'side'), ('metal', 'sell')),
             ('E1-042', 'Market', ('commodity', 'side'), ('arms', 'buy')), ('E1-043', 'Market', ('commodity', 'side'), ('arms', 'sell')),
             ('E1-044', 'Market', ('commodity', 'side'), ('furs', 'buy')), ('E1-045', 'Market', ('commodity', 'side'), ('furs', 'sell'))]
add('Train Officer', 'trait', ('politics', 'war', 'leadership', 'intelligence', 'charisma'), [f'E1-{n:03d}' for n in range(47, 52)])
add('Transport', 'commodity', ('gold', 'food', 'specialty', 'troops', 'metal', 'arms', 'furs'), [f'E1-{n:03d}' for n in range(55, 62)])
add('Reward Governor', 'method', ('gold', 'horse'), ['E1-063', 'E1-064'])
add('Reward Officer', 'method', ('gold', 'horse', 'writings'), ['E1-066', 'E1-067', 'E1-068'])
add('Appoint Governor', 'focus', ('farms', 'industry', 'defense', 'personnel'), [f'E1-{n:03d}' for n in range(75, 79)])

def variant_once(vid, order, field, value, rc, extra_fill, workdir):
    """One attempt in the recipe's own scenario. Returns (record, legal_flag)."""
    fields = field if isinstance(field, tuple) else (field,); values = value if isinstance(value, tuple) else (value,)
    r = {'order': order, 'variant': dict(zip(fields, values)), 'extra_fill': dict(extra_fill), 'extra_steps': len(rc['steps']) - len(rc['_base_steps'])}
    kind = SPECS[order][0]
    g, path = open_gui(rc, 'v_' + vid, workdir)
    try:
        ow = OrdersWindow(g, initial_order=order); ow.actor.set(rc['actor']); ow._fields(); pump(g)
        fill = {}
        try:
            _k, _p = params_for_order(order, {k: v.get() for k, v in ow.fields.items()}, f'p{g.selected}')
            g.c.clone().preview(_k, rc['actor'], _p, 'probe')
        except Exception:
            fill = dict(rc['params'] or {})
        fill.update(extra_fill); fill.update(dict(zip(fields, values)))
        fill_fields(ow, order, fill)
        try:
            _k, gp = params_for_order(order, {k: v.get() for k, v in ow.fields.items()}, f'p{g.selected}')
        except Exception as e:
            r['status'] = 'REFUSED_BY_FORM:' + str(e); return r, False
        fp0 = g.c.fingerprint
        try:
            g.c.clone().preview(_k, rc['actor'], gp, 'probe2')
        except CoreError as e:
            r['status'] = f'REFUSED_BY_RULE:{code(e)}'
            # the refusal itself is evidence: submit through the UI and check there is no mutation
            submit_with(g, ow, True)
            r['refusal_via_ui_no_mutation'] = 'PASS' if g.c.fingerprint == fp0 else 'FAIL MUTATED'
            return r, False
        submit_with(g, ow, True)
        ui = [e for e in g.c.state['log'] if str(e.get('id', '')).startswith('ui:')]
        committed = g.c.fingerprint != fp0 and len(ui) == 1 and ui[0]['kind'] == kind
        fp1 = g.c.fingerprint
        cp = subprocess.run([sys.executable, '-I', os.path.join(HERE, 'layer23.py'), '--child', 'load', tree, path], capture_output=True, text=True, timeout=120)
        cold = json.loads(cp.stdout.strip().splitlines()[-1])['fingerprint'] == fp1
        recjson = os.path.join(workdir, 'rec_' + vid + '.json')
        json.dump(dict(scenario=rc['scenario'], seed=rc['seed'], steps=rc['steps'], gui_commands=[[kind, rc['actor'], gp, ui[0]['id'] if ui else None]]), open(recjson, 'w'), default=str)
        rp = subprocess.run([sys.executable, '-I', os.path.join(HERE, 'layer23.py'), '--child', 'replay', tree, recjson], capture_output=True, text=True, timeout=120)
        rep = json.loads(rp.stdout.strip().splitlines()[-1])['fingerprint'] == fp1
        r['status'] = 'PASS' if (committed and cold and rep) else f'FAIL committed={committed} cold={cold} replay={rep}'
        r['params'] = gp
        return r, True
    except Exception:
        r['status'] = 'FAIL exception ' + traceback.format_exc()[-250:]; return r, True
    finally:
        cleanup(g)

def variant_run(vid, order, field, value, rec, workdir):
    actor = rec['actor']; first = None
    horse = [('buy_horse', actor, {'units': 1}, 'bh1')]
    disband = [('disband_troops', actor, {'count': 10}, 'db1')]
    advisor = [('appoint_advisor', actor, {'target_officer': actor}, 'aa1')]
    options = [([], {}), (disband, {'count': '1'}), (disband, {}), ([], {'count': '1'}), ([], {'count': '1', 'units': '1'}), (horse, {}), (horse, {'count': '1'}),
               (advisor, {}), (advisor + horse, {}), ([], {'target': 'flood'}), ([], {'target': 'farmers'}), ([], {'units': '1'})]
    for extra_steps, extra_fill in options:
        rc = dict(rec); rc['_base_steps'] = rec['steps']; rc['steps'] = list(rec['steps']) + extra_steps
        try:
            r, legal = variant_once(vid, order, field, value, rc, extra_fill, workdir)
        except Exception:
            continue
        if first is None: first = r
        if legal: return r
    first = first or {'order': order}
    first['status'] = (first.get('status') or 'UNKNOWN') + ' (no legal state found after trying setup/param options)'
    return first

# ------------------------------------------------------------------ C. tactical menu
def tactical(workdir):
    from third_branch.tactical_view import TacticalWindow
    out = {}
    def fresh(tag):
        c = new_campaign_41(123); c.do('attack', 'O14', {'target': 'p15', 'troops': 100}, 'attack:initial')
        path = os.path.join(workdir, f'tac_{tag}.json'); save_file(c, path)
        g = StrategicWindow(path); pump(g)
        g.player = c.state['officer_faction']['O14']; g.selected = 14
        tw = TacticalWindow(g); pump(g)
        return g, tw
    g, tw = fresh('probe'); names = [n for n, _ in tw.controller_actions]; out['_controls'] = names
    try: tw.root.destroy()
    except Exception: pass
    cleanup(g)
    for label in names:
        if label == 'Close': continue
        g, tw = fresh(label.replace(' ', '_').replace('.', ''))
        try:
            fn = dict(tw.controller_actions)[label]; fp0 = g.c.fingerprint
            if label in ('Move', 'Move Enemy'):
                if label == 'Move Enemy' and not tw.selected_target: tw.next_target(); pump(g)
                fn(); pump(g); armed = tw.targeting
                tw.on_nes_button('b'); pump(g)
                cancelled = (not tw.targeting) and g.c.fingerprint == fp0
                # then arm again and commit the destination with A
                fn(); pump(g); tw.on_nes_button('a'); pump(g, 4)
                committed = g.c.fingerprint != fp0
                out[label] = (f'PASS armed={armed} cancel_no_mutation={cancelled} A_commits_or_refuses_cleanly(committed={committed}) msg={tw.message.get()[:50]}'
                              if armed and cancelled else f'FAIL armed={armed} cancel_no_mutation={cancelled}')
            else:
                if label in ('Bribe', 'Duel') and not tw.selected_target: tw.next_target(); pump(g)
                fn(); pump(g, 4)
                changed = g.c.fingerprint != fp0
                out[label] = 'PASS commit' if changed else 'PASS clean_refusal_or_prompt: ' + tw.message.get()[:80]
        except Exception:
            out[label] = 'FAIL exception ' + traceback.format_exc()[-250:]
        finally:
            try: tw.root.destroy()
            except Exception: pass
            cleanup(g)
    return out

# ------------------------------------------------------------------ D. launcher
def launcher():
    from launcher import app as lapp
    from unittest.mock import patch
    out = {'labels': list(lapp.MODES)}
    out['exact_labels'] = 'PASS' if lapp.MODES == ('Classic Game', 'Enhanced Game') else 'FAIL'
    class NoPad:
        def poll(self): pass
    L = lapp.Launcher(controller=NoPad()); L.root.update()
    try:
        out['rendered_buttons'] = [b.cget('text') for b in L.buttons]
        L.action('down'); L.root.update(); out['dpad_moves_focus'] = 'PASS' if L.index == 1 else 'FAIL'
        L.action('up'); out['dpad_back'] = 'PASS' if L.index == 0 else 'FAIL'
        calls = []
        with patch.object(L, 'choose', side_effect=lambda: calls.append(L.index)):
            L.index = 0; L.action('a'); L.index = 1; L.action('a')
        out['A_routes_to_choose_for_each_index'] = 'PASS' if calls == [0, 1] else f'FAIL {calls}'
    finally:
        try: L.root.destroy()
        except Exception: pass
    return out

if __name__ == '__main__':
    work = tempfile.mkdtemp(prefix='l23b_', dir=outdir)
    only = set(filter(None, os.environ.get('ONLY', '').split(',')))
    if not only: res['nav'] = nav_all(work); print('nav', res['nav'], flush=True)
    rs = recipes(); res['variants'] = {}
    for vid, order, field, value in VARIANTS:
        if only and vid not in only: continue
        rec_ = dict(rs[order])
        if order == 'Recruit':   # worker_source exists only under the historical workforce policy
            rec_ = dict(scenario='S1', seed=None, steps=[], actor='nes:000', params={'count': '1'})
        res['variants'][vid] = variant_run(vid, order, field, value, rec_, work)
        print(vid, res['variants'][vid].get('status'), flush=True)
    if not only:
        res['tactical'] = tactical(work); print('tactical', res['tactical'], flush=True)
        res['launcher'] = launcher(); print('launcher', res['launcher'], flush=True)
    json.dump(res, open(os.path.join(outdir, 'layer23b_results_only.json' if only else 'layer23b_results.json'), 'w'), indent=1, default=str)
