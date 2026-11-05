#!/usr/bin/env python3
"""Record a reviewed troop-writer search and bounded entry-map facts.

This consumes the authorized original ROM and private regenerated listings. It
does not run the game, emulate a CPU/VM, or prove legitimate-play reachability.
Only metadata, hashes, classifications, and short anchors are written out.
"""
import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path
import platform
import re
import struct
import sys

from collect_static_evidence import EXPECTED_ROM_SHA256, resource_directory


def digest(data):
    return hashlib.sha256(data).hexdigest()


# These categories were assigned by manual review of the calling context and
# instruction listing. Symbol names were not used as record-type authority.
CLASSIFICATIONS = {
    ("comroot", 0x8C11): ("officer_troops", "zero"),
    ("comroot", 0x9A45): ("officer_troops", "battle_entry_mul100"),
    ("comroot", 0x9BA1): ("officer_troops", "assignment_cap100"),
    ("comroot", 0xAF28): ("officer_troops", "battle_return_udiv100"),
    ("comroot", 0xB522): ("officer_troops", "battle_reentry_mul100"),
    ("comroot", 0xB836): ("province_population", "recruitment_deduction"),
    ("csovl2", 0xD336): ("province_population", "return_to_population_cap30000"),
    ("csovl2", 0xD584): ("officer_troops", "assignment_constant100"),
    ("csovl2", 0xD5BA): ("officer_troops", "recruitment_add_cap100"),
    ("csovl3", 0xCD67): ("province_population", "dismissal_return_cap30000"),
    ("excsub2", 0xC580): ("officer_troops", "battle_entry_mul100"),
    ("excsub3", 0xC6C3): ("officer_troops", "battle_entry_mul100"),
    ("excsub3", 0xC794): ("officer_troops", "battle_entry_mul100"),
    ("excsub4", 0xC3AA): ("officer_troops", "zero_for_redistribution"),
    ("excsub4", 0xC3F0): ("officer_troops", "redistribution_cap100"),
    ("excsub5", 0xC30D): ("officer_troops", "clear_then_refill_at_most100"),
    ("excsub6", 0xC30C): ("officer_troops", "saturating_subtraction"),
    ("excsub6", 0xC5E2): ("officer_troops", "battle_return_udiv100"),
    ("excsub6", 0xCB4B): ("officer_troops", "battle_entry_mul100"),
    ("hex", 0x7585): ("officer_troops", "battle_subtraction"),
    ("hex", 0x75EC): ("officer_troops", "battle_subtraction"),
    ("hex", 0x7E2D): ("officer_troops", "zero"),
    ("hex", 0xB75C): ("officer_troops", "battle_subtraction"),
    ("hex", 0xB7D5): ("officer_troops", "battle_subtraction"),
    ("hsovl2", 0xD648): ("officer_troops", "peaceful_return_udiv100"),
    ("hsovl3", 0xD693): ("officer_troops", "new_battle_entry_mul100"),
    ("hsovl4", 0xCE27): ("officer_troops", "peaceful_return_udiv100"),
    ("hsovl5", 0xD367): ("officer_troops", "battle_percentage_reduction"),
    ("kisetsu", 0x7670): ("ruler_troop_aggregate", "aggregate_write"),
    ("kisetsu", 0x81B4): ("officer_troops", "peaceful_event_percentage"),
    ("kisetsu", 0x821B): ("province_population", "event_population_change"),
    ("kisetsu", 0x8C39): ("officer_troops", "event_percentage_reduction"),
    ("kisetsu", 0x8F31): ("province_population", "population_growth_cap30000"),
    ("root", 0x4EB0): ("officer_troops", "zero"),
    ("root", 0x5122): ("officer_troops", "zero"),
    ("sengo", 0x721A): ("officer_troops", "peaceful_return_udiv100"),
    ("sengo", 0x735C): ("officer_troops", "battle_result_udiv100"),
    ("sengo", 0x82A4): ("officer_troops", "battle_return_udiv100"),
    ("sengo", 0x837D): ("officer_troops", "battle_reentry_mul100"),
    ("sengo", 0x85FE): ("officer_troops", "prisoner_udiv100"),
    ("sengo", 0x8749): ("officer_troops", "prisoner_udiv100_or_battle_reentry_mul100"),
    ("settei", 0x7370): ("officer_troops", "new_ruler_constant100_or10"),
    ("settei", 0x7AB6): ("officer_troops", "scenario_multiplier2_3_or5"),
    ("settei", 0x932C): ("ruler_troop_aggregate", "initialize_zero"),
    ("settei", 0x93D6): ("officer_troops", "scenario_constant100_50_or10"),
}

RANGES = [
    ("root", 0x2066, 0x2097, "continue_without_terminal_result_overlay"),
    ("root", 0x23B1, 0x23E6, "month_strategy_then_battle_resume_order"),
    ("root", 0x4072, 0x408F, "alliance_bit_test"),
    ("root", 0x4820, 0x48C0, "continued_battle_scheduler"),
    ("root", 0x5154, 0x51CC, "resident_officer_insertion"),
    ("comroot", 0x7487, 0x7675, "strategic_source_scheduler"),
    ("comroot", 0x8522, 0x85CC, "mutual_alliance_clear"),
    ("comroot", 0x9A45, 0x9B3A, "allied_send_and_scale"),
    ("hsovl3", 0xD31E, 0xD362, "entry_region_decode"),
    ("hsovl3", 0xD362, 0xD48C, "battle_map_load"),
    ("hsovl3", 0xD900, 0xD9F8, "pending_ally_return_without_troop_write"),
    ("hsovl3", 0xDA43, 0xDAC7, "new_vs_resumed_setup"),
    ("hsovl1", 0xD14D, 0xD1A3, "serialize_pending_heads_and_flags"),
    ("hsovl1", 0xD2A4, 0xD2C1, "thirty_day_continuation_without_return"),
    ("hsovl6", 0xCDF6, 0xCE45, "stop_placement_when_no_cell"),
    ("hsovl6", 0xCF94, 0xD016, "commit_only_placed_auxiliaries"),
    ("hex", 0xBEB9, 0xBF79, "native_available_entry_cells"),
]

ANCHORS = [
    ("comroot", 0x752E, "a4 5f f8 7f d3 8c ff 00 c1", "source_battle_marker_test"),
    ("comroot", 0x753F, "d8 7d 75", "enter_command_path_only_when_peaceful"),
    ("comroot", 0x9A94, "8d 64 0a 7c b4 b3 b0 b5 b1", "pending_ally_troop_mul100"),
    ("hsovl3", 0xD946, "e9 72 40 04 d7 dd d9", "alliance_present_skips_return"),
    ("hsovl3", 0xD9B3, "e9 d0 69 02 27 37 08 b1", "append_pending_to_resident_tail"),
    ("hsovl3", 0xD9DB, "40 b1", "clear_pending_head"),
    ("hsovl3", 0xDA72, "ac 00 d9", "resume_calls_pending_return"),
    ("hex", 0xBF0D, "b1 12 29 2f d0 1b", "entry_cell_exclusion_mask"),
    ("hsovl6", 0xCFA1, "8c ff 00 c0 d8 a9 cf cf", "unplaced_first_auxiliary_kept_pending"),
]


def listing_bytes(path):
    result = {}
    pattern = re.compile(r"^\s*\$([0-9A-F]{4})\s+((?:[0-9A-F]{2}(?:\s+|$))+)" )
    for line in path.read_text().splitlines():
        match = pattern.match(line)
        if not match:
            continue
        address = int(match[1], 16)
        data = bytes.fromhex(match[2])
        for index, value in enumerate(data):
            assert address + index not in result
            result[address + index] = value
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--rom", type=Path, required=True)
    parser.add_argument("--listings", type=Path, required=True)
    parser.add_argument("--out", type=Path, required=True)
    args = parser.parse_args()
    rom = args.rom.read_bytes()
    if digest(rom) != EXPECTED_ROM_SHA256:
        raise SystemExit("REFUSED: unexpected authorized ROM hash")
    resources = resource_directory(rom)
    writers = []
    sources = []
    pattern = re.compile(r"(?:mem\[.*?\+ \$C\)\]\s*=|sub_67(?:03|30)\(\([^\n]+\+ \$C\))")
    for source in sorted(args.listings.glob("*.c")):
        entry = None
        sources.append({"module": source.stem, "c_sha256": digest(source.read_bytes()),
                        "src_sha256": digest(source.with_suffix(".src").read_bytes())})
        for line_number, line in enumerate(source.read_text().splitlines(), 1):
            match = re.match(r"/\* \$([0-9A-F]{4}) \(body", line)
            if match:
                entry = int(match[1], 16)
            if not pattern.search(line):
                continue
            if (source.stem, entry) == ("kisetsu", 0x7E73):
                if line.lstrip().startswith("mem[(local0 + $C)]"):
                    field, effect = "officer_troops", "event_percentage_reduction"
                else:
                    field, effect = "province_population", "event_population_transfer"
            else:
                assert (source.stem, entry) in CLASSIFICATIONS, (source.stem, entry, line_number)
                field, effect = CLASSIFICATIONS[(source.stem, entry)]
            writers.append({"module": source.stem, "routine_entry": f"0x{entry:04x}",
                            "private_lifted_c_line": line_number, "record_field": field,
                            "reviewed_effect_category": effect})
    assert len(sources) == 29, len(sources)

    cached_listings = {}
    ranges = []
    anchors = []
    for module, start, end, purpose in RANGES:
        resource = resources[module]
        file_start = resource["file_offset"]
        base = struct.unpack_from("<H", rom, file_start + 1)[0] - 3
        image = rom[file_start + start - base:file_start + end - base]
        if module not in cached_listings:
            cached_listings[module] = listing_bytes(args.listings / (module + ".src"))
        listing = bytes(cached_listings[module][address] for address in range(start, end))
        assert image == listing, purpose
        ranges.append({"module": module, "purpose": purpose, "start": f"0x{start:04x}",
                       "end_exclusive": f"0x{end:04x}", "file_offset": hex(file_start + start - base),
                       "bytes": len(image), "sha256": digest(image), "listing_matches_rom": True})
    for module, start, expected_hex, purpose in ANCHORS:
        resource = resources[module]
        file_start = resource["file_offset"]
        base = struct.unpack_from("<H", rom, file_start + 1)[0] - 3
        expected = bytes.fromhex(expected_hex)
        offset = file_start + start - base
        observed = rom[offset:offset + len(expected)]
        assert expected == observed, purpose
        anchors.append({"module": module, "address": hex(start), "file_offset": hex(offset),
                        "purpose": purpose, "bytes": observed.hex(" "), "verified": True})

    h = resources["hpoint"]
    s = resources["sdata"]
    hpoint = rom[h["file_offset"]:h["file_offset"] + h["size"]]
    sdata = rom[s["file_offset"]:s["file_offset"] + s["size"]]
    maps = []
    total_regions = 0
    for province_index in range(41):
        nibble_bytes = hpoint[4 + province_index * 78:4 + (province_index + 1) * 78]
        adjacency = sdata[0x9f7c + province_index * 6:0x9f7c + (province_index + 1) * 6]
        codes = [nibble for byte in nibble_bytes for nibble in (byte >> 4, byte & 15)]
        counts = Counter(codes)
        assert len(codes) == 156 and set(codes) <= set(range(8))
        assert counts[7] == 20
        regions = []
        for direction, neighbor in enumerate(adjacency, 1):
            count = counts[direction]
            if neighbor == 255:
                assert count == 0
            else:
                assert count == 5
                regions.append({"adjacent_province_number": neighbor, "entry_cells": count})
        total_regions += len(regions)
        maps.append({"province_number": province_index + 1, "home_cells": counts[7],
                     "neighbor_regions": regions,
                     "packed_entry_map_sha256": digest(nibble_bytes)})

    result = {
        "evidence_class": "VERIFIED_STATIC",
        "ordinary_play_reachability_proved": False,
        "original_game_executed": False,
        "command": [sys.executable, *sys.argv], "python": sys.version,
        "platform": platform.platform(), "rom_sha256": digest(rom),
        "search_scope": "All 29 lifted modules: literal +C word assignments and calls to root 6703/6730 with literal +C arguments; manual record-type review.",
        "limitations": [
            "A syntax search is not a proof that every aliased or bulk memory write has been discovered.",
            "Direct list splices such as hsovl3 D900 can change the scale interpretation without writing officer+C.",
            "Byte agreement establishes source identity, not full decoder or semantic correctness.",
            "No CPU/VM, original emulator, Windows implementation, or complete campaign was executed.",
        ],
        "reviewed_writer_matches": writers,
        "writer_match_counts_by_record_field": dict(Counter(row["record_field"] for row in writers)),
        "private_source_hashes": sources,
        "reviewed_transition_ranges": ranges,
        "short_original_byte_anchors": anchors,
        "map_resource": {"name": "hpoint", **h},
        "adjacency_resource": {"name": "sdata", **s},
        "entry_region_summary": {"maps": len(maps), "valid_neighbor_regions": total_regions,
                                 "all_valid_neighbor_regions_have_five_cells": True,
                                 "all_home_regions_have_twenty_cells": True},
        "entry_region_counts": maps,
    }
    args.out.write_text(json.dumps(result, indent=2) + "\n")
    print(json.dumps({"writer_matches": len(writers),
                      "counts": result["writer_match_counts_by_record_field"],
                      "ranges_verified": len(ranges), "anchors_verified": len(anchors),
                      "map_summary": result["entry_region_summary"],
                      "ordinary_play_reachability_proved": False}))


if __name__ == "__main__":
    main()
