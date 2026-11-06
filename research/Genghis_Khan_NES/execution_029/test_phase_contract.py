"""Portable EXEC0029 tests; no private ROM or SRAM required."""
import unittest
from phase_contract import (ARCHIVED_AFTER_VM_PC, PhaseEvidenceError,
                            PhaseSignature, same_captured_phase, word)


class PhaseContractTests(unittest.TestCase):
    def test_decode_source_script_pc(self):
        zp = bytearray(256)
        zp[6:8] = bytes([0xF1, 0x8F])
        zp[0x50], zp[0x52] = 0x13, 0x11
        sig = PhaseSignature.from_zeropage(zp)
        self.assertEqual((sig.vm_script_pc, sig.raw_zp50, sig.raw_zp52),
                         (0x8FF1, 0x13, 0x11))

    def test_byte_order_and_stack(self):
        zp = bytearray(256)
        zp[2:6] = bytes([0xF4, 0x04, 0xFE, 0x04])
        sig = PhaseSignature.from_zeropage(zp)
        self.assertEqual((sig.vm_stack, sig.vm_stack_end), (0x04F4, 0x04FE))

    def test_length_rejection(self):
        for value in (b"", b"\0" * 255, b"\0" * 257):
            with self.assertRaises(PhaseEvidenceError):
                PhaseSignature.from_zeropage(value)

    def test_wrong_address_rejection(self):
        for addr in (-1, 255, 256):
            with self.assertRaises(PhaseEvidenceError):
                word(bytes(256), addr)

    def test_equal_phase_only(self):
        x = PhaseSignature(0xCC2B, 0x04E9, 0x04F9, 0, 1, 6, 0)
        self.assertTrue(same_captured_phase(x, x))
        for pos in range(7):
            fields = list(x.__dict__.values())
            fields[pos] += 1
            self.assertFalse(same_captured_phase(x, PhaseSignature(*fields)))

    def test_incomplete_evidence_rejection(self):
        with self.assertRaises(PhaseEvidenceError):
            same_captured_phase(None, None)

    def test_historical_groups(self):
        self.assertEqual(len(ARCHIVED_AFTER_VM_PC), 13)
        self.assertEqual(set(ARCHIVED_AFTER_VM_PC.values()),
                         {0xCC2B, 0xCBC0, 0x91C9, 0x8FF1, 0xE6C5})
        self.assertEqual(ARCHIVED_AFTER_VM_PC["terrain0"], 0x8FF1)


if __name__ == "__main__":
    unittest.main()
