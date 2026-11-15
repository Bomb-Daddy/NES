#!/usr/bin/env python3
"""EXEC0295: SHA-pinned integration of EXEC0294's public C11 source-attestation
into the actual inherited Win32 game after EXEC0289. Private ROM never staged.
All source inputs are checked before writing; failures leave the tree untouched.
"""
import hashlib
from pathlib import Path
import sys

HERE = Path(__file__).resolve().parent
UPSTREAM = HERE.parent / 'execution_294_stock_seed_opcode_attestation'
OLD = {'gk1_app_state.c':'1ece1ea80db941450d3f67a9395286f715c84132cc4728cf5141a6ac8f745ff9',
       'gk1_app_state.h':'2a92dc2676679bda96f596d633cfdcb6db586717a35433751f86ffb8d7349f4d'}
NEW = {'gk1_app_state.c':'f3df3cc44c23555b5670d17d66423a7578e3fb40d0fc226a1614fe76950f246c',
       'gk1_app_state.h':'c2b7910184ffd2b7dc74e6236770fbac70313ee1b42874bcf2d5f094ef36952f'}
ADDED = {'gk294_seed_writer_attest.c':'905e8cf5c9b4867a3995b4cd3655c61ce95ecbd5e9f41d4baca14bd39d6eed71',
         'gk294_seed_writer_attest.h':'33d5ccad8f5314a344f5a64a68fb59f12e70c38195b32a3c0b6684d3d781c479',
         'test_gk294_seed_writer_attest.c':'e311aca65454994816dc23ca3d00bdc3d6f0303412921d1a6ae6efd5faec0382'}
TEST_NAME='GK1_294_OriginalRomSourceSeedWriterAndAtomicRefusals'
LIB_OLD='gk288_original_rng_stream.c gk282_stock_rng_trace.c'
LIB_NEW='gk288_original_rng_stream.c gk294_seed_writer_attest.c gk282_stock_rng_trace.c'
HEADER_OLD='#include "gk288_original_rng_stream.h"\n'
HEADER_NEW=HEADER_OLD+'#include "gk294_seed_writer_attest.h"\n'
PROTO_OLD='Gk288Status gk1_app_original_rng_stream(const Gk1App *app,\n'
PROTO_NEW='''/* EXEC0294: authentic-ROM STORE-byte attestation at pristine Classic setup.
 * This is SOURCE_STATIC candidate evidence, not an executed Mesen trace.
 * Never alters app or awards Spring orders. */
Gk294Status gk1_app_attest_seed_writer_source(const Gk1App *app,
 const char *private_rom,const Gk294Write *claimed,size_t count,
 Gk294SourceProof *out);
'''+PROTO_OLD
APP_APPEND='''
/* EXEC0294 — authenticated raw STORE source candidates, stage-gated using
 * the SAME non-playing boundary as EXEC0288. Separate from real Mesen CPU
 * execution, Spring economy initialization, and controller turn admission. */
Gk294Status gk1_app_attest_seed_writer_source(const Gk1App *app,
 const char *private_rom,const Gk294Write *claimed,size_t count,
 Gk294SourceProof *out){
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
    app->enhanced_continuation.active)return GK294_STAGE;
 return gk294_attest_seed_writers(private_rom,claimed,count,out);
}
'''
CTEST='''
# EXEC0295 Windows: original stock source candidate verification only.
# ROM remains private and optional; CI tests fail-atomic rejection without ROM.
add_executable(test_gk294_seed_writer_attest test_gk294_seed_writer_attest.c)
target_link_libraries(test_gk294_seed_writer_attest PRIVATE gk1_app_state)
add_test(NAME GK1_294_OriginalRomSourceSeedWriterAndAtomicRefusals
 COMMAND test_gk294_seed_writer_attest
 ${CMAKE_CURRENT_SOURCE_DIR}/../execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)
'''

def digest(b): return hashlib.sha256(b).hexdigest()

def replace_once(text, before, after, name):
    if text.count(before)!=1 or after in text:
        raise ValueError('Refused ambiguous or duplicate integration: '+name)
    return text.replace(before,after,1)

def stage(app_dir):
    app=Path(app_dir)
    if not app.is_dir(): raise ValueError('Complete inherited C11 app missing')
    planned={}
    for name,expected in OLD.items():
        data=(app/name).read_bytes()
        if digest(data)!=expected: raise ValueError('Exact EXEC0289 Win32 source preimage differs: '+name)
        if b'\r\n' in data: raise ValueError('Unreviewed CRLF app source: '+name)
        text=data.decode('utf-8')
        if name.endswith('.h'):
            text=replace_once(text,HEADER_OLD,HEADER_NEW,name+' includes')
            text=replace_once(text,PROTO_OLD,PROTO_NEW,name+' prototype')
        else: text+=APP_APPEND
        result=text.encode('utf-8')
        if digest(result)!=NEW[name]: raise ValueError('Source postimage authentication failed: '+name)
        planned[name]=result
    for name,expected in ADDED.items():
        if (app/name).exists(): raise ValueError('Duplicate source refused: '+name)
        data=(UPSTREAM/name).read_bytes()
        if digest(data)!=expected: raise ValueError('Public EXEC0294 source mismatch: '+name)
        planned[name]=data
    cm_path=app/'CMakeLists.txt'
    cm=cm_path.read_text('utf-8')
    if cm.count(TEST_NAME) or 'gk294_seed_writer_attest.c' in cm:
        raise ValueError('Duplicate CMake 294 integration refused')
    if cm.count('GK1_288_SourceOriginalC289MultiDrawNativeSetupAndRollback')!=1:
        raise ValueError('Required EXEC0289 native predecessor test absent')
    cm=replace_once(cm,LIB_OLD,LIB_NEW,'CMake native library')+CTEST
    planned['CMakeLists.txt']=cm.encode('utf-8')
    gate_path=app/'packaging/test_gk265_current_win32_source.py'
    gate=gate_path.read_text('utf-8')
    prior="EXPECTED_APP_SHA = '"+OLD['gk1_app_state.c']+"'"
    after="EXPECTED_APP_SHA = '"+NEW['gk1_app_state.c']+"'"
    gate=replace_once(gate,prior,after,'actual Win32 source-authentication gate')
    planned['packaging/test_gk265_current_win32_source.py']=gate.encode('utf-8')
    # Every input must pass before the first write. A failed source check
    # cannot partially alter the inherited game or its source gate.
    for name,data in planned.items():
        p=app/name;p.write_bytes(data)
    for name,expected in {**NEW,**ADDED}.items():
        if digest((app/name).read_bytes())!=expected:
            raise ValueError('Unexpected disk mutation after source staging: '+name)
    print('EXEC0295 VERIFIED_STATIC: same native C11 Win32 game; authenticated EXEC0294 module and CTest staged; NO ROM OR FIRST SPRING')

if __name__=='__main__':
    try:
        if len(sys.argv)!=2: raise ValueError('Usage: stage295.py <current-gk1/execution_199>')
        stage(sys.argv[1])
    except (OSError,UnicodeError,ValueError) as exc:
        print('EXEC0295 FAIL CLOSED:',exc,file=sys.stderr)
        sys.exit(2)