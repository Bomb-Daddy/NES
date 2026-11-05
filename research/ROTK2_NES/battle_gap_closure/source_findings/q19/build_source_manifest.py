#!/usr/bin/env python3
"""Record Q19 source anchors without redistributing original module bytes."""
import argparse
import hashlib
import json
import re
from pathlib import Path
from probe_native_q19 import ROM_SHA, resource

RANGES = {
    "root": (0x2000, [
        ("rng_and_static_officer_fields", 0x24c8, 0x2663),
        ("eligibility_owner_and_ruler_helpers", 0x3fd9, 0x4129),
        ("officer_and_province_link_mutation", 0x4eb0, 0x5240),
        ("officer_stat_maximum", 0x606f, 0x608b),
        ("compatibility_distance", 0x64ca, 0x64eb),
        ("native_numeric_link_and_candidate_helpers", 0x66aa, 0x6b44),
    ]),
    "comroot": (0x7200, [
        ("probability_helpers", 0x76c4, 0x7b31),
        ("termination_and_joint_agreement_validation", 0x8277, 0x83d6),
        ("accepted_diplomacy_and_recruitment_effects", 0x85cc, 0x899a),
        ("transfer_mutations", 0x98f4, 0x9a44),
        ("transfer_and_vulnerable_target_helpers", 0x9eab, 0xa123),
        ("partner_and_rival_selection", 0xa191, 0xa2c6),
        ("diplomatic_covert_result_dispatch", 0xaba4, 0xafb3),
        ("interception_and_common_travel", 0xb2fb, 0xb68e),
        ("spy_activation", 0xb891, 0xb979),
        ("foreign_recruitment", 0xb97a, 0xbb24),
        ("native_actor_queue", 0xbba3, 0xbc77),
    ]),
    "csovl1": (0xcd00, [
        ("partner_diplomacy", 0xcd0c, 0xce54),
        ("covert_action", 0xce55, 0xcf7b),
        ("rival_frontier", 0xcf7c, 0xd003),
        ("joint_action", 0xd004, 0xd130),
        ("preferred_partner_wrapper", 0xd131, 0xd15a),
        ("five_way_planner", 0xd15b, 0xd21f),
        ("route_transfer_destination", 0xd220, 0xd393),
        ("adjacent_transfer_destination", 0xd394, 0xd408),
        ("transfer_main", 0xd409, 0xd584),
        ("hostile_attack_contrast_and_fallback", 0xd926, 0xd9c4),
        ("strategic_caller", 0xdb77, 0xdccf),
    ]),
    "excsub1": (0xc300, [
        ("spy_activation_message", 0xc35e, 0xc3ba),
        ("recruitment_mode_messages", 0xca59, 0xcaf0),
    ]),
    "excsub5": (0xc300, [
        ("spy_planting_advisor_counterspy", 0xc30d, 0xc5a8),
    ]),
}


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--rom", type=Path, required=True)
    parser.add_argument("--source-dir", type=Path, required=True)
    parser.add_argument("--out", type=Path, required=True)
    args = parser.parse_args()
    rom = args.rom.read_bytes()
    assert hashlib.sha256(rom).hexdigest() == ROM_SHA
    result = {
        "evidence": "VERIFIED_STATIC original ROM identity and exact listed-byte equality; semantic interpretation is in the accompanying report",
        "rom_sha256": ROM_SHA,
        "symbols_repo_commit": "7c2bab7d634e22a3f5870fe548d6a0556760be98",
        "generator_commit": "9a436f493bf20f760c5eac49014163c2570e77bd",
        "modules": [],
    }
    for mod, (runtime_base, ranges) in RANGES.items():
        payload, meta = resource(rom, mod)
        source = args.source_dir / (mod + ".src")
        text = source.read_text()
        seen = {}
        for line in text.splitlines():
            match = re.match(r"\s*\$([0-9A-F]{4})\s+(.+)", line)
            if match:
                address = int(match[1], 16)
                tokens = []
                for token in match[2].split():
                    if not re.fullmatch(r"[0-9A-F]{2}", token):
                        break
                    tokens.append(token)
                for j, value in enumerate(bytes.fromhex(" ".join(tokens))):
                    assert address + j not in seen, (mod, "overlap", hex(address + j))
                    seen[address + j] = value
        assert len(seen) == len(payload), (mod, "size mismatch", len(seen), len(payload))
        assert set(seen) == set(range(runtime_base, runtime_base + len(payload))), (mod, "coverage mismatch")
        assert bytes(seen[a] for a in range(runtime_base, runtime_base + len(payload))) == payload, (mod, "byte mismatch")
        anchors = []
        for label, lo, hi in ranges:
            assert runtime_base <= lo <= hi < runtime_base + len(payload), (mod, label, "bad range")
            chunk = payload[lo - runtime_base:hi + 1 - runtime_base]
            file_lo = int(meta["file_offset"], 16) + lo - runtime_base
            anchors.append({"purpose": label, "wram_start": f"7E:{lo:04X}", "wram_end_inclusive": f"7E:{hi:04X}",
                            "rom_file_offset": hex(file_lo), "size": len(chunk), "sha256": hashlib.sha256(chunk).hexdigest()})
        result["modules"].append({**meta, "wram_base": f"7E:{runtime_base:04X}",
                                   "module_sha256": hashlib.sha256(payload).hexdigest(),
                                   "listing_path": str(source), "listing_sha256": hashlib.sha256(source.read_bytes()).hexdigest(),
                                   "all_listing_bytes_match_rom": True, "listed_bytes": len(seen), "anchors": anchors})
    result["summary"] = {"modules_verified": len(result["modules"]),
                           "listed_bytes_verified": sum(m["listed_bytes"] for m in result["modules"]),
                           "anchors": sum(len(m["anchors"]) for m in result["modules"]), "status": "PASS"}
    args.out.write_text(json.dumps(result, indent=2) + "\n")
    print(json.dumps(result["summary"], sort_keys=True))


if __name__ == "__main__":
    main()
