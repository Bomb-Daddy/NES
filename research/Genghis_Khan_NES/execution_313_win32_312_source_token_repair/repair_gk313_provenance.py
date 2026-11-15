#!/usr/bin/env python3
"""EXEC0313: advance inherited 0265 hostile SHA to actual 0312 app afterimage.
Never disable/skip the old audit or substitute a synthetic first Spring.
"""
from pathlib import Path
import hashlib
import os
import sys
import tempfile

CHECKED_APP = {
    'gk1_app_state.c': 'a9772cda18258580dc46d47a3b77934d25a5cdc940d88130d42ee5694023abdf',
    'gk1_app_state.h': '077673206942e635dc866a72fec21cab6d4871769ede0f93486bded98e4c9be9',
    'CMakeLists.txt': '972b1d9f4877f51ef6859bacc19859100dc4c3f1f9e5af28bce2f895196d6312',
}
# Exact observed post-EXEC0310 inherited gate SHA from Win32 run 43757065128.
# Gate has been deliberately advanced by prior stages after EXEC0303;
# updating from its real latest postimage preserves the ten hostile controls.
GATE_303_SHA = '7586b22e7ac5ddec09207e3725ce0248b4dea54ebebd729e178613650e85e988'
OLD_APP = '2922886bf38ff441bdf7d629ae47455734363e213992d45ff4dcba68c5ebe71e'
NEW_APP = CHECKED_APP['gk1_app_state.c']
OLD_LINE = "EXPECTED_APP_SHA = '" + OLD_APP + "'"
NEW_LINE = "EXPECTED_APP_SHA = '" + NEW_APP + "'"
REQUIRED = (
    'EXPECTED_TEST_SHA', 'EXPECTED_PREDECESSOR_SHA',
    'GK1_264_AtomicScenarioReinstallClearsSourceAndPortState',
    'GK1_265_CurrentWin32BuildSourceMustMatchEXEC0264',
    'for index,values in enumerate(mutations,1)', 'def audit(',
    'GK1_EXEC0265_AtomicRestart_SourceOnly.patch',
)

def sha(data): return hashlib.sha256(data).hexdigest()

def normalized(data):
    if b'\x00' in data or b'\r\r\n' in data or b'\r' in data.replace(b'\r\n',b''):
        raise ValueError('illegal source encoding or newline')
    return data.replace(b'\r\n',b'\n')

def updated_gate(source, expected_preimage_sha=GATE_303_SHA):
    raw=normalized(source)
    if sha(raw)!=expected_preimage_sha:
        fingerprint = next((line for line in raw.decode('utf-8',errors='replace').splitlines()
                            if line.startswith('EXPECTED_APP_SHA = ')), '<missing>')
        raise ValueError('old hostile provenance gate SHA mismatch: actual=' +
                         sha(raw) + ' expected=' + expected_preimage_sha +
                         ' source_literal=' + fingerprint)
    text=raw.decode('utf-8')
    if text.count(OLD_LINE)!=1 or NEW_LINE in text:
        raise ValueError('old original source SHA literal missing/duplicated')
    for token in REQUIRED:
        if token not in text: raise ValueError('lost inherited hostile source token '+token)
    return text.replace(OLD_LINE,NEW_LINE,1).encode('utf-8')

def run(app_root):
    app=Path(app_root)
    for name,expected in CHECKED_APP.items():
        got=sha(normalized((app/name).read_bytes()))
        if got!=expected: raise ValueError('not exact EXEC0312 production C11 afterimage: '+name+' '+got)
    target=app/'packaging'/'test_gk265_current_win32_source.py'
    before=target.read_bytes()
    after=updated_gate(before)
    fd,temp=tempfile.mkstemp(prefix='.gk313-gate-',dir=target.parent)
    try:
        with os.fdopen(fd,'wb') as f:
            f.write(after);f.flush();os.fsync(f.fileno())
        os.replace(temp,target)
        if target.read_bytes()!=after:raise OSError('source SHA gate readback mismatch')
    except BaseException:
        target.write_bytes(before)
        raise
    finally:
        if os.path.exists(temp):os.unlink(temp)
    print('EXEC0313 inherited 0265 source SHA gate ADVANCED not disabled, all existing hostile mutations retained',sha(after))

if __name__=='__main__':
    if len(sys.argv)!=2:sys.exit('repair_gk313_provenance.py COMPLETE_STAGED_GAME_ROOT')
    try:run(sys.argv[1])
    except (ValueError,OSError,UnicodeError) as e:sys.exit('EXEC0313 FAIL-CLOSED '+str(e))
