#!/usr/bin/env python3
"""EXEC0320 independent hostile tests; no private original NES material."""
import importlib.util
from pathlib import Path
import tempfile
import shutil
import unittest
import hashlib

spec = importlib.util.spec_from_file_location('verify320',Path(__file__).with_name('verify320.py'))
m=importlib.util.module_from_spec(spec)
spec.loader.exec_module(m)
HERE=Path(__file__).resolve().parent
SRC=HERE.parent/'execution_319_cpu_trace_math_bridge'
if not SRC.is_dir(): SRC=HERE/'qa_inherited_319'

class Tests(unittest.TestCase):
    def setUp(self):
        self.tmp=tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root=Path(self.tmp.name)/'source'
        self.root.mkdir()
        for name in m.EXPECTED:
            shutil.copyfile(SRC/name,self.root/name)
    def case(self,fn,expect=ValueError):
        with self.assertRaises(expect):fn()
    def minimal_app(self):
        app=Path(self.tmp.name)/'app';app.mkdir()
        (app/'CMakeLists.txt').write_text(
            'add_library(gk1_app_state STATIC gk300_stock_nmi_seed_clock.c '
            'gk316_cpu_claims_ingest.c gk319_cpu_trace_math.c)\n'
            'add_test(NAME '+m.TEST_ID+' COMMAND test_gk319_cpu_trace_math)\n')
        for name in ('gk319_cpu_trace_math.c','gk319_cpu_trace_math.h','test_gk319_cpu_trace_math.c'):
            shutil.copyfile(SRC/name,app/name)
        return app
    def test_exact_identity(self):
        self.assertEqual(len(m.audit(self.root)),6)
    def test_exact_binary_reconstruction(self):
        self.assertEqual(m.normalize_checkout(self.root),0)
    def test_empty_placeholder_closed(self):
        (self.root/'stage319.py').write_bytes(b'')
        self.case(lambda:m.audit(self.root))
    def test_missing_file_closed(self):
        (self.root/'test_stage319.py').unlink()
        self.case(lambda:m.audit(self.root),FileNotFoundError)
    def test_mutated_script_closed(self):
        f=self.root/'stage319.py';f.write_bytes(f.read_bytes()+b'\n# fake pass\n')
        self.case(lambda:m.audit(self.root))
    def test_mutated_c_closed(self):
        f=self.root/'gk319_cpu_trace_math.c';f.write_bytes(f.read_bytes()+b'\n')
        self.case(lambda:m.audit(self.root))
    def test_crlf_is_valid_if_exact(self):
        for name in m.EXPECTED:
            f=self.root/name;f.write_bytes(f.read_bytes().replace(b'\n',b'\r\n'))
        self.assertEqual(m.normalize_checkout(self.root),6)
        self.assertEqual(m.audit(self.root)["stage319.py"],(SRC/'stage319.py').read_bytes())
    def test_double_cr_refused_without_partial_write(self):
        a=self.root/'stage319.py';b=self.root/'test_stage319.py'
        a.write_bytes(a.read_bytes().replace(b'\n',b'\r\n'))
        b.write_bytes(b.read_bytes().replace(b'\n',b'\r\r\n'))
        before=a.read_bytes();self.case(lambda:m.normalize_checkout(self.root))
        self.assertEqual(a.read_bytes(),before)
    def test_solitary_cr_refused(self):
        f=self.root/'stage319.py';f.write_bytes(f.read_bytes().replace(b'\n',b'\r'))
        self.case(lambda:m.audit(self.root))
    def test_nul_refused(self):
        f=self.root/'stage319.py';f.write_bytes(b'\0'+f.read_bytes())
        self.case(lambda:m.audit(self.root))
    def test_complete_stage_pass(self):
        m.verify_staged(self.root,self.minimal_app())
    def test_no_c11_link_refused(self):
        app=self.minimal_app();p=app/'CMakeLists.txt'
        p.write_text(p.read_text().replace('gk319_cpu_trace_math.c','gk314_dummy.c'))
        self.case(lambda:m.verify_staged(self.root,app))
    def test_no_ctest_refused(self):
        app=self.minimal_app();p=app/'CMakeLists.txt'
        p.write_text(p.read_text().replace(m.TEST_ID,'GK_FAKE_PASS'))
        self.case(lambda:m.verify_staged(self.root,app))
    def test_duplicate_ctest_refused(self):
        app=self.minimal_app();p=app/'CMakeLists.txt'
        p.write_text(p.read_text()+f'add_test(NAME {m.TEST_ID} COMMAND fake)\n')
        self.case(lambda:m.verify_staged(self.root,app))
    def test_staged_file_corruption_refused(self):
        app=self.minimal_app();p=app/'gk319_cpu_trace_math.c'
        p.write_bytes(p.read_bytes()+b'\n')
        self.case(lambda:m.verify_staged(self.root,app))
    def test_missing_inherited_dependency_refused(self):
        app=self.minimal_app();p=app/'CMakeLists.txt'
        p.write_text(p.read_text().replace('gk300_stock_nmi_seed_clock.c','other.c'))
        self.case(lambda:m.verify_staged(self.root,app))
    def test_nonexisting_app_refused(self):
        self.case(lambda:m.verify_staged(self.root,Path(self.tmp.name)/'missing'),FileNotFoundError)

if __name__=='__main__':
    unittest.main(verbosity=2)
