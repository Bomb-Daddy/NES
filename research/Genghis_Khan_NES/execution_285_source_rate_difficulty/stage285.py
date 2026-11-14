#!/usr/bin/env python3
"""EXEC0285 source-exact Windows integration after EXEC0284: native C11, no ROM.

Only the C289 analysis, native setup-scoped entry and ONE actual CTest change.
Never imports or copies the private NES ROM, Mesen RAM or checkpoints.
"""
import hashlib
from pathlib import Path
import sys

HERE = Path(__file__).resolve().parent
SOURCES = ("gk282_stock_rng_trace.c", "gk282_stock_rng_trace.h")
APP = "gk1_app_state.c"
TEST = "test_gk285_source_difficulty.c"
OLD_SHA = {
    "gk282_stock_rng_trace.c": "2de1d86d4a357da7e8a1711ab953eba3ad344ff9e3fce461ef4a2d30018e27d8",
    "gk282_stock_rng_trace.h": "6a3be153b608f26782e7c8c682f123429785ce0f3d9c30403ab612053d5091ab",
    "gk1_app_state.c": "4f9319989b5f82b99d7b3211c6fb5def892f4c9db58ad69d3ca29a754532a28f",
}
NEW_SHA = {
    "gk282_stock_rng_trace.c": "d928f2180314738124aa833d2d0810a0d4d0db8bf7907a02d38384e5c6e69999",
    "gk282_stock_rng_trace.h": "6db8bb42f5630cabd506e2033b5d1892da538f8099f8074188c03aa62254f3d3",
    "gk1_app_state.c": "9ef37baf8713186622ece40472bd176078d4fec245cd8884c0cd9a06fecc1c97",
    "test_gk285_source_difficulty.c": "d40b8039547a0fd011e7a96020436e4892ab0ffbad6def5191319bbcada91862",
}
CMAKE_TEXT = '''
# EXEC0285 original C289 choices classified by the native setup difficulty.
# The test reads GK1_PRIVATE_ORIGINAL_ROM at RUNTIME, not configure-time.
add_executable(test_gk285_source_difficulty test_gk285_source_difficulty.c)
target_link_libraries(test_gk285_source_difficulty PRIVATE gk1_app_state)
add_test(NAME GK1_285_SelectedDifficulty_OriginalC289_CausalAndAtomic
 COMMAND test_gk285_source_difficulty
 "${CMAKE_CURRENT_BINARY_DIR}/gk285_source_diff.tmp")
'''
APP_OLD = '''   a->worlds[a->active_world].phase!=GK1_PRE_SETUP_TEMPLATE||
   a->stock_checkpoint.valid)return GK282_ARGUMENT;
 return gk282_check_c289_trace(rom,trace,out);'''
APP_NEW = '''   a->worlds[a->active_world].phase!=GK1_PRE_SETUP_TEMPLATE||
   a->setup_difficulty[a->active_world]<1u||
   a->setup_difficulty[a->active_world]>5u||
   a->stock_checkpoint.valid)return GK282_ARGUMENT;
 /* Carry actual controller setup difficulty into the ORIGINAL C289 source
  * comparison. This filters analytical possibilities only; it does not
  * fabricate player-ready Spring or associate past Mesen RNG history with
  * this user's new controller setup. */
 Gk282Report staged={0};
 Gk282Status r=gk282_check_c289_trace_for_difficulty(rom,trace,
   a->setup_difficulty[a->active_world],&staged);
 if(r!=GK282_OK)return r;
 /* The only source-attested fresh WorkRAM trace is Mongol; prevent it from
  * being relabeled as World merely because both store 83-byte countries. */
 if(a->active_world==1u&&staged.scenario==1u)return GK282_ARGUMENT;
 *out=staged;
 return GK282_OK;'''

def sha(b):
    return hashlib.sha256(b).hexdigest()

def stage(app):
    app = Path(app)
    cm = app / "CMakeLists.txt"
    gate = app / "packaging" / "test_gk265_current_win32_source.py"
    # Read/validate before any write, keeping a failed stage atomic.
    pre = {f: (app/f).read_bytes().replace(b'\r\n', b'\n') for f in (*SOURCES, APP)}
    for f, contents in pre.items():
        if sha(contents) != OLD_SHA[f]:
            raise ValueError(f"EXEC0285 stale inherited source refused: {f}")
    out = {f: (HERE/f).read_bytes().replace(b'\r\n', b'\n') for f in (*SOURCES,TEST)}
    before = pre[APP].decode('utf8')
    if before.count(APP_OLD) != 1:
        raise ValueError("EXEC0285 source-history setup entry anchor absent/duplicate")
    out[APP] = before.replace(APP_OLD, APP_NEW).encode('utf8')
    if any(sha(out[f]) != NEW_SHA[f] for f in (*SOURCES,APP,TEST)):
        raise ValueError("EXEC0285 source postimage hash mismatch")
    if (app/TEST).exists():
        raise ValueError("EXEC0285 attempted duplicate test integration")
    text = cm.read_bytes().replace(b'\r\n',b'\n').decode('utf8')
    if ('GK1_284_Stock_C289_RateHistory_' not in text or
        'GK1_285_SelectedDifficulty_' in text or
        text.count('add_executable(test_gk282_stock_rng_trace') != 1):
        raise ValueError("EXEC0285 CMake source and previous C289 history registry missing")
    gate_text = gate.read_bytes().replace(b'\r\n',b'\n').decode('utf8')
    old_pin = "EXPECTED_APP_SHA = '"+OLD_SHA[APP]+"'"
    new_pin = "EXPECTED_APP_SHA = '"+sha(out[APP])+"'"
    if gate_text.count(old_pin) != 1 or gate_text.count(new_pin) != 0:
        raise ValueError("EXEC0285 legacy Win32 hostile gate cannot be reconciled")
    new_cmake = text + CMAKE_TEXT
    # Commit after every immutable preflight succeeds.
    for f in (*SOURCES, APP):
        (app/f).write_bytes(out[f])
    (app/TEST).write_bytes(out[TEST])
    cm.write_text(new_cmake,encoding='utf8',newline='\n')
    gate.write_text(gate_text.replace(old_pin,new_pin),encoding='utf8',newline='\n')
    for f in (*SOURCES, APP, TEST):
        if sha((app/f).read_bytes()) != sha(out[f]):
            raise ValueError("EXEC0285 after-write source integrity mismatch "+f)
    print("EXEC0285 ORIGINAL C289 SELECTED DIFFICULTY STAGED; no Spring generation; ROM private")

if __name__ == "__main__":
    try:
        if len(sys.argv)!=2:
            raise ValueError("Usage: stage285.py <native execution_199 source directory>")
        stage(sys.argv[1])
    except (ValueError, OSError) as err:
        print("EXEC0285 fail-closed stage: "+str(err), file=sys.stderr)
        sys.exit(2)
