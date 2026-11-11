#!/usr/bin/env python3
import json, pathlib, tempfile, unittest
from verify_reconstructed_199 import audit
class NegativeControls(unittest.TestCase):
    def test_missing_manifest_refused(self):
        with tempfile.TemporaryDirectory() as d:
            p=pathlib.Path(d)/'m.json';p.write_text(json.dumps({'execution':'0199','file_sha256':{}}))
            self.assertFalse(audit(d,p)[0])
    def test_path_traversal_refused(self):
        with tempfile.TemporaryDirectory() as d:
            p=pathlib.Path(d)/'m.json';p.write_text(json.dumps({'execution':'0199','file_sha256':{'../secret':'0'*64}}))
            self.assertFalse(audit(d,p)[0])
    def test_corrupt_json_refused(self):
        with tempfile.TemporaryDirectory() as d:
            p=pathlib.Path(d)/'m.json';p.write_text('{')
            with self.assertRaises(json.JSONDecodeError):audit(d,p)
if __name__=='__main__': unittest.main()