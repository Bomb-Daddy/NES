#!/usr/bin/env python3
"""Non-destructive Mesen-for-AI smoke / checkpoint test for original NES KOEI games.

Uses the *existing* mesen-for-ai daemon and bridge; does not vendor an emulator,
ROM, firmware, mapper binary, game state, or a complete gameplay AI controller.

Historical GK1 state offsets below apply ONLY to the English USA NES ROM with
SHA-256 ada7066d9... . For other games, take a generic RAM digest, not a
misleading ruler/country interpretation.

Exit: 0 = requested checks passed; 1 = test failure; 2 = bad CLI arguments.
"""
from __future__ import annotations

import argparse
from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import platform
import re
import shutil
import sys
import traceback
from typing import Any

GK1_ROM_SHA256 = "ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3"
GK1_SAVE_START_SEASON = ("Spring", "Summer", "Autumn", "Winter")
ALLOWED_UI_PROBES = ("up", "down", "left", "right")
# Original GK1 title -> default Mongol Conquest/Temujin (historical research prefix).
# Used ONLY in a temporary emulator session BEFORE overwriting it with an .mss.
GK1_BOOT_PREFIX = ([('start', 5, 40)] + [('a', 5, 40)] * 10 +
    [('b', 20, 100), ('a', 20, 100), ('start', 20, 100), ('b', 20, 100)] +
    [('a', 20, 100)] * 8 +
    [('wait', 300, 0), ('b', 20, 80), ('wait', 300, 0),
     ('a', 20, 80), ('wait', 300, 0), ('start', 20, 80), ('wait', 300, 0)])


def pulse(emulator: Any, key: str, hold: int, release: int) -> None:
    emulator.tool("input.set", port=0, subport=0, buttons={key: True})
    emulator.tool("run.step_frames", frames=hold, reset=False)
    emulator.tool("input.set", port=0, subport=0, buttons={})
    emulator.tool("run.step_frames", frames=release, reset=False)


def bootstrap_gk1(emulator: Any) -> None:
    """Bring the game to its VM callback; NOT a replay of the source checkpoint."""
    emulator.tool("run.step_frames", frames=180, reset=True)
    for key, hold, release in GK1_BOOT_PREFIX:
        if key == "wait":
            emulator.tool("run.step_frames", frames=hold, reset=False)
        else:
            pulse(emulator, key, hold, release)


def await_state_operation(emulator: Any, expected: str, limit_frames: int = 180) -> dict[str, Any]:
    """Never mistake an asynchronous {operation:pending} for success."""
    status = emulator.tool("state.status")
    frames = 0
    while status.get("operation") == "pending" and frames < limit_frames:
        emulator.tool("run.step_frames", frames=2, reset=False)
        frames += 2
        status = emulator.tool("state.status")
    if status.get("operation") != expected:
        raise RuntimeError(f"savestate {expected} did not complete at game VM callback: {status} (frames={frames})")
    return {"state_status": status, "wait_frames": frames}


def sha256(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            h.update(chunk)
    return h.hexdigest()


def w(ram: bytes, address: int) -> int:
    off = address - 0x6000
    if not (0 <= off and off + 1 < len(ram)):
        raise ValueError(f"address outside captured NES $6000..$7FFF: ${address:04X}")
    return ram[off] | (ram[off + 1] << 8)


def decode_gk1(ram: bytes) -> dict[str, Any]:
    if len(ram) != 8192:
        raise ValueError(f"GK1 SRAM read must be 8192 bytes, got {len(ram)}")
    base = 0x6DCD
    troop = w(ram, base + 0x18)
    farmers = w(ram, base + 0x3E)
    town = w(ram, base + 0x1C)
    masons = w(ram, base + 0x20)
    artisans = w(ram, base + 0x3A)
    ptr = w(ram, base + 0x51)
    if 0x6000 <= ptr <= 0x7FEC:
        abilities = [w(ram, ptr + 14 + 2 * i) for i in range(6)]
    else:
        abilities = None  # Opaque until a valid officer pointer exists.
    year, season = w(ram, 0x65BB), w(ram, 0x65BD)
    return {
        "year": year,
        "season_index": season,
        "season_name": GK1_SAVE_START_SEASON[season] if 0 <= season < 4 else "unknown",
        "orders": w(ram, 0x7B36),
        "market_rate": w(ram, 0x7A8A),
        "gold": w(ram, base + 0x40),
        "food": w(ram, base + 0x36),
        "fur": w(ram, base + 0x22),
        "morale": w(ram, base + 0x42),
        "troop_skill": w(ram, base + 0x16),
        "arms": w(ram, base + 0x14),
        "economy": w(ram, base + 0x1A),
        "soldiers": troop,
        "civilians": farmers + town + masons + artisans,
        "population": troop + farmers + town + masons + artisans,
        "undrafted_reserve": w(ram, base + 0x49),
        "workforce": {"farmers": farmers, "town": town, "masons": masons, "artisans": artisans},
        "ruler_pointer": ptr,
        "ruler_abilities": dict(zip(("Leadership", "Judgment", "Planning", "Charm", "Body", "Battle"), abilities)) if abilities else None,
    }


def policy_v2_read_only(state: dict[str, Any]) -> dict[str, Any]:
    """Non-mutating checks of the Director's policy; NOT an action planner."""
    year = state["year"]
    campaign_year = max(1, year - 1175 + 1)
    target = min(250, 60 + 10 * campaign_year)
    abilities = state.get("ruler_abilities") or {}
    return {
        "campaign_year": campaign_year,
        "army_target": target,
        "soldier_shortfall": max(0, target - state["soldiers"]),
        "winter_10_hire_gold_at_rate_140": 144,
        "leadership_target_minimum": 451,
        "leadership_gap": max(0, 451 - abilities.get("Leadership", 0)) if "Leadership" in abilities else None,
        "battle_451_phase": "pre-expansion only; NOT activated by this test",
        "winter_hire_needed_to_be_attempted": state["season_name"] == "Winter",
        "note": "Read-only planning metrics; merchant offers, protected payroll/food, commands and costs must be checked in-game.",
    }


def assert_expected(state: dict[str, Any], expected: dict[str, int]) -> None:
    bad = {key: {"expected": want, "actual": state.get(key)} for key, want in expected.items() if state.get(key) != want}
    if bad:
        raise AssertionError(f"checkpoint state mismatch: {bad}")


def validate_ppm(path: Path) -> dict[str, Any]:
    with path.open("rb") as fh:
        head = fh.read(128)
    match = re.match(rb"^P6\s+(\d+)\s+(\d+)\s+(\d+)\s", head)
    if not match:
        raise AssertionError("video.export_frame did not emit expected binary P6 PPM")
    width, height, maxval = map(int, match.groups())
    if not (128 <= width <= 1024 and 120 <= height <= 1024 and maxval == 255):
        raise AssertionError(f"unexpected PPM dimensions or maxval: {width}x{height}@{maxval}")
    if path.stat().st_size < width * height * 3:
        raise AssertionError("PPM image data truncated")
    return {"file": path.name, "width": width, "height": height, "bytes": path.stat().st_size, "sha256": sha256(path)}


def clean_output_dir(path: Path) -> Path:
    path = path.expanduser().resolve()
    if path.exists() and (not path.is_dir() or any(path.iterdir())):
        raise ValueError(f"output must be a NEW/empty directory; refusing overwrite: {path}")
    path.mkdir(parents=True, exist_ok=True)
    return path


def parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument("--mode", choices=("doctor", "smoke", "checkpoint"), default="doctor")
    p.add_argument("--rom", type=Path, help="User-supplied PRIVATE ROM file")
    p.add_argument("--expected-rom-sha256", help="Pin full 64-digit hash (recommended)")
    p.add_argument("--mesen-bin", type=Path, help="Executable MesenCE binary")
    p.add_argument("--bridge-repo", type=Path, help="mesen-for-ai checkout containing daemon.py and bridge.lua")
    p.add_argument("--restore", type=Path, help="Existing private full-machine .mss (checkpoint mode only)")
    p.add_argument("--expected-save-sha256", help="Pin restored .mss input SHA-256")
    p.add_argument("--out", type=Path, help="NEW local evidence folder (never overwrite)")
    p.add_argument("--frames", type=int, default=10, help="Bounded step after boot/restore (default 10)")
    p.add_argument("--session-timeout", type=int, default=120, help="Mesen test session limit seconds")
    p.add_argument("--probe-button", choices=ALLOWED_UI_PROBES, help="Optional D-pad pulse only; NO automatic A/Start")
    p.add_argument("--hold-frames", type=int, default=5)
    p.add_argument("--release-frames", type=int, default=25)
    p.add_argument("--checkpoint-copy", action="store_true", help="Explicitly write a NEW state file into --out (never overwrite source)")
    p.add_argument("--expect-year", type=int)
    p.add_argument("--expect-season", type=int, choices=range(4))
    p.add_argument("--expect-gold", type=int)
    p.add_argument("--expect-orders", type=int)
    return p


def preflight(a: argparse.Namespace) -> dict[str, Any]:
    if a.mode != "doctor" and (not a.rom or not a.bridge_repo or not a.mesen_bin or not a.out):
        raise ValueError("smoke/checkpoint needs --rom, --bridge-repo, --mesen-bin, and --out")
    if a.mode == "checkpoint" and not a.restore:
        raise ValueError("checkpoint requires --restore .mss")
    if a.restore and a.mode != "checkpoint":
        raise ValueError("--restore requires --mode checkpoint")
    if a.mode != "doctor" and not 1 <= a.frames <= 300:
        raise ValueError("--frames must be 1..300; use short frame batches")
    if not 1 <= a.hold_frames <= 15 or not 1 <= a.release_frames <= 120:
        raise ValueError("controller hold must be 1..15 frames and release 1..120")
    if not 30 <= a.session_timeout <= 14400:
        raise ValueError("--session-timeout must be 30..14400 seconds")
    if a.expected_rom_sha256 and not re.fullmatch(r"[a-fA-F0-9]{64}", a.expected_rom_sha256):
        raise ValueError("--expected-rom-sha256 must be a 64-digit hex SHA-256")
    if a.expected_save_sha256 and not a.restore:
        raise ValueError("--expected-save-sha256 requires --restore")
    if a.expected_save_sha256 and not re.fullmatch(r"[a-fA-F0-9]{64}", a.expected_save_sha256):
        raise ValueError("--expected-save-sha256 must be a 64-digit hex SHA-256")
    checks: dict[str, Any] = {
        "python": sys.version.split()[0], "platform": platform.platform(),
        "xvfb_run_available": shutil.which("xvfb-run") is not None,
        "rom": str(a.rom.resolve()) if a.rom else None,
        "mesen_bin": str(a.mesen_bin.resolve()) if a.mesen_bin else None,
        "bridge_repo": str(a.bridge_repo.resolve()) if a.bridge_repo else None,
    }
    if a.mode != "doctor":
        if not checks["xvfb_run_available"]:
            raise RuntimeError("xvfb-run not found; install Xvfb/xauth before headless execution")
        paths = [(a.rom, "ROM"), (a.mesen_bin, "Mesen executable"), (a.bridge_repo / "src/mesen_mcp/daemon.py", "mesen-for-ai daemon"), (a.bridge_repo / "bridge.lua", "bridge.lua"), (a.bridge_repo / "skills/mesen-emulator/scripts/mesen_client.py", "Mesen Python client")]
        for path, label in paths:
            if not path.is_file():
                raise FileNotFoundError(f"{label} missing: {path}")
        if not os.access(a.mesen_bin, os.X_OK):
            raise PermissionError(f"Mesen binary not executable: {a.mesen_bin}")
        if a.restore and not a.restore.is_file():
            raise FileNotFoundError(f"checkpoint missing: {a.restore}")
        checks["rom_sha256"] = sha256(a.rom)
        checks["mesen_sha256"] = sha256(a.mesen_bin)
        checks["bridge_sha256"] = sha256(a.bridge_repo / "bridge.lua")
        if a.expected_rom_sha256 and checks["rom_sha256"].lower() != a.expected_rom_sha256.lower():
            raise AssertionError("ROM SHA-256 mismatch: refusing to load wrong game")
        if a.restore:
            checks["restored_state_sha256_before"] = sha256(a.restore)
            if a.expected_save_sha256 and checks["restored_state_sha256_before"].lower() != a.expected_save_sha256.lower():
                raise AssertionError("state SHA-256 mismatch: refusing to load wrong checkpoint")
    return checks


def json_event(path: Path, kind: str, **fields: Any) -> None:
    with path.open("a", encoding="utf-8") as fh:
        fh.write(json.dumps({"event": kind, "utc": datetime.now(timezone.utc).isoformat(), **fields}, sort_keys=True) + "\n")


def run(a: argparse.Namespace) -> int:
    checks = preflight(a)
    if a.mode == "doctor":
        print(json.dumps({"status": "DOCTOR_ONLY_NOT_A_LIVE_EMULATOR_TEST", **checks}, indent=2))
        return 0

    out = clean_output_dir(a.out)
    events = out / "events.jsonl"
    receipt: dict[str, Any] = {
        "status": "IN_PROGRESS", "mode": a.mode, "created_utc": datetime.now(timezone.utc).isoformat(),
        "preflight": checks, "input_checkpoint": str(a.restore.resolve()) if a.restore else None,
        "frames_requested": a.frames,
        "note": "Real original emulator test only if status=PASS and expected assertions passed; no campaign auto-play.",
    }
    os.environ.update({
        "MESEN_BIN": str(a.mesen_bin.resolve()),
        "MESEN_FOR_AI_REPO": str(a.bridge_repo.resolve()),
        "SDL_AUDIODRIVER": "dummy",
        "DOTNET_ROLL_FORWARD": os.environ.get("DOTNET_ROLL_FORWARD", "Major"),
    })
    sys.path.insert(0, str((a.bridge_repo / "skills/mesen-emulator/scripts").resolve()))
    try:
        from mesen_client import Mesen  # local dependency, not copied into this repo
        json_event(events, "launch", rom_sha256=checks["rom_sha256"], mode=a.mode)
        with Mesen(repo=str(a.bridge_repo.resolve())) as emulator:
            loaded = emulator.load_rom(str(a.rom.resolve()), timeout=a.session_timeout)
            receipt["emulator_session"] = loaded.get("session")
            receipt["rom_info"] = loaded.get("romInfo")
            json_event(events, "rom_loaded", session=loaded.get("session"))

            if a.mode == "checkpoint":
                if checks["rom_sha256"] == GK1_ROM_SHA256:
                    bootstrap_gk1(emulator)
                    json_event(events, "gk1_bootstrap_reached_game", prefix_count=len(GK1_BOOT_PREFIX))
                tools = {t.get("name") for t in emulator.tools()}
                if "state.restore" not in tools:
                    raise RuntimeError("This mesen-for-ai bridge lacks state.restore: restore the EXACT private GK1 campaign bridge, not stock upstream. See runbook.")
                emulator.tool("input.set", port=0, subport=0, buttons={})
                response = emulator.tool("state.restore", path=str(a.restore.resolve()))
                receipt["restore_response"] = response
                # Full-machine restore is asynchronous at the game's VM callback.
                # An acknowledgement {operation:pending} is NOT a restored save.
                completion = await_state_operation(emulator, "loaded")
                receipt["restore_completion"] = completion
                emulator.tool("run.step_frames", frames=a.frames, reset=False)
                json_event(events, "restored", checkpoint_sha256=checks["restored_state_sha256_before"], callback=completion)
            else:
                emulator.tool("run.step_frames", frames=a.frames, reset=True)
                json_event(events, "boot_reset", frames=a.frames)

            status = emulator.tool("run.status")
            ram_bytes = emulator.tool("cpu.read_memory", memoryType="nesMemory", address=0x6000, length=8192)["bytes"]
            ram = bytes(ram_bytes)
            if len(ram) != 8192:
                raise AssertionError("cpu.read_memory returned incomplete 8192-byte NES read")
            receipt["status"] = "CAPTURED"
            receipt["frame_status"] = status
            receipt["ram_sha256"] = hashlib.sha256(ram).hexdigest()
            json_event(events, "memory_read", length=len(ram), sha256=receipt["ram_sha256"])
            if checks["rom_sha256"] == GK1_ROM_SHA256:
                state = decode_gk1(ram)
                receipt["gk1_state"] = state
                receipt["gk1_policy_v2_readonly"] = policy_v2_read_only(state)
                expected = {k: v for k, v in {
                    "year": a.expect_year, "season_index": a.expect_season,
                    "gold": a.expect_gold, "orders": a.expect_orders,
                }.items() if v is not None}
                assert_expected(state, expected)
            else:
                if any(v is not None for v in (a.expect_year, a.expect_season, a.expect_gold, a.expect_orders)):
                    raise ValueError("GK1 state assertions only allowed for known original USA NES ROM hash")
                receipt["gk1_state"] = None
                receipt["gk1_policy_v2_readonly"] = None
            if a.probe_button:
                if a.mode != "checkpoint":
                    raise ValueError("D-pad probe requires checkpoint mode: no blind inputs on boot screens")
                before = receipt.get("gk1_state")
                pulse(emulator, a.probe_button, a.hold_frames, a.release_frames)
                json_event(events, "ui_dpad_pulse", button=a.probe_button, hold=a.hold_frames, release=a.release_frames)
                if before:
                    after = decode_gk1(bytes(emulator.tool("cpu.read_memory", memoryType="nesMemory", address=0x6000, length=8192)["bytes"]))
                    for field in ("year", "season_index", "orders", "gold"):
                        if before[field] != after[field]:
                            raise AssertionError(f"unexpected game mutation during D-pad probe: {field}")
                receipt["probe_button"] = a.probe_button
            frame = out / "frame.ppm"
            emulator.tool("video.export_frame", path=str(frame))
            receipt["frame"] = validate_ppm(frame)
            json_event(events, "frame_export", **receipt["frame"])
            if a.checkpoint_copy:
                if "state.checkpoint" not in {t.get("name") for t in emulator.tools()}:
                    raise RuntimeError("This bridge lacks state.checkpoint; cannot create a native machine state")
                dest = out / "checkpoint_copy.mss"
                emulator.tool("input.set", port=0, subport=0, buttons={})
                pending_save = emulator.tool("state.checkpoint", path=str(dest))
                receipt["checkpoint_copy_response"] = pending_save
                receipt["checkpoint_copy_completion"] = await_state_operation(emulator, "saved")
                if not dest.exists() or dest.stat().st_size < 1024:
                    raise AssertionError("state.checkpoint did not create a valid-size file")
                receipt["checkpoint_copy"] = {"path": dest.name, "bytes": dest.stat().st_size, "sha256": sha256(dest)}
                json_event(events, "checkpoint_copy", save_file=dest.name, bytes=dest.stat().st_size, sha256=receipt["checkpoint_copy"]["sha256"])
        if a.restore and sha256(a.restore) != checks["restored_state_sha256_before"]:
            raise AssertionError("source checkpoint bytes changed during smoke test")
        receipt["status"] = "PASS"
        return 0
    except Exception as exc:
        receipt["status"] = "FAIL"
        receipt["error"] = f"{type(exc).__name__}: {exc}"
        receipt["traceback"] = traceback.format_exc()
        json_event(events, "failure", error=receipt["error"])
        print(receipt["error"], file=sys.stderr)
        return 1
    finally:
        receipt["finished_utc"] = datetime.now(timezone.utc).isoformat()
        (out / "RUN_RECEIPT.json").write_text(json.dumps(receipt, indent=2, sort_keys=True, default=str) + "\n", encoding="utf-8")
        print(f"Receipt: {out / 'RUN_RECEIPT.json'} / status={receipt['status']}")


def main(argv: list[str] | None = None) -> int:
    a = parser().parse_args(argv)
    try:
        return run(a)
    except Exception as exc:
        print(f"ERROR: {type(exc).__name__}: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
