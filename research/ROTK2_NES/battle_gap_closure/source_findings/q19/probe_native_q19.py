#!/usr/bin/env python3
"""Bounded original-byte microexecution, not a SNES emulator/runtime test.

Executes only recovered comroot BBA3-BC77 queue code and CPU-bank-00
9EAA-9EF8 arithmetic handlers against synthetic RAM. No game ROM is emitted.
The implementation deliberately rejects instructions/widths outside this scope.
"""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path

ROM_SHA = "815bfcf4fd6eb23a20c2e50dde023c210b273ffb6cd86a93909d803c3643ce46"


def sha(data):
    return hashlib.sha256(data).hexdigest()


def word(mem, at):
    return mem[at & 0xffff] | mem[(at + 1) & 0xffff] << 8


def put(mem, at, value):
    mem[at & 0xffff] = value & 255
    mem[(at + 1) & 0xffff] = value >> 8 & 255


def resource(rom, name):
    for offset in range(0x7800, 0x8000, 16):
        entry = rom[offset:offset + 16]
        if entry[:8].rstrip(b"\0 ").decode("ascii", errors="replace") == name:
            bank = int.from_bytes(entry[8:10], "little")
            addr = int.from_bytes(entry[10:12], "little")
            size = int.from_bytes(entry[12:16], "little")
            file_offset = (bank & 0x7f) * 0x8000 + (addr & 0x7fff)
            return rom[file_offset:file_offset + size], {
                "resource": name, "directory_offset": hex(offset),
                "rom_address": f"{bank:02X}:{addr:04X}",
                "file_offset": hex(file_offset), "size": size,
            }
    raise ValueError(f"resource not found: {name}")


class Bounded65816:
    """Only the instructions actually used by the two audited native blocks.

    Native mode, decimal disabled, DP=0, 16-bit A/X/Y. Synthetic RAM is one
    64-KiB bank. PHP/PLP and JSR/RTS are represented by separate logical stacks;
    this is equivalent here because these blocks never inspect their stack.
    CPU/memory mapping, interrupts, peripherals and game initialization are absent.
    """
    def __init__(self, code, lo, hi, ram=None, max_steps=5000):
        self.code, self.lo, self.hi = code, lo, hi
        self.ram = bytearray(65536) if ram is None else ram
        self.max_steps = max_steps
        self.a = self.y = 0
        self.c = self.z = self.n = False
        self.m = self.x = False
        self.calls, self.status = [], []
        self.trace = []

    def flags(self, value):
        value &= 0xffff
        self.z, self.n = value == 0, bool(value & 0x8000)
        return value

    def byte(self):
        if not self.lo <= self.pc <= self.hi:
            raise RuntimeError(f"instruction fetch outside audited range: {self.pc:04X}")
        value = self.code[self.pc]
        self.pc += 1
        return value

    def immediate(self):
        low = self.byte()
        return low | self.byte() << 8

    def branch(self, condition):
        displacement = self.byte()
        if condition:
            self.pc = (self.pc + (displacement if displacement < 128 else displacement - 256)) & 65535

    def execute(self, start, stop=None):
        self.pc = start
        for steps in range(1, self.max_steps + 1):
            if self.pc == stop:
                return steps - 1
            location, op = self.pc, self.byte()
            self.trace.append(location)
            if self.m or self.x:
                raise RuntimeError("unexpected 8-bit register width")
            if op == 0x08:  # PHP
                self.status.append((self.c, self.z, self.n, self.m, self.x))
            elif op == 0x28:  # PLP
                self.c, self.z, self.n, self.m, self.x = self.status.pop()
            elif op == 0xc2:  # REP
                mask = self.byte()
                if mask != 0x30:
                    raise RuntimeError("unexpected REP mask")
                self.m = self.x = False
            elif op == 0x20:  # JSR
                target = self.immediate()
                self.calls.append(self.pc)
                self.pc = target
            elif op == 0x60:  # RTS
                self.pc = self.calls.pop()
            elif op == 0x6b:  # RTL -- only permitted terminal
                if self.calls or self.status:
                    raise RuntimeError("unbalanced logical native stacks")
                return steps
            elif op == 0x4c:  # JMP
                self.pc = self.immediate()
            elif op == 0xa0:  # LDY immediate
                self.y = self.flags(self.immediate())
            elif op == 0xa9:  # LDA immediate
                self.a = self.flags(self.immediate())
            elif op == 0xa5:  # LDA direct
                self.a = self.flags(word(self.ram, self.byte()))
            elif op == 0xad:  # LDA absolute
                self.a = self.flags(word(self.ram, self.immediate()))
            elif op == 0xb1:  # LDA (direct),Y
                self.a = self.flags(word(self.ram, word(self.ram, self.byte()) + self.y))
            elif op == 0x85:  # STA direct
                put(self.ram, self.byte(), self.a)
            elif op == 0x84:  # STY direct
                put(self.ram, self.byte(), self.y)
            elif op == 0x91:  # STA (direct),Y
                put(self.ram, word(self.ram, self.byte()) + self.y, self.a)
            elif op == 0x29:  # AND immediate
                self.a = self.flags(self.a & self.immediate())
            elif op == 0xc5:  # CMP direct
                value = word(self.ram, self.byte())
                self.c = self.a >= value
                self.flags(self.a - value)
            elif op == 0xd0:
                self.branch(not self.z)
            elif op == 0xf0:
                self.branch(self.z)
            elif op == 0x90:
                self.branch(not self.c)
            elif op == 0xc8:
                self.y = self.flags(self.y + 1)
            elif op == 0x88:
                self.y = self.flags(self.y - 1)
            elif op == 0x18:
                self.c = False
            elif op == 0x38:
                self.c = True
            elif op == 0x4a:  # LSR A
                self.c, self.a = bool(self.a & 1), self.flags(self.a >> 1)
            elif op == 0x65:  # ADC direct (binary)
                value = self.a + word(self.ram, self.byte()) + self.c
                self.c, self.a = value > 65535, self.flags(value)
            elif op == 0xe5:  # SBC direct (binary)
                value = self.a - word(self.ram, self.byte()) - (not self.c)
                self.c, self.a = value >= 0, self.flags(value)
            elif op in (0x06, 0x26, 0x66, 0xe6):
                address = self.byte()
                value = word(self.ram, address)
                if op == 0x06:  # ASL direct
                    self.c, value = bool(value & 0x8000), value << 1
                elif op == 0x26:  # ROL direct
                    old_c = self.c
                    self.c, value = bool(value & 0x8000), value << 1 | old_c
                elif op == 0x66:  # ROR direct
                    old_c = self.c
                    self.c, value = bool(value & 1), value >> 1 | (int(old_c) << 15)
                else:
                    value += 1
                put(self.ram, address, self.flags(value))
            else:
                raise RuntimeError(f"unsupported opcode {op:02X} at {location:04X}")
        raise RuntimeError(f"step limit {self.max_steps} reached")


def queue_fixture(code, entry, next_actor, flags=None, successors=None):
    p, head, sub1, sub2 = 0xf424, 0xd000, 0xd020, 0xd040
    ram = bytearray(65536)
    put(ram, 0xf85f, p)
    put(ram, p + 2, head)
    for pointer, following in (successors or {head: sub1, sub1: sub2, sub2: 0}).items():
        put(ram, pointer, following)
    for pointer, (byte2, byte3) in (flags or {}).items():
        ram[pointer + 2], ram[pointer + 3] = byte2, byte3
    put(ram, 0xfdfa, next_actor)
    before = bytes(ram)
    machine = Bounded65816(code, 0xbba3, 0xbc77, ram)
    steps = machine.execute(entry)
    return {
        "return": word(ram, 0x0a), "actor": word(ram, 0xfd9e),
        "next_actor": word(ram, 0xfdfa), "steps": steps,
        "ram_before_sha256": sha(before), "ram_after_sha256": sha(ram),
        "changed_addresses": [f"{i:04X}" for i in range(65536) if before[i] != ram[i]],
        "executed_address_sha256": sha(bytes().join(x.to_bytes(2, "little") for x in machine.trace)),
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--rom", type=Path, required=True)
    parser.add_argument("--out", type=Path, required=True)
    args = parser.parse_args()
    rom = args.rom.read_bytes()
    if sha(rom) != ROM_SHA:
        raise SystemExit("REFUSED: ROM SHA256 does not match the authorized source")
    comroot, metadata = resource(rom, "comroot")
    code = bytearray(65536)
    code[0x7200:0x7200 + len(comroot)] = comroot
    h, s1, s2 = 0xd000, 0xd020, 0xd040
    specs = [
        ("empty_next_ends", 0xbba3, 0, {}, None, (1, 0, 0)),
        ("pick_subordinate", 0xbba3, s1, {}, None, (0, s1, s2)),
        ("advance_skips_acted", 0xbba3, h, {s1: (1, 0)}, None, (0, h, s2)),
        ("advance_skips_low_status", 0xbba3, h, {s1: (0, 2)}, None, (0, h, s2)),
        ("advance_accepts_high_status", 0xbba3, h, {s1: (0, 0xf0)}, None, (0, h, s1)),
        ("wrap_to_eligible_governor", 0xbba3, s2, {}, None, (0, s2, h)),
        ("no_wrap_to_acted_governor", 0xbba3, s2, {h: (1, 0)}, None, (0, s2, 0)),
        ("reset_starts_subordinate", 0xbbf6, 0, {}, None, (0, s1, s2)),
        ("reset_skips_ineligible_subordinate", 0xbbf6, 0, {s1: (1, 0)}, None, (0, s2, h)),
        ("reset_single_governor", 0xbbf6, 0, {}, {h: 0}, (0, h, 0)),
        ("reset_single_acted_governor_still_selected", 0xbbf6, 0, {h: (1, 0)}, {h: 0}, (0, h, 0)),
        ("reset_ineligible_subordinates_end_even_eligible_governor", 0xbbf6, 0, {s1: (1, 0), s2: (0, 1)}, None, (1, 0, 0)),
    ]
    fixtures = []
    for name, entry, nxt, flags, successors, expected in specs:
        result = queue_fixture(code, entry, nxt, flags, successors)
        actual = tuple(result[k] for k in ("return", "actor", "next_actor"))
        assert actual == expected, (name, actual, expected)
        fixtures.append({"name": name, "entry": f"{entry:04X}",
                         "input": {"next_actor": nxt, "officer_flags_byte2_byte3": flags,
                                   "successors": successors or {h: s1, s1: s2, s2: 0}},
                         "expected": expected, "result": result, "status": "PASS"})

    mutation = bytearray(code)
    assert mutation[0xbc42] == 1
    mutation[0xbc42] = 0
    changed = queue_fixture(mutation, 0xbba3, h, {s1: (1, 0)})
    assert changed["next_actor"] != s2
    negatives = [{"name": "eligibility_mask_byte_mutation_detected", "mutated_address": "7E:BC42", "status": "PASS", "observed_next_actor": changed["next_actor"], "expected_original_next_actor": s2}]
    try:
        queue_fixture(code, 0xbbf6, 0, {s1: (1, 0)}, {h: s1, s1: s1})
    except RuntimeError as err:
        assert "step limit" in str(err)
        negatives.append({"name": "invalid_cyclic_list_fails_closed", "status": "PASS", "diagnostic": str(err)})
    else:
        raise AssertionError("cyclic-list negative control did not fail closed")

    cpu_code = bytearray(65536)
    cpu_code[0x8000:] = rom[:0x8000]
    handlers = {}
    for opcode, expected in ((0xb5, 0x9eaa), (0xb8, 0x9ec8)):
        actual = int.from_bytes(rom[0x20e6 + 2 * opcode:0x20e8 + 2 * opcode], "little")
        assert actual == expected
        handlers[f"{opcode:02X}"] = f"00:{actual:04X}"
    arithmetic = []
    for rice, sent, total in [(450, 50, 150), (6000, 200, 600), (30000, 300, 900), (0, 200, 600), (65535, 1, 65534), (65535, 65535, 65534), (65535, 65535, 65535)]:
        ram = bytearray(65536)
        put(ram, 0x0a, rice)
        put(ram, 0x0e, sent)
        mul = Bounded65816(cpu_code, 0x9eaa, 0x9ef8, ram)
        mul_steps = mul.execute(0x9eaa, stop=0x9a90)
        low_product, high_product = word(ram, 0x0a), word(ram, 0)
        assert low_product == (rice * sent & 65535)
        assert high_product == (rice * sent >> 16)
        denominator = (total + 1) & 65535
        put(ram, 0x0e, denominator)
        div = Bounded65816(cpu_code, 0x9eaa, 0x9ef8, ram)
        div_steps = div.execute(0x9ec8, stop=0x9a90)
        quotient = word(ram, 0x0a)
        expected = low_product // denominator if denominator else 65535
        assert quotient == expected
        arithmetic.append({"rice": rice, "selected_troops": sent, "total_troops": total,
                           "native_product_low": low_product, "native_product_high": high_product,
                           "native_divisor": denominator, "native_rice_transfer": quotient,
                           "widened_formula_for_contrast": rice * sent // (total + 1),
                           "mul_steps": mul_steps, "div_steps": div_steps, "status": "PASS"})

    wrong = bytearray(rom)
    wrong[0] ^= 1
    assert sha(wrong) != ROM_SHA
    negatives.append({"name": "single_changed_rom_byte_breaks_identity", "status": "PASS"})
    result = {
        "evidence": "VERIFIED_STATIC: bounded execution of original native instruction bytes against synthetic RAM; NOT OBSERVED_RUNTIME, NOT an original emulator run",
        "scope": "comroot 7E:BBA3-BC77; CPU 00:9EAA-9EF8; no game startup, graphics, hardware, full VM, or RNG",
        "rom_sha256": sha(rom), "rom_size": len(rom), "comroot": {**metadata, "sha256": sha(comroot)},
        "queue_code_sha256": sha(code[0xbba3:0xbc78]),
        "arithmetic_code_sha256": sha(cpu_code[0x9eaa:0x9ef9]),
        "vm_native_handlers": handlers,
        "synthetic_officer_pointers": {"governor": h, "first_subordinate": s1, "second_subordinate": s2},
        "queue_fixtures": fixtures, "arithmetic_fixtures": arithmetic,
        "negative_controls": negatives,
        "summary": {"queue_pass": len(fixtures), "arithmetic_pass": len(arithmetic), "negative_pass": len(negatives), "status": "PASS"},
    }
    args.out.write_text(json.dumps(result, indent=2) + "\n")
    print(json.dumps(result["summary"], sort_keys=True))
    print(result["evidence"])


if __name__ == "__main__":
    main()
