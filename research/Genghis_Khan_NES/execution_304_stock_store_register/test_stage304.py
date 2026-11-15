#!/usr/bin/env python3
"""EXEC0304 source-staging tests; no original ROM or proprietary bytes."""
from pathlib import Path
import hashlib
import importlib.util
import shutil
import tempfile
import unittest
import sys
import stage304 as s

ROOT=Path(__file__).resolve().parent
BASE=Path(sys.argv.pop(1)) if len(sys.argv)>1 and Path(sys.argv[1]).is_dir() else None

def h(x):return hashlib.sha256(x).hexdigest()

class Stage304Tests(unittest.TestCase):
    def setUp(self):
        self.tmp=tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.p=Path(self.tmp.name)
        if BASE is None: raise RuntimeError('baseline304 source fixture directory required')
        for name in s.PRE:shutil.copyfile(BASE/name,self.p/name)
        self.snapshot={n:(self.p/n).read_bytes() for n in s.PRE}
    def same(self):
        self.assertEqual(self.snapshot,{n:(self.p/n).read_bytes() for n in s.PRE})
        self.assertFalse(any((self.p/n).exists() for n in s.NEW))
    def test_clean_exact_source_stages_all_six(self):
        z=s.stage(self.p,ROOT);s.commit(self.p,z)
        self.assertEqual(len(z),6)
        for name,digest in s.POST.items():self.assertEqual(h((self.p/name).read_bytes()),digest)
        for name,digest in s.NEW.items():self.assertEqual(h((self.p/name).read_bytes()),digest)
        with self.assertRaises(ValueError):s.stage(self.p,ROOT)
    def test_crlf_safe_source_normalizes_exactly(self):
        f=self.p/'gk1_app_state.c';f.write_bytes(f.read_bytes().replace(b'\n',b'\r\n'))
        z=s.stage(self.p,ROOT);s.commit(self.p,z)
        self.assertEqual(h(f.read_bytes()),s.POST[f.name])
    def test_modified_app_refused_atomically(self):
        f=self.p/'gk1_app_state.c';f.write_bytes(f.read_bytes()+b'X')
        old=f.read_bytes()
        with self.assertRaises(ValueError):s.stage(self.p,ROOT)
        self.assertEqual(f.read_bytes(),old)
        self.assertFalse((self.p/'gk304_stock_store_value.c').exists())
    def test_modified_cmake_refused_atomically(self):
        f=self.p/'CMakeLists.txt';f.write_bytes(f.read_bytes().replace(b'GK1_302',b'GK1_999'))
        old=f.read_bytes()
        with self.assertRaises(ValueError):s.stage(self.p,ROOT)
        self.assertEqual(f.read_bytes(),old)
        self.assertFalse((self.p/'gk304_stock_store_value.c').exists())
    def test_modified_header_refused_atomically(self):
        f=self.p/'gk1_app_state.h';f.write_bytes(f.read_bytes().replace(b'Gk302Status',b'Gk303Status'))
        old=f.read_bytes()
        with self.assertRaises(ValueError):s.stage(self.p,ROOT)
        self.assertEqual(f.read_bytes(),old)
    def test_preexisting_source_refused(self):
        (self.p/'gk304_stock_store_value.c').write_text('fake')
        with self.assertRaises(ValueError):s.stage(self.p,ROOT)
        for n in s.PRE:self.assertEqual((self.p/n).read_bytes(),self.snapshot[n])
    def test_old_mac_cr_refused(self):
        f=self.p/'gk1_app_state.h';f.write_bytes(f.read_bytes().replace(b'\n',b'\r'))
        with self.assertRaises(ValueError):s.stage(self.p,ROOT)
    def test_source_tampering_detected(self):
        old=s.NEW['gk304_stock_store_value.c']
        try:
            s.NEW['gk304_stock_store_value.c']='f'*64
            with self.assertRaises(ValueError):s.stage(self.p,ROOT)
            self.same()
        finally:s.NEW['gk304_stock_store_value.c']=old
    def test_partial_write_rollback(self):
        old=s.os.replace
        events=[0]
        def bad_replace(*args):
            events[0]+=1
            if events[0]==2:raise OSError('injected write failure')
            return old(*args)
        z=s.stage(self.p,ROOT)
        s.os.replace=bad_replace
        try:
            with self.assertRaises(OSError):s.commit(self.p,z)
            self.same()
        finally:s.os.replace=old

if __name__=='__main__':unittest.main(verbosity=2)
