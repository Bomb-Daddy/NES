import unittest
import tempfile
from pathlib import Path
import repair314_provenance as p

RAW=(p.CURRENT_LITERAL+'\n'+'\n'.join(p.REQUIRED)+'\n').encode('utf-8')
OK_SHA='a'*64
class TestGate(unittest.TestCase):
    def test_pinned_one_literal_exact_reversal(self):
        previous=RAW.replace(p.CURRENT_LITERAL.encode(),p.OLD_LITERAL.encode(),1)
        after=p.update_gate(RAW,OK_SHA,p.sha(previous))
        self.assertIn(("EXPECTED_APP_SHA = '"+OK_SHA+"'").encode(),after)
        self.assertEqual(after.replace(("EXPECTED_APP_SHA = '"+OK_SHA+"'").encode(),p.CURRENT_LITERAL.encode()),RAW)
    def test_reject_unpinned_gate(self):
        with self.assertRaisesRegex(ValueError,'SHA mismatch'):
            p.update_gate(RAW,OK_SHA)
    def test_reject_corrupt_gate(self):
        old=RAW.replace(p.CURRENT_LITERAL.encode(),p.OLD_LITERAL.encode(),1)
        changed=RAW+b'garbage'
        with self.assertRaisesRegex(ValueError,'SHA mismatch'):
            p.update_gate(changed,OK_SHA,p.sha(old))
    def test_missing_check(self):
        bad=RAW.replace(b'EXPECTED_TEST_SHA',b'INERT_TEST')
        old=bad.replace(p.CURRENT_LITERAL.encode(),p.OLD_LITERAL.encode(),1)
        with self.assertRaisesRegex(ValueError,'lost inherited'):
            p.update_gate(bad,OK_SHA,p.sha(old))
    def test_reject_duplicate_literal(self):
        bad=RAW+p.CURRENT_LITERAL.encode()
        with self.assertRaisesRegex(ValueError,'missing/duplicated'):
            p.update_gate(bad,OK_SHA)
    def test_reject_invalid_sha(self):
        with self.assertRaisesRegex(ValueError,'invalid next'):
            p.update_gate(RAW,'xxxx')
    def test_reject_CRCRLF(self):
        with self.assertRaisesRegex(ValueError,'forbidden source'):
            p.update_gate(RAW+b'\r\r\n',OK_SHA)
    def test_normal_CRLF(self):
        old=RAW.replace(p.CURRENT_LITERAL.encode(),p.OLD_LITERAL.encode(),1)
        self.assertEqual(p.update_gate(RAW.replace(b'\n',b'\r\n'),OK_SHA,p.sha(old)),p.update_gate(RAW,OK_SHA,p.sha(old)))
    def test_fail_closed_wrong_app(self):
        with tempfile.TemporaryDirectory() as d:
            root=Path(d);(root/'gk1_app_state.c').write_bytes(b'wrong')
            with self.assertRaises(ValueError):p.authenticate_patched_source(root)

if __name__=='__main__': unittest.main()
