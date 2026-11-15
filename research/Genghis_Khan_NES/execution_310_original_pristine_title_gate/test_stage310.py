#!/usr/bin/env python3
"""Hostile source-staging tests. Synthetic 0276-shaped fixture for I/O tests;
production SOURCE_SHA remains hard-pinned to the reviewed EXEC0309 hash.
"""
import hashlib, importlib.util, pathlib, shutil, tempfile, unittest

HERE=pathlib.Path(__file__).resolve().parent
spec=importlib.util.spec_from_file_location('stage310',HERE/'stage310.py')
m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
PRIOR=pathlib.Path(__file__).resolve().parents[1]/'GK1_EXEC0276_gk162_original_entry.c.txt'
# Above is optional in external archive; this source-free fixture is safe and
# intentionally NOT accepted by production pin.
SYNTHETIC='''static int pristine_title(const Gk1App *a,unsigned w) {return 1;}
int gk162_original_setup_compatible(const Gk1App *a,unsigned profile){
if(a->setup_stage[0]==GK53_SETUP_PLAYERS)return 1;
return 0;
}
'''

class Tests(unittest.TestCase):
    def setUp(self):
        self.tmp=tempfile.TemporaryDirectory()
        self.app=pathlib.Path(self.tmp.name)
        self.src=SYNTHETIC.encode()
        self.old=m.SOURCE_SHA
        m.SOURCE_SHA=m.digest(self.src) # test ONLY: prove production rejects this exact synthetic source
        (self.app/'gk162_original_entry.c').write_bytes(self.src)
        (self.app/'CMakeLists.txt').write_text('add_test(NAME GK1_309_StockHumanRulers_FailClosed14And27 COMMAND prior)\n')
    def tearDown(self):
        m.SOURCE_SHA=self.old;self.tmp.cleanup()
    def original(self):
        return {p:(self.app/p).read_bytes() if (self.app/p).exists() else None
                for p in ['gk162_original_entry.c','CMakeLists.txt',m.TEST_NAME]}
    def test_stage_afterimages(self):
        o=self.original();r=m.stage(self.app)
        self.assertEqual(set(r),set(o));self.assertEqual(len(r),3)
        self.assertIn(m.AFTER,(self.app/'gk162_original_entry.c').read_text())
        self.assertNotIn(m.BEFORE,(self.app/'gk162_original_entry.c').read_text())
        self.assertIn(m.MARKER,(self.app/'CMakeLists.txt').read_text())
        self.assertEqual(m.digest((self.app/m.TEST_NAME).read_bytes()),m.TEST_SHA)
        self.assertNotEqual(self.original(),o)
    def test_rollback_all_three_boundaries(self):
        for fail in (1,2,3):
            o=self.original()
            with self.assertRaises(OSError):m.stage(self.app,fail_after=fail)
            self.assertEqual(o,self.original(),f'rollback after replacement {fail}')
    def test_wrong_source_digest(self):
        (self.app/'gk162_original_entry.c').write_bytes(self.src+b'\n/*tamper*/')
        before=self.original()
        with self.assertRaisesRegex(ValueError,'exact EXEC0309'):m.stage(self.app)
        self.assertEqual(before,self.original())
    def test_duplicate_refusal(self):
        m.stage(self.app);before=self.original()
        with self.assertRaises(ValueError):m.stage(self.app)
        self.assertEqual(before,self.original())
    def test_missing_309(self):
        (self.app/'CMakeLists.txt').write_text('project(fake)\n')
        with self.assertRaisesRegex(ValueError,'EXEC0309'):m.stage(self.app)
    def test_missing_unique_anchor(self):
        new=self.src.replace(m.BEFORE.encode(),b'return 1;')
        (self.app/'gk162_original_entry.c').write_bytes(new)
        m.SOURCE_SHA=m.digest(new)
        with self.assertRaisesRegex(ValueError,'unique anchor'):m.stage(self.app)
    def test_multi_anchor_refusal(self):
        src=self.src.replace(m.BEFORE.encode(),(m.BEFORE+'\n'+m.BEFORE).encode())
        (self.app/'gk162_original_entry.c').write_bytes(src)
        m.SOURCE_SHA=m.digest(src)
        with self.assertRaisesRegex(ValueError,'unique anchor'):m.stage(self.app)
    def test_malformed_newline_source(self):
        (self.app/'gk162_original_entry.c').write_bytes(self.src.replace(b'\n',b'\r\r\n'))
        with self.assertRaisesRegex(ValueError,'newlines'):m.stage(self.app)
    def test_windows_crlf_source_with_sha_matching_normal_form(self):
        (self.app/'gk162_original_entry.c').write_bytes(self.src.replace(b'\n',b'\r\n'))
        # Windows Path.write_text() can ALREADY have produced CRLF in setUp.
        # Normalize first: blindly inserting CR before every LF creates CRCRLF.
        cmake_normal=m.normalize((self.app/'CMakeLists.txt').read_bytes())
        cmake_crlf=cmake_normal.replace(b'\n',b'\r\n')
        self.assertNotIn(b'\r\r\n',cmake_crlf)
        (self.app/'CMakeLists.txt').write_bytes(cmake_crlf)
        m.stage(self.app)
        self.assertIn(m.AFTER,(self.app/'gk162_original_entry.c').read_text())
        self.assertNotIn(b'\r\r\n',(self.app/'CMakeLists.txt').read_bytes())
    def test_crcrlf_cmake_rejected_without_mutation(self):
        # Deliberately malformed CRCRLF must still be rejected, never silently repaired.
        cmake_normal=m.normalize((self.app/'CMakeLists.txt').read_bytes())
        (self.app/'CMakeLists.txt').write_bytes(cmake_normal.replace(b'\n',b'\r\r\n'))
        before=self.original()
        with self.assertRaisesRegex(ValueError,'malformed source newlines'):
            m.stage(self.app)
        self.assertEqual(before,self.original())
    def test_tampered_test_file(self):
        original=m.TEST_SHA;m.TEST_SHA='0'*64
        try:
            with self.assertRaisesRegex(ValueError,'test changed'):m.stage(self.app)
        finally:m.TEST_SHA=original
    def test_production_sha_never_matches_synthetic(self):
        self.assertNotEqual(self.old,m.digest(self.src))

if __name__ == '__main__': unittest.main(verbosity=2)
