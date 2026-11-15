#!/usr/bin/env python3
"""Hostile Win32 staging tests. Source fixture pins actual EXEC0288 C11 preimage.
Run on source copy, not the user's active game. Uses no stock ROM bytes.
"""
import hashlib
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest

from stage295 import stage, OLD, NEW, ADDED, TEST_NAME, LIB_OLD

ROOT=Path(__file__).resolve().parent

def sha(data): return hashlib.sha256(data).hexdigest()

class StageTests(unittest.TestCase):
    def setUp(self):
        self.t=tempfile.TemporaryDirectory(prefix='GK295_HQA_')
        self.root=Path(self.t.name)
        self.src=self.root/'execution_199';self.src.mkdir()
        exemplar=os.environ.get('GK295_PREDECESSOR_DIR')
        if not exemplar: self.skipTest('Set GK295_PREDECESSOR_DIR to real inherited EXEC0288 C11 source')
        exemplar=Path(exemplar)
        for name in OLD:
            data=(exemplar/name).read_bytes()
            self.assertEqual(sha(data),OLD[name])
            (self.src/name).write_bytes(data)
        cm=(exemplar/'CMakeLists.txt').read_bytes()
        self.assertIn(b'GK1_288_SourceOriginalC289MultiDrawNativeSetupAndRollback',cm)
        (self.src/'CMakeLists.txt').write_bytes(cm)
        gate=self.src/'packaging/test_gk265_current_win32_source.py'
        gate.parent.mkdir(parents=True)
        gate.write_text("EXPECTED_APP_SHA = '"+OLD['gk1_app_state.c']+"'\n")
    def tearDown(self): self.t.cleanup()
    def snapshot(self):
        return {str(p.relative_to(self.src)):sha(p.read_bytes())
                for p in self.src.rglob('*') if p.is_file()}
    def test_stages_exactly_and_one_time(self):
        stage(self.src)
        for k,v in {**NEW,**ADDED}.items(): self.assertEqual(sha((self.src/k).read_bytes()),v)
        cm=(self.src/'CMakeLists.txt').read_text()
        self.assertEqual(cm.count(TEST_NAME),1)
        self.assertEqual(cm.count('gk288_original_rng_stream.c gk294_seed_writer_attest.c gk282_stock_rng_trace.c'),1)
        self.assertIn("EXPECTED_APP_SHA = '"+NEW['gk1_app_state.c']+"'",
                      (self.src/'packaging/test_gk265_current_win32_source.py').read_text())
        after=self.snapshot()
        with self.assertRaises(ValueError): stage(self.src)
        self.assertEqual(self.snapshot(),after)
    def test_corrupt_app_preimage_rejected_atomically(self):
        f=self.src/'gk1_app_state.c';f.write_bytes(f.read_bytes()+b'\n')
        before=self.snapshot()
        with self.assertRaises(ValueError): stage(self.src)
        self.assertEqual(self.snapshot(),before)
        self.assertFalse((self.src/'gk294_seed_writer_attest.c').exists())
    def test_corrupt_header_preimage_rejected_atomically(self):
        f=self.src/'gk1_app_state.h';f.write_bytes(f.read_bytes().replace(b'Gk1App;',b'Gk1App_renamed;'))
        before=self.snapshot()
        with self.assertRaises(ValueError): stage(self.src)
        self.assertEqual(self.snapshot(),before)
    def test_duplicate_source_rejected_before_changes(self):
        (self.src/'gk294_seed_writer_attest.h').write_text('bogus')
        before=self.snapshot()
        with self.assertRaises(ValueError): stage(self.src)
        self.assertEqual(self.snapshot(),before)
    def test_corrupt_cmake_refused(self):
        f=self.src/'CMakeLists.txt';f.write_bytes(f.read_bytes().replace(LIB_OLD.encode(),b'BROKEN'))
        before=self.snapshot()
        with self.assertRaises(ValueError): stage(self.src)
        self.assertEqual(self.snapshot(),before)
    def test_no_288_predecessor_refused(self):
        f=self.src/'CMakeLists.txt';f.write_bytes(f.read_bytes().replace(b'GK1_288_SourceOriginalC289MultiDrawNativeSetupAndRollback',b'NOT_AN_INHERITED_GAME'))
        before=self.snapshot()
        with self.assertRaises(ValueError): stage(self.src)
        self.assertEqual(self.snapshot(),before)
    def test_bad_hostile_source_gate_refused(self):
        f=self.src/'packaging/test_gk265_current_win32_source.py';f.write_text('EXPECTED_APP_SHA = None\n')
        before=self.snapshot()
        with self.assertRaises(ValueError): stage(self.src)
        self.assertEqual(self.snapshot(),before)
    def test_failure_with_cmake_294_registration_preexisting(self):
        f=self.src/'CMakeLists.txt';f.write_bytes(f.read_bytes()+b'\n# GK1_294_OriginalRomSourceSeedWriterAndAtomicRefusals\n')
        before=self.snapshot()
        with self.assertRaises(ValueError): stage(self.src)
        self.assertEqual(self.snapshot(),before)

if __name__=='__main__': unittest.main(verbosity=2)