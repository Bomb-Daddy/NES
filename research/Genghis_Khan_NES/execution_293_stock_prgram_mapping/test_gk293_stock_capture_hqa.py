#!/usr/bin/env python3
"""Private-only historical evidence adversarial checks; no ROM bytes leave host."""
import copy
import json
import pathlib
import sys
import tempfile
import unittest
import zipfile
from gk293_verify_existing_stock_capture import (verify_private_evidence,
  EvidenceError, check_views, STAGES)

class PrivateFreshSpringTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.stock=sys.argv[1]
        cls.evidence=sys.argv[2]
        with zipfile.ZipFile(cls.evidence) as z:
            cls.early,cls.late=[json.loads(z.read('private/'+name)) for name in STAGES]
    def test_private_original_rom_and_capture(self):
        result=verify_private_evidence(self.stock,self.evidence)
        self.assertEqual((result['authentic_historical_records'],result['pre_spring_orders'],result['fresh_spring_orders']),(14,0,3))
        self.assertFalse(result['new_mesen_runtime_executed'])
    def test_mutate_each_bus_field_rejected(self):
        count=0
        for sample,label in ((self.early,'before'),(self.late,'after')):
            for k in ('year','gold','food','troops','rate','orders','country_count'):
                bad=copy.deepcopy(sample)
                bad['views']['nesWorkRam'][k]+=1
                with self.subTest(label=label,field=k):
                    with self.assertRaises(EvidenceError):check_views(bad,label)
                count+=1
                bad=copy.deepcopy(sample)
                bad['views']['nesSaveRam'][k]=1
                with self.subTest(label=label,field=k,bank='save'):
                    with self.assertRaises(EvidenceError):check_views(bad,label)
                count+=1
        print('GK293 archived 28/28 adversarial CPU/WorkRAM/SaveRAM field mutations rejected')

if __name__=='__main__':
    # unittest command-line arguments must exclude private paths.
    stock,archive=sys.argv[1:3]
    unittest.main(argv=[sys.argv[0]],verbosity=2)
