#!/usr/bin/env python3
"""EXEC0231 strict, instrumentation-only overlay for inherited EXEC0230 WinMain."""
from pathlib import Path
import sys

TRACE = r'''
/* EXEC0231: opt-in per-stage Windows GUI diagnosis; never reads gameplay bytes. */
static int gk231_diagnostic_active=0;
static void gk231_trace(const char *stage) {
 const char *path=getenv("GK1_SMOKE_TRACE");
 if(!gk231_diagnostic_active||!path||!*path)return;
 FILE *f=fopen(path,"ab");
 if(f) {
  fprintf(f,"%llu\t%s\n",(unsigned long long)GetTickCount64(),stage);
  fclose(f);
 }
}
'''

ANCHORS = [
(
 'static unsigned gk230_painted_frames=0u;',
 TRACE+'\nstatic unsigned gk230_painted_frames=0u;'
),
(
 'if(msg==WM_PAINT){PAINTSTRUCT p;screen_dc=BeginPaint(h,&p);render();EndPaint(h,&p);\n  ++gk230_painted_frames;return 0;}',
 'if(msg==WM_PAINT){gk231_trace("WM_PAINT entered");PAINTSTRUCT p;screen_dc=BeginPaint(h,&p);\n  gk231_trace("BeginPaint returned");render();gk231_trace("render returned");EndPaint(h,&p);\n  ++gk230_painted_frames;gk231_trace("WM_PAINT completed");return 0;}'
),
(
 'if(smoke_mode<0)return 4;\n gk1_app_init(&app);gk1_pad_init(&pad);load_optional_battle_view();load_optional_mesen_map();load_optional_spring_archive();load_optional_stock_spring();(void)load_scenario_data();',
 'if(smoke_mode<0)return 4;\n gk231_diagnostic_active=smoke_mode>0;\n gk231_trace("WinMain arguments accepted");\n gk1_app_init(&app);gk1_pad_init(&pad);load_optional_battle_view();load_optional_mesen_map();load_optional_spring_archive();load_optional_stock_spring();(void)load_scenario_data();\n gk231_trace("app and scenario initialized");'
),
(
 'ShowWindow(hwnd,smoke_mode?SW_SHOWNORMAL:show);UpdateWindow(hwnd);',
 'gk231_trace("before ShowWindow");\n ShowWindow(hwnd,smoke_mode?SW_SHOWNORMAL:show);\n gk231_trace("after ShowWindow, before UpdateWindow");\n UpdateWindow(hwnd);\n gk231_trace("after UpdateWindow");'
),
(
 'poll_xinput();InvalidateRect(hwnd,NULL,FALSE);UpdateWindow(hwnd);\n   Sleep(1);',
 'gk231_trace("before XInput");poll_xinput();gk231_trace("after XInput");\n   InvalidateRect(hwnd,NULL,FALSE);gk231_trace("before frame UpdateWindow");\n   UpdateWindow(hwnd);gk231_trace("after frame UpdateWindow");\n   Sleep(1);'
)
]

def apply(source: str) -> str:
    if 'EXEC0231: opt-in per-stage' in source:
        raise ValueError("EXEC0231 overlay already present")
    for before, after in ANCHORS:
        if source.count(before)!=1:
            raise ValueError("refusing changed predecessor anchor count="+str(source.count(before))+" token="+repr(before[:85])+" nearby="+repr([line for line in source.splitlines() if "gk230_painted" in line][:4]))
        source=source.replace(before,after,1)
    return source

if __name__=="__main__":
    try:
        if len(sys.argv)!=2: raise ValueError("usage: apply_trace.py hydrated-root")
        p=Path(sys.argv[1])/"execution_199/gk1_native_shell.c"
        old=p.read_text(encoding="utf-8")
        new=apply(old)
        p.write_text(new,encoding="utf-8",newline="")
        print("EXEC0231 opt-in WinMain stage tracing applied (no game-state changes)")
    except (OSError,ValueError) as exc:
        print("EXEC0231 TRACE REFUSED: "+str(exc),file=sys.stderr)
        raise SystemExit(1)
