#!/usr/bin/env python3
"""Audit the authored NES white-marker investigation against private inputs.

Read-only: this script does not run or patch an emulator, and does not copy ROM,
screenshots, memory dumps, native checkpoints, or decoded commercial source.
Requires Pillow. The runtime evidence was produced by unmodified battle_probe.py.
"""
from __future__ import annotations
import argparse
import hashlib
import json
from pathlib import Path
import re
import sys
from PIL import Image, __version__ as pillow_version

ROM_SHA256 = "11e312691b516e5e29dade3c8ce5434df3561cc69e1aecad9c948acdfddc517a"
RUNS = ("white_marker_noinput_001", "white_marker_writer_001", "white_marker_initializer_001")
ROI = (216, 52, 232, 68)
# Actual MMC5 8 KiB bank identities. End addresses are exclusive.
ANCHORS = (
    (8, 0x8000, 0x8120, 0x816B, "acting officer and cursor initialization"),
    (8, 0x8000, 0x81C0, 0x81CC, "disable cursor after command"),
    (14, 0xA000, 0xAEBC, 0xAED5, "hex to tile coordinates"),
    (14, 0xA000, 0xAF98, 0xB02C, "white template and cursor configuration"),
    (31, 0xE000, 0xEEE6, 0xEF06, "VM to native syscall bridge"),
    (31, 0xE000, 0xF91A, 0xF944, "native syscall dispatch"),
    (31, 0xE000, 0xF976, 0xF978, "syscall 25 target FD77"),
    (31, 0xE000, 0xFD77, 0xFDCF, "configure four-sprite blinking template"),
    (31, 0xE000, 0xF8A5, 0xF8FB, "ten-frame blink update"),
    (31, 0xE000, 0xF671, 0xF6BB, "NMI OAM DMA and blink update order"),
)


def digest(data):
    return hashlib.sha256(data).hexdigest()


def file_fact(path, root):
    data = path.read_bytes()
    return {"path": str(path.relative_to(root)), "bytes": len(data), "sha256": digest(data)}


def load_run(root, name):
    folder = root / "runtime/experiments" / name
    receipt = json.loads((folder / "probe_receipt.json").read_text())
    native = json.loads((folder / "process/receipt.json").read_text())
    assert native["exit_code"] == 0 and native["runtime_identity_passed"]
    for product in receipt["products"]:
        data = (folder / product["file"]).read_bytes()
        assert len(data) == product["bytes"], product["file"]
        assert digest(data) == product["sha256"], product["file"]
    if name == "white_marker_noinput_001":
        assert receipt["evidence"] == "INCOMPLETE_RUNTIME"
        assert receipt["failures"] == [
            "Screenshot 1: Missing/invalid native PNG screenshot", "Missing shots: [1]"]
    else:
        assert receipt["evidence"] == "OBSERVED_RUNTIME" and not receipt["failures"]
    schedule = json.loads((folder / "schedule.json").read_text())
    assert not schedule.get("events") and not schedule.get("patches")
    return folder, receipt, native, schedule


def image_phase(path):
    with Image.open(path) as source:
        image = source.convert("RGB")
    assert image.size == (256, 240)
    region = image.crop(ROI)
    return (region.getcolors(257) == [(256, (255, 254, 255))], digest(region.tobytes()))


def parse_writes(folder):
    result = []
    for line in (folder / "process/stdout.log").read_text().splitlines():
        if line.startswith("WRITE "):
            values = [int(x) for x in line.split()[1:]]
            assert len(values) == 9
            result.append(dict(zip(
                ("frame", "address", "old", "new", "callback_pc", "vm_ip",
                 "mapped_vm_prg_offset", "mapped_memory_type", "battle_day"), values)))
    return result


def inspect_snapshot(folder, frame):
    ram = (folder / f"memory_{frame:06d}_cpu0000.bin").read_bytes()
    save = (folder / f"memory_{frame:06d}_cpu6000.bin").read_bytes()
    assert len(ram) == 0x800 and len(save) == 0x2000
    x, y = save[0x1D1A:0x1D1C]
    index = y * 13 + x
    return {
        "frame": frame, "x": x, "y": y, "index": index,
        "occupancy": save[0x1DF4 + index], "terrain": save[0x1D58 + index],
        "current_officer": int.from_bytes(save[0x1D2C:0x1D2E], "little"),
        "template_071a": list(ram[0x71A:0x72A]),
        "blink_enable_072a": ram[0x72A], "phase_072b": ram[0x72B],
        "sprite_slot_072c": ram[0x72C], "countdown_072d": ram[0x72D],
        "shadow_0680": list(ram[0x680:0x690]),
    }


def verify_vm_anchor(root, rom, bank, base, start, stop):
    path = root / "static_sources/nes_marker_sources" / f"bank{bank:02d}_vm.asm"
    matched = bytearray()
    expected_address = start + 5 if rom[16 + bank * 8192 + start - base:][:3] == bytes.fromhex("20 d1 e4") else start
    # VM listing prints the stub in its heading, not as an instruction.
    if expected_address == start + 5:
        matched.extend(rom[16 + bank * 8192 + start - base:16 + bank * 8192 + start - base + 5])
    for line in path.read_text().splitlines():
        m = re.match(r"\s*[ >]\$([0-9A-F]{4})\s+((?:[0-9A-F]{2} )+)", line)
        if not m:
            continue
        addr = int(m[1], 16)
        if not start <= addr < stop:
            continue
        raw = bytes.fromhex(m[2])
        # Account for interior routine stubs in the white-cursor pipeline.
        while expected_address < addr:
            gap = rom[16 + bank * 8192 + expected_address - base:][:5]
            assert gap[:3] == bytes.fromhex("20 d1 e4") and expected_address + 5 <= addr
            matched.extend(gap)
            expected_address += 5
        assert addr == expected_address
        off = 16 + bank * 8192 + addr - base
        assert raw == rom[off:off + len(raw)]
        matched.extend(raw)
        expected_address += len(raw)
    assert expected_address == stop, (hex(start), hex(expected_address), hex(stop))
    assert len(matched) == stop - start
    return {"compared_bytes": len(matched), "omissions": 0, "mismatches": 0,
            "listing": file_fact(path, root)}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--workspace", type=Path, required=True)
    parser.add_argument("--rom", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    root = args.workspace.resolve()
    rom = args.rom.read_bytes()
    assert len(rom) == 524304 and digest(rom) == ROM_SHA256
    result = {"author": "Cao Cao / ChatGPT source audit", "date": "2026-11-05",
              "python": sys.version, "pillow": pillow_version,
              "rom_sha256": ROM_SHA256,
              "evidence": ["VERIFIED_STATIC", "OBSERVED_RUNTIME"],
              "historical_S02_exact_image_identity": "INFERRED / original sheet unavailable",
              "runs": [], "source_anchors": [], "selected_input_files": []}
    for bank, base, start, stop, purpose in ANCHORS:
        offset = 16 + bank * 8192 + start - base
        item = {"bank_8k": bank, "cpu_start": start, "cpu_end_exclusive": stop,
                "file_offset": offset, "bytes": stop - start,
                "sha256": digest(rom[offset:offset + stop - start]), "purpose": purpose}
        if bank in (8, 14):
            item["listing_comparison"] = verify_vm_anchor(root, rom, bank, base, start, stop)
        result["source_anchors"].append(item)
    assert int.from_bytes(rom[0x3F986:0x3F988], "little") == 0xFD77
    for name in RUNS:
        folder, receipt, native, schedule = load_run(root, name)
        facts = {"name": name, "status": receipt["evidence"], "failures": receipt["failures"],
                 "exit_code": native["exit_code"], "native_identity_passed": native["runtime_identity_passed"],
                 "input_state_sha256": receipt["input_state_sha256"],
                 "frames": schedule["stop"], "no_controller_input": True,
                 "no_state_interventions": True, "verified_products": len(receipt["products"]),
                 "receipt": file_fact(folder / "probe_receipt.json", root),
                 "native_receipt": file_fact(folder / "process/receipt.json", root),
                 "stdout": file_fact(folder / "process/stdout.log", root),
                 "stderr": file_fact(folder / "process/stderr.log", root),
                 "schedule": file_fact(folder / "schedule.json", root),
                 "script": file_fact(folder / "probe.lua", root)}
        if name != "white_marker_initializer_001":
            phases = []
            roi_hashes = set()
            for frame in schedule["shots"]:
                if frame == 1:
                    continue
                white, roi_hash = image_phase(folder / f"f{frame:06d}.png")
                assert white == (4 <= frame % 20 <= 13), frame
                phases.append([frame, "white" if white else "unit"])
                roi_hashes.add(roi_hash)
            assert len(roi_hashes) == 2
            facts["roi_xyxy"] = ROI
            facts["roi_sha256_values"] = sorted(roi_hashes)
            facts["verified_frame_phases"] = phases
        if name == "white_marker_writer_001":
            snapshots = [inspect_snapshot(folder, f) for f in schedule["snapshots"]]
            for s in snapshots:
                assert (s["x"], s["y"], s["current_officer"], s["occupancy"], s["terrain"]) == (12, 2, 0x672A, 4, 0)
                assert s["blink_enable_072a"] == 1 and s["sprite_slot_072c"] == 32
                assert s["template_071a"] == [51,224,3,216,51,224,3,224,59,224,3,216,59,224,3,224]
            writes = parse_writes(folder)
            assert len(writes) == 20
            for index, write in enumerate(writes):
                phase, corner = divmod(index, 4)
                assert write["frame"] == 1 + 10 * phase
                assert write["address"] == 0x680 + 4 * corner
                assert write["new"] == ([51,51,59,59][corner] if phase % 2 == 0 else 248)
                assert write["callback_pc"] == (0xF8DD if phase % 2 == 0 else [0xF8F1,0xF8F4,0xF8F7,0xF8FA][corner])
            facts["stable_state_checked_samples"] = len(snapshots)
            facts["snapshot_examples"] = [snapshots[0], snapshots[12]]
            facts["exact_watch_events"] = writes
            facts["occupancy_write_events"] = 0
        if name == "white_marker_initializer_001":
            writes = parse_writes(folder)
            enable = [w for w in writes if w["address"] == 0x72A]
            assert [(w["frame"], w["new"]) for w in enable] == [(41,0),(101,1),(142,0),(162,0),(163,0),(180,1)]
            assert all(w["callback_pc"] == 0xFDCE and w["vm_ip"] == 0xAFBC
                       and w["mapped_vm_prg_offset"] == 0x1CFBC for w in enable)
            facts["enable_events"] = enable
            facts["all_watch_events_count"] = len(writes)
        result["runs"].append(facts)
    comparisons = []
    for name, frame in (("wait_001",2200),("fire_prompt_001",700),("forest_ready_001",2200)):
        folder = root / "runtime/experiments" / name
        receipt = json.loads((folder / "probe_receipt.json").read_text())
        assert receipt["evidence"] == "OBSERVED_RUNTIME" and not receipt["failures"]
        s = inspect_snapshot(folder, frame)
        s["run"] = name
        s["receipt"] = file_fact(folder / "probe_receipt.json", root)
        comparisons.append(s)
        for file in (f"f{frame:06d}.png", f"memory_{frame:06d}_cpu0000.bin", f"memory_{frame:06d}_cpu6000.bin"):
            result["selected_input_files"].append(file_fact(folder / file, root))
    result["comparison_states_inspected"] = comparisons
    assert comparisons[1]["sprite_slot_072c"] == 36
    assert comparisons[2]["occupancy"] == 0x94 and comparisons[2]["sprite_slot_072c"] == 32
    for frame in (4,14,24,34):
        result["selected_input_files"].append(file_fact(root / "runtime/experiments/white_marker_writer_001" / f"f{frame:06d}.png", root))
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + "\n")
    print(json.dumps({"result":"PASS", "output":str(args.output), "sha256":digest(args.output.read_bytes()),
                      "source_ranges":len(ANCHORS), "runtime_runs":len(RUNS),
                      "first_run_retained_as_incomplete":True}))


if __name__ == "__main__":
    main()
