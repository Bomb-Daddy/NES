#!/usr/bin/env python3
"""Win32 source overlay for exact hydrated EXEC0228 game, strict fail-closed."""
import pathlib, sys
BEFORE = '''#ifdef _WIN32
# include <windows.h>
# include <xinput.h>
#else'''
AFTER = '''#ifdef _WIN32
# include <windows.h>
# include <xinput.h>
# include <sys/stat.h>
#else'''

def apply(root):
    source=pathlib.Path(root)/'execution_199/gk1_native_shell.c'
    data=source.read_text(encoding='utf-8')
    if data.count(BEFORE)!=1 or '# include <sys/stat.h>\n#else' in data:
        raise ValueError('source changed or patch already applied')
    new=data.replace(BEFORE,AFTER,1)
    assert new.count(AFTER)==1
    source.write_text(new,encoding='utf-8',newline='')
    print('EXEC0229 Windows _stat64 include portability patch: applied')
if __name__=='__main__':
    try:
        if len(sys.argv)!=2: raise ValueError('usage: apply_win32_portability.py hydrated-root')
        apply(sys.argv[1])
    except (OSError,ValueError) as exc:
        print('EXEC0229 portability REFUSED:',exc,file=sys.stderr)
        raise SystemExit(1)
