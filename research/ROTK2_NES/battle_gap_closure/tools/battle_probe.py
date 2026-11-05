#!/usr/bin/env python3
"""Bounded original-ROM experiment; keep generated evidence outside public Git.

Controller scheduling and base64 framing adapt the Nov 3 repository capture
tools. Native state restoration is checked; all stdout/stderr is retained.
"""
from __future__ import annotations
import argparse
import base64
import binascii
import contextlib
import hashlib
import io
import json
from pathlib import Path
import struct
import sys
import zlib
import run_mesen


def read_checkpoint(path):
    try:
        value = Path(path).read_text(encoding="ascii").strip()
        raw = base64.b64decode(value, validate=True)
    except (ValueError, UnicodeError, binascii.Error) as exc:
        raise ValueError("Invalid checkpoint encoding") from exc
    if len(raw) < 128:
        raise ValueError("Empty or implausibly short checkpoint")
    return value  # Native loadSavestate must still confirm validity.


def preflight(rom, out, state):
    rom = Path(rom)
    if rom.stat().st_size != 524304 or run_mesen.digest(rom) != run_mesen.ROM_SHA256:
        raise ValueError("Original ROM hash/size mismatch; execution refused")
    if Path(out).exists() and any(Path(out).iterdir()):
        raise FileExistsError("Nonempty output; preserve existing evidence")
    return read_checkpoint(state) if state else None


def prepare_output(out):
    out = Path(out)
    if out.exists() and any(out.iterdir()):
        raise FileExistsError("Nonempty output; preserve existing evidence")
    out.mkdir(parents=True, exist_ok=True)


def check_completion(stdout, exit_code, restoring, stop):
    if exit_code != 0:
        raise ValueError(f"Emulator exit was {exit_code}")
    lines = stdout.splitlines()
    if restoring and "LOADED" not in lines:
        raise ValueError("Native checkpoint restoration was not confirmed")
    if f"DONE {stop}" not in lines:
        raise ValueError("Expected DONE frame receipt is missing")
    if any(s.startswith("PROBE_ERROR") for s in lines):
        raise ValueError("Probe reported an error")


def check_png(raw):
    """Validate a bounded native RGB/RGBA screenshot, including its image data."""
    if len(raw) < 33 or len(raw) > 2 * 1024 * 1024 or raw[:8] != b'\x89PNG\r\n\x1a\n':
        raise ValueError("Missing/invalid native PNG screenshot")
    position = 8
    image_parts = []
    header_seen = image_seen = image_ended = end_seen = palette_seen = False
    stride = None
    while position < len(raw):
        if len(raw) - position < 12:
            raise ValueError("Truncated PNG chunk")
        length = struct.unpack_from('>I', raw, position)[0]
        if length > len(raw) - position - 12:
            raise ValueError("Truncated PNG chunk data")
        kind = raw[position + 4:position + 8]
        data = raw[position + 8:position + 8 + length]
        crc = struct.unpack_from('>I', raw, position + 8 + length)[0]
        if any(not (65 <= c <= 90 or 97 <= c <= 122) for c in kind) or kind[2] & 32:
            raise ValueError("Invalid PNG chunk type")
        if zlib.crc32(kind + data) & 0xffffffff != crc:
            raise ValueError("PNG chunk CRC mismatch")
        if not header_seen and kind != b'IHDR':
            raise ValueError("PNG must begin with IHDR")
        position += length + 12
        if kind == b'IHDR':
            if header_seen or length != 13:
                raise ValueError("Invalid/duplicate PNG IHDR")
            width, height, depth, color, compression, filtering, interlace = struct.unpack('>IIBBBBB', data)
            if (width, height) != (256, 240):
                raise ValueError("Unexpected native PNG dimensions")
            # The pinned native encoder emits 8-bit, noninterlaced RGB.
            # Also accept the equivalent RGBA representation, with bounded rows.
            if depth != 8 or color not in (2, 6) or compression or filtering or interlace:
                raise ValueError("Unsupported native PNG pixel format")
            stride = 1 + width * (3 if color == 2 else 4)
            header_seen = True
        elif kind == b'PLTE':
            if palette_seen or image_seen or not length or length > 768 or length % 3:
                raise ValueError("Invalid PNG palette chunk")
            palette_seen = True
        elif kind == b'IDAT':
            if image_ended:
                raise ValueError("Noncontiguous PNG IDAT chunks")
            image_seen = True
            image_parts.append(data)
        elif kind == b'IEND':
            if length or not image_seen or position != len(raw):
                raise ValueError("Invalid PNG IEND/trailing bytes")
            end_seen = True
            break
        else:
            if not kind[0] & 32:
                raise ValueError("Unknown critical PNG chunk")
            if image_seen:
                image_ended = True
    if not end_seen or not any(image_parts):
        raise ValueError("PNG lacks complete image data/IEND")
    expected = stride * 240
    decoder = zlib.decompressobj()
    try:
        pixels = decoder.decompress(b''.join(image_parts), expected + 1)
    except zlib.error as exc:
        raise ValueError("Invalid PNG compressed image data") from exc
    if len(pixels) != expected or not decoder.eof or decoder.unused_data or decoder.unconsumed_tail:
        raise ValueError("Incomplete/oversized PNG decompressed image data")
    if any(pixels[row * stride] > 4 for row in range(240)):
        raise ValueError("Invalid PNG scanline filter")


LUA = r'''
local alphabet='ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/'
local function b64(data)
  local out={}
  for i=1,#data,3 do
    local a,b,c=data:byte(i,i+2)
    local n=(a<<16)|((b or 0)<<8)|(c or 0)
    out[#out+1]=alphabet:sub((n>>18)+1,(n>>18)+1)..alphabet:sub(((n>>12)&63)+1,((n>>12)&63)+1)..
      (b and alphabet:sub(((n>>6)&63)+1,((n>>6)&63)+1) or '=')..
      (c and alphabet:sub((n&63)+1,(n&63)+1) or '=')
  end
  return table.concat(out)
end
local function unb64(s)
  local rb={} for i=1,64 do rb[alphabet:byte(i)]=i-1 end
  local out={}
  for i=1,#s,4 do
    local a,b,c,d=s:byte(i,i+3)
    local n=(rb[a]<<18)|(rb[b]<<12)|((rb[c] or 0)<<6)|(rb[d] or 0)
    out[#out+1]=string.char((n>>16)&255)
    if c~=61 then out[#out+1]=string.char((n>>8)&255) end
    if d~=61 then out[#out+1]=string.char(n&255) end
  end
  return table.concat(out)
end
local function dump(kind,addr,size)
  local t={}
  for a=addr,addr+size-1 do t[#t+1]=string.char(emu.read(a,kind)) end
  return b64(table.concat(t))
end
local frame=0
local want=nil
local stopped=false
local exec_ref=nil
print('PROBE_STARTED')
for _,r in ipairs(watches) do
  emu.addMemoryCallback(function(addr,value)
    local st=emu.getState()
    local ip=emu.read(6,emu.memType.nesDebug)+256*emu.read(7,emu.memType.nesDebug)
    local mapped=emu.convertAddress(ip,emu.memType.nesMemory,emu.cpuType.nes)
    print('WRITE '..frame..' '..addr..' '..emu.read(addr,emu.memType.nesDebug)..' '..value..' '..
      tostring(st['cpu.pc'])..' '..ip..' '..tostring(mapped and mapped.address)..' '..
      tostring(mapped and mapped.memType)..' '..emu.read(0x7D1C,emu.memType.nesDebug))
  end,emu.callbackType.write,r[1],r[2])
end
emu.addEventCallback(function()
  local inp={}
  for _,e in ipairs(events) do
    if frame>=e[1] and frame<e[1]+e[3] then inp[e[2]]=true end
  end
  emu.setInput({},1-inputport)
  emu.setInput(inp,inputport)
end,emu.eventType.inputPolled)
local function serve_native()
  if loadstate then
    local bytes=unb64(loadstate);loadstate=nil
    if emu.loadSavestate(bytes) then print('LOADED')
    else print('PROBE_ERROR restore failed');emu.stop(1);return end
  end
  if want then
    local state=emu.createSavestate()
    if #state==0 then print('PROBE_ERROR empty checkpoint');emu.stop(1);return end
    print('STATE '..want..' '..b64(state));want=nil
  end
  emu.removeMemoryCallback(exec_ref,emu.callbackType.exec,0x8000,0xFFFF)
  exec_ref=nil
end
local function arm_native()
  if not exec_ref then exec_ref=emu.addMemoryCallback(serve_native,emu.callbackType.exec,0x8000,0xFFFF) end
end
if loadstate then arm_native() end
emu.addEventCallback(function()
  if stopped then return end
  frame=frame+1
  for _,p in ipairs(patches) do
    if frame==p[1] then
      for i,value in ipairs(p[3]) do
        local addr=p[2]+i-1
        local m=emu.convertAddress(addr,emu.memType.nesMemory,emu.cpuType.nes)
        if not m or m.memType~=emu.memType.nesSaveRam then
          print('PROBE_ERROR intervention is not mapped save RAM');stopped=true;emu.stop(1);return
        end
        local old=emu.read(m.address,m.memType)
        emu.write(m.address,value,m.memType)
        local actual=emu.read(m.address,m.memType)
        print('INTERVENTION '..frame..' '..addr..' '..m.address..' '..old..' '..value..' '..actual)
        if actual~=value then print('PROBE_ERROR intervention failed');stopped=true;emu.stop(1);return end
      end
    end
  end
  if shots[frame] then print('SHOT '..frame..' '..b64(emu.takeScreenshot())) end
  if saves[frame] then want=frame;arm_native() end
  if snapshots[frame] then
    local st=emu.getState()
    local m=emu.convertAddress(0x6000,emu.memType.nesMemory,emu.cpuType.nes)
    print('BANK '..frame..' '..tostring(m and m.address)..' '..tostring(m and m.memType)..' '..tostring(st['mapper.prgBanks0'])..' '..tostring(st['mapper.prgMode']))
    print('MEM '..frame..' cpu0000 '..dump(emu.memType.nesDebug,0,0x800))
    print('MEM '..frame..' cpu6000 '..dump(emu.memType.nesDebug,0x6000,0x2000))
    if emu.memType.nesSaveRam then
      print('MEM '..frame..' save0000 '..dump(emu.memType.nesSaveRam,0,0x8000))
    end
  end
  for _,p in ipairs(peeks) do
    if frame==p[1] then print('MEM '..frame..' peek'..p[2]..' '..dump(emu.memType.nesDebug,p[2],p[3])) end
  end
  if frame>=stopframe then
    stopped=true
    if want then print('PROBE_ERROR checkpoint still pending');emu.stop(1)
    else print('DONE '..frame);emu.stop(0) end
  end
end,emu.eventType.endFrame)
'''


def frame_set(values):
    return "{" + ",".join(f"[{int(f)}]=true" for f in values) + "}"


def build(config, checkpoint):
    stop = int(config["stop"])
    if stop < 1:
        raise ValueError("stop must be positive")
    port = config.get("input_port", 0)
    if port not in (0, 1):
        raise ValueError("input_port must be 0 or 1")
    for f, b, hold in config.get("events", []):
        if b not in {"a", "b", "up", "down", "left", "right", "start", "select"} or int(f) < 0 or int(hold) < 1:
            raise ValueError("Invalid controller event")
    if any(not 0 < int(f) < stop for f in config.get("saves", [])):
        raise ValueError("Checkpoint frame must precede stop")
    events = "{" + ",".join("{%d,%s,%d}" % (f, json.dumps(b), h) for f, b, h in config.get("events", [])) + "}"
    peeks = "{" + ",".join("{%d,%d,%d}" % tuple(p) for p in config.get("peeks", [])) + "}"
    watches = "{" + ",".join("{%d,%d}" % tuple(p) for p in config.get("watches", [])) + "}"
    patches = "{" + ",".join("{%d,%d,{%s}}" % (f, a, ",".join(str(int(v)) for v in vs))
                             for f, a, vs in config.get("patches", [])) + "}"
    head = f"local events={events}\nlocal stopframe={stop}\nlocal inputport={port}\n"
    for key in ("shots", "saves", "snapshots"):
        head += f"local {key}={frame_set(config.get(key, []))}\n"
    return head + f"local peeks={peeks}\nlocal watches={watches}\nlocal patches={patches}\nlocal loadstate={json.dumps(checkpoint) if checkpoint else 'nil'}\n" + LUA


def run(args):
    out = args.output.resolve()
    state = preflight(args.rom, out, args.state)
    config = json.loads(args.schedule.read_text())
    lua = build(config, state)
    prepare_output(out)
    script = out / "probe.lua"
    script.write_text(lua)
    (out / "schedule.json").write_text(json.dumps(config, indent=2) + "\n")
    native = argparse.Namespace(rom=args.rom, mesen=args.mesen, script=script,
                               output=out / "process", emulator_timeout=args.timeout)
    with contextlib.redirect_stdout(io.StringIO()):
        rc = run_mesen.run(native)
    native_receipt = json.loads((out / "process" / "receipt.json").read_text())
    stdout = (out / "process" / "stdout.log").read_text(errors="replace")
    products = []
    failures = []
    observed = {"shots": set(), "saves": set(), "snapshots": set()}
    for line in stdout.splitlines():
        parts = line.split(" ", 3)
        if parts[0] in ("SHOT", "STATE") and len(parts) >= 3:
            kind, frame, encoded = line.split(" ", 2)
            raw = base64.b64decode(encoded, validate=True)
            if kind == "SHOT":
                path = out / f"f{int(frame):06d}.png"
                path.write_bytes(raw)
                try:
                    check_png(raw)
                    observed["shots"].add(int(frame))
                except ValueError as exc:
                    failures.append(f"Screenshot {frame}: {exc}")
            else:
                path = out / f"state_{int(frame):06d}.b64"
                path.write_text(encoded + "\n")
                observed["saves"].add(int(frame))
        elif parts[0] == "MEM" and len(parts) == 4:
            _, frame, name, encoded = parts
            path = out / f"memory_{int(frame):06d}_{name}.bin"
            path.write_bytes(base64.b64decode(encoded, validate=True))
            if name == "cpu6000":
                observed["snapshots"].add(int(frame))
        else:
            continue
        products.append({"file": path.name, "bytes": path.stat().st_size,
                         "sha256": run_mesen.digest(path)})
    try:
        check_completion(stdout, native_receipt["exit_code"], bool(state), config["stop"])
    except ValueError as exc:
        failures.append(str(exc))
    for kind in observed:
        missing = set(config.get(kind, [])) - observed[kind]
        if missing:
            failures.append(f"Missing {kind}: {sorted(missing)}")
    if rc:
        failures.append("Native runtime identity/exit gate failed")
    receipt = {"evidence": "OBSERVED_RUNTIME" if not failures else "INCOMPLETE_RUNTIME",
               "controlled_state_interventions": config.get("patches", []),
               "failures": failures, "restoration_requested": bool(state),
               "input_state_sha256": hashlib.sha256(base64.b64decode(state)).hexdigest() if state else None,
               "schedule_sha256": run_mesen.digest(out / "schedule.json"),
               "products": products, "raw_receipt": "process/receipt.json"}
    (out / "probe_receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")
    print(json.dumps({"output": str(out), "failures": failures, "products": len(products),
                      "exit_code": native_receipt["exit_code"], "elapsed_seconds": native_receipt["elapsed_seconds"]}))
    return 1 if failures else 0


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--rom", type=Path, required=True)
    p.add_argument("--mesen", type=Path, required=True)
    p.add_argument("--schedule", type=Path, required=True)
    p.add_argument("--output", type=Path, required=True)
    p.add_argument("--state", type=Path)
    p.add_argument("--timeout", type=int, default=120)
    try:
        return run(p.parse_args())
    except (ValueError, OSError) as exc:
        print(str(exc), file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
