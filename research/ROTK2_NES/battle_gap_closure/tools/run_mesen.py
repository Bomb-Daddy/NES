#!/usr/bin/env python3
"""Run one original ROTK II Lua experiment in an isolated portable Mesen.

No X server, shared profile, socket bridge, or game-specific callback is used.
The supplied Lua script owns the bounded experiment and its private outputs.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import time


ROM_SHA256 = "11e312691b516e5e29dade3c8ce5434df3561cc69e1aecad9c948acdfddc517a"
MESEN_SHA256 = "f02685dafe559addf8dd8d769621a89777137d0c4592436aed65694a7e740676"
CORE_SHA256 = "9d21269be65268c1f56543b83e2f10650f3a28ef2566a4acd861615ecc6ed189"
SETTINGS = {
    "Version": "2.2.1",
    "ConfigUpgrade": 1,
    "EnableTestMode": True,
    "Debug": {"ScriptWindow": {
        "AllowIoOsAccess": True,
        "AllowNetworkAccess": True,
        "ScriptTimeout": 60,
    }},
    "Nes": {
        "AutoConfigureInput": False,
        "Port1": {"Type": "NesController"},
        "Port2": {"Type": "NesController"},
        "RamPowerOnState": "AllZeros",
        "RandomizeMapperPowerOnState": False,
        "RandomizeCpuPpuAlignment": False,
    },
}


def digest(path: Path) -> str:
    with path.open("rb") as stream:
        return hashlib.file_digest(stream, "sha256").hexdigest()


def run(args: argparse.Namespace) -> int:
    rom = args.rom.expanduser().resolve(strict=True)
    mesen = args.mesen.expanduser().resolve(strict=True)
    script = args.script.expanduser().resolve(strict=True)
    output = args.output.expanduser().resolve()

    if rom.stat().st_size != 524304 or digest(rom) != ROM_SHA256:
        raise ValueError("Original ROTK II NES ROM size/SHA-256 mismatch; emulator not launched")
    if digest(mesen) != MESEN_SHA256:
        raise ValueError("Historical Mesen SHA-256 mismatch; emulator not launched")
    if not os.access(mesen, os.X_OK):
        raise ValueError("Mesen is not executable")
    if args.emulator_timeout < 1:
        raise ValueError("Emulator timeout must be at least one second")
    if not script.is_file():
        raise ValueError("Lua script is not a regular file")
    if output.exists():
        raise ValueError("Output path already exists; choose a fresh run directory")

    output.mkdir(parents=True, exist_ok=False)
    portable = output / "portable"
    portable.mkdir()
    executable = portable / "Mesen"
    shutil.copy2(mesen, executable)
    copied_sha256 = digest(executable)
    if copied_sha256 != MESEN_SHA256:
        raise ValueError("Copied Mesen SHA-256 mismatch; emulator not launched")
    settings = portable / "settings.json"
    settings.write_text(json.dumps(SETTINGS, indent=2) + "\n", encoding="utf-8")

    command = [
        str(executable), "--testRunner", "--enableStdout", "--doNotSaveSettings",
        f"--timeout={args.emulator_timeout}", str(script), str(rom),
    ]
    env = os.environ.copy()
    env.update(LC_ALL="C", DOTNET_SYSTEM_GLOBALIZATION_INVARIANT="1", SDL_AUDIODRIVER="dummy")
    env.pop("DISPLAY", None)
    stdout = output / "stdout.log"
    stderr = output / "stderr.log"
    receipt = {
        "command": command,
        "cwd": str(output),
        "rom": {"path": str(rom), "bytes": rom.stat().st_size, "sha256": ROM_SHA256},
        "mesen": {"path": str(executable), "sha256": copied_sha256},
        "script": {"path": str(script), "sha256": digest(script)},
        "settings": {"path": str(settings), "sha256": digest(settings)},
        "environment_changes": {
            "LC_ALL": "C", "DOTNET_SYSTEM_GLOBALIZATION_INVARIANT": "1",
            "SDL_AUDIODRIVER": "dummy", "DISPLAY": "removed",
        },
        "xvfb_used": False,
        "home_environment_changed": False,
        "host_timeout_seconds": args.emulator_timeout + 15,
    }
    started = time.monotonic()
    with stdout.open("wb") as out, stderr.open("wb") as err:
        try:
            result = subprocess.run(
                command, cwd=output, env=env, stdin=subprocess.DEVNULL,
                stdout=out, stderr=err, timeout=args.emulator_timeout + 15,
            )
            receipt.update(exit_code=result.returncode, timeout=False)
        except subprocess.TimeoutExpired:
            receipt.update(exit_code=None, timeout=True)
    receipt["elapsed_seconds"] = time.monotonic() - started
    core = portable / "MesenCore.so"
    receipt["core"] = {"path": str(core), "sha256": digest(core)} if core.is_file() else None
    receipt["original_rom_unchanged"] = digest(rom) == ROM_SHA256
    receipt["mesen_after"] = ({"path": str(executable), "sha256": digest(executable)}
                              if executable.is_file() else None)
    receipt["copied_executable_unchanged"] = bool(
        receipt["mesen_after"] and receipt["mesen_after"]["sha256"] == MESEN_SHA256
    )
    receipt["raw_output"] = {
        "stdout": {"path": str(stdout), "bytes": stdout.stat().st_size, "sha256": digest(stdout)},
        "stderr": {"path": str(stderr), "bytes": stderr.stat().st_size, "sha256": digest(stderr)},
    }
    receipt["runtime_identity_passed"] = bool(
        receipt["core"] and receipt["core"]["sha256"] == CORE_SHA256
        and receipt["original_rom_unchanged"] and receipt["copied_executable_unchanged"]
    )
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(receipt, indent=2))
    return 0 if receipt["exit_code"] == 0 and receipt["runtime_identity_passed"] else 1


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--rom", type=Path, required=True)
    parser.add_argument("--mesen", type=Path, required=True)
    parser.add_argument("--script", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True, help="New, non-existent run directory")
    parser.add_argument("--emulator-timeout", type=int, default=60)
    args = parser.parse_args()
    try:
        return run(args)
    except (ValueError, OSError) as exc:
        print(f"Preflight/setup failed: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
