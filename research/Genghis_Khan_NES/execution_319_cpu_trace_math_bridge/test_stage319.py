#!/usr/bin/env python3
"""Hostile source integration regression: current app source stays inherited."""
import hashlib
import os
from pathlib import Path
import tempfile
from unittest.mock import patch
import stage319

CM='''cmake_minimum_required(VERSION 3.15)
add_library(gk1_app_state STATIC gk300_stock_nmi_seed_clock.c gk316_cpu_claims_ingest.c)
add_test(NAME GK1_317_OriginalCpuClaimsAreWorldBoundWithoutOrders COMMAND existing)
'''
checks=0
def ck(condition):
    global checks
    checks+=1
    if not condition:raise AssertionError('GK319 staging test '+str(checks))
def setup(p,content=CM): (p/'CMakeLists.txt').write_bytes(content.encode())
with tempfile.TemporaryDirectory(prefix='gk319-hostile-') as d:
    base=Path(d)
    ok=base/'ok';ok.mkdir();setup(ok)
    old=(ok/'CMakeLists.txt').read_bytes()
    r=stage319.stage(ok)
    ck(len(r)==4 and set(r)=={'CMakeLists.txt',*stage319.SOURCES})
    ck(r['CMakeLists.txt']==hashlib.sha256((ok/'CMakeLists.txt').read_bytes()).hexdigest())
    ck('GK1_319_SourceCpuNmiStoreC289MathClaimsNoOrders' in (ok/'CMakeLists.txt').read_text())
    ck((ok/'CMakeLists.txt').read_bytes().count(b'gk316_cpu_claims_ingest.c gk319_cpu_trace_math.c')==1)
    saved={n:(ok/n).read_bytes() for n in r}
    try:stage319.stage(ok)
    except ValueError:pass
    else:raise AssertionError('duplicate silently accepted')
    ck(all((ok/n).read_bytes()==v for n,v in saved.items()))
    for name,cm in [('missing','add_library(gk1_app_state STATIC gk316_cpu_claims_ingest.c)\n'),
                    ('oldstage','add_library(gk1_app_state STATIC gk300_stock_nmi_seed_clock.c gk316_cpu_claims_ingest.c)\n'),
                    ('forged',CM.replace('gk316_cpu_claims_ingest.c','gk316_cpu_claims_ingest.c gk316_cpu_claims_ingest.c'))]:
        p=base/name;p.mkdir();setup(p,cm);prev=(p/'CMakeLists.txt').read_bytes()
        try:stage319.stage(p)
        except ValueError:pass
        else:raise AssertionError('inherited source corruption accepted '+name)
        ck((p/'CMakeLists.txt').read_bytes()==prev and not (p/'gk319_cpu_trace_math.c').exists())
    p=base/'rollback';p.mkdir();setup(p);prev=(p/'CMakeLists.txt').read_bytes()
    real_replace=stage319.os.replace;calls=0
    def fail_second(src,dst):
        global calls
        calls+=1
        if calls==2:raise OSError('fault injection')
        return real_replace(src,dst)
    try:
        with patch.object(stage319.os,'replace',side_effect=fail_second):stage319.stage(p)
    except OSError:pass
    else:raise AssertionError('injected IO failure accepted')
    ck((p/'CMakeLists.txt').read_bytes()==prev)
    ck(all(not (p/n).exists() for n in stage319.SOURCES))
    ck(all(not x.name.startswith('.gk319-') for x in p.iterdir()))
print('GK319 PASS',checks,'hostile integration checks; complete native app inherited unchanged')
