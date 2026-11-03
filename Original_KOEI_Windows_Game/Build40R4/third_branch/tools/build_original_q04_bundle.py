"""Extract Q04 logical maps from an owned verified English ROTK II NES ROM.

Usage: python -m third_branch.tools.build_original_q04_bundle /path/to/owned.nes out.json
No ROM bytes are included in the source distribution. Reproduction must match
third_branch.rom_battle_map.BUNDLE_SHA256 before deployment.
"""
from __future__ import annotations
import hashlib,json,sys
from pathlib import Path

ROM_SHA='11e312691b516e5e29dade3c8ce5434df3561cc69e1aecad9c948acdfddc517a'
SYMBOLS='.FHMWTC'

def extract(rom: bytes):
    if len(rom)!=524304 or hashlib.sha256(rom).hexdigest()!=ROM_SHA:
        raise ValueError('Q04 requires exact owned English NES ROM SHA-256')
    out={}
    for i in range(41):
        def unpack(offset):
            raw=rom[offset+i*78:offset+(i+1)*78]
            return [v for b in raw for v in (b>>4,b&15)]
        terrain=unpack(0x26FF8);zones=unpack(0x28934)
        neighbors=[None if x==255 else x for x in rom[0x28014+i*6:0x28014+(i+1)*6]]
        if any(v>6 for v in terrain) or any(v>7 for v in zones) or any(n is not None and not 1<=n<=41 for n in neighbors):
            raise ValueError('bad Q04 map source')
        castles=[(j%13,j//13) for j,v in enumerate(terrain) if v==6]
        if len(castles)!=1:raise ValueError('not exactly one castle')
        out[str(i+1)]={
            'terrain':[''.join(SYMBOLS[v] for v in terrain[y*13:(y+1)*13]) for y in range(12)],
            'zones':[''.join(str(v) for v in zones[y*13:(y+1)*13]) for y in range(12)],
            'neighbors':neighbors,'castle':list(castles[0]),
        }
    source=b''.join(rom[base+i*78:base+(i+1)*78] for base in (0x26FF8,0x28934) for i in range(41))+rom[0x28014:0x28014+246]
    return {'schema':'NES_ROM_VERIFIED_41','rom_sha256':ROM_SHA,'table_sha256':hashlib.sha256(source).hexdigest(),
            'offsets':{'terrain':'0x26FF8','zones':'0x28934','neighbors':'0x28014'},'provinces':out}

def main(argv):
    if len(argv)!=3:raise SystemExit('Usage: python -m third_branch.tools.build_original_q04_bundle owned.nes out.json')
    obj=extract(Path(argv[1]).read_bytes())
    Path(argv[2]).write_text(json.dumps(obj,sort_keys=True,separators=(',',':'))+'\n',encoding='utf8')
    print(hashlib.sha256(Path(argv[2]).read_bytes()).hexdigest())

if __name__=='__main__':main(sys.argv)
