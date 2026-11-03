"""Installed-game entry: opens the Classic / Enhanced launcher. Errors go to %APPDATA%\\KOEI_Classic_Enhanced\\launcher_error.log."""
import os, sys, traceback
HERE = os.path.dirname(os.path.abspath(__file__))
os.chdir(HERE)
sys.path.insert(0, HERE)
try:
    from launcher.app import main
    code = main()
except SystemExit as exc:
    code = exc.code
except BaseException:
    folder = os.path.join(os.environ.get('APPDATA') or HERE, 'KOEI_Classic_Enhanced')
    os.makedirs(folder, exist_ok=True)
    with open(os.path.join(folder, 'launcher_error.log'), 'a', encoding='utf-8') as fh:
        fh.write(traceback.format_exc() + '\n')
    code = 1
sys.exit(code if isinstance(code, int) else 0)
