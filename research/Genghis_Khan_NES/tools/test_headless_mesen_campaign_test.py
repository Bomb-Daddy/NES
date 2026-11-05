#!/usr/bin/env python3
"""Offline regression checks for headless_mesen_campaign_test.py (no ROM required)."""
import argparse
import importlib.util
import json
from pathlib import Path
import tempfile
import unittest

MODULE_PATH = Path(__file__).with_name("headless_mesen_campaign_test.py")
spec = importlib.util.spec_from_file_location("headless_mesen_campaign_test", MODULE_PATH)
m = importlib.util.module_from_spec(spec)
spec.loader.exec_module(m)


def fixture():
    ram = bytearray(8192)
    def put(addr, value):
        p = addr - 0x6000
        ram[p:p+2] = int(value).to_bytes(2, "little")
    b = 0x6DCD
    for adr, val in [
        (0x65BB,1186),(0x65BD,0),(0x7B36,3),(0x7A8A,55),
        (b+0x40,832),(b+0x36,443),(b+0x22,160),(b+0x42,129),
        (b+0x18,146),(b+0x3E,62),(b+0x1C,46),(b+0x20,15),(b+0x3A,31),
        (b+0x49,65),(b+0x14,108),(b+0x16,601),(b+0x1A,414),
        (b+0x51,0x6900),
    ]: put(adr,val)
    for i, v in enumerate([137,102,125,119,170,129]): put(0x6900+14+2*i,v)
    return bytes(ram)


class Decoder(unittest.TestCase):
    def test_decode_1186_and_policy(self):
        state = m.decode_gk1(fixture())
        self.assertEqual((state["year"],state["season_index"],state["orders"]),(1186,0,3))
        self.assertEqual((state["soldiers"],state["civilians"],state["population"]),(146,154,300))
        self.assertEqual(state["ruler_abilities"]["Leadership"],137)
        policy = m.policy_v2_read_only(state)
        self.assertEqual(policy["army_target"],180)
        self.assertEqual(policy["soldier_shortfall"],34)
        self.assertEqual(policy["winter_10_hire_gold_at_rate_140"],144)
        self.assertEqual(policy["leadership_gap"],314)
        self.assertFalse(policy["winter_hire_needed_to_be_attempted"])

    def test_policy_year_boundaries(self):
        st=m.decode_gk1(fixture())
        for year,target in [(1175,70),(1176,80),(1186,180),(1193,250),(1205,250)]:
            st["year"]=year
            self.assertEqual(m.policy_v2_read_only(st)["army_target"],target)
        st["season_name"]="Winter"
        self.assertTrue(m.policy_v2_read_only(st)["winter_hire_needed_to_be_attempted"])

    def test_truncated_memory_rejected(self):
        with self.assertRaises(ValueError): m.decode_gk1(b"\x00"*8191)
        with self.assertRaises(ValueError): m.w(fixture(),0x5fff)

    def test_near_end_officer_pointer_safe(self):
        data=bytearray(fixture())
        offset=0x6DCD+0x51-0x6000
        data[offset:offset+2]=(0x7FE7).to_bytes(2,"little")
        self.assertIsNone(m.decode_gk1(bytes(data))["ruler_abilities"])

    def test_invalid_officer_pointer_safe(self):
        b=bytearray(fixture())
        pos=0x6DCD+0x51-0x6000
        b[pos:pos+2]=b"\x00\x00"
        self.assertIsNone(m.decode_gk1(bytes(b))["ruler_abilities"])

    def test_expected_checkpoint_negative_control(self):
        st=m.decode_gk1(fixture())
        m.assert_expected(st,{"year":1186,"gold":832})
        with self.assertRaises(AssertionError): m.assert_expected(st,{"year":1185})

    def test_ppm_valid_and_truncated(self):
        with tempfile.TemporaryDirectory() as td:
            p=Path(td)/"frame.ppm"
            p.write_bytes(b"P6\n256 240\n255\n"+b"\0"*(256*240*3))
            details=m.validate_ppm(p)
            self.assertEqual((details["width"],details["height"]),(256,240))
            p.write_bytes(b"P6\n256 240\n255\n"+b"\0"*20)
            with self.assertRaises(AssertionError):m.validate_ppm(p)

    def test_no_overwrite_outputs(self):
        with tempfile.TemporaryDirectory() as td:
            path=Path(td)/"new"
            m.clean_output_dir(path)
            (path/"old.log").write_text("archived")
            with self.assertRaises(ValueError):m.clean_output_dir(path)

    def test_cli_rejects_restore_without_checkpoint(self):
        args=m.parser().parse_args(["--mode","smoke","--restore","old.mss"])
        with self.assertRaises(ValueError):m.preflight(args)

    def test_invalid_hash_argument(self):
        args=m.parser().parse_args(["--expected-rom-sha256","123"])
        with self.assertRaises(ValueError):m.preflight(args)

    def test_raw_rom_hash_known_pattern(self):
        self.assertEqual(len(m.GK1_ROM_SHA256),64)
        self.assertEqual(len(bytes.fromhex(m.GK1_ROM_SHA256)),32)


class AsyncCheckpointTests(unittest.TestCase):
    class FakeEmulator:
        def __init__(self, states):
            self.states = iter(states)
            self.steps = 0
        def tool(self, op, **kwargs):
            if op == "state.status":
                return next(self.states)
            if op == "run.step_frames":
                self.steps += kwargs["frames"]
                return {"ok": True}
            raise AssertionError(op)

    def test_async_restore_pending_then_loaded(self):
        f=self.FakeEmulator([{"operation":"pending"}, {"operation":"pending"}, {"operation":"loaded"}])
        response=m.await_state_operation(f, "loaded")
        self.assertEqual(response["wait_frames"],4)
        self.assertEqual(f.steps,4)

    def test_async_restore_pending_not_sufficient(self):
        f=self.FakeEmulator([{"operation":"pending"}]*5)
        with self.assertRaises(RuntimeError):m.await_state_operation(f, "loaded", limit_frames=8)

    def test_save_completion_is_distinct_from_restore(self):
        f=self.FakeEmulator([{"operation":"pending"}, {"operation":"saved"}])
        self.assertEqual(m.await_state_operation(f,"saved")["state_status"]["operation"],"saved")


if __name__ == "__main__":
    unittest.main(verbosity=2)
