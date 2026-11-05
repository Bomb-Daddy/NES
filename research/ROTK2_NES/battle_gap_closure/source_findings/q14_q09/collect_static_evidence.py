#!/usr/bin/env python3
"""Collect bounded source anchors and scenario data from the authorized ROTK II ROM.

This is byte inspection, not game execution. It makes no reachability claim.
The commercial ROM and complete decoded listings remain private inputs.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import sys

EXPECTED_ROM_SHA256 = "815bfcf4fd6eb23a20c2e50dde023c210b273ffb6cd86a93909d803c3643ce46"

# Manually reviewed instruction anchors. Each record checks source identity;
# interpretation and its limits are documented in STATIC_FINDINGS_v1.00.md.
ANCHORS = [
    ("q14_fallback_return", "sengo", 0x8462, "0c 72 b0 cf"),
    ("q14_owned_province_head_insert", "root", 0x5091, "0c 72 b3 0d b1 3d 40 b1"),
    ("q14_ruler_province_prepend", "root", 0x51c1, "3c 09 72 b0 b1 09 72 b3 0c b1"),
    ("province_owner_field", "root", 0x69c9, "a0 0e b1 0e 4c 90 69"),
    ("officer_liege_field", "root", 0x698c, "a0 07 b1 0e aa c9 ff"),
    ("q14_ordinary_no_destination", "sengo", 0x84f6, "3c aa 28 fc e9 b0 4e 04"),
    ("q14_defeated_ruler_zero_return_branch", "sengo", 0x868d, "d8 cb 86"),
    ("q09_individual_destination_prompt", "hsovl2", 0xd989, "e9 8c d7 06"),
    ("q09_individual_writer_call", "hsovl2", 0xd9aa, "e9 48 d6 06"),
    ("q09_whole_army_writer_call", "hsovl2", 0xd8f2, "e9 48 d6 06"),
    ("q09_detach_battle_list", "hsovl2", 0xd679, "e9 70 7d 04"),
    ("q09_clear_battle_hex", "hsovl2", 0xd67e, "e9 95 93 02"),
    ("q09_peaceful_destination_troop_division", "hsovl2", 0xd69a, "8d 64 0c 7c b4 b3 b0 b8 b1"),
    ("q09_destination_province_insertion", "hsovl2", 0xd6a5, "e9 54 51 04"),
    ("q09_failed_escape_capture_call", "hsovl2", 0xd6c8, "e9 2d 7e 08"),
    ("scenario_officer_table_base", "settei", 0x93f3, "8a 4c e0"),
    ("scenario_officer_stride", "settei", 0x9564, "81 d7 8f 15"),
    ("g09_player_assignment_limit", "comroot", 0x9bb5, "8b 64 c8 d8 c0 9b 89 64"),
    ("g09_player_assignment_write", "comroot", 0x9c80, "0c 7c b3 0b b1 cf"),
    ("g09_scenario_multiplier_offset", "settei", 0x7b1f, "8c ce 1f"),
    ("g09_scenario_multiply", "settei", 0x7ade, "0b 7c b4 b3 b0 b5 b1"),
    ("q03_defending_province_pointer_write", "excsub2", 0xc54e, "a4 28 fc a8 2d fc"),
    ("q03_defending_resident_list_read", "hsovl3", 0xd614, "0b 72 b0 72 b0"),
]


def digest(data):
    return hashlib.sha256(data).hexdigest()


def resource_directory(rom):
    result = {}
    offset = 0x7800
    while rom[offset:offset + 16] != bytes(16):
        row = rom[offset:offset + 16]
        name = row[:8].split(b"\0")[0].decode("ascii")
        bank, address, size = struct.unpack_from("<HHI", row, 8)
        file_offset = bank * 0x8000 + (address & 0x7fff)
        result[name] = {
            "archive_row_file_offset": offset,
            "rom_bank": bank,
            "rom_address": address,
            "file_offset": file_offset,
            "size": size,
            "sha256": digest(rom[file_offset:file_offset + size]),
        }
        offset += 16
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--rom", type=Path, required=True)
    parser.add_argument("--out", type=Path, required=True)
    args = parser.parse_args()
    rom = args.rom.read_bytes()
    rom_hash = digest(rom)
    if rom_hash != EXPECTED_ROM_SHA256:
        raise SystemExit("REFUSED: unexpected original ROM hash")
    resources = resource_directory(rom)
    anchors = []
    for identifier, module, wram_address, expected_hex in ANCHORS:
        resource = resources[module]
        start = resource["file_offset"]
        assert rom[start] == 0x4c, f"{module}: unexpected module header"
        base = struct.unpack_from("<H", rom, start + 1)[0] - 3
        file_offset = start + wram_address - base
        expected = bytes.fromhex(expected_hex)
        observed = rom[file_offset:file_offset + len(expected)]
        assert observed == expected, f"{identifier}: source anchor changed"
        anchors.append({
            "id": identifier, "module": module,
            "module_wram_base": hex(base), "wram_address": hex(wram_address),
            "file_offset": hex(file_offset), "bytes": observed.hex(" "),
            "verified": True,
        })
    root = resources["root"]
    resource_name_offset = root["file_offset"] + 0x6c4b - 0x2000
    resource_name = rom[resource_name_offset:resource_name_offset + 8].split(b"\0")[0].decode("ascii")
    assert resource_name == "sdata"
    scenario_resource = resources[resource_name]
    table_start = scenario_resource["file_offset"] + 0x1fce
    table = rom[table_start:table_start + 14 * 4]
    rows = []
    for index in range(14):
        province, officer_id, multiplier = struct.unpack_from("<BHB", table, index * 4)
        rows.append({
            "row": index, "scenario_index": 0 if index < 10 else 1,
            "file_offset": hex(table_start + index * 4),
            "province_index_zero_based": province,
            "officer_id": officer_id, "multiplier": multiplier,
        })
    for subset in (rows[:10], rows[10:]):
        assert len({row["officer_id"] for row in subset}) == len(subset)
    assert {row["multiplier"] for row in rows} == {2, 3, 5}
    result = {
        "evidence_class": "VERIFIED_STATIC",
        "runtime_executed": False,
        "command": [sys.executable, *sys.argv],
        "rom_sha256": rom_hash, "rom_bytes": len(rom),
        "anchors": anchors,
        "scenario_resource": {"name": resource_name, **scenario_resource},
        "scenario_multiplier_table": {
            "resource_offset": "0x1fce", "file_offset": hex(table_start),
            "bytes": len(table), "sha256": digest(table), "rows": rows,
            "max_multiplier": max(row["multiplier"] for row in rows),
            "unique_officer_ids_within_each_scenario_subset": True,
            "conservative_initialization_bound_given_max_base_100": 500,
            "global_reachability_proved": False,
        },
        "limitations": [
            "No emulator or original VM execution is performed.",
            "Instruction meaning is a manual static interpretation, not inferred from symbol names.",
            "Initialization bound does not prove the bound over every reachable strategic state.",
        ],
    }
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(result, indent=2) + "\n")
    print(json.dumps({"anchors_verified": len(anchors), "scenario_rows": len(rows),
                      "max_multiplier": 5, "global_reachability_proved": False}))


if __name__ == "__main__":
    main()
