#!/usr/bin/env python3
"""EXEC0301: install the exact EXEC0300 C11 source into the real Win32 pipeline.

EXEC0300's original staging erroneously compared the complete CMakeLists.txt
against a Linux-only archive while the actual CI predecessor has preserved
older Win32 CTest registrations. This stage preserves those registrations,
checks exact native C/H pre/post hashes and existing app gates, and changes
only the NMI library link and test registration in the *actual* full game.
"""
from pathlib import Path
import hashlib, os, sys, tempfile

ROOT = Path(__file__).resolve().parent
SOURCE = ROOT.parent / 'execution_300_original_stock_nmi_seed_clock'
# For a self-contained local regression, the same exact source can be supplied
# by environment; hashes are identical to published GitHub source.
if os.environ.get('GK301_SOURCE_DIR'):
    SOURCE = Path(os.environ['GK301_SOURCE_DIR'])
BEFORE = {
 'gk1_app_state.c':'79ed178dec0a345886932bda2f887645c427b4b12e721438d937b7f97dbcf581',
 'gk1_app_state.h':'95df64d698550869749d5f740e4d55e8726e5ae3d7637712e676cb482daa999b',
}
AFTER = {
 'gk1_app_state.c':'c4a6707d70f809d9cb3d3bfaa775cc1757c331c81b985a82ec97e3a17f560fc6',
 'gk1_app_state.h':'dbaf417047418e431fa08e2578a64cd506850e6dca079cb5ef6c6c8ae3d1a29e',
 'gk300_stock_nmi_seed_clock.c':'9ffa5fb6f36975060829a1053c760da75f25a9376b7cc954d2cc08657ad43c98',
 'gk300_stock_nmi_seed_clock.h':'ff8fbaa39b576abe0869ed8f385f8d117c95ef0352619b6d171bcb8bf92438d8',
 'test_gk300_stock_nmi_seed_clock.c':'42a5b8dcd830d9f38086aca225ba065810300458263946d826cb2d8d16271290',
}
C_APPEND = '''\n/* EXEC0300 source-authenticated NMI clock in the same native game.
 * Exact sampled NMI gate history must be provided; this cannot fabricate
 * the original first-C289 preseed or unlock three seasonal orders. */
Gk300Status gk1_app_original_nmi_seed_clock(const Gk1App *app,
 const char *rom,const uint8_t seed[6],const uint8_t *gates,size_t count,
 Gk300NmiResult *out){
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
    app->enhanced_continuation.active)return GK300_STAGE;
 return gk300_authentic_nmi_gate_history(rom,seed,gates,count,out);
}
'''
H_INSERT = '''/* EXEC0300: fixed-bank SOURCE NMI clock, verified ROM and provided gate
 * history; no NMI timing inferred and no game-stage advancement. */
Gk300Status gk1_app_original_nmi_seed_clock(const Gk1App *app,
 const char *original_rom,const uint8_t initial[6],
 const uint8_t *observed_nmi_gate_8a,size_t count,Gk300NmiResult *out);
'''
TEST_APPEND = '''\n# EXEC0300: authenticated stock NMI +2 tick under $008A gate. Read-only
# first-Spring RNG dependency; not source-authentic full gameplay.
add_executable(test_gk300_stock_nmi_seed_clock test_gk300_stock_nmi_seed_clock.c)
target_link_libraries(test_gk300_stock_nmi_seed_clock PRIVATE gk1_app_state)
add_test(NAME GK1_300_OriginalStockNmiGateClockAndRollback
 COMMAND test_gk300_stock_nmi_seed_clock
 ${CMAKE_CURRENT_SOURCE_DIR}/../execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)
'''

def sha(b): return hashlib.sha256(b).hexdigest()
def canon(b):
    clean = b.replace(b'\r\n', b'\n')
    if b'\r' in clean: raise ValueError('lone CR/malformed source')
    return clean

def one(s,old,new,label):
    if s.count(old)!=1 or new in s: raise ValueError('wrong or duplicate '+label)
    return s.replace(old,new,1)

def stage(directory, source_dir=None):
    root=Path(directory)
    src=Path(source_dir or SOURCE)
    if not root.is_dir(): raise ValueError('complete native game directory missing')
    before={}; outputs={}
    for n,h in BEFORE.items():
        before[n]=(root/n).read_bytes(); data=canon(before[n])
        if sha(data)!=h: raise ValueError('wrong native predecessor '+n)
        s=data.decode('utf-8')
        if n.endswith('.c'):
            # Exact source original ends in newline. Append exact reviewed source.
            if 'gk1_app_original_nmi_seed_clock' in s: raise ValueError('duplicate NMI app')
            s += C_APPEND
        else:
            s=one(s,'#include "gk297_stock_reset_seed.h"\n',
                  '#include "gk297_stock_reset_seed.h"\n#include "gk300_stock_nmi_seed_clock.h"\n','NMI header')
            s=one(s,'Gk297Status gk1_app_original_reset_initializer(',
                  H_INSERT+'Gk297Status gk1_app_original_reset_initializer(','NMI API')
        outputs[n]=s.encode()
        if sha(outputs[n])!=AFTER[n]: raise ValueError('source postimage differs from independently reviewed EXEC0300 '+n)
    for n in AFTER:
        if n in BEFORE: continue
        if (root/n).exists(): raise ValueError('duplicate source module '+n)
        b=canon((src/n).read_bytes())
        if sha(b)!=AFTER[n]: raise ValueError('reviewed NMI module changed '+n)
        outputs[n]=b
    before['CMakeLists.txt']=(root/'CMakeLists.txt').read_bytes()
    cm=canon(before['CMakeLists.txt']).decode('utf-8')
    # Essential source, CTest and older acceptance gates must remain intact.
    required=('gk294_seed_writer_attest.c','gk296_stock_seed_origin.c',
              'GK1_294_OriginalRomSourceSeedWriterAndAtomicRefusals',
              'GK1_296_OriginalFourWriterOriginAndUnknownTailRollback',
              'GK1_297_StockResetInitializerAndUnknownC289Preseed')
    for marker in required:
        if marker not in cm: raise ValueError('required linked predecessor or test absent '+marker)
    if 'GK1_300_OriginalStockNmiGateClockAndRollback' in cm: raise ValueError('duplicate NMI CTest')
    cm=one(cm,'gk297_stock_reset_seed.c gk282_stock_rng_trace.c',
              'gk297_stock_reset_seed.c gk300_stock_nmi_seed_clock.c gk282_stock_rng_trace.c',
              'actual linked game library')
    outputs['CMakeLists.txt']=(cm+TEST_APPEND).encode()
    # The real Win32 source-provenance gate is generated by older workflow
    # stages, then updated by EXEC0298 to the exact 0297 app postimage.
    # Without this update, the new C11 source would fail its own CI guard.
    gate_name='packaging/test_gk265_current_win32_source.py'
    gate=root/gate_name
    if gate.exists():
        before[gate_name]=gate.read_bytes()
        text=canon(before[gate_name]).decode('utf-8')
        old="EXPECTED_APP_SHA = '"+BEFORE['gk1_app_state.c']+"'"
        new="EXPECTED_APP_SHA = '"+AFTER['gk1_app_state.c']+"'"
        outputs[gate_name]=one(text,old,new,'real Win32 source SHA gate').encode()
    # All evidence and source validation complete BEFORE the first write.
    touched=[]
    try:
        for n,b in outputs.items():
            target=root/n
            with tempfile.NamedTemporaryFile(prefix='.gk301_',dir=root,delete=False) as out:
                temp=Path(out.name); out.write(b)
            try: os.replace(temp,target)
            finally: temp.unlink(missing_ok=True)
            touched.append(n)
    except BaseException:
        for n in reversed(touched):
            if n in before: (root/n).write_bytes(before[n])
            else: (root/n).unlink(missing_ok=True)
        raise
    for n,h in AFTER.items():
        if sha((root/n).read_bytes())!=h: raise RuntimeError('postwrite source SHA mismatch '+n)
    print('EXEC0301 verified Win32 CMake-variant staging; actual linked native app, prior tests and registrations preserved. No original first Spring claim.')

if __name__=='__main__':
    if len(sys.argv)!=2: sys.exit('Usage: stage301.py /path/to/full/native/execution_199')
    try: stage(sys.argv[1])
    except Exception as e: sys.exit('EXEC0301 REFUSED: '+str(e))
