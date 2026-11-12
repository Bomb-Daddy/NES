#!/usr/bin/env python3
"""Hostile runner safety QA; fake executors never count as Mesen observation."""
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0,str(Path(__file__).parent))
import gk221_original_stock_capture as gk

class RunnerTests(unittest.TestCase):
    def setUp(self):
        self.t=tempfile.TemporaryDirectory()
        self.addCleanup(self.t.cleanup)
        self.root=Path(self.t.name)
        self.rom=self.root/'original.nes'; self.rom.write_bytes(b'rom')
        self.mesen=self.root/'Mesen'; self.mesen.write_bytes(b'mesen'); self.mesen.chmod(0o700)
        self.core=self.root/'MesenCore.so'; self.core.write_bytes(b'core')
        self.lua=self.root/'probe.lua'; self.lua.write_text('-- script')
        self.native=self.root/'native'; self.native.write_text('#!/bin/sh\n'); self.native.chmod(0o700)
        self.out=self.root/'private'
        self.keys=('rom','mesen','core','lua','native','private')

    def invoke(self,executor=None):
        return gk.capture(self.rom,self.mesen,self.core,self.lua,self.native,
                          self.out, 30,executor or self.fake)
    def fake(self,command,**kw):
        if '--audit-private-stock-spring-write-trace' in command:
            return subprocess.CompletedProcess(command,0,'VERIFIED_STATIC owner-supplied trace schema (NOT ORIGINAL OBSERVATION): writes=6\n','')
        Path(kw['env']['GK218_TRACE_PRIVATE_PATH']).write_bytes(b'W'*200)
        return subprocess.CompletedProcess(command,0,'fake','')
    def auth(self):
        # Patches TEST-ONLY identity constants, never changes production hashes.
        return patch.multiple(gk, STOCK_SHA=gk.digest(self.rom),
            MESEN_SHA=gk.digest(self.mesen),CORE_SHA=gk.digest(self.core))
    def test_exact_hash_refusal(self):
        with self.assertRaises(gk.CaptureError):self.invoke()
        self.assertFalse(self.out.exists())
    def test_success_fake_not_runtime_stock(self):
        with self.auth():
            report=self.invoke()
        self.assertEqual(report['mesen_exit'],0)
        self.assertEqual(report['native_validator_exit'],0)
        self.assertIn('IF_RUNTIME_INDEPENDENTLY_VERIFIED',report['evidence_class'])
        self.assertEqual((self.out/gk.TRACE_NAME).stat().st_size,200)
        self.assertEqual((self.out/gk.TRACE_NAME).stat().st_mode&0o777,0o600)
        self.assertEqual((self.out/gk.RECEIPT_NAME).stat().st_mode&0o777,0o600)
        self.assertEqual(json.loads((self.out/gk.RECEIPT_NAME).read_text())['status'],
                         'CAPTURE_EXECUTED_AND_NATIVE_TRACE_SCHEMA_PASSED')
    def test_missing_and_replay_refused(self):
        with self.auth():
            self.invoke()
            with self.assertRaises(gk.CaptureError):self.invoke()
        self.assertEqual((self.out/gk.TRACE_NAME).read_bytes(),b'W'*200)
    def test_nonzero_emulator_and_no_receipt(self):
        def failed(cmd,**kw): return subprocess.CompletedProcess(cmd,9,'','')
        with self.auth(),self.assertRaises(gk.CaptureError):self.invoke(failed)
        self.assertFalse((self.out/gk.RECEIPT_NAME).exists())
    def test_timeout_no_success_receipt(self):
        def timeout(cmd,**kw):raise subprocess.TimeoutExpired(cmd,1)
        with self.auth(),self.assertRaises(gk.CaptureError):self.invoke(timeout)
        self.assertFalse((self.out/gk.RECEIPT_NAME).exists())
    def test_zero_exit_without_trace_rejected(self):
        def no_trace(cmd,**kw):return subprocess.CompletedProcess(cmd,0,'','')
        with self.auth(),self.assertRaises(gk.CaptureError):self.invoke(no_trace)
    def test_validator_nonzero_rejected(self):
        def rejected(cmd,**kw):
            if '--audit-private-stock-spring-write-trace' in cmd:
                return subprocess.CompletedProcess(cmd,2,'BAD_TRACE','')
            return self.fake(cmd,**kw)
        with self.auth(),self.assertRaises(gk.CaptureError):self.invoke(rejected)
        self.assertFalse((self.out/gk.RECEIPT_NAME).exists())
    def test_validator_zero_without_pass_rejected(self):
        def rejected(cmd,**kw):
            if '--audit-private-stock-spring-write-trace' in cmd:
                return subprocess.CompletedProcess(cmd,0,'success','')
            return self.fake(cmd,**kw)
        with self.auth(),self.assertRaises(gk.CaptureError):self.invoke(rejected)
    def test_symlinked_output_rejected(self):
        outside=self.root/'other';outside.mkdir()
        self.out.symlink_to(outside)
        with self.auth(),self.assertRaises(gk.CaptureError):self.invoke()
    def test_timeout_bounds_refused(self):
        with self.auth():
            with self.assertRaises(gk.CaptureError):
                gk.capture(self.rom,self.mesen,self.core,self.lua,self.native,self.out,1,self.fake)
            with self.assertRaises(gk.CaptureError):
                gk.capture(self.rom,self.mesen,self.core,self.lua,self.native,self.out,901,self.fake)
    def test_source_tamper_core_rejected(self):
        with self.auth():
            self.core.write_bytes(b'altered')
            with self.assertRaises(gk.CaptureError):self.invoke()
    def test_source_tamper_rom_rejected(self):
        with self.auth():
            self.rom.write_bytes(b'altered')
            with self.assertRaises(gk.CaptureError):self.invoke()

if __name__=='__main__':unittest.main()
