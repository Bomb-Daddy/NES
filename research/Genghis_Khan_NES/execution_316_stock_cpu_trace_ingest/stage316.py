#!/usr/bin/env python3
"""EXEC0316 exact-source additive integration into latest inherited C11 Gk1App.
Never unlock Spring from claimed CPU events or publish original NES data.
"""
import hashlib
import os
from pathlib import Path
import sys
import tempfile

SOURCE_DIR=Path(__file__).resolve().parent
SOURCE_SHA256={
 'gk316_cpu_claims_ingest.c':'15d093cb07ecbd18ec95ff6c4f58f1fe46fece19259e45dad9c693623a8cfe3b',
 'gk316_cpu_claims_ingest.h':'104217c7f3d11dc7992766996b24f48150553ad47dfc15b3377bf01e8f0f4d8e',
 'test_gk316_cpu_claims.c':'e91a23234e1ff69d0992c85024f5b6f202d611363b5b0a022c0ecfb99e06cbc5',
 'test_gk316_controller_fuzz.c':'182ae7d066301217ab9bc71a9b4231016a965d827f72bbded17c27e20063e8c2',
}
WRAPPER='''
/* EXEC0316: ingest externally claimed original CPU trace as a source-research
 * dependency inside the existing game. Never authorize player-ready Spring. */
Gk316Status gk1_app_original_cpu_claims_parse(const Gk1App *app,
 const char *private_trace,Gk316Trace *out){
 if(!app||app->rules_profile!=0u||app->active_world>1u||
    !app->world_ready[app->active_world]||
    app->worlds[app->active_world].phase!=GK1_PRE_SETUP_TEMPLATE||
    app->setup_stage[app->active_world]!=GK53_SETUP_AWAIT_SOURCE)
  return GK316_STAGE;
 return gk316_load_cpu_claims(private_trace,out);
}
'''
PROTO='''/* EXEC0316: source-research-only claimed emulator CPU events. Not gameplay. */
Gk316Status gk1_app_original_cpu_claims_parse(const Gk1App *app,
 const char *private_trace,Gk316Trace *out);
'''
CMAKE='''
# EXEC0316: new source-research-only reader linked into SAME native game.
add_executable(test_gk316_cpu_claims test_gk316_cpu_claims.c)
target_link_libraries(test_gk316_cpu_claims PRIVATE gk1_app_state)
add_test(NAME GK1_316_CpuTraceClaimsAtomicAndClassicSetupGate
 COMMAND test_gk316_cpu_claims
 ${CMAKE_CURRENT_SOURCE_DIR}/../execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)
add_executable(test_gk316_controller_fuzz test_gk316_controller_fuzz.c)
target_link_libraries(test_gk316_controller_fuzz PRIVATE gk1_app_state)
add_test(NAME GK1_316_ControllerStatefulHostileNoFalseSpring
 COMMAND test_gk316_controller_fuzz
 ${CMAKE_CURRENT_SOURCE_DIR}/../execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)
'''

def source_sha(x):return hashlib.sha256(x).hexdigest()
def norm(data):
    if b'\x00' in data or b'\r' in data.replace(b'\r\n',b''):
        raise ValueError('noncanonical inherited source')
    return data.replace(b'\r\n',b'\n')
def replace_once(s,old,new):
    if s.count(old)!=1 or new in s:
        raise ValueError('missing/duplicate exact C11 integration anchor '+old[:64])
    return s.replace(old,new,1)
def prepare(root):
    root=Path(root)
    old={n:norm((root/n).read_bytes()) for n in ('gk1_app_state.c','gk1_app_state.h','CMakeLists.txt')}
    added={}
    for n,expected in SOURCE_SHA256.items():
        data=(SOURCE_DIR/n).read_bytes()
        if source_sha(data)!=expected:raise ValueError('tested source SHA mismatch: '+n)
        if (root/n).exists():raise ValueError('refusing duplicate source: '+n)
        added[n]=data
    c=old['gk1_app_state.c'].decode('utf-8')
    h=old['gk1_app_state.h'].decode('utf-8')
    cm=old['CMakeLists.txt'].decode('utf-8')
    if 'gk316_cpu_claims_ingest' in c+h+cm or 'gk1_app_original_cpu_claims_parse' in c+h:
        raise ValueError('EXEC0316 was already applied')
    if ('GK1_PRE_SETUP_TEMPLATE' not in c or 'GK53_SETUP_AWAIT_SOURCE' not in c or
       'gk1_app_install_templates' not in c):
        raise ValueError('complete original native game source missing')
    if 'GK1_257_CurrentWin32Workflow_RejectsStaleSourceAndFalsePass' not in cm:
        raise ValueError('inherited Win32 code/test registry unexpected')
    h=replace_once(h,'#include "gk300_stock_nmi_seed_clock.h"',
        '#include "gk300_stock_nmi_seed_clock.h"\n#include "gk316_cpu_claims_ingest.h"')
    tail=h.rfind('#endif')
    if tail<0 or h[tail:].strip()!='#endif':
        raise ValueError('invalid inherited app header guard')
    h=h[:tail]+PROTO+h[tail:]
    c+='\n'+WRAPPER
    start=cm.find('add_library(gk1_app_state STATIC ')
    end=cm.find(')',start)
    if start<0 or end<0:raise ValueError('inherited app CMake library missing')
    lib=cm[start:end]
    lib=replace_once(lib,'gk300_stock_nmi_seed_clock.c',
        'gk300_stock_nmi_seed_clock.c gk316_cpu_claims_ingest.c')
    cm=cm[:start]+lib+cm[end:]
    cm+='\n'+CMAKE
    return {'gk1_app_state.c':c.encode(),'gk1_app_state.h':h.encode(),
            'CMakeLists.txt':cm.encode(),**added}
def stage(root):
    root=Path(root);changes=prepare(root)
    saved={n:(root/n).read_bytes() if (root/n).exists() else None for n in changes}
    committed=[]
    try:
        for name,data in changes.items():
            fd,tmp=tempfile.mkstemp(dir=root,prefix='.gk316-')
            try:
                with os.fdopen(fd,'wb') as f:
                    f.write(data);f.flush();os.fsync(f.fileno())
                os.replace(tmp,root/name)
                committed.append(name)
            finally:
                if os.path.exists(tmp):os.unlink(tmp)
    except Exception:
        for name in reversed(committed):
            if saved[name] is None:(root/name).unlink(missing_ok=True)
            else:(root/name).write_bytes(saved[name])
        raise
    return {n:source_sha(data) for n,data in changes.items()}
if __name__=='__main__':
    if len(sys.argv)!=2:sys.exit('Usage: stage316.py PATH_TO_CURRENT_EXECUTION_199')
    try:
        for name,digest in stage(sys.argv[1]).items():print('EXEC0316 STAGED',name,digest)
    except (ValueError,OSError) as e:sys.exit('EXEC0316 FAIL CLOSED: '+str(e))