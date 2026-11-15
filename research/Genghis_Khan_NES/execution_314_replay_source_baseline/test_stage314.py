import importlib.util
import tempfile
import unittest
from pathlib import Path

spec = importlib.util.spec_from_file_location('stage314',Path(__file__).with_name('stage314.py'))
stage = importlib.util.module_from_spec(spec)
spec.loader.exec_module(stage)

class Staging(unittest.TestCase):
    def setUp(self):
        self.tmp=tempfile.TemporaryDirectory()
        self.folder=Path(self.tmp.name)/'execution_199'
        self.folder.mkdir()
        self.app=self.folder/'gk1_app_state.c'
        self.cmake=self.folder/'CMakeLists.txt'
        # Binary writes preserve exact LF on Windows (write_text produces CRLF).
        self.app.write_bytes(('before\n'+stage.SOURCE_ANCHOR+'\nafter\n').encode('utf-8'))
        self.cmake.write_bytes(b'add_library(gk1_app_state STATIC foo.c)\n')
        self.before={p:p.read_bytes() for p in [self.app,self.cmake]}
    def test_fixture_exact_lf_cross_platform(self):
        raw=self.app.read_bytes()
        self.assertIn(stage.SOURCE_ANCHOR.encode('utf-8'),raw)
        self.assertNotIn(b'\r',raw)
        self.assertNotIn(b'\r',self.cmake.read_bytes())

    def test_translated_crlf_fixture_rejected_without_mutation(self):
        self.app.write_bytes(self.app.read_bytes().replace(b'\n',b'\r\n'))
        prior={p:p.read_bytes() for p in [self.app,self.cmake]}
        with self.assertRaises(ValueError):stage.integrate(self.folder)
        self.assertEqual(prior,{p:p.read_bytes() for p in [self.app,self.cmake]})
        self.assertFalse((self.folder/'test_gk314_replay_source_baseline.c').exists())

    def tearDown(self):
        self.tmp.cleanup()
    def verify_unchanged(self):
        self.assertEqual(self.before,{p:p.read_bytes() for p in [self.app,self.cmake]})
        self.assertFalse((self.folder/'test_gk314_replay_source_baseline.c').exists())
    def test_success_exact_and_no_forged_spring(self):
        stage.integrate(self.folder)
        a=self.app.read_text()
        self.assertIn('memcpy(next->finance_source_baseline',a)
        self.assertIn('old->finance_source_baseline',a)
        self.assertNotIn('gameplay_spring_unlocked=1',a)
        self.assertIn('GK1_314_ReplayPreservesSourceBaseline',self.cmake.read_text())
        self.assertTrue((self.folder/'test_gk314_replay_source_baseline.c').is_file())
    def test_double_application_rejected_atomically(self):
        stage.integrate(self.folder)
        original={p:p.read_bytes() for p in [self.app,self.cmake]}
        with self.assertRaises(ValueError):stage.integrate(self.folder)
        self.assertEqual(original,{p:p.read_bytes() for p in [self.app,self.cmake]})
    def test_reject_source_drift(self):
        self.app.write_text('other source\n')
        original=self.app.read_bytes()
        with self.assertRaises(ValueError):stage.integrate(self.folder)
        self.assertEqual(original,self.app.read_bytes())
        self.assertEqual(self.before[self.cmake],self.cmake.read_bytes())
    def test_reject_cmake_drift(self):
        self.cmake.write_text('project(unrelated)\n')
        original=self.cmake.read_bytes()
        with self.assertRaises(ValueError):stage.integrate(self.folder)
        self.assertEqual(original,self.cmake.read_bytes())
        self.assertEqual(self.before[self.app],self.app.read_bytes())
    def test_reject_existing_test_without_mutation(self):
        test=self.folder/'test_gk314_replay_source_baseline.c'
        test.write_text('existing nonowned data')
        with self.assertRaises(ValueError):stage.integrate(self.folder)
        self.assertEqual(test.read_text(),'existing nonowned data')
        self.assertEqual(self.before[self.app],self.app.read_bytes())
        self.assertEqual(self.before[self.cmake],self.cmake.read_bytes())
    def test_rollback_on_first_write_failure(self):
        with self.assertRaises(OSError):stage.integrate(self.folder,inject_failure_after=1)
        self.verify_unchanged()
    def test_rollback_on_second_write_failure(self):
        with self.assertRaises(OSError):stage.integrate(self.folder,inject_failure_after=2)
        self.verify_unchanged()
    def test_rollback_after_test_written(self):
        with self.assertRaises(OSError):stage.integrate(self.folder,inject_failure_after=3)
        self.verify_unchanged()

if __name__=='__main__':unittest.main()
