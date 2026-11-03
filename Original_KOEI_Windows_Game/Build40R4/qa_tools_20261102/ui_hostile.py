"""Hostile UI QA: map hit-testing, setup flow refusals, window geometry/aspect, random-button fuzz."""
import sys, os, time, random, json, traceback, collections
tree, outdir = sys.argv[1], sys.argv[2]; sys.path.insert(0, tree)
import tkinter, tkinter.messagebox as mb, tkinter.filedialog as fd, tkinter.simpledialog as sd
for mod in (mb,):
    for n in ('showinfo','showwarning','showerror','askyesno','askokcancel','askquestion','askretrycancel','askyesnocancel'):
        setattr(mod, n, lambda *a, **k: True)
fd.askopenfilename = fd.asksaveasfilename = lambda *a, **k: ''
from third_branch import strategic_game as sg
from third_branch.strategic_game import StrategicWindow, SLOTS, province_at_pixel, MAP_X, MAP_Y, MAP_DW, MAP_DH, W, H
res = collections.OrderedDict(); bad = []
sp = os.path.join(outdir, 'ui_hostile_save.json')
if os.path.exists(sp): os.remove(sp)
w = StrategicWindow(sp)
def pump(n=4):
    for _ in range(n): w.root.update()
class E:
    def __init__(s, x, y): s.x, s.y = x, y
# 1. hit-test every province, and misses
hits_bad = []
for i in range(1, 42):
    x, y, ww, hh = SLOTS[str(i)]['rect_px']
    px, py = MAP_X + (x + ww / 2) * MAP_DW / 1448, MAP_Y + (y + hh / 2) * MAP_DH / 1086
    w.selected = 0; w.on_click(E(px, py))
    if w.selected != i: hits_bad.append((i, w.selected))
res['hit_all_41_centres'] = 'PASS' if not hits_bad else f'FAIL {hits_bad}'
misses = []
for (x, y) in [(2, 2), (MAP_X - 30, 300), (W - 3, H - 3), (W // 2, H - 20), (W // 2, MAP_Y + MAP_DH + 20), (0, H // 2), (-50, -50), (10**6, 10**6)]:
    w.selected = 7; w.on_click(E(x, y))
    if w.selected != 7: misses.append((x, y, w.selected))
res['clicks_outside_map_or_in_border_strip_ignored'] = 'PASS' if not misses else f'FAIL {misses}'
# 2. setup flow refusals
assert w.map_mode == 'setup' and w.new_game_pending
vacant = next(int(p[1:]) for p, o in w.c.state['owners'].items() if o is None)
w.selected = vacant; w._finish_new_game_setup(); pump()
ruler_open = getattr(w, 'ruler_dialog', None) is not None and w.ruler_dialog.winfo_exists()
res['vacant_province_refused_no_dialog_no_save'] = 'PASS' if (not ruler_open and not os.path.exists(sp) and w.map_mode == 'setup') else f'FAIL dialog={ruler_open} save={os.path.exists(sp)}'
for b in ('start', 'select', 'b', 'x', 'y', 'bogus'):
    try: w.on_nes_button(b); pump()
    except Exception as e: bad.append(('setup_button', b, repr(e)))
res['setup_buttons_do_not_leave_setup_or_save'] = 'PASS' if (w.map_mode in ('setup', None) and w.pause_dialog is None and not os.path.exists(sp)) else f'FAIL mode={w.map_mode} save={os.path.exists(sp)}'
for d in list(w.root.winfo_children()):
    if isinstance(d, tkinter.Toplevel): d.destroy()
w.help_dialog = None; pump(); w.map_mode = 'setup'
# 3. confirm ruler on an owned province
owned = next(int(p[1:]) for p, o in w.c.state['owners'].items() if o is not None)
w.selected = owned; w._finish_new_game_setup(); pump()
dlg_ok = getattr(w, 'ruler_dialog', None) is not None and w.ruler_dialog.winfo_exists()
res['owned_province_opens_ruler_dialog'] = 'PASS' if dlg_ok else 'FAIL'
if dlg_ok:
    ok = w.ruler_confirm(); pump(10)
    res['confirm_saves_and_enters_province_screen'] = 'PASS' if (ok and os.path.exists(sp) and w.province_stage is not None and w.province_stage.active()) else f'FAIL ok={ok} save={os.path.exists(sp)}'
# 4. geometry / aspect / on-screen of every window
def windows():
    out = [('main', w.root)]
    for d in w.root.winfo_children():
        if isinstance(d, tkinter.Toplevel): out.append((d.title(), d))
    return out
def check_geometry(tag):
    for name, win in windows():
        win.update_idletasks(); ww, hh = win.winfo_width(), win.winfo_height()
        ratio = ww / hh if hh else 0
        if abs(ratio - 4 / 3) > 0.012: bad.append(('NOT_4_3', tag, name, ww, hh, round(ratio, 3)))
        if win.winfo_rootx() < -5 or win.winfo_rooty() < -5 or win.winfo_rootx() + ww > 1400 or win.winfo_rooty() + hh > 900: bad.append(('OFFSCREEN', tag, name, win.winfo_rootx(), win.winfo_rooty(), ww, hh))
pump(8); check_geometry('after_start')
for label, fn in (('help', lambda: w.show_context_help()), ('pause', lambda: w.pause())):
    try: fn(); pump(8); check_geometry(label)
    except Exception as e: bad.append((label, repr(e), traceback.format_exc()[-300:]))
    for d in list(w.root.winfo_children()):
        if isinstance(d, tkinter.Toplevel) and d is not getattr(w.province_stage, 'root', None): 
            try: d.destroy()
            except Exception: pass
    w.pause_dialog = None; w.help_dialog = None; pump()
# 5. random-button fuzz
BTN = ['up', 'down', 'left', 'right', 'a', 'b', 'start', 'select', 'help', 'x', 'y']
rng = random.Random(1234); counts = collections.Counter(); t0 = time.time(); steps = 0
while time.time() - t0 < 150:
    b = rng.choice(BTN); steps += 1
    try: w.on_nes_button(b)
    except Exception as e:
        counts[type(e).__name__ + ':' + str(e)[:70]] += 1
        if counts[type(e).__name__ + ':' + str(e)[:70]] == 1: bad.append(('BUTTON_FUZZ_EXC', b, repr(e)[:150], traceback.format_exc()[-400:]))
    if steps % 25 == 0:
        try: pump(2)
        except Exception as e: bad.append(('PUMP_EXC', repr(e)[:150]))
    if steps % 400 == 0:  # keep stray dialogs from stacking forever
        for d in list(w.root.winfo_children()):
            if isinstance(d, tkinter.Toplevel):
                try: d.destroy()
                except Exception: pass
        w.pause_dialog = w.help_dialog = None
res['random_button_steps'] = steps; res['random_button_exception_kinds'] = dict(counts)
try: sg.save_file  # sanity
except Exception: pass
json.dump({'res': res, 'bad': bad}, open(os.path.join(outdir, 'ui_hostile.json'), 'w'), indent=1, default=str)
for k, v in res.items(): print(k, '|', v)
print(len(bad), 'problems')
for b in bad[:12]: print(str(b)[:300])
try: w.root.destroy()
except Exception: pass
