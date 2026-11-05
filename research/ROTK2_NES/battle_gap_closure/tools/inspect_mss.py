#!/usr/bin/env python3
"""Inspect a private Mesen MSS container by its typed/keyed structure.

The binary format is grounded in MesenCE SaveStateManager.cpp and
Utilities/Serializer.cpp, commit 4b8a196e299b3de297b204fd1208d2981562c89b.
This tool does not load a game or guess CPU addresses from container offsets.
"""

from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import struct
import zlib


def parse_mss(path: Path) -> tuple[dict, dict[str, bytes]]:
    raw = path.read_bytes()
    if raw[:3] != b"MSS":
        raise ValueError("Not a Mesen MSS container")
    position = 3

    def take(length: int) -> bytes:
        nonlocal position
        if length < 0 or position + length > len(raw):
            raise ValueError("Truncated MSS container")
        value = raw[position:position + length]
        position += length
        return value

    def u32() -> int:
        return struct.unpack("<I", take(4))[0]

    emulator_version = u32()
    format_version = u32()
    if format_version <= 3:
        take(40)  # Legacy SHA-1 text field, as in the native loader.
    console_type = u32()
    frame_bytes, width, height, scale100, image_compressed_bytes = [u32() for _ in range(5)]
    if frame_bytes > 10 * 1024 * 1024 or image_compressed_bytes > 2 * 1024 * 1024:
        raise ValueError("Unexpected frame size")
    image = zlib.decompress(take(image_compressed_bytes))
    if len(image) != frame_bytes:
        raise ValueError("Frame data length disagrees with MSS header")
    name_length = u32()
    rom_name = take(name_length).decode("utf-8", errors="strict")
    serialization_offset = position
    compressed = take(1)
    if compressed == b"\x01":
        payload_bytes, compressed_bytes = u32(), u32()
        if max(payload_bytes, compressed_bytes) >= 10 * 1024 * 1024:
            raise ValueError("Serialized state exceeds native loader size limit")
        compressed_data_offset = position
        payload = zlib.decompress(take(compressed_bytes))
        if len(payload) != payload_bytes:
            raise ValueError("Payload length disagrees with compression header")
    elif compressed == b"\x00":
        compressed_data_offset = None
        payload = take(len(raw) - position)
        payload_bytes, compressed_bytes = len(payload), None
    else:
        raise ValueError("Unknown serialization compression flag")
    if position != len(raw):
        raise ValueError("Unexpected bytes after serialized payload")

    entries: dict[str, bytes] = {}
    records = []
    cursor = 0
    while cursor < len(payload):
        record_offset = cursor
        terminator = payload.find(b"\x00", cursor)
        if terminator < 0:
            raise ValueError("Unterminated serialized field name")
        key_bytes = payload[cursor:terminator]
        if not key_bytes or any(c <= 32 or c >= 127 for c in key_bytes):
            raise ValueError("Invalid serialized field name")
        key = key_bytes.decode("ascii")
        cursor = terminator + 1
        if cursor + 4 > len(payload):
            raise ValueError("Truncated serialized field size")
        length = struct.unpack_from("<I", payload, cursor)[0]
        cursor += 4
        value_offset = cursor
        if cursor + length > len(payload):
            raise ValueError("Serialized field extends beyond payload")
        value = payload[cursor:cursor + length]
        cursor += length
        if key in entries:
            raise ValueError("Duplicate serialized field name")
        entries[key] = value
        record = {
            "key": key, "record_payload_offset": record_offset,
            "value_payload_offset": value_offset, "bytes": length,
            "sha256": hashlib.sha256(value).hexdigest(),
        }
        if length <= 8:
            record["little_endian_unsigned"] = int.from_bytes(value, "little")
        records.append(record)

    manifest = {
        "path": str(path.resolve()), "bytes": len(raw),
        "sha256": hashlib.sha256(raw).hexdigest(),
        "emulator_version": emulator_version, "format_version": format_version,
        "console_type": console_type, "rom_name": rom_name,
        "saved_frame": {
            "width": width, "height": height, "scale_percent": scale100,
            "uncompressed_bytes": frame_bytes, "compressed_bytes": image_compressed_bytes,
        },
        "serialization_file_offset": serialization_offset,
        "compression_envelope_bytes": 9 if compressed == b"\x01" else 1,
        "compressed_data_file_offset": compressed_data_offset,
        "serialized_payload_bytes": payload_bytes,
        "compressed_payload_bytes": compressed_bytes,
        "field_count": len(records), "fields": records,
    }
    return manifest, entries


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("state", type=Path)
    parser.add_argument("--manifest", type=Path)
    parser.add_argument("--extract-key")
    parser.add_argument("--extract-file", type=Path)
    args = parser.parse_args()
    if bool(args.extract_key) != bool(args.extract_file):
        parser.error("--extract-key and --extract-file must be supplied together")
    for output in [args.manifest, args.extract_file]:
        if output is not None and output.exists():
            parser.error(f"Refusing to overwrite {output}")
    manifest, entries = parse_mss(args.state)
    if args.extract_key:
        if args.extract_key not in entries:
            parser.error(f"No exact serialized field {args.extract_key!r}")
        args.extract_file.write_bytes(entries[args.extract_key])
    rendered = json.dumps(manifest, indent=2) + "\n"
    if args.manifest:
        args.manifest.write_text(rendered, encoding="utf-8")
    else:
        print(rendered, end="")


if __name__ == "__main__":
    main()
