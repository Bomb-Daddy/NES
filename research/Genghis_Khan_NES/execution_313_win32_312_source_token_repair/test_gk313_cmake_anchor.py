#!/usr/bin/env python3
"""EXEC0313: eight hostile checks for EXEC0312 Windows CMake source staging.
This tests executable source transformation, not stock-NES first Spring.
"""
import importlib.util
from pathlib import Path
import unittest

stage_path = Path(__file__).resolve().parents[1] / "execution_312_original_event_pc_witness" / "stage312.py"
spec = importlib.util.spec_from_file_location("gk312_stage", stage_path)
stage = importlib.util.module_from_spec(spec)
spec.loader.exec_module(stage)
NATIVE = ("add_library(gk1_app_state STATIC\n"
          " gk302_stock_seed_timeline.c gk304_stock_store_value.c gk282_stock_rng_trace.c\n"
          ")\n")
TEST = ("add_executable(test_gk304_stock_store_value test_gk304_stock_store_value.c)\n"
        "add_test(NAME GK1_304_PublicRegisterStoreRefusalNoROM COMMAND test_gk304_stock_store_value)\n")

class Tests(unittest.TestCase):
    def test_source_vs_test_name_collision(self):
        original = NATIVE + TEST
        self.assertEqual(original.count("gk304_stock_store_value.c"), 2)
        result = stage.insert_312_native_source(original)
        self.assertEqual(result.count("gk312_stock_event_pc.c"), 1)
        self.assertEqual(result.count("test_gk304_stock_store_value.c"), 1)
        self.assertIn("gk304_stock_store_value.c gk312_stock_event_pc.c gk282", result)

    def test_prior_stage_fails_for_observed_shape(self):
        with self.assertRaises(ValueError):
            stage.one(NATIVE+TEST, "gk304_stock_store_value.c", "anything")

    def test_duplicate_native_entry_refused(self):
        with self.assertRaisesRegex(ValueError, "exactly one"):
            stage.insert_312_native_source(NATIVE+NATIVE+TEST)

    def test_missing_native_entry_refused(self):
        with self.assertRaisesRegex(ValueError, "exactly one"):
            stage.insert_312_native_source(TEST)

    def test_prefixed_native_name_refused(self):
        with self.assertRaisesRegex(ValueError, "exactly one"):
            stage.insert_312_native_source(NATIVE.replace("gk304_stock_store_value.c","xgk304_stock_store_value.c")+TEST)

    def test_suffixed_native_name_refused(self):
        with self.assertRaisesRegex(ValueError, "exactly one"):
            stage.insert_312_native_source(NATIVE.replace("gk304_stock_store_value.c","gk304_stock_store_value.c_tmp")+TEST)

    def test_exact_byte_span_insertion(self):
        original=NATIVE+TEST
        result=stage.insert_312_native_source(original)
        self.assertTrue(result.endswith("\n"))
        self.assertEqual(len(result),len(original)+len(" gk312_stock_event_pc.c"))

    def test_ambiguous_bare_comment_refused(self):
        with self.assertRaisesRegex(ValueError,"exactly one"):
            stage.insert_312_native_source(NATIVE+"# gk304_stock_store_value.c\n"+TEST)

if __name__ == "__main__":
    unittest.main(verbosity=2)
