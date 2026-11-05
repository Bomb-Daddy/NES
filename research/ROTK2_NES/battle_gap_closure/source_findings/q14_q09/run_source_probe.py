#!/usr/bin/env python3
"""Regenerate selected original-ROM modules and preserve a private execution receipt."""
import argparse
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import sys
from datetime import datetime, timezone

EXPECTED_ROM_SHA256 = "815bfcf4fd6eb23a20c2e50dde023c210b273ffb6cd86a93909d803c3643ce46"

def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--rom", type=Path, required=True)
    parser.add_argument("--symbols", type=Path, required=True)
    parser.add_argument("--tools", type=Path, required=True)
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--modules", default="root,comroot,csovl1,hsovl2,sengo")
    args = parser.parse_args()
    rom, symbols, tool_repo, output = [p.resolve() for p in (args.rom, args.symbols, args.tools, args.out)]
    if digest(rom) != EXPECTED_ROM_SHA256:
        raise SystemExit("REFUSED: original SNES ROM hash does not match pinned research source")
    output.mkdir(parents=True, exist_ok=True)
    receipt_path = output / ("generation_" + args.modules.replace(",", "_") + ".json")
    if receipt_path.exists():
        raise SystemExit("REFUSED: receipt already exists")
    if not (output / "data").exists():
        shutil.copytree(symbols / "data", output / "data")
    def revision(repo):
        result = subprocess.run(["git", "-C", str(repo), "rev-parse", "HEAD"], check=True, capture_output=True, text=True)
        return result.stdout.strip()
    command = [sys.executable, "-m", "emu_harness.vm.koei.snes.gen_layers", str(rom), str(output), "--game", "rot3k2", "--only", args.modules]
    receipt = {
        "started_utc": datetime.now(timezone.utc).isoformat(),
        "rom_sha256": digest(rom), "rom_bytes": rom.stat().st_size,
        "symbol_repo": "https://github.com/chrisaacson69/rot3k2-snes-decompiler",
        "symbol_commit": revision(symbols),
        "tool_repo": "https://github.com/chrisaacson69/emu-harness",
        "tool_commit": revision(tool_repo),
        "command": command, "cwd": str(Path.cwd()), "environment": {"python": sys.version}
    }
    result = subprocess.run(command, capture_output=True, text=True)
    receipt.update({"finished_utc":datetime.now(timezone.utc).isoformat(), "stdout": result.stdout, "stderr": result.stderr, "exit_code":result.returncode})
    receipt_path.with_suffix(".stdout.txt").write_text(result.stdout)
    receipt_path.with_suffix(".stderr.txt").write_text(result.stderr)
    receipt["outputs"] = {str(p.relative_to(output)):{"sha256":digest(p),"bytes":p.stat().st_size} for p in sorted((output / "src").glob("*"))}
    receipt_path.write_text(json.dumps(receipt, indent=2) + "\n")
    print(result.stdout, end="")
    if result.stderr:
        print(result.stderr, end="", file=sys.stderr)
    print("Receipt: " + str(receipt_path))
    return result.returncode

if __name__ == "__main__":
    raise SystemExit(main())

