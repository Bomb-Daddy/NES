"""EXEC0029: Fail-closed VM phase signature for GK1 archived native parity tests.

The source-pointer at zero-page $06:$07 is a VM SCRIPT address, not a
6502 CPU PC. A matching signature is necessary for comparing a pair of
frame-timed snapshots, but is never proof of command completion. No
commercial ROM or SRAM is embedded in this source module.
"""
from dataclasses import dataclass


class PhaseEvidenceError(ValueError):
    pass


def word(zp: bytes, address: int) -> int:
    if not 0 <= address < 255 or len(zp) != 256:
        raise PhaseEvidenceError("exact 256-byte ZP and valid word address required")
    return zp[address] | (zp[address + 1] << 8)


@dataclass(frozen=True)
class PhaseSignature:
    vm_script_pc: int
    vm_stack: int
    vm_stack_end: int
    vm_a: int
    vm_b: int
    raw_zp50: int
    raw_zp52: int

    @classmethod
    def from_zeropage(cls, zp: bytes) -> "PhaseSignature":
        if len(zp) != 256:
            raise PhaseEvidenceError("ZP length != 256 bytes")
        return cls(word(zp, 6), word(zp, 2), word(zp, 4),
                   word(zp, 8), word(zp, 12), zp[0x50], zp[0x52])


def same_captured_phase(a: PhaseSignature, b: PhaseSignature) -> bool:
    """Compare the entire evidence tuple, never only VM PC or elapsed frames.

    True does not mean either capture is an end-of-command state, nor that
    the active PRG bank is known; confirm both independently in a native trace.
    """
    if not isinstance(a, PhaseSignature) or not isinstance(b, PhaseSignature):
        raise PhaseEvidenceError("complete PhaseSignature objects required")
    return a == b


# Published, byte-verified EXEC0020 after-capture VM source pointers only.
# These are OBSERVED MODIFIED NES capture contexts, NOT battle rules.
ARCHIVED_AFTER_VM_PC = {
    "forest_reference": 0xCC2B,
    "infantry_unit": 0xCC2B,
    "archer_unit": 0xCC2B,
    "forest_10soldiers": 0xCC2B,
    "forest_20soldiers": 0xCC2B,
    "forest_60soldiers": 0xCC2B,
    "plain_destination": 0xCBC0,
    "castle_destination": 0xCBC0,
    "terrain5": 0xCBC0,
    "terrain1": 0x91C9,
    "terrain4": 0x91C9,
    "terrain0": 0x8FF1,
    "forest_1soldier": 0xE6C5,
}
