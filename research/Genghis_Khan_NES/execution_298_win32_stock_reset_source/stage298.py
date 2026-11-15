#!/usr/bin/env python3
"""GK1 EXEC0298: fail-closed staging of EXEC0296 and EXEC0297 into the
existing native Win32 game after the EXEC0295 authentication stage.
This is the same application; NO ROM, gameplay checkpoint or player Spring
is distributed or fabricated. Refuses inherited-state ambiguity.
"""
from pathlib import Path
import hashlib, os, sys, tempfile
ROOT = Path(__file__).resolve().parent
SOURCE_296=ROOT.parent/'execution_296_stock_four_writer'
SOURCE_297=ROOT.parent/'execution_297_stock_reset_initializer'
OLD_C_HASH = 'f3df3cc44c23555b5670d17d66423a7578e3fb40d0fc226a1614fe76950f246c'
OLD_H_HASH = 'c2b7910184ffd2b7dc74e6236770fbac70313ee1b42874bcf2d5f094ef36952f'
NEW_C_HASH = '79ed178dec0a345886932bda2f887645c427b4b12e721438d937b7f97dbcf581'
NEW_H_HASH = '95df64d698550869749d5f740e4d55e8726e5ae3d7637712e676cb482daa999b'
MODULES = {'gk296_stock_seed_origin.c': 'd8dabe4dd08379d96b59bde1fa9828922ff4ac9a797e73cbe33edf12ba5cfcff', 'gk296_stock_seed_origin.h': 'cd73eaed97f0f3934091d5a1a1cceece767ae11f7ad83b5ef67035d08f9304cb', 'test_gk296_stock_four_writer.c': '8560631c5f9ce9e6e5edf1b6985b851d71c4735543831a1180f5e55d42a5d8e3', 'gk297_stock_reset_seed.c': '91acda7830f471d901fd7775e8625e0143ba62ae1975d8d753bdf82496586fdf', 'gk297_stock_reset_seed.h': '85ab7bb67e97aafd5e8e48ca581ff214171daf6a396b6a1fa64411b93c54a8ef', 'test_gk297_stock_reset_seed.c': '13ace18dbdb3a0d7ec2f3edff99c09fb722c34df63a79a4be076c6d15e090df9'}
C_INCLUDE_OLD = '#include "gk294_seed_writer_attest.h"\n'
C_INCLUDE_NEW = '#include "gk294_seed_writer_attest.h"\n#include "gk296_stock_seed_origin.h"\n#include "gk297_stock_reset_seed.h"\n'
PROTO_OLD = 'Gk294Status gk1_app_attest_seed_writer_source(const Gk1App *app,\n'
PROTO_NEW = 'Gk297Status gk1_app_original_reset_initializer(\n const Gk1App *app,const char *private_original_rom,\n const uint8_t external_ram_tail[2],Gk297ResetSeed *out);\nGk296Status gk1_app_source_four_writer_first_draw(\n const Gk1App *app,const char *private_rom,\n const Gk296Witness *writes,size_t count,const uint8_t initial[6],\n const uint8_t before_first_draw[6],const uint8_t after_first_draw[6],\n uint16_t draw15,Gk296Report *out);\n'
APP_APPEND = '\n/* EXEC0296: authentic source-validated four-writer boundary, not an\n * original first Spring unlock or independently authenticated CPU transcript.\n * Preserve full existing Classic stage validation and atomic world state. */\nGk296Status gk1_app_source_four_writer_first_draw(\n const Gk1App *app,const char *private_rom,\n const Gk296Witness *writes,size_t count,const uint8_t initial[6],\n const uint8_t before_first_draw[6],const uint8_t after_first_draw[6],\n uint16_t draw15,Gk296Report *out){\n if(!app||app->active_world>1u||app->rules_profile!=0u||\n    !app->world_ready[app->active_world]||\n    app->worlds[app->active_world].phase!=GK1_PRE_SETUP_TEMPLATE||\n    app->worlds[app->active_world].scenario_id!=(app->active_world?\n       GK1_WORLD_CONQUEST:GK1_MONGOL_CONQUEST)||\n    app->worlds[app->active_world].country_count!=(app->active_world?27u:14u)||\n    memcmp(&app->worlds[app->active_world],\n     &app->finance_source_baseline[app->active_world],\n     sizeof app->worlds[app->active_world])!=0||\n    app->setup_stage[app->active_world]!=GK53_SETUP_AWAIT_SOURCE||\n    app->setup_difficulty[app->active_world]<1u||\n    app->setup_difficulty[app->active_world]>5u||\n    app->stock_checkpoint.valid||app->enhanced_assign.active||\n    app->enhanced_summer.active||app->enhanced_fall.active||\n    app->enhanced_winter.active||app->enhanced_year2.active||\n    app->enhanced_continuation.active)return GK296_STAGE;\n return gk296_check_four_writer_first_draw(private_rom,writes,count,\n      initial,before_first_draw,after_first_draw,draw15,out);\n}\n\n/* EXEC0297: source-known original reset initializer FOUR byte values.\n * Read-only proof; DOES NOT infer the 948-writer C289 chronology or unlock\n * gameplay. Both inherited Classic scenario templates remain immutable. */\nGk297Status gk1_app_original_reset_initializer(\n const Gk1App *app,const char *rom,const uint8_t tail[2],\n Gk297ResetSeed *out){\n if(!app||app->active_world>1u||app->rules_profile!=0u||\n    !app->world_ready[app->active_world]||\n    app->worlds[app->active_world].phase!=GK1_PRE_SETUP_TEMPLATE||\n    app->worlds[app->active_world].scenario_id!=(app->active_world?\n       GK1_WORLD_CONQUEST:GK1_MONGOL_CONQUEST)||\n    app->worlds[app->active_world].country_count!=(app->active_world?27u:14u)||\n    memcmp(&app->worlds[app->active_world],\n      &app->finance_source_baseline[app->active_world],\n      sizeof app->worlds[app->active_world])!=0||\n    app->setup_stage[app->active_world]!=GK53_SETUP_AWAIT_SOURCE||\n    app->setup_difficulty[app->active_world]<1u||\n    app->setup_difficulty[app->active_world]>5u||\n    app->stock_checkpoint.valid||app->enhanced_assign.active||\n    app->enhanced_summer.active||app->enhanced_fall.active||\n    app->enhanced_winter.active||app->enhanced_year2.active||\n    app->enhanced_continuation.active)return GK297_STAGE;\n return gk297_reset_initializer_from_source(rom,tail,out);\n}\n'
CMAKE_LIB_OLD = 'gk294_seed_writer_attest.c gk282_stock_rng_trace.c'
CMAKE_LIB_NEW = 'gk294_seed_writer_attest.c gk296_stock_seed_origin.c gk297_stock_reset_seed.c gk282_stock_rng_trace.c'
CMAKE_APPEND = '\n# EXEC0296: original-stock real-run revealed only four source writes before\n# first C289. Do not counterfeit two unwitnessed power-on source stores.\nadd_executable(test_gk296_stock_four_writer test_gk296_stock_four_writer.c)\ntarget_link_libraries(test_gk296_stock_four_writer PRIVATE gk1_app_state)\nadd_test(NAME GK1_296_OriginalFourWriterOriginAndUnknownTailRollback\n COMMAND test_gk296_stock_four_writer\n ${CMAKE_CURRENT_SOURCE_DIR}/../execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)\n\n# EXEC0297: original reset initializer source proof, not first Spring.\nadd_executable(test_gk297_stock_reset_seed test_gk297_stock_reset_seed.c)\ntarget_link_libraries(test_gk297_stock_reset_seed PRIVATE gk1_app_state)\nadd_test(NAME GK1_297_StockResetInitializerAndUnknownC289Preseed\n COMMAND test_gk297_stock_reset_seed\n ${CMAKE_CURRENT_SOURCE_DIR}/../execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)\n'
NAME_296 = 'GK1_296_OriginalFourWriterOriginAndUnknownTailRollback'
NAME_297 = 'GK1_297_StockResetInitializerAndUnknownC289Preseed'

def sha(b): return hashlib.sha256(b).hexdigest()
def replace_one(s, old, new, label):
    if s.count(old)!=1 or new in s: raise ValueError('Ambiguous/duplicate '+label)
    return s.replace(old,new,1)
def canonical(data, label):
    data=data.replace(b'\r\n',b'\n')
    if b'\r' in data: raise ValueError('Unexpected lone CR: '+label)
    return data

def stage(app_dir):
    app=Path(app_dir)
    if not app.is_dir(): raise ValueError('Inherited complete C11 game missing')
    before={}; after={}
    # Normalize only known CRLF during comparisons, not arbitrary source
    # modifications; do not rewrite or delete existing game state.
    for name,oldhash in [('gk1_app_state.c',OLD_C_HASH),('gk1_app_state.h',OLD_H_HASH)]:
        data=(app/name).read_bytes(); clean=canonical(data,name)
        if sha(clean)!=oldhash: raise ValueError('Wrong EXEC0295 Win32 preimage: '+name)
        before[name]=data
        source=clean.decode('utf-8')
        if name.endswith('.c'): source=source+APP_APPEND
        else:
            source=replace_one(source,C_INCLUDE_OLD,C_INCLUDE_NEW,'header includes')
            source=replace_one(source,PROTO_OLD,PROTO_NEW+PROTO_OLD,'header prototype')
        result=source.encode('utf-8')
        if sha(result)!=(NEW_C_HASH if name.endswith('.c') else NEW_H_HASH):
            raise ValueError('Reviewed EXEC0297 postimage mismatch: '+name)
        after[name]=result
    for name,expected in MODULES.items():
        if (app/name).exists():raise ValueError('Duplicate source: '+name)
        src=(SOURCE_296 if '296' in name else SOURCE_297)/name
        data=canonical(src.read_bytes(),name)
        if sha(data)!=expected:raise ValueError('Source SHA mismatch: '+name)
        after[name]=data
    cm_path=app/'CMakeLists.txt';before['CMakeLists.txt']=cm_path.read_bytes()
    cm=canonical(before['CMakeLists.txt'],'CMakeLists.txt').decode('utf-8')
    if NAME_296 in cm or NAME_297 in cm or 'gk296_stock_seed_origin.c' in cm:
        raise ValueError('Duplicate CTest/source integration')
    if cm.count('GK1_294_OriginalRomSourceSeedWriterAndAtomicRefusals')!=1:
        raise ValueError('Actual inherited Win32 EXEC0295 test missing')
    cm=replace_one(cm,CMAKE_LIB_OLD,CMAKE_LIB_NEW,'linked C11 sources')+CMAKE_APPEND
    after['CMakeLists.txt']=cm.encode('utf-8')
    gate='packaging/test_gk265_current_win32_source.py';gp=app/gate
    before[gate]=gp.read_bytes()
    gated=canonical(before[gate],gate).decode('utf-8')
    gated=replace_one(gated,"EXPECTED_APP_SHA = '"+OLD_C_HASH+"'",
        "EXPECTED_APP_SHA = '"+NEW_C_HASH+"'",'Win32 compiled-source gate')
    after[gate]=gated.encode('utf-8')
    # In all reject cases above, the user's game tree has not been touched.
    # If a disk write fails, roll back altered pre-existing source and remove
    # newly created modules; content is not destroyed on normal validation.
    touched=[]
    try:
        for name,data in after.items():
            dest=app/name
            with tempfile.NamedTemporaryFile(prefix='.gk298_',dir=dest.parent,delete=False) as f:
                temp=Path(f.name);f.write(data)
            try: os.replace(temp,dest)
            finally:
                if temp.exists():temp.unlink()
            touched.append(name)
    except Exception:
        for name in reversed(touched):
            dest=app/name
            if name in before:dest.write_bytes(before[name])
            else:dest.unlink(missing_ok=True)
        raise
    assert sha((app/'gk1_app_state.c').read_bytes())==NEW_C_HASH
    assert sha((app/'gk1_app_state.h').read_bytes())==NEW_H_HASH
    print('GK1 EXEC0298 VERIFIED_STATIC: 296+297 staged into SAME Windows C11 app; NO playable first Spring')
if __name__=='__main__':
    try:
        if len(sys.argv)!=2:raise ValueError('Usage: stage298.py <complete-app>/execution_199')
        stage(sys.argv[1])
    except (ValueError,OSError,UnicodeError) as e:
        print('GK1 EXEC0298 FAIL CLOSED:',e,file=sys.stderr);sys.exit(2)