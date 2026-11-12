#!/usr/bin/env python3
"""EXEC0221: exact-emulator, private-only, fail-closed original-stock Mesen capture.

This launches the existing EXEC0218 Lua write probe in an isolated MesenCE
configuration, then requires a genuine process exit 0 AND the C11 native
validator. No trace is called 'observed' on a parser pass alone. It never
copies the ROM, emulator, state, screenshots, or trace into public outputs.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
from typing import Callable

STOCK_SHA = 'ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3'
MESEN_SHA = 'e777af28bc6cc49d2e61ca0c1a82fbccf13811355332a53ea00749fc37a92761'
CORE_SHA = '0668f6c09bc89a1c8199df6cbf15aeebba243169b30f8116df97127dbfd3d7af'
TRACE_NAME = 'EXEC0221_PRIVATE_FRESH_ORIGINAL_WORKRAM.csv'
RECEIPT_NAME = 'EXEC0221_PRIVATE_SOURCE_CAPTURE_RECEIPT.json'

class CaptureError(Exception):
    pass

def digest(path: Path) -> str:
    h = hashlib.sha256()
    with path.open('rb') as fh:
        for part in iter(lambda: fh.read(131072), b''):
            h.update(part)
    return h.hexdigest()

def authenticated(path: Path, expected: str, description: str) -> None:
    if not path.is_file() or not path.stat().st_size:
        raise CaptureError(description + ' missing/empty')
    got = digest(path)
    if got != expected:
        raise CaptureError(description + ' SHA-256 mismatch (refuses other build/source)')

def private_directory(path: Path) -> None:
    # Never automatically publish this directory. In particular, disallow
    # a symlink to a public checkout or an existing world-readable folder.
    if path.is_symlink():
        raise CaptureError('private output cannot be a symlink')
    path.mkdir(mode=0o700, parents=True, exist_ok=True)
    if not path.is_dir():
        raise CaptureError('private output is not a directory')
    path.chmod(0o700)

def write_receipt(path: Path, data: dict) -> None:
    tmp = path.with_suffix('.tmp')
    with tmp.open('x', encoding='utf-8') as fh:
        json.dump(data, fh, indent=2, sort_keys=True)
        fh.write('\n')
    tmp.chmod(0o600)
    tmp.replace(path)

def capture(rom: Path, mesen: Path, core: Path, lua: Path, native: Path,
            private: Path, timeout: int = 120,
            executor: Callable = subprocess.run) -> dict:
    # Authoritative identity is deliberately not overrideable by flags.
    authenticated(rom, STOCK_SHA, 'original stock USA ROM')
    authenticated(mesen, MESEN_SHA, 'matched MesenCE executable')
    authenticated(core, CORE_SHA, 'matched MesenCore.so')
    if not (lua.is_file() and native.is_file() and os.access(mesen, os.X_OK)
            and os.access(native, os.X_OK)):
        raise CaptureError('Mesen Lua or native validation executable unavailable/not executable')
    if shutil.which('xvfb-run') is None:
        raise CaptureError('xvfb-run not installed; no actual Mesen runtime started')
    if timeout < 15 or timeout > 900:
        raise CaptureError('runtime timeout outside safe bound')
    private_directory(private)
    trace = private / TRACE_NAME
    receipt = private / RECEIPT_NAME
    # Do not accidentally certify stale results from a previous execution.
    if trace.exists() or receipt.exists():
        raise CaptureError('private trace/receipt already exists: use a NEW empty run directory')
    with tempfile.TemporaryDirectory(prefix='gk221_mesen_home_', dir=private) as home_raw:
        home = Path(home_raw)
        config = home / '.config' / 'MesenCE'
        config.mkdir(parents=True)
        (config / 'MesenCore.so').write_bytes(core.read_bytes())
        (config / 'settings.json').write_text(json.dumps({
            'Debug': {'ScriptWindow': {'AllowIoOsAccess': True, 'AllowNetworkAccess': False,
                                      'ScriptTimeout': timeout}},
            'Nes': {'AutoConfigureInput': False,
                    'Port1': {'Type': 'NesController'},
                    'Port2': {'Type': 'NesController'}, 'RamPowerOnState': 1}
        }))
        env = dict(os.environ)
        env.update(HOME=str(home), DOTNET_ROLL_FORWARD='Major',
                   GK218_TRACE_PRIVATE_PATH=str(trace))
        command = ['xvfb-run', '-a', str(mesen), '--testrunner', str(rom),
                   str(lua), f'--timeout={timeout}']
        try:
            run = executor(command, cwd=str(private), env=env, capture_output=True,
                           text=True, timeout=timeout + 15, check=False)
        except subprocess.TimeoutExpired:
            raise CaptureError('Mesen timed out: trace must NOT be called complete or exit0')
        if run.returncode != 0:
            raise CaptureError('Mesen nonzero exit: ' + str(run.returncode))
        if not trace.is_file() or trace.stat().st_size <= 100:
            raise CaptureError('Mesen exited 0 but no complete write trace exists')
    # The C11 native validator requires ROM identity, 14 stages, correct
    # WorkRAM, six RNG byte writes and actual final three orders.
    try:
        verified = executor([str(native), '--audit-private-stock-spring-write-trace',
                             str(rom), str(trace)], cwd=str(private),
                            capture_output=True, text=True, timeout=45, check=False)
    except subprocess.TimeoutExpired:
        raise CaptureError('native validator timed out; cannot certify trace')
    if verified.returncode != 0 or not verified.stdout.startswith(
            'VERIFIED_STATIC owner-supplied trace schema (NOT ORIGINAL OBSERVATION):'):
        raise CaptureError('native trace validation failed (no receipt written)')
    report = {
        'schema': 'GK221_SOURCE_CAPTURE_V1',
        'status': 'CAPTURE_EXECUTED_AND_NATIVE_TRACE_SCHEMA_PASSED',
        'evidence_class': 'OBSERVED_RUNTIME_STOCK_FRESH_ONLY_IF_RUNTIME_INDEPENDENTLY_VERIFIED',
        'limitations': [
          'Trace schema can be forged; SHA of input ROM is not cryptographic proof of event origin',
          'No claim of original NES game formula, source parity or independent playable Spring',
          'This run does not verify archived SaveRAM profile, Win32 or physical Xbox'
        ],
        'original_stock_rom_sha256': STOCK_SHA,
        'mesen_binary_sha256': MESEN_SHA,
        'core_sha256': CORE_SHA,
        'lua_sha256': digest(lua),
        'private_trace_sha256': digest(trace),
        'private_trace_bytes': trace.stat().st_size,
        'native_validator_stdout': verified.stdout.strip()[:1200],
        'mesen_exit': 0,
        'native_validator_exit': 0,
        'file_mode': 'owner-private',
    }
    trace.chmod(0o600)
    write_receipt(receipt, report)
    return report

def main(argv=None) -> int:
    p=argparse.ArgumentParser(description=__doc__)
    for name in ['rom','mesen','core','lua','native','private']:
        p.add_argument('--'+name, type=Path, required=True)
    p.add_argument('--timeout', type=int, default=120)
    args=p.parse_args(argv)
    try:
        # Paths resolved without following private destination symlink at
        # creation time; no executable or original ROM is written publicly.
        result=capture(args.rom.resolve(),args.mesen.resolve(),
              args.core.resolve(),args.lua.resolve(),args.native.resolve(),
              args.private, args.timeout)
    except (CaptureError,OSError) as e:
        print('EXEC0221 NOT VERIFIED:',str(e), file=sys.stderr)
        return 2
    print('EXEC0221 PRIVATE CAPTURE/SCHEMA PASS; not first Spring parity')
    print('private trace sha256',result['private_trace_sha256'])
    return 0

if __name__=='__main__':
    raise SystemExit(main())
