#!/usr/bin/env python3
"""Read-only original-ROM battle census from full physical NES SRAM bank 1.

CPU $6000..$7FFF corresponds to physical SRAM $2000..$3FFF here. This
deliberately does not assume that the live CPU window currently maps bank 1.
Field labels are the current research hypotheses; preserve raw record bytes.
"""
from __future__ import annotations
import argparse
import hashlib
import json
from pathlib import Path

ROM_SHA = "11e312691b516e5e29dade3c8ce5434df3561cc69e1aecad9c948acdfddc517a"


def census(sram: bytes, rom: bytes) -> dict:
    if len(sram) != 32768:
        raise ValueError("Full physical SRAM must contain exactly 32768 bytes")
    if len(rom) != 524304 or hashlib.sha256(rom).hexdigest() != ROM_SHA:
        raise ValueError("Original NES ROM identity does not match")
    bank = sram[0x2000:0x4000]

    def raw(address, size):
        if address < 0x6000 or address + size > 0x8000:
            raise ValueError(f"Address outside bank1 CPU view: {address:04X}")
        return bank[address - 0x6000:address - 0x6000 + size]

    def u(address, size=2):
        return int.from_bytes(raw(address, size), "little")

    def name(name_id):
        if name_id >= 352:
            return None
        names = []
        for part in range(2):
            offset = 0x34014 + name_id * 4 + part * 2
            ptr = int.from_bytes(rom[offset:offset + 2], "little")
            if not 0xA584 <= ptr < 0xC000:
                raise ValueError(f"Name pointer outside source bank: {ptr:04X}")
            offset = 0x34010 + ptr - 0xA000
            value = rom[offset:offset + 32].split(b"\0", 1)[0]
            names.append(value.decode("ascii"))
        return " ".join(names)

    def chain(head):
        items, seen = [], set()
        p = head
        while p:
            if p in seen:
                raise ValueError(f"Cycle in officer chain: {p:04X}")
            if not 0x6031 <= p < 0x6031 + 215 * 21 or (p - 0x6031) % 21:
                raise ValueError(f"Invalid officer pointer: {p:04X}")
            seen.add(p)
            name_id = u(p + 0x13)
            troops = u(p + 0x0C)
            pos = u(p + 0x12, 1)
            items.append({"pointer": f"{p:04X}", "record_slot": (p - 0x6031) // 21,
                          "next": f"{u(p):04X}", "name_id": name_id,
                          "name": name(name_id), "troops_raw": troops,
                          "troops_raw_div100": troops / 100,
                          "mobility_raw": u(p + 0x11, 1),
                          "position_raw": pos,
                          "position_xy_nibbles": None if pos == 255 else [pos >> 4, pos & 15],
                          "cell_index13": None if pos == 255 else (pos & 15) * 13 + (pos >> 4),
                          "record_hex": raw(p, 21).hex()})
            p = u(p)
        return {"head": f"{head:04X}", "count": len(items),
                "troops_raw": sum(i["troops_raw"] for i in items), "officers": items}

    defprovince = u(0x7BF7)
    province_index, rem = divmod(defprovince - 0x73F4, 25)
    if not 0 <= province_index < 41 or rem:
        raise ValueError(f"Invalid defending province pointer: {defprovince:04X}")
    lists = {"defender_field": chain(u(0x7BF9)),
             "defender_resident": chain(u(defprovince + 2)),
             "attacker_field": chain(u(0x7C0B))}
    field_ptrs = {o["pointer"] for o in lists["defender_field"]["officers"]}
    resident_ptrs = {o["pointer"] for o in lists["defender_resident"]["officers"]}
    if field_ptrs & resident_ptrs:
        raise ValueError("Defender field and resident lists overlap")
    return {"source": {"sram_sha256": hashlib.sha256(sram).hexdigest(),
                        "rom_sha256": ROM_SHA, "physical_bank": 1,
                        "physical_bank_offset": "2000", "cpu_origin": "6000"},
            "defending_province": {"pointer": f"{defprovince:04X}",
                                    "number": province_index + 1,
                                    "record_hex": raw(defprovince, 25).hex()},
            "fields": {"rice_defender_7C1D": u(0x7C1D, 4),
                       "rice_attacker_7C21": u(0x7C21, 4),
                       "weather_7C25": u(0x7C25, 1), "wind_7C26": u(0x7C26, 1),
                       "day_7D1C": u(0x7D1C, 1), "old_day_7D25": u(0x7D25, 1)},
            "lists": lists,
            "defender_total_troops_raw": sum(lists[n]["troops_raw"] for n in
                                                ("defender_field", "defender_resident")),
            "list_slots_raw_7BF5_27bytes": raw(0x7BF5, 27).hex(),
            "occupancy_nonzero_7DF4": [[i, v] for i, v in enumerate(raw(0x7DF4, 156)) if v],
            "terrain_7D58_hex": raw(0x7D58, 156).hex(),
            "region_7E90_hex": raw(0x7E90, 156).hex()}


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--sram", required=True, type=Path)
    ap.add_argument("--rom", required=True, type=Path)
    ap.add_argument("--output", required=True, type=Path)
    args = ap.parse_args()
    report = census(args.sram.read_bytes(), args.rom.read_bytes())
    report["source"]["sram_file"] = str(args.sram.resolve())
    with args.output.open("x") as f:
        json.dump(report, f, indent=2)
        f.write("\n")
    print(json.dumps({"output": str(args.output), "defending_province": report["defending_province"]["number"],
                      "lists": {k: {f: v[f] for f in ("count", "troops_raw")} for k, v in report["lists"].items()},
                      "fields": report["fields"]}))


if __name__ == "__main__":
    main()
