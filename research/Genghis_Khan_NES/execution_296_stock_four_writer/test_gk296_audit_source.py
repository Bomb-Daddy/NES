#!/usr/bin/env python3
"""EXEC0296 adversarial corpus — requires PRIVATE original ROM + real transcript.
Negative controls do not ship or disclose those inputs. All must be rejected.
"""
import copy
import importlib.util
import json
import sys
from pathlib import Path
import unittest

HERE=Path(__file__).parent
spec=importlib.util.spec_from_file_location('gk296_audit',HERE/'gk296_audit_stock_trace_v3.py')
module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)
PRIVATE_ARGS=sys.argv[1:]
sys.argv=sys.argv[:1]
class HostileLiveTrace(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        if len(PRIVATE_ARGS)!=2:raise ValueError('private-ROM and real-trace arguments required')
        cls.rom=Path(PRIVATE_ARGS[0]).read_bytes()
        cls.real=Path(PRIVATE_ARGS[1]).read_text().splitlines()
        cls.w=next(i for i,l in enumerate(cls.real) if l.startswith('W,'))
    def test_authenticated_positive_original_stock(self):
        receipt=module.audit(self.rom,self.real)
        self.assertEqual(receipt['witnessed_source_target_mask_at_first_draw'],'0xf')
        self.assertEqual(receipt['not_source_written_before_first_draw_mask'],'0x30')
        self.assertGreaterEqual(receipt['c289_source_arithmetic_calls_verified'],1)
        self.assertFalse(receipt['native_playable_spring_implemented'])
    def reject(self,lines,rom=None):
        with self.assertRaises(module.TraceError):module.audit(self.rom if rom is None else rom,lines)
    def test_wrong_rom_byte(self):
        raw=bytearray(self.rom);raw[-1]^=1;self.reject(self.real,bytes(raw))
    def test_wrong_rom_length(self):self.reject(self.real,self.rom[:-1])
    def test_wrong_header(self):
        lines=self.real.copy();lines[0]='GK1_293_STOCK_TRACE_V3,FAKE';self.reject(lines)
    def test_false_source_pc(self):
        lines=self.real.copy();x=lines[self.w].split(',');x[2]=str(int(x[2])+1);lines[self.w]=','.join(x);self.reject(lines)
    def test_false_source_bank(self):
        lines=self.real.copy();x=lines[self.w].split(',');x[3]='14';lines[self.w]=','.join(x);self.reject(lines)
    def test_unwitnessed_88_store_injection(self):
        lines=self.real.copy();x=lines[self.w].split(',');x[4]='136';lines[self.w]=','.join(x);self.reject(lines)
    def test_corrupted_store_value(self):
        lines=self.real.copy();x=lines[self.w].split(',');x[6]=str((int(x[6])+1)%256);lines[self.w]=','.join(x);self.reject(lines)
    def test_corrupted_write_commit(self):
        lines=self.real.copy();x=lines[self.w].split(',');x[8]='0';lines[self.w]=','.join(x);self.reject(lines)
    def test_reversed_cpu_cycles(self):
        lines=self.real.copy();x=lines[self.w+1].split(',');x[1]=self.real[self.w].split(',')[1];lines[self.w+1]=','.join(x);self.reject(lines)
    def test_bad_c289_draw(self):
        lines=self.real.copy();k=next(i for i,x in enumerate(lines) if x.startswith('C,'));x=lines[k].split(',');x[6]=str((int(x[6])+1)%32768);lines[k]=','.join(x);self.reject(lines)
    def test_bad_c289_pre_seed(self):
        lines=self.real.copy();k=next(i for i,x in enumerate(lines) if x.startswith('C,'));x=lines[k].split(',');x[4]='ff'*6;lines[k]=','.join(x);self.reject(lines)
    def test_bad_c289_post_seed(self):
        lines=self.real.copy();k=next(i for i,x in enumerate(lines) if x.startswith('C,'));x=lines[k].split(',');x[5]='ff'*6;lines[k]=','.join(x);self.reject(lines)
    def test_missing_last_stage(self):self.reject(self.real[:-1])
    def test_bad_stage_call_count(self):
        lines=self.real.copy();k=next(i for i,x in enumerate(lines) if x.startswith('S,'));x=lines[k].split(',');x[7]='1';lines[k]=','.join(x);self.reject(lines)
    def test_false_ram_claim(self):
        lines=self.real.copy();k=next(i for i,x in enumerate(lines) if x.startswith('S,'));x=lines[k].split(',');x[6]='1';x[9]='0';lines[k]=','.join(x);self.reject(lines)
    def test_duplicate_source_record(self):
        lines=self.real.copy();lines.insert(self.w+1,lines[self.w]);self.reject(lines)
if __name__=='__main__':unittest.main()
