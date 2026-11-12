#!/usr/bin/env python3
"""EXEC0231 fail-closed overlay regression, including unchanged/non-231 paths."""
from apply_trace import apply, ANCHORS
import unittest
class TraceTests(unittest.TestCase):
    def template(self):
        return "\n".join(a for a,b in ANCHORS)
    def test_exact_once(self):
        out=apply(self.template())
        self.assertIn("EXEC0231: opt-in per-stage",out)
        self.assertIn('gk231_trace("WM_PAINT entered")',out)
        for before,after in ANCHORS:
            self.assertIn(after,out)
        with self.assertRaises(ValueError):apply(out)
    def test_missing_anchor_rejects(self):
        for before,_ in ANCHORS:
            with self.subTest(anchor=before[:30]):
                with self.assertRaises(ValueError):
                    apply(self.template().replace(before,"CHANGED",1))
    def test_no_mutation_on_failure(self):
        data=self.template().replace(ANCHORS[-1][0],"not the current loop")
        saved=data[:]
        with self.assertRaises(ValueError):apply(data)
        self.assertEqual(data,saved)
if __name__=="__main__":
    unittest.main(verbosity=2)
