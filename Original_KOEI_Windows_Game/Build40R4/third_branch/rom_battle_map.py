"""Q04 verified English NES 41-province terrain, regions and neighbour zones.

Decoded from the Director-owned English MMC5 ROM, SHA-256
11e312691b516e5e29dade3c8ce5434df3561cc69e1aecad9c948acdfddc517a.
This compact tactical data bundle is not the ROM, graphics, or a substitute for
emulator timing. Its deployment selection is deterministic PORT policy; exact
SNES random placement/stacking remains an independent acceptance gate.
"""
from __future__ import annotations
import hashlib
import json
from functools import lru_cache
from pathlib import Path

BUNDLE_PATH=Path(__file__).resolve().parent/'assets'/'battlefields'/'nes_original_41.json'
BUNDLE_SHA256='9bc5a0c945920e0d6fa50c4fcca3cd4c75ff2b986cd147c912f59aba6edb5aa1'
ROM_SHA256='11e312691b516e5e29dade3c8ce5434df3561cc69e1aecad9c948acdfddc517a'
MAP_SOURCE='NES_ROM_VERIFIED_41'
W,H=13,12

@lru_cache(maxsize=1)
def _bundle():
    raw=BUNDLE_PATH.read_bytes()
    if hashlib.sha256(raw).hexdigest()!=BUNDLE_SHA256:
        raise ValueError('Q04 ROM map bundle SHA-256 mismatch')
    data=json.loads(raw)
    if (data['schema']!=MAP_SOURCE or data['rom_sha256']!=ROM_SHA256 or
        set(data['provinces'])!={str(i) for i in range(1,42)} or
        data['offsets']!={'terrain':'0x26FF8','zones':'0x28934','neighbors':'0x28014'}):
        raise ValueError('Q04 map provenance or 41-map index mismatch')
    for key,p in data['provinces'].items():
        if (len(p['terrain'])!=H or len(p['zones'])!=H or
            any(len(row)!=W or set(row)-set('.FHMWTC') for row in p['terrain']) or
            any(len(row)!=W or set(row)-set('01234567') for row in p['zones']) or
            len(p['neighbors'])!=6 or
            any(n is not None and (type(n) is not int or not 1<=n<=41) for n in p['neighbors']) or
            sum(row.count('C') for row in p['terrain'])!=1):
            raise ValueError('Q04 malformed map '+key)
        x,y=p['castle']
        if p['terrain'][y][x]!='C' or p['zones'][y][x]!='7':
            raise ValueError('Q04 castle/defender zone mismatch '+key)
    return data

def province(number):
    if type(number) is not int or not 1<=number<=41:
        raise ValueError('province must be 1..41')
    return _bundle()['provinces'][str(number)]

def deployment_province(target,x,y):
    if not (type(x) is int and type(y) is int and 0<=x<W and 0<=y<H):
        raise ValueError('deployment coordinate out of bounds')
    p=province(target)
    code=int(p['zones'][y][x])
    return None if code==0 else target if code==7 else p['neighbors'][code-1]

def legal_cells(target,source,*,occupied=(),burning=(),include_castle=True):
    """Original region eligibility; deterministic ordering is PORT-only."""
    p=province(target)
    if type(source) is not int or source not in (target,*p['neighbors']):
        raise ValueError('source is not an original neighbouring province')
    blocked=set(map(tuple,occupied))|set(map(tuple,burning))
    return [(x,y) for y in range(H) for x in range(W)
            if (x,y) not in blocked and p['terrain'][y][x]!='M'
            and (include_castle or p['terrain'][y][x]!='C')
            and deployment_province(target,x,y)==source]
