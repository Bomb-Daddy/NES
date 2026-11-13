#!/usr/bin/env python3
"""Real Win32 source fragment syntax-compiled on Linux; actual MSVC CI still required."""
import subprocess
import sys
import tempfile
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
from apply243 import apply, PRE_SHA, POST_SHA, OLD, NEW, normalize, digest

if len(sys.argv)!=2: sys.exit('Usage: test243.py <execution_199/gk1_native_shell.c>')
source=Path(sys.argv[1])
raw=normalize(source.read_bytes())
assert digest(raw) in (PRE_SHA,POST_SHA), 'unexpected source lineage'

def winproc_compile(blob):
    start=blob.index(b'static LRESULT CALLBACK window_proc(')
    end=blob.index(b'typedef DWORD(WINAPI *XinputGetStateFn)',start)
    code=b'''typedef int LRESULT; typedef int HWND; typedef int UINT;
typedef int WPARAM; typedef int LPARAM; typedef int PAINTSTRUCT;
#define CALLBACK
#define WM_DESTROY 2
#define WM_PAINT 15
static int running; static int screen_dc; static unsigned gk230_painted_frames;
void PostQuitMessage(int a);int BeginPaint(HWND a,PAINTSTRUCT *b);
void EndPaint(HWND a,PAINTSTRUCT *b);void render(void);
void gk231_trace(const char *a);
LRESULT DefWindowProcA(HWND h,UINT m,WPARAM w,LPARAM l);
'''+blob[start:end]
    with tempfile.TemporaryDirectory() as td:
        p=Path(td)/'winproc.c';p.write_bytes(code)
        r=subprocess.run(['cc','-std=c11','-Werror','-fsyntax-only',str(p)],capture_output=True,text=True)
        return r.returncode,r.stderr

with tempfile.TemporaryDirectory() as td:
    base=Path(td)/'execution_199';base.mkdir()
    p=base/'gk1_native_shell.c'
    pre=raw if digest(raw)==PRE_SHA else raw.replace(NEW,OLD)
    assert digest(pre)==PRE_SHA
    p.write_bytes(pre)
    rc,error=winproc_compile(pre)
    assert rc!=0,'old WM_PAINT function unexpectedly compiled'
    assert apply(p)==POST_SHA
    rc,error=winproc_compile(p.read_bytes())
    assert rc==0,'corrected real native function fails parsing: '+error
    unchanged=p.read_bytes()
    try: apply(p)
    except ValueError: pass
    else: raise AssertionError('repeated application accepted')
    assert p.read_bytes()==unchanged
    for corrupted in (b'not current source',pre.replace(b'WM_PAINT',b'WM_PIANT',1),pre+b'\x00'):
        p.write_bytes(corrupted)
        snap=p.read_bytes()
        try: apply(p)
        except ValueError: pass
        else: raise AssertionError('unverified predecessor accepted')
        assert p.read_bytes()==snap
print('EXEC0243: original Win32 parse fails; corrected source syntax-compiles; 4 rollback controls PASS')
