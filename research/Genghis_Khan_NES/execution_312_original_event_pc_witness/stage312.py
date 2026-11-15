#!/usr/bin/env python3
"""EXEC0312: fail-closed stage for COMPLETE inherited C11 application.
Never modifies a private NES ROM, gameplay source checkpoint, or human orders.
The prerequisite EXEC0302+0304 source must have been staged first.
"""
import hashlib
import os
import pathlib
import sys
import tempfile
ROOT = pathlib.Path(__file__).resolve().parent

def digest(data): return hashlib.sha256(data).hexdigest()
def norm(data):
    if b'\r\r\n' in data or b'\r' in data.replace(b'\r\n',b''):
        raise ValueError('malformed source line ending')
    return data.replace(b'\r\n',b'\n')
def one(s, old, new):
    if s.count(old) != 1: raise ValueError('unexpected or duplicate exact source anchor: '+old[:80])
    return s.replace(old,new,1)

# Constant generated from this new public C11 source, not any original game ROM.
CHECKED_SOURCE_SHA = {
  'gk312_stock_event_pc.c': '85e802815a91f9c833c023dd02bee608622168c949cd81c329886daf45fe443e',
  'gk312_stock_event_pc.h': '33b362896cac283e3119dc4ce8cf950561e9d582af393db214ca6d18c72f4dd8',
  'test_gk312_stock_event_pc.c': 'e0130f303751ce59ae11914e76c20f0a0bc2b71f7ba5c9b9ddaf9644a60c6b32',
}

APP_WRAPPER = '''
/* EXEC0312: Source-authenticated event PC witness admission in the SAME
 * inherited C11 game. Strict Classic original PRE_SETUP only. Nothing
 * commits first Spring, grants orders, or mutates the country data. */
Gk312Status gk1_app_original_event_pc_witnesses(const Gk1App *app,
 const char *rom,const uint8_t seed[6],
 const Gk302SeedEvent *events,size_t count,
 const Gk304Registers *regs,size_t nr,
 const Gk312PcWitness *pcs,size_t np,Gk312Report *out){
 if(!app||app->active_world>1u||app->rules_profile!=0u||
    !app->world_ready[app->active_world]||
    app->worlds[app->active_world].phase!=GK1_PRE_SETUP_TEMPLATE||
    app->worlds[app->active_world].scenario_id!=(app->active_world?
       GK1_WORLD_CONQUEST:GK1_MONGOL_CONQUEST)||
    app->worlds[app->active_world].country_count!=(app->active_world?27u:14u)||
    memcmp(&app->worlds[app->active_world],
      &app->finance_source_baseline[app->active_world],
      sizeof app->worlds[app->active_world])!=0||
    app->setup_stage[app->active_world]!=GK53_SETUP_AWAIT_SOURCE||
    app->setup_difficulty[app->active_world]<1u||
    app->setup_difficulty[app->active_world]>5u||
    app->stock_checkpoint.valid||app->enhanced_assign.active||
    app->enhanced_summer.active||app->enhanced_fall.active||
    app->enhanced_winter.active||app->enhanced_year2.active||
    app->enhanced_continuation.active)return GK312_STAGE;
 return gk312_verify_original_event_pc(rom,seed,events,count,regs,nr,
                                       pcs,np,out);
}
'''
HEADER = '''/* EXEC0312: immutable Classic pre-setup C11 app-level PC/cycle evidence gate.
 * Consistent claimed source events are NOT observed Mesen runtime or orders. */
Gk312Status gk1_app_original_event_pc_witnesses(const Gk1App *app,
 const char *private_original_rom,const uint8_t initial[6],
 const Gk302SeedEvent *events,size_t count,
 const Gk304Registers *regs,size_t nr,
 const Gk312PcWitness *pcs,size_t np,Gk312Report *out);
'''
CMAKE = '''
# EXEC0312 original-stock CPU-PC strict claim replay; never Spring generator.
add_executable(test_gk312_stock_event_pc test_gk312_stock_event_pc.c)
target_link_libraries(test_gk312_stock_event_pc PRIVATE gk1_app_state)
add_test(NAME GK1_312_OriginalSourceEventPcWitnessProvenance COMMAND test_gk312_stock_event_pc)
'''

def prepare(app):
    app=pathlib.Path(app)
    def text(name): return norm((app/name).read_bytes()).decode('utf-8')
    cm=text('CMakeLists.txt')
    if 'gk302_stock_seed_timeline.c' not in cm or 'gk304_stock_store_value.c' not in cm:
        raise ValueError('EXEC0302+EXEC0304 inherited app prerequisite missing')
    if 'GK1_312_OriginalSourceEventPcWitnessProvenance' in cm:
        raise ValueError('duplicate EXEC0312 staging')
    header=text('gk1_app_state.h')
    source=text('gk1_app_state.c')
    if 'Gk312Status gk1_app_original_event_pc_witnesses' in header+source:
        raise ValueError('duplicate EXEC0312 app ABI')
    if 'Gk300Status gk1_app_original_nmi_seed_clock(' not in header+source:
        raise ValueError('source-attested real Gk1App prerequisite missing')
    # Refuse to stage over an unrelated app entirely, without restamping it.
    if ('gk1_app_original_reset_initializer(' not in source or
        'app->finance_source_baseline[app->active_world]' not in source):
        raise ValueError('missing immutable original pre-setup gates')
    source=source.rstrip('\n')+'\n'+APP_WRAPPER
    header=one(header,'#include "gk300_stock_nmi_seed_clock.h"',
               '#include "gk300_stock_nmi_seed_clock.h"\n#include "gk312_stock_event_pc.h"')
    header=one(header,'Gk297Status gk1_app_original_reset_initializer(',
               HEADER+'Gk297Status gk1_app_original_reset_initializer(')
    cm=one(cm,'gk304_stock_store_value.c',
           'gk304_stock_store_value.c gk312_stock_event_pc.c')
    cm=cm.rstrip('\n')+'\n'+CMAKE
    changed={'gk1_app_state.c':source.encode(),'gk1_app_state.h':header.encode(),
             'CMakeLists.txt':cm.encode()}
    for name,expected in CHECKED_SOURCE_SHA.items():
        raw=norm((ROOT/name).read_bytes())
        if digest(raw)!=expected: raise ValueError('new source SHA mismatch: '+name)
        if (app/name).exists(): raise ValueError('duplicate C11 module: '+name)
        changed[name]=raw
    return changed

def stage(app,fail_after=0):
    app=pathlib.Path(app)
    replacements=prepare(app)
    prior={k:(app/k).read_bytes() if (app/k).exists() else None for k in replacements}
    tmp=[]
    try:
        for k,v in replacements.items():
            fd,name=tempfile.mkstemp(prefix='.gk312-',dir=app)
            tmp.append(name)
            with os.fdopen(fd,'wb') as f:
                f.write(v);f.flush();os.fsync(f.fileno())
        for j,(k,v) in enumerate(replacements.items(),1):
            os.replace(tmp.pop(0),app/k)
            if j==fail_after:raise OSError('injected post-write failure')
        for k,v in replacements.items():
            if (app/k).read_bytes()!=v:raise OSError('staged afterimage mismatch')
    except BaseException:
        for k,v in prior.items():
            if v is None:(app/k).unlink(missing_ok=True)
            else:(app/k).write_bytes(v)
        raise
    finally:
        for p in tmp:
            pathlib.Path(p).unlink(missing_ok=True)
    return {k:digest(v) for k,v in replacements.items()}

if __name__=='__main__':
    if len(sys.argv)!=2:raise SystemExit('stage312.py INHERITED_CURRENT_C11_APP_PATH')
    try:
        for k,sha in stage(sys.argv[1]).items(): print('EXEC0312 staged',k,sha)
    except (ValueError,OSError,UnicodeError) as e:
        print('EXEC0312 REFUSED '+str(e),file=sys.stderr);sys.exit(1)
