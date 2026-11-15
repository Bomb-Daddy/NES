#!/usr/bin/env python3
"""EXEC0313 hostile exact-source provenance mutation/rollback test."""
import unittest
import tempfile
from pathlib import Path
import repair_gk313_provenance as m

GATE = m.OLD_LINE+'\n'+'\n'.join(m.REQUIRED)+'\n'
RAW = GATE.encode('utf-8')

class Tests(unittest.TestCase):
    def test_expected_literal_replaced_once(self):
        result=m.updated_gate(RAW,m.sha(RAW));self.assertEqual(result.count(m.NEW_LINE.encode()),1)
        self.assertNotIn(m.OLD_LINE.encode(),result)
        self.assertEqual(result.replace(m.NEW_LINE.encode(),m.OLD_LINE.encode()),RAW)
    def test_original_gate_not_implicitly_trusted(self):
        with self.assertRaisesRegex(ValueError,'SHA mismatch'):
            m.updated_gate(RAW)
    def test_changed_preimage_refused(self):
        with self.assertRaisesRegex(ValueError,'SHA mismatch'):
            m.updated_gate(RAW+b'altered',m.sha(RAW))
    def test_duplicate_literal_refused(self):
        bad=RAW+m.OLD_LINE.encode()+b'\n'
        with self.assertRaisesRegex(ValueError,'missing/duplicated'):
            m.updated_gate(bad,m.sha(bad))
    def test_missing_literal_refused(self):
        bad=RAW.replace(m.OLD_LINE.encode(),b'wrong')
        with self.assertRaisesRegex(ValueError,'missing/duplicated'):
            m.updated_gate(bad,m.sha(bad))
    def test_missing_hostile_control_refused(self):
        bad=RAW.replace(b'EXPECTED_TEST_SHA',b'UNTRUSTED')
        with self.assertRaisesRegex(ValueError,'lost inherited hostile'):
            m.updated_gate(bad,m.sha(bad))
    def test_malformed_encoding_refused(self):
        with self.assertRaisesRegex(ValueError,'encoding'):
            m.updated_gate(RAW+b'\r\r\n',m.sha(RAW+b'\r\r\n'))
    def test_crlf_normalization(self):
        with_crlf=RAW.replace(b'\n',b'\r\n')
        self.assertEqual(m.updated_gate(with_crlf,m.sha(RAW)),m.updated_gate(RAW,m.sha(RAW)))
    def test_root_missing_refused_without_write(self):
        with tempfile.TemporaryDirectory() as d:
            p=Path(d);a=p/'gk1_app_state.c';a.write_bytes(b'incorrect C11')
            with self.assertRaisesRegex(ValueError,'not exact'):
                m.run(p)
            self.assertEqual(a.read_bytes(),b'incorrect C11')

if __name__=='__main__':unittest.main(verbosity=2)
