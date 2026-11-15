#!/usr/bin/env python3
"""Fail-closed negative controls on the actual inherited app source shape."""
from pathlib import Path
import tempfile,shutil,hashlib,sys
import stage312 as m

def fixture(src,root):
    out=root/'app';out.mkdir()
    for name in ('gk1_app_state.c','gk1_app_state.h','CMakeLists.txt'):
        raw=(src/name).read_text()
        if name=='gk1_app_state.c':
            assert raw.count(m.APP_WRAPPER)==1
            raw=raw.replace(m.APP_WRAPPER,'')
        if name=='gk1_app_state.h':
            raw=raw.replace('#include "gk312_stock_event_pc.h"\n','')
            raw=raw.replace(m.HEADER,'')
        if name=='CMakeLists.txt':
            raw=raw.replace('gk304_stock_store_value.c gk312_stock_event_pc.c','gk304_stock_store_value.c')
            raw=raw.replace(m.CMAKE,'')
        (out/name).write_bytes(raw.encode())
    return out

def snapshot(app):return {x.name:x.read_bytes() for x in app.iterdir() if x.is_file()}

def main(src):
    checks=0
    with tempfile.TemporaryDirectory() as d:
        d=Path(d)
        for n in (0,1,2,3,4,5,6):
            f=d/('fixture'+str(n));f.mkdir();app=fixture(src,f)
            before=snapshot(app)
            if n:
                try:m.stage(app,fail_after=n)
                except OSError:pass
                else:raise AssertionError('expected failure '+str(n))
                assert snapshot(app)==before
                checks+=1
            else:
                after=m.stage(app)
                assert len(after)==6 and snapshot(app)!=before
                assert len(list(app.glob('.gk312-*')))==0
                try:m.stage(app)
                except ValueError:pass
                else:raise AssertionError('duplicate accepted')
                checks+=2
        f=d/'broken';f.mkdir();app=fixture(src,f)
        c=app/'CMakeLists.txt';c.write_bytes(c.read_bytes()+b'\r\r\n')
        before=snapshot(app)
        try:m.stage(app)
        except ValueError:pass
        else:raise AssertionError('CRCRLF accepted')
        assert snapshot(app)==before;checks+=1
        f=d/'old';f.mkdir();app=fixture(src,f)
        c=app/'CMakeLists.txt';c.write_text(c.read_text().replace('gk304_stock_store_value.c','gk304_NOT_SOURCE.c'))
        before=snapshot(app)
        try:m.stage(app)
        except ValueError:pass
        else:raise AssertionError('missing original C11 predecessor accepted')
        assert snapshot(app)==before;checks+=1
        f=d/'tamper';f.mkdir();app=fixture(src,f)
        (app/'gk312_stock_event_pc.c').write_text('fake source')
        before=snapshot(app)
        try:m.stage(app)
        except ValueError:pass
        else:raise AssertionError('unexpected new C11 module accepted')
        assert snapshot(app)==before;checks+=1
        f=d/'crlf';f.mkdir();app=fixture(src,f)
        for p in app.iterdir():p.write_bytes(p.read_bytes().replace(b'\n',b'\r\n'))
        out=m.stage(app)
        assert len(out)==6;checks+=1
    print(f'EXEC0312 {checks}/{checks} hostile native-source staging assertions PASS')
if __name__=='__main__':
    if len(sys.argv)!=2:raise SystemExit('test_stage312.py CURRENT_STAGED_EXEC312_APP')
    main(Path(sys.argv[1]))
