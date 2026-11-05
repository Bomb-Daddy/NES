#!/usr/bin/env python3
"""Independently compare every listing byte with the authorized ROM resource archive."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import struct

EXPECTED_ROM_SHA256 = "815bfcf4fd6eb23a20c2e50dde023c210b273ffb6cd86a93909d803c3643ce46"
ROW_PATTERN = re.compile(r"^\s+\$([0-9A-F]{4})\s+((?:[0-9A-F]{2}(?:\s+|$))+)")
def sha(data): return hashlib.sha256(data).hexdigest()
def parse_rows(text):
    for line_no, line in enumerate(text.splitlines(), 1):
        match=ROW_PATTERN.match(line)
        if match: yield line_no, int(match.group(1), 16), bytes.fromhex(match.group(2))
def compare(text, image, base):
    seen={}
    mismatches=[]
    overlaps=[]
    for line_no, address, data in parse_rows(text):
        for index, value in enumerate(data):
            a=address+index
            if a in seen: overlaps.append({"address":hex(a),"line":line_no,"prior_line":seen[a]})
            seen[a]=line_no
            offset=a-base
            if not 0 <= offset < len(image) or image[offset] != value:
                mismatches.append({"address":hex(a),"line":line_no,"listed":value,"rom":image[offset] if 0 <= offset<len(image) else None})
    missing=[hex(base+i) for i in range(len(image)) if base+i not in seen]
    return {"listed_bytes":len(seen),"mismatches":mismatches,"overlaps":overlaps,"missing_bytes":len(missing),"first_missing":missing[:8]}
def main():
    p=argparse.ArgumentParser()
    p.add_argument("--rom",type=Path,required=True)
    p.add_argument("--listings",type=Path,required=True)
    p.add_argument("--out",type=Path,required=True)
    args=p.parse_args()
    rom=args.rom.read_bytes()
    if sha(rom)!=EXPECTED_ROM_SHA256: raise SystemExit("REFUSED: unexpected ROM hash")
    modules={}
    directory=0x7800
    while True:
        row=rom[directory:directory+16]
        if row==bytes(16): break
        name=row[:8].split(b"\0")[0].decode("ascii")
        bank,address,length=struct.unpack_from("<HHI",row,8)
        offset=bank*0x8000+(address&0x7fff)
        modules[name]={"directory_file_offset":directory,"rom_bank":bank,"rom_address":address,"rom_offset":offset,"length":length}
        directory+=16
    results={}
    for path in sorted(args.listings.glob("*.src")):
        m=modules[path.stem].copy()
        image=rom[m["rom_offset"]:m["rom_offset"]+m["length"]]
        if image[0]!=0x4C: raise SystemExit("Module header is not JMP")
        base=struct.unpack_from("<H",image,1)[0]-3
        text=path.read_text()
        result=compare(text,image,base)
        first=next(parse_rows(text))
        changed_bytes = bytes([first[2][0] ^ 1]) + first[2][1:]
        broken = text.replace(first[2].hex(" ").upper(), changed_bytes.hex(" ").upper(), 1)
        negative=compare(broken,image,base)
        m.update({"wram_base":base,"module_sha256":sha(image),"listing_sha256":sha(path.read_bytes()),"verification":result,"negative_control_detected":bool(negative["mismatches"])})
        results[path.stem]=m
    record={"rom_sha256":sha(rom),"rom_bytes":len(rom),"archive_directory_file_offset":0x7800,"modules":results}
    args.out.parent.mkdir(parents=True,exist_ok=True)
    args.out.write_text(json.dumps(record,indent=2)+"\n")
    print(json.dumps({name:{"module_bytes":m["length"],**m["verification"],"negative_control_detected":m["negative_control_detected"]} for name,m in results.items()},indent=2))
    return int(any(m["verification"]["mismatches"] or m["verification"]["overlaps"] or m["verification"]["missing_bytes"] or not m["negative_control_detected"] for m in results.values()))
if __name__=="__main__": raise SystemExit(main())

