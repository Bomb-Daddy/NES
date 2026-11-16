#!/usr/bin/env python3
"""EXEC0318 reproducible, fail-closed verification of real EXEC0317 Win32 CI231 artifact.

This checks an actual GitHub Actions artifact, not a synthetic controller, native
Windows execution, Mesen ROM replay, physical device or full-campaign acceptance.
"""
from __future__ import annotations
import hashlib
import io
import json
from pathlib import Path
import sys
import zipfile

RUN_ID = '43777224990'
GITHUB_SHA = '4f5818dd31546384774db2ff75ca99d2b654aa80'
ARCHIVE_SHA = '8c8fcc44c42ea5a69ca3ea0aff06c048095b623d9df7bfa6835fac7d3c3b548c'
ARCHIVE_SIZE = 579155
INSTALLER_NAME = 'GK1-Native-Research-1.99.0-win64.exe'
INSTALLER_SHA = 'fa4df44aa0c11b326173403492526666e2b86ffe9f405da5b0e4cea78d06fee6'
INSTALLED_EXE_SHA = '538f7f929d451659622316ac67d8b3fe6e770fa8ea4fd09a5c6047e6cc0b62a1'
COUNTRY_CSV_SHA = 'fa3958f156f09d4b4c15bb559c12aa357fc617a9b1f9217505fe07e8800de581'
MANIFEST_NAME = 'GK1_EXEC0263_QA_ONLY_manifest.json'
EXPECTED_FILES = {INSTALLER_NAME, MANIFEST_NAME}
TRUE_FLAGS = ('tested_port_win32_ci', 'native_ctest_passed',
              'winmain_software_controller_passed', 'nsis_installed_executable_passed')
FALSE_FLAGS = ('release_accepted', 'physical_xbox_tested', 'full_campaign_tested')

class Rejected(ValueError):
    """Artifact rejected; never implicitly upgrade QA to a release."""

def sha256(blob: bytes) -> str:
    return hashlib.sha256(blob).hexdigest()

def strict_json(blob: bytes) -> dict:
    def unique_pairs(pairs):
        out = {}
        for k,v in pairs:
            if k in out:
                raise Rejected('duplicate manifest key: '+k)
            out[k] = v
        return out
    try:
        out = json.loads(blob.decode('utf-8'), object_pairs_hook=unique_pairs)
    except (UnicodeError, json.JSONDecodeError) as exc:
        raise Rejected('invalid UTF-8/JSON manifest') from exc
    if type(out) is not dict:
        raise Rejected('manifest not object')
    return out

def validate_archive_bytes(raw: bytes, *, enforce_artifact_pin: bool=True) -> dict:
    if enforce_artifact_pin and (len(raw)!=ARCHIVE_SIZE or sha256(raw)!=ARCHIVE_SHA):
        raise Rejected('actual GitHub CI artifact SHA/byte length mismatch')
    try:
        with zipfile.ZipFile(io.BytesIO(raw)) as z:
            infos = z.infolist()
            names = [x.filename for x in infos]
            if len(names)!=2 or len(set(names))!=2 or set(names)!=EXPECTED_FILES:
                raise Rejected('missing/duplicate/unexpected ZIP member; no private ROM data allowed')
            if any(n.startswith('/') or '..' in n.split('/') or '\\' in n for n in names):
                raise Rejected('unsafe member path')
            if any(x.file_size > 1000000 or x.flag_bits & 0x1 for x in infos):
                raise Rejected('oversized or encrypted ZIP entry')
            if z.testzip() is not None:
                raise Rejected('ZIP member CRC rejected')
            installer=z.read(INSTALLER_NAME)
            manifest_raw=z.read(MANIFEST_NAME)
    except (zipfile.BadZipFile, EOFError, OSError, RuntimeError) as exc:
        raise Rejected('corrupt ZIP data') from exc
    if sha256(installer)!=INSTALLER_SHA:
        raise Rejected('installer bytes do not match real CI manifest')
    manifest=strict_json(manifest_raw)
    expected={
        'schema':'GK1_EXEC0263_QA_ONLY',
        'github_sha':GITHUB_SHA,
        'github_run_id':RUN_ID,
        'installer_name':INSTALLER_NAME,
        'installer_sha256':INSTALLER_SHA,
        'installed_exe_sha256':INSTALLED_EXE_SHA,
        'built_exe_sha256':INSTALLED_EXE_SHA,
        'country_csv_sha256':COUNTRY_CSV_SHA,
    }
    for k,v in expected.items():
        if type(manifest.get(k)) is not str or manifest[k]!=v:
            raise Rejected('incorrect manifest field: '+k)
    if set(manifest)!=set(expected)|set(TRUE_FLAGS)|set(FALSE_FLAGS):
        raise Rejected('manifest unexpected or missing fields')
    for k in TRUE_FLAGS:
        if manifest.get(k) is not True:
            raise Rejected('missing CI acceptance condition: '+k)
    for k in FALSE_FLAGS:
        if manifest.get(k) is not False:
            raise Rejected('unsupported claimed release/hardware/campaign: '+k)
    return {'verification':'PASS', 'label':'TESTED_PORT_WIN32_CI_QA_ONLY',
            'run_id':RUN_ID, 'commit':GITHUB_SHA,
            'zip_size':len(raw), 'zip_sha256':sha256(raw),
            'installer_size':len(installer), 'installer_sha256':sha256(installer),
            'manifest_sha256':sha256(manifest_raw),
            'ctest_count_from_actual_separate_job_log':231,
            'release_accepted':False, 'physical_xbox_tested':False,
            'full_campaign_tested':False}

def main(argv):
    if len(argv)!=2:
        print('Usage: verify_ci231.py ACTUAL_ACTIONS_ARTIFACT.zip', file=sys.stderr)
        return 2
    try:
        evidence=validate_archive_bytes(Path(argv[1]).read_bytes())
    except (OSError, Rejected) as exc:
        print('EXEC0318 QA-ONLY ARTIFACT REJECT: '+str(exc),file=sys.stderr)
        return 1
    print(json.dumps(evidence, indent=2,sort_keys=True))
    return 0

if __name__=='__main__':
    raise SystemExit(main(sys.argv))
