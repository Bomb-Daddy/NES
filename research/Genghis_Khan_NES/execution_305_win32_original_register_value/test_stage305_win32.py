#!/usr/bin/env python3
"""EXEC0305 hostile staging regression. Fixture is synthetic, not an NES capture."""
import tempfile
import shutil
from pathlib import Path
import unittest
import stage305_win32 as s

SOURCE=Path(__file__).resolve().parent.parent/'execution_304_stock_store_register'
def mk(root):
    (root/'packaging').mkdir()
    d={
      'gk1_app_state.c':'/* inherited */\nreturn gk302_replay_stock_seed_timeline(rom,initial,events,count,out);\n',
      'gk1_app_state.h':'#include "gk302_stock_seed_timeline.h"\nGk302Status gk1_app_original_seed_timeline(\n',
      'CMakeLists.txt':'gk302_stock_seed_timeline.c gk282_stock_rng_trace.c\nGK1_302_PublicTimelineRefusalNoROM\nGK1_257_CurrentWin32Workflow_RejectsStaleSourceAndFalsePass\n',
      s.GATE_NAME:"EXPECTED_APP_SHA = '"+s.OLD_GATE_SHA+"'\nEXPECTED_PREDECESSOR_SHA\nEXPECTED_TEST_SHA\ndef audit(\nfor index,values in enumerate(mutations,1)\nGK1_264_AtomicScenarioReinstallClearsSourceAndPortState\n"
    }
    for n,v in d.items():(root/n).write_text(v)
    return {n:s.sha(d[n].encode()) for n in s.PRE}

class Tests(unittest.TestCase):
    def setUp(self):
        self.tmp=tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root=Path(self.tmp.name)
        self.pin=mk(self.root)
        self.before={n:(self.root/n).read_bytes() for n in (*s.PRE,s.GATE_NAME)}
    def unchanged(self):
        for n,v in self.before.items():self.assertEqual((self.root/n).read_bytes(),v)
        for n in list(s.SOURCES)[:3]:self.assertFalse((self.root/n).exists())
    def test_stage_and_duplicate_refusal(self):
        change=s.prepare(self.root,SOURCE,self.pin)
        self.assertEqual(len(change),7)
        s.commit(self.root,change)
        newsha=s.sha((self.root/'gk1_app_state.c').read_bytes())
        self.assertIn(newsha,(self.root/s.GATE_NAME).read_text())
        self.assertIn('GK1_304_PublicRegisterStoreRefusalNoROM',(self.root/'CMakeLists.txt').read_text())
        with self.assertRaises(ValueError):s.prepare(self.root,SOURCE,self.pin)
    def test_crlf_normalization(self):
        p=self.root/'gk1_app_state.c'
        # A Windows write_text() fixture already has CRLF. Canonicalize first;
        # re-encoding raw CRLF as CRCRLF would falsely fail this positive case.
        canonical = s.norm(p.read_bytes())
        p.write_bytes(canonical.replace(b'\n',b'\r\n'))
        self.assertEqual(len(s.prepare(self.root,SOURCE,self.pin)),7)
    def test_double_crlf_refused(self):
        p=self.root/'gk1_app_state.c'
        canonical = s.norm(p.read_bytes())
        p.write_bytes(canonical.replace(b'\n',b'\r\r\n'))
        with self.assertRaises(ValueError):s.prepare(self.root,SOURCE,self.pin)
    def test_modified_app_refused(self):
        p=self.root/'gk1_app_state.c';p.write_bytes(p.read_bytes()+b'evil')
        with self.assertRaises(ValueError):s.prepare(self.root,SOURCE,self.pin)
    def test_modified_cmake_refused(self):
        p=self.root/'CMakeLists.txt';p.write_text(p.read_text().replace('GK1_302','GK1_999'))
        with self.assertRaises(ValueError):s.prepare(self.root,SOURCE,self.pin)
    def test_bad_carriage_return_refused(self):
        p=self.root/'gk1_app_state.h';p.write_bytes(p.read_bytes().replace(b'\n',b'\r'))
        with self.assertRaises(ValueError):s.prepare(self.root,SOURCE,self.pin)
    def test_provenance_negative_gate_removed(self):
        p=self.root/s.GATE_NAME;p.write_text(p.read_text().replace('def audit(','def unsafe('))
        with self.assertRaises(ValueError):s.prepare(self.root,SOURCE,self.pin)
    def test_changed_authoritative_304_source_refused(self):
        copy=self.root/'sources';copy.mkdir()
        for n in s.SOURCES:shutil.copyfile(SOURCE/n,copy/n)
        p=copy/'gk304_stock_store_value.c';p.write_bytes(p.read_bytes()+b'X')
        with self.assertRaises(ValueError):s.prepare(self.root,copy,self.pin)
    def test_partial_commit_rollback(self):
        change=s.prepare(self.root,SOURCE,self.pin)
        real=s.os.replace;cnt=[0]
        def bad(a,b):
            cnt[0]+=1
            if cnt[0]==2:raise OSError('injected rename failure')
            return real(a,b)
        s.os.replace=bad
        try:
            with self.assertRaises(OSError):s.commit(self.root,change)
        finally:s.os.replace=real
        self.unchanged()

if __name__=='__main__':unittest.main(verbosity=2)
