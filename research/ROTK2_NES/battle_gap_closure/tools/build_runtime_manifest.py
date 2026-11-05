#!/usr/bin/env python3
"""Audit private original-runtime artifacts into a payload-free public manifest.

This is a read-only artifact scanner, not an emulator runner or a mechanics test.
Exit 0 means the manifest was built; individual failed/in-progress runs remain
explicit in the JSON. Missing final receipts do not prove a process is alive.
"""

from __future__ import annotations

import argparse
import base64
from collections import Counter, defaultdict
from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import re
import sys
import tempfile

from battle_probe import check_png
from run_mesen import CORE_SHA256, MESEN_SHA256, ROM_SHA256


def sha(raw):
    return hashlib.sha256(raw).hexdigest()


def label(path, archive):
    """Keep relocatable archive paths; do not expose external absolute paths."""
    path = Path(path)
    try:
        return path.resolve().relative_to(archive.resolve()).as_posix()
    except ValueError:
        return 'external/' + path.name


def stat_key(st):
    # ctime changes during authorized hard-link deduplication are not byte changes.
    return st.st_dev, st.st_ino, st.st_size, st.st_mtime_ns


class HashCache:
    """Avoid repeatedly reading hard-linked 70 MB executable copies."""

    def __init__(self, archive):
        self.archive = archive
        self.cache = {}

    def metadata(self, path):
        result = {'path': label(path, self.archive)}
        try:
            for _ in range(2):
                before = path.stat()
                key = stat_key(before)
                if key in self.cache:
                    return dict(result, bytes=before.st_size, sha256=self.cache[key], stable=True)
                with path.open('rb') as stream:
                    opened = stat_key(os.fstat(stream.fileno()))
                    digest = hashlib.file_digest(stream, 'sha256').hexdigest()
                    closed = stat_key(os.fstat(stream.fileno()))
                if key == opened == closed == stat_key(path.stat()):
                    self.cache[key] = digest
                    return dict(result, bytes=before.st_size, sha256=digest, stable=True)
            return dict(result, stable=False, error='ChangedDuringScan')
        except OSError as exc:
            return dict(result, stable=False, error=type(exc).__name__)


def read_json(path):
    if not path.is_file():
        return None, None
    try:
        value = json.loads(path.read_text(encoding='utf-8'))
        if not isinstance(value, dict):
            return None, 'ExpectedJSONObject'
        return value, None
    except (OSError, UnicodeError, json.JSONDecodeError) as exc:
        return None, type(exc).__name__


def inspect_log(path, archive, protocol=False):
    result = {'path': label(path, archive)}
    messages = {'started': 0, 'loaded': 0, 'done_frames': [], 'probe_errors': [],
                'intervention_lines': 0, 'intervention_frames': [], 'shot_lines': 0, 'state_lines': 0,
                'memory_lines': 0, 'write_lines': 0, 'bank_lines': 0}
    try:
        before = path.stat()
        digest = hashlib.sha256()
        size = 0
        with path.open('rb') as stream:
            # A growing active log is sampled only to its initial byte length.
            remaining = before.st_size
            while remaining:
                line = stream.readline(remaining)
                if not line:
                    break
                remaining -= len(line)
                size += len(line)
                digest.update(line)
                if not protocol:
                    continue
                prefix = line.split(b' ', 1)[0].strip()
                if prefix == b'PROBE_STARTED':
                    messages['started'] += 1
                elif prefix == b'LOADED':
                    messages['loaded'] += 1
                elif prefix == b'DONE':
                    try:
                        messages['done_frames'].append(int(line.split()[1]))
                    except (ValueError, IndexError):
                        messages['probe_errors'].append('Malformed DONE line')
                elif prefix == b'PROBE_ERROR':
                    messages['probe_errors'].append(line.decode('utf-8', 'replace').strip()[:1000])
                elif prefix == b'INTERVENTION':
                    messages['intervention_lines'] += 1
                    try:
                        frame = int(line.split()[1])
                        if frame not in messages['intervention_frames']:
                            messages['intervention_frames'].append(frame)
                    except (ValueError, IndexError):
                        messages['probe_errors'].append('Malformed INTERVENTION line')
                else:
                    key = {b'SHOT': 'shot_lines', b'STATE': 'state_lines', b'MEM': 'memory_lines',
                           b'WRITE': 'write_lines', b'BANK': 'bank_lines'}.get(prefix)
                    if key:
                        messages[key] += 1
        result.update(bytes=size, sha256=digest.hexdigest(), stable=stat_key(before) == stat_key(path.stat()))
    except OSError as exc:
        result.update(stable=False, error=type(exc).__name__)
    return result, messages


def inspect_run(directory, archive, cache):
    process_path = directory / 'process/receipt.json'
    probe_path = directory / 'probe_receipt.json'
    schedule_path = directory / 'schedule.json'
    process, process_error = read_json(process_path)
    probe, probe_error = read_json(probe_path)
    schedule, schedule_error = read_json(schedule_path)
    native, observed, config = process or {}, probe or {}, schedule or {}
    finalized = process is not None and probe is not None
    issues = []

    def issue(code, artifact=None, detail=None):
        item = {'code': code}
        if artifact is not None:
            item['artifact'] = label(artifact, archive)
        if detail is not None:
            item['detail'] = str(detail).replace(str(archive), archive.name)[:1000]
        issues.append(item)

    receipt_errors = {k: v for k, v in [('process', process_error), ('probe', probe_error),
                                       ('schedule', schedule_error)] if v}
    if finalized and schedule is None:
        issue('SCHEDULE_MISSING_OR_INVALID', schedule_path)
    schedule_meta = cache.metadata(schedule_path)
    expected_schedule = observed.get('schedule_sha256')
    if expected_schedule and schedule_meta.get('sha256') != expected_schedule:
        issue('SCHEDULE_HASH_MISMATCH', schedule_path)
    schedule_meta.update(recorded_sha256=expected_schedule, purpose=config.get('purpose'),
                         stop_frame=config.get('stop'), event_count=len(config.get('events', [])))

    direct = []
    for patch in list(config.get('patches', [])) + list(observed.get('controlled_state_interventions', [])):
        if patch not in direct:
            direct.append(patch)
    if ('controlled_state_interventions' in observed and
            observed['controlled_state_interventions'] != config.get('patches', [])):
        issue('INTERVENTION_METADATA_MISMATCH')

    logs = {}
    completion = {}
    for kind in ('stdout', 'stderr'):
        path = directory / 'process' / (kind + '.log')
        logs[kind], messages = inspect_log(path, archive, protocol=kind == 'stdout')
        if kind == 'stdout':
            completion = messages
        expected = native.get('raw_output', {}).get(kind)
        if expected:
            logs[kind]['recorded_sha256'] = expected.get('sha256')
            if (logs[kind].get('sha256') != expected.get('sha256') or
                    logs[kind].get('bytes') != expected.get('bytes')):
                issue('RAW_LOG_HASH_MISMATCH', path)
    if completion['intervention_lines'] and not direct:
        issue('UNDECLARED_LOGGED_INTERVENTION')

    input_sha = observed.get('input_state_sha256')
    input_verified = False
    script_path = directory / 'probe.lua'
    if script_path.is_file():
        try:
            script = script_path.read_text(encoding='utf-8')
            match = re.search(r"^local loadstate=(nil|'[A-Za-z0-9+/=]*'|\"[A-Za-z0-9+/=]*\")\s*$", script, re.M)
            if match:
                token = match.group(1)
                actual_input = None if token == 'nil' else sha(base64.b64decode(token[1:-1], validate=True))
                input_verified = actual_input == input_sha and probe is not None
                if probe is not None and not input_verified:
                    issue('INPUT_CHECKPOINT_HASH_MISMATCH', script_path)
                input_sha = actual_input
        except (OSError, UnicodeError, ValueError) as exc:
            issue('INPUT_CHECKPOINT_UNREADABLE', script_path, type(exc).__name__)
    if finalized and not input_verified:
        issue('INPUT_CHECKPOINT_UNVERIFIED', script_path)

    counts = {'declared': len(observed.get('products', [])), 'verified': 0,
              'missing_or_mismatched': 0, 'png_checked': 0, 'png_valid': 0,
              'checkpoint_payloads_hashed': 0}
    state_outputs = []
    for product in observed.get('products', []):
        name = product.get('file', '')
        path = directory / name
        try:
            if not name or Path(name).is_absolute() or not path.resolve().is_relative_to(directory.resolve()):
                raise ValueError('Product path leaves run directory')
            metadata = cache.metadata(path)
            verified = metadata.get('sha256') == product.get('sha256') and metadata.get('bytes') == product.get('bytes')
            if verified:
                counts['verified'] += 1
            else:
                counts['missing_or_mismatched'] += 1
                issue('PRODUCT_HASH_OR_SIZE_MISMATCH', path)
            if path.suffix == '.png':
                counts['png_checked'] += 1
                try:
                    check_png(path.read_bytes())
                    counts['png_valid'] += 1
                except (OSError, ValueError) as exc:
                    issue('PNG_INVALID', path, str(exc))
            if path.suffix == '.b64' and path.name.startswith('state_'):
                try:
                    raw = base64.b64decode(path.read_text(encoding='ascii').strip(), validate=True)
                    if not raw.startswith(b'MSS'):
                        raise ValueError('Missing MSS signature')
                    frame_match = re.fullmatch(r'state_(\d+)\.b64', path.name)
                    state_outputs.append({'file': label(path, archive), 'frame': int(frame_match[1]) if frame_match else None,
                                          'decoded_mss_sha256': sha(raw), 'decoded_bytes': len(raw),
                                          'declared_product_verified': verified})
                    counts['checkpoint_payloads_hashed'] += 1
                except (OSError, UnicodeError, ValueError) as exc:
                    issue('CHECKPOINT_PAYLOAD_INVALID', path, type(exc).__name__)
        except (OSError, ValueError, TypeError) as exc:
            counts['missing_or_mismatched'] += 1
            issue('PRODUCT_UNREADABLE_OR_UNSAFE', detail=type(exc).__name__)

    identity = {}
    for role, relative, expected_digest in [
        ('mesen', 'process/portable/Mesen', MESEN_SHA256),
        ('core', 'process/portable/MesenCore.so', CORE_SHA256),
        ('script', 'probe.lua', None),
        ('settings', 'process/portable/settings.json', None),
    ]:
        path = directory / relative
        recorded = native.get(role)
        if recorded:
            actual = cache.metadata(path)
            actual['recorded_sha256'] = recorded.get('sha256')
            actual['matches_recorded'] = actual.get('sha256') == recorded.get('sha256')
            if expected_digest:
                actual['expected_sha256'] = expected_digest
                actual['matches_expected'] = actual.get('sha256') == expected_digest
            identity[role] = actual
            if not actual['matches_recorded'] or (expected_digest and not actual['matches_expected']):
                issue('RETAINED_EXECUTABLE_MISMATCH' if role == 'mesen' else 'RETAINED_' + role.upper() + '_MISMATCH', path)
        elif finalized:
            issue('MISSING_' + role.upper() + '_IDENTITY_RECEIPT', path)
    if process:
        if native.get('rom', {}).get('sha256') != ROM_SHA256 or not native.get('original_rom_unchanged'):
            issue('ROM_IDENTITY_RECEIPT_FAILED')
        if not native.get('runtime_identity_passed'):
            issue('RECORDED_RUNTIME_IDENTITY_FAILED')
        if native.get('mesen_after'):
            if 'mesen' not in identity:
                identity['mesen'] = cache.metadata(directory / 'process/portable/Mesen')
            identity['mesen']['recorded_postrun_sha256'] = native['mesen_after'].get('sha256')
            identity['mesen']['matches_recorded_postrun'] = identity['mesen'].get('sha256') == native['mesen_after'].get('sha256')
            if not identity['mesen']['matches_recorded_postrun'] or not native.get('copied_executable_unchanged'):
                issue('POSTRUN_EXECUTABLE_IDENTITY_FAILED')

    native_failed = process is not None and (native.get('timeout') or native.get('exit_code') != 0)
    if finalized:
        if config.get('stop') not in completion['done_frames']:
            issue('EXPECTED_DONE_MISSING')
        if observed.get('restoration_requested') and completion['loaded'] == 0:
            issue('RESTORE_CONFIRMATION_MISSING')
        if completion['probe_errors']:
            issue('PROBE_ERROR_IN_STDOUT')
    unstable = finalized and any(not logs[k].get('stable') and 'error' not in logs[k] for k in logs)
    if unstable:
        finalized = False
    execution = ('FAILED' if native_failed else 'COMPLETED' if finalized else 'IN_PROGRESS')
    recorded_failures = observed.get('failures', [])
    status = ('IN_PROGRESS' if execution == 'IN_PROGRESS' else
              'FAILED' if native_failed or recorded_failures or issues else 'PASS_VERIFIED')
    command = [label(arg, archive) if isinstance(arg, str) and Path(arg).is_absolute() else arg
               for arg in native.get('command', [])]
    receipts = {}
    for role, path in [('process', process_path), ('probe', probe_path)]:
        if path.exists():
            receipts[role] = cache.metadata(path)
    return {
        'run': directory.name, 'archive_path': label(directory, archive), 'status': status,
        'execution': {'status': execution, 'native_exit': native.get('exit_code'), 'timeout': native.get('timeout'),
                      'elapsed_seconds': native.get('elapsed_seconds'), 'native_command': command,
                      'recorded_runtime_identity_passed': native.get('runtime_identity_passed'),
                      'recorded_probe_evidence': observed.get('evidence'), 'recorded_probe_failures': recorded_failures,
                      'completion_messages': completion,
                      'incomplete_note': 'Final receipts missing/changing; active execution is not independently proven.' if execution == 'IN_PROGRESS' else None},
        'schedule': schedule_meta, 'direct_interventions': direct,
        'has_direct_intervention': bool(direct or completion['intervention_lines']),
        'input_checkpoint_sha256': input_sha, 'recorded_input_checkpoint_sha256': observed.get('input_state_sha256'),
        'input_checkpoint_verified_from_script': input_verified,
        'restoration_requested': observed.get('restoration_requested'),
        'products': counts, 'saved_checkpoints': state_outputs, 'raw_logs': logs,
        'retained_identity': identity, 'receipts': receipts, 'receipt_read_errors': receipt_errors,
        'integrity': {'status': 'IN_PROGRESS' if execution == 'IN_PROGRESS' else 'FAILED' if issues else 'PASS_VERIFIED',
                      'issues': issues},
    }


def add_lineage(rows):
    by_name = {row['run']: row for row in rows}
    states = defaultdict(list)
    for row in rows:
        for state in row['saved_checkpoints']:
            states[state['decoded_mss_sha256']].append(dict(state, run=row['run']))

    def walk(name, visiting):
        if name in visiting:
            return set(), set(), set(), set(), True, True
        row = by_name[name]
        checkpoint = row['input_checkpoint_sha256']
        prepared, roots, unresolved, nonpassing = set(), set(), set(), set()
        cycle = False
        uncertain = not row['input_checkpoint_verified_from_script']
        if not checkpoint:
            roots.add(name)
            return prepared, roots, unresolved, nonpassing, cycle, uncertain
        parents = [p for p in states.get(checkpoint, []) if p['run'] != name]
        if not parents:
            unresolved.add(checkpoint)
            return prepared, roots, unresolved, nonpassing, cycle, True
        uncertain |= len(parents) > 1
        for parent in parents:
            p_row = by_name[parent['run']]
            prior = walk(parent['run'], visiting | {name})
            for target, values in zip((prepared, roots, unresolved, nonpassing), prior[:4]):
                target.update(values)
            cycle |= prior[4]
            uncertain |= prior[5] or not parent['declared_product_verified']
            if p_row['status'] != 'PASS_VERIFIED':
                nonpassing.add(parent['run'])
            # A checkpoint saved before all local patches does not inherit those
            # later patches. Incoming prepared ancestry still propagates above.
            for patch in p_row['direct_interventions']:
                if (not isinstance(patch, list) or not patch or parent['frame'] is None or
                        not isinstance(patch[0], int) or patch[0] <= parent['frame']):
                    prepared.add(parent['run'])
            logged = p_row['execution']['completion_messages']
            if logged['intervention_lines'] and (not logged['intervention_frames'] or parent['frame'] is None or
                    any(frame <= parent['frame'] for frame in logged['intervention_frames'])):
                prepared.add(parent['run'])
        return prepared, roots, unresolved, nonpassing, cycle, uncertain

    for row in rows:
        prepared, roots, unresolved, nonpassing, cycle, uncertain = walk(row['run'], set())
        matches = [p for p in states.get(row['input_checkpoint_sha256'], []) if p['run'] != row['run']]
        classification = ('CONTROLLED_RAM_INTERVENTION' if row['has_direct_intervention'] else
                          'PREPARED_RAM_DESCENDANT' if prepared else
                          'UNKNOWN_ANCESTRY' if uncertain or unresolved or cycle else
                          'NO_DECLARED_INTERVENTION_ANCESTRY')
        row['lineage'] = {
            'classification': classification, 'intervention_ancestors': sorted(prepared),
            'root_runs': sorted(roots), 'unresolved_checkpoint_sha256': sorted(unresolved),
            'nonpassing_ancestors': sorted(nonpassing), 'cycle_detected': cycle,
            'ambiguous_parent_checkpoint': len(matches) > 1,
            'parent_matches': [{'run': p['run'], 'checkpoint': p['file'], 'frame': p['frame'],
                                'declared_product_verified': p['declared_product_verified']} for p in matches],
        }


def build_manifest(runs, canonical_mesen=None):
    runs = Path(runs).expanduser().resolve(strict=True)
    if not runs.is_dir():
        raise ValueError('--runs must be a directory')
    archive = runs.parent
    cache = HashCache(archive)
    canonical = Path(canonical_mesen) if canonical_mesen else archive / 'mesen/Mesen'
    canonical_meta = cache.metadata(canonical)
    canonical_meta['expected_sha256'] = MESEN_SHA256
    canonical_meta['matches_expected'] = canonical_meta.get('sha256') == MESEN_SHA256
    directories = [d for d in sorted(runs.iterdir()) if d.is_dir() and any(
        (d / marker).exists() for marker in ('schedule.json', 'probe.lua', 'probe_receipt.json',
                                            'process/receipt.json', 'process/stdout.log'))]
    rows = [inspect_run(d, archive, cache) for d in directories]
    add_lineage(rows)
    return {
        'schema_version': '1.0', 'generated_utc': datetime.now(timezone.utc).isoformat(),
        'evidence_class': 'READ_ONLY_ARTIFACT_AUDIT', 'runs_directory': label(runs, archive),
        'path_convention': 'Paths are relative to the private runtime archive; external/ names denote inputs outside it.',
        'boundary_notes': [
            'PASS_VERIFIED validates retained artifacts and protocol receipts, not gameplay correctness or Windows acceptance.',
            'An IN_PROGRESS row has incomplete/changing receipts; this does not independently prove a live process.',
            'No-declared-intervention ancestry is not proof of ordinary campaign legitimacy; exact schedules remain private evidence.',
            'Checkpoint ancestry compares decoded MSS payload hashes, not hashes of the base64 text files.',
            'Multiple possible parents and cycles remain uncertain; any prepared possibility propagates conservatively.',
            'A failed producer can still supply a verified earlier checkpoint; producer failure and lineage are separate.',
            'Authorized hard-link deduplication can change inode/ctime without changing bytes; those changes alone are not failures.',
            'This point-in-time scan is read-only and may need repeating after active runs finish.',
        ],
        'canonical_source_executable': canonical_meta,
        'expected_rom_sha256': ROM_SHA256, 'expected_core_sha256': CORE_SHA256,
        'builder_sha256': sha(Path(__file__).read_bytes()),
        'png_validator_sha256': sha(Path(__file__).with_name('battle_probe.py').read_bytes()),
        'summary': {
            'runs': len(rows), 'status_counts': dict(Counter(r['status'] for r in rows)),
            'execution_status_counts': dict(Counter(r['execution']['status'] for r in rows)),
            'lineage_counts': dict(Counter(r['lineage']['classification'] for r in rows)),
            'declared_products': sum(r['products']['declared'] for r in rows),
            'verified_products': sum(r['products']['verified'] for r in rows),
            'png_checked': sum(r['products']['png_checked'] for r in rows),
            'png_valid': sum(r['products']['png_valid'] for r in rows),
            'saved_checkpoint_payloads': sum(r['products']['checkpoint_payloads_hashed'] for r in rows),
        },
        'runs': rows,
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runs', type=Path, required=True, help='Private runtime/experiments directory')
    parser.add_argument('--output', type=Path, required=True, help='Public JSON outside the private run directory')
    parser.add_argument('--canonical-mesen', type=Path, help='Defaults to ../mesen/Mesen beside experiments')
    args = parser.parse_args()
    try:
        output = args.output.expanduser().resolve()
        if output.suffix != '.json' or output.is_relative_to(args.runs.expanduser().resolve()):
            raise ValueError('Output must be a .json file outside the original run directory')
        manifest = build_manifest(args.runs, args.canonical_mesen)
        output.parent.mkdir(parents=True, exist_ok=True)
        with tempfile.NamedTemporaryFile(mode='w', encoding='utf-8', dir=output.parent,
                                         prefix='.runtime-manifest-', delete=False) as stream:
            json.dump(manifest, stream, indent=2)
            stream.write('\n')
            temporary = Path(stream.name)
        temporary.replace(output)
        print(json.dumps({'manifest': output.name, 'manifest_sha256': sha(output.read_bytes()),
                          'summary': manifest['summary']}, indent=2))
        return 0
    except (OSError, ValueError) as exc:
        print('Manifest build failed: ' + str(exc), file=sys.stderr)
        return 2


if __name__ == '__main__':
    raise SystemExit(main())
