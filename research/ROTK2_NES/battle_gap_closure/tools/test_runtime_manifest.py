"""Synthetic filesystem integration cases; no original emulator is run."""

import base64
import hashlib
import importlib.util
import json
from pathlib import Path
import struct
import sys
import tempfile
import unittest
from unittest import mock
import zlib


TOOLS = Path(__file__).resolve().parent
SCRIPT = TOOLS / 'build_runtime_manifest.py'
sys.path.insert(0, str(TOOLS))


def sha(data):
    return hashlib.sha256(data).hexdigest()


def png():
    def chunk(kind, data):
        return struct.pack('>I', len(data)) + kind + data + struct.pack('>I', zlib.crc32(kind + data) & 0xffffffff)
    return (b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('>IIBBBBB', 256, 240, 8, 2, 0, 0, 0))
            + chunk(b'IDAT', zlib.compress((b'\0' + b'\x12\x34\x56' * 256) * 240)) + chunk(b'IEND', b''))


class ManifestCases(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.root = Path(self.temp.name) / 'runtime'
        self.runs = self.root / 'experiments'
        self.runs.mkdir(parents=True)
        self.mesen = b'synthetic executable identity'
        self.core = b'synthetic core identity'
        (self.root / 'mesen').mkdir()
        (self.root / 'mesen/Mesen').write_bytes(self.mesen)

    def tearDown(self):
        self.temp.cleanup()

    def run_fixture(self, name, *, state_in=None, state_out=None, patches=(), exit_code=0, png_bytes=None):
        d = self.runs / name
        portable = d / 'process/portable'
        portable.mkdir(parents=True)
        (portable / 'Mesen').write_bytes(self.mesen)
        (portable / 'MesenCore.so').write_bytes(self.core)
        (portable / 'settings.json').write_text('{}\n')
        schedule = {'stop': 20, 'shots': [10], 'saves': [10] if state_out else [], 'snapshots': [], 'events': [], 'patches': list(patches)}
        (d / 'schedule.json').write_text(json.dumps(schedule) + '\n')
        (d / 'probe.lua').write_text('local loadstate=' + (repr(base64.b64encode(state_in).decode()) if state_in else 'nil') + '\n')
        raw_png = png() if png_bytes is None else png_bytes
        (d / 'f000010.png').write_bytes(raw_png)
        stdout = 'PROBE_STARTED\n' + ('LOADED\n' if state_in else '') + 'SHOT 10 ' + base64.b64encode(raw_png).decode() + '\n'
        products = [{'file': 'f000010.png', 'bytes': len(raw_png), 'sha256': sha(raw_png)}]
        if state_out:
            encoded = base64.b64encode(state_out).decode()
            (d / 'state_000010.b64').write_text(encoded + '\n')
            products.append({'file': 'state_000010.b64', 'bytes': len(encoded) + 1, 'sha256': sha((encoded + '\n').encode())})
            stdout += 'STATE 10 ' + encoded + '\n'
        stdout += 'DONE 20\n' if exit_code == 0 else 'PROBE_ERROR synthetic failure\n'
        (d / 'process/stdout.log').write_text(stdout)
        (d / 'process/stderr.log').write_text('')
        meta = lambda p: {'path': str(p), 'bytes': p.stat().st_size, 'sha256': sha(p.read_bytes())}
        process = {'command': [str(portable / 'Mesen'), '--testRunner', str(d / 'probe.lua')],
                   'cwd': str(d / 'process'), 'exit_code': exit_code, 'timeout': False,
                   'rom': {'path': '/external/original.nes', 'bytes': 524304, 'sha256': 'a' * 64},
                   'mesen': meta(portable / 'Mesen'), 'mesen_after': meta(portable / 'Mesen'),
                   'copied_executable_unchanged': True, 'core': meta(portable / 'MesenCore.so'),
                   'script': meta(d / 'probe.lua'), 'settings': meta(portable / 'settings.json'),
                   'original_rom_unchanged': True, 'runtime_identity_passed': True,
                   'raw_output': {k: meta(d / 'process' / (k + '.log')) for k in ('stdout', 'stderr')}}
        (d / 'process/receipt.json').write_text(json.dumps(process) + '\n')
        probe = {'evidence': 'OBSERVED_RUNTIME' if exit_code == 0 else 'INCOMPLETE_RUNTIME',
                 'controlled_state_interventions': list(patches), 'failures': [] if exit_code == 0 else ['Native runtime failed'],
                 'restoration_requested': state_in is not None, 'input_state_sha256': sha(state_in) if state_in else None,
                 'schedule_sha256': sha((d / 'schedule.json').read_bytes()), 'products': products}
        (d / 'probe_receipt.json').write_text(json.dumps(probe) + '\n')
        return d

    def manifest(self):
        self.assertTrue(SCRIPT.is_file(), 'The requested manifest builder has not been implemented')
        spec = importlib.util.spec_from_file_location('runtime_manifest_fixture', SCRIPT)
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        with mock.patch.multiple(module, MESEN_SHA256=sha(self.mesen), CORE_SHA256=sha(self.core), ROM_SHA256='a' * 64):
            result = module.build_manifest(self.runs)
        return result, {r['run']: r for r in result['runs']}

    def test_valid_artifacts_pass_without_exposing_payloads_or_absolute_paths(self):
        # Break caught: ignoring valid evidence or leaking a whole state into public JSON.
        payload = b'MSS synthetic private state payload'
        self.run_fixture('ordinary', state_out=payload)
        result, rows = self.manifest()
        self.assertEqual(rows['ordinary']['status'], 'PASS_VERIFIED')
        self.assertEqual(rows['ordinary']['products']['verified'], 2)
        self.assertEqual(rows['ordinary']['products']['png_valid'], 1)
        self.assertEqual(rows['ordinary']['lineage']['classification'], 'NO_DECLARED_INTERVENTION_ANCESTRY')
        public = json.dumps(result)
        self.assertNotIn(base64.b64encode(payload).decode(), public)
        self.assertNotIn(str(self.root), public)

    def test_missing_receipts_are_in_progress_not_failed(self):
        # Break caught: turning a producer's still-open output directory into a failure.
        d = self.runs / 'active'
        d.mkdir()
        (d / 'schedule.json').write_text('{"stop": 100}\n')
        _, rows = self.manifest()
        self.assertEqual(rows['active']['status'], 'IN_PROGRESS')
        self.assertEqual(rows['active']['execution']['status'], 'IN_PROGRESS')

    def test_header_only_png_and_modified_log_do_not_pass(self):
        # Break caught: trusting recorded product/log hashes without independent checks.
        d = self.run_fixture('bad', png_bytes=png()[:33])
        (d / 'process/stdout.log').write_text('tampered\n')
        _, rows = self.manifest()
        self.assertEqual(rows['bad']['status'], 'FAILED')
        codes = {e['code'] for e in rows['bad']['integrity']['issues']}
        self.assertIn('PNG_INVALID', codes)
        self.assertIn('RAW_LOG_HASH_MISMATCH', codes)

    def test_retained_executable_mismatch_survives_recorded_success(self):
        # Break caught: laundering the known weather-copy problem through exit 0.
        d = self.run_fixture('retained_mismatch')
        (d / 'process/portable/Mesen').write_bytes(self.mesen[:8])
        _, rows = self.manifest()
        self.assertEqual(rows['retained_mismatch']['execution']['status'], 'COMPLETED')
        self.assertEqual(rows['retained_mismatch']['status'], 'FAILED')
        self.assertIn('RETAINED_EXECUTABLE_MISMATCH', {e['code'] for e in rows['retained_mismatch']['integrity']['issues']})

    def test_prepared_state_propagates_through_failed_parent_and_grandchild(self):
        # Break caught: classifying an unpatched descendant as ordinary because its own patch list is empty.
        a, b = b'MSS prepared parent payload', b'MSS prepared child payload'
        self.run_fixture('prepared', state_out=a, patches=[[2, 0x6000, 0, 1]], exit_code=255)
        self.run_fixture('child', state_in=a, state_out=b)
        self.run_fixture('grandchild', state_in=b)
        _, rows = self.manifest()
        self.assertEqual(rows['prepared']['status'], 'FAILED')
        self.assertEqual(rows['child']['lineage']['classification'], 'PREPARED_RAM_DESCENDANT')
        self.assertEqual(rows['grandchild']['lineage']['classification'], 'PREPARED_RAM_DESCENDANT')
        self.assertEqual(rows['grandchild']['lineage']['intervention_ancestors'], ['prepared'])
        self.assertIn('prepared', rows['grandchild']['lineage']['nonpassing_ancestors'])

    def test_unknown_checkpoint_remains_unresolved(self):
        # Break caught: assuming an external native state was generated through ordinary play.
        self.run_fixture('external', state_in=b'MSS unrecovered checkpoint')
        _, rows = self.manifest()
        self.assertEqual(rows['external']['lineage']['classification'], 'UNKNOWN_ANCESTRY')
        self.assertEqual(len(rows['external']['lineage']['unresolved_checkpoint_sha256']), 1)

    def test_older_schedule_only_patches_still_taint_descendants(self):
        # Break caught: assuming old receipts without the intervention field were unpatched.
        state = b'MSS old declared intervention payload'
        d = self.run_fixture('older', state_out=state, patches=[[2, 0x6000, 0, 1]])
        p = d / 'probe_receipt.json'
        receipt = json.loads(p.read_text())
        receipt.pop('controlled_state_interventions')
        p.write_text(json.dumps(receipt))
        self.run_fixture('child', state_in=state)
        _, rows = self.manifest()
        self.assertEqual(rows['older']['lineage']['classification'], 'CONTROLLED_RAM_INTERVENTION')
        self.assertEqual(rows['child']['lineage']['classification'], 'PREPARED_RAM_DESCENDANT')

    def test_ambiguous_checkpoint_producers_cannot_erase_prepared_ancestry(self):
        # Break caught: choosing an arbitrary clean producer for an identical checkpoint hash.
        state = b'MSS duplicate saved payload'
        self.run_fixture('clean', state_out=state)
        self.run_fixture('prepared', state_out=state, patches=[[2, 0x6000, 0, 1]])
        self.run_fixture('child', state_in=state)
        _, rows = self.manifest()
        self.assertEqual(rows['child']['lineage']['classification'], 'PREPARED_RAM_DESCENDANT')
        self.assertTrue(rows['child']['lineage']['ambiguous_parent_checkpoint'])

    def test_checkpoint_cycle_remains_unknown(self):
        # Break caught: a cyclic provenance graph being labelled as an ordinary cold-rooted sequence.
        a, b = b'MSS cycle A', b'MSS cycle B'
        self.run_fixture('a', state_in=b, state_out=a)
        self.run_fixture('b', state_in=a, state_out=b)
        _, rows = self.manifest()
        self.assertEqual(rows['a']['lineage']['classification'], 'UNKNOWN_ANCESTRY')
        self.assertTrue(rows['a']['lineage']['cycle_detected'])

    def test_logged_intervention_without_metadata_cannot_look_ordinary(self):
        # Break caught: ignoring actual native intervention events when metadata is incomplete.
        state = b'MSS logged intervention payload'
        d = self.run_fixture('logged', state_out=state)
        log = d / 'process/stdout.log'
        log.write_text(log.read_text().replace('DONE 20', 'INTERVENTION 2 24576 8192 1 0 0\nDONE 20'))
        rp = d / 'process/receipt.json'
        receipt = json.loads(rp.read_text())
        receipt['raw_output']['stdout'].update(bytes=log.stat().st_size, sha256=sha(log.read_bytes()))
        rp.write_text(json.dumps(receipt))
        self.run_fixture('child', state_in=state)
        _, rows = self.manifest()
        self.assertEqual(rows['logged']['lineage']['classification'], 'CONTROLLED_RAM_INTERVENTION')
        self.assertEqual(rows['child']['lineage']['classification'], 'PREPARED_RAM_DESCENDANT')
        self.assertIn('UNDECLARED_LOGGED_INTERVENTION', {e['code'] for e in rows['logged']['integrity']['issues']})

    def test_partial_identity_receipt_is_reported_without_aborting_scan(self):
        # Break caught: mesen_after without mesen crashing the whole inventory.
        d = self.run_fixture('partial_identity')
        p = d / 'process/receipt.json'
        receipt = json.loads(p.read_text())
        receipt.pop('mesen')
        p.write_text(json.dumps(receipt))
        try:
            _, rows = self.manifest()
        except KeyError as exc:
            self.fail('One partial identity receipt aborted the scan: ' + str(exc))
        self.assertEqual(rows['partial_identity']['status'], 'FAILED')

    def test_missing_product_error_does_not_leak_absolute_archive_path(self):
        # Break caught: copying an OSError's absolute private path into public JSON.
        d = self.run_fixture('missing_product')
        (d / 'f000010.png').unlink()
        result, rows = self.manifest()
        self.assertEqual(rows['missing_product']['status'], 'FAILED')
        self.assertNotIn(str(self.root), json.dumps(result))


if __name__ == '__main__':
    unittest.main(verbosity=2)
