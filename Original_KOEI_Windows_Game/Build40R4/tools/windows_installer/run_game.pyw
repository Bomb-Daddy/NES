"""Installed-game entry: opens the Classic / Enhanced launcher.
This program has no console, so everything it would print (and any crash) goes to
%APPDATA%\\KOEI_Classic_Enhanced\\game.log, and a crash is also shown in a message box."""
import os, sys, traceback, time
HERE = os.path.dirname(os.path.abspath(__file__))
os.chdir(HERE)
sys.path.insert(0, HERE)
LOGDIR = os.path.join(os.environ.get('APPDATA') or HERE, 'KOEI_Classic_Enhanced')
LOG = os.path.join(LOGDIR, 'game.log')
try:
    os.makedirs(LOGDIR, exist_ok=True)
    _fh = open(LOG, 'a', encoding='utf-8', buffering=1)
    _fh.write('--- start %s ---\n' % time.strftime('%Y-%m-%d %H:%M:%S'))
    if sys.stderr is None: sys.stderr = _fh
    if sys.stdout is None: sys.stdout = _fh
except Exception:
    pass

def _box(text):
    try:
        import ctypes
        ctypes.windll.user32.MessageBoxW(0, text, 'KOEI Classic / Enhanced', 0x10)
    except Exception:
        pass

try:
    from launcher.app import main
    code = main()
except SystemExit as exc:
    code = exc.code
except BaseException:
    err = traceback.format_exc()
    try:
        sys.stderr.write(err + '\n')
    except Exception:
        pass
    _box('The game stopped because of an error.\n\nDetails were saved to:\n' + LOG + '\n\n' + err[-900:])
    code = 1
sys.exit(code if isinstance(code, int) else 0)
