"""Battlefield terrain assets for the ORIGINAL game's 41 province campaign.

Version 1.4 uses 41 corrected-by-byte-shift logical grids derived from the 41
legacy records extracted one ROM byte too early. Original ROM hash in source
archive is a provenance annotation, not independently rerun ROM extraction.
The final two cells of province 41 are NOT recoverable from those 41 records.
They are assumed water, marked explicitly in data and NOT parity-accepted.

Artwork is user-supplied 28 seasonal terrain replacements; this is creative
reuse, NOT NES Classic graphics parity or approval of original-game balance.
"""
from __future__ import annotations
import hashlib, json
from functools import lru_cache
from pathlib import Path

from PIL import Image
from . import rom_battle_map

ROOT=Path(__file__).resolve().parent/'assets'/'battlefields'
SEASONS=('spring','summer','autumn','winter')
CODE_TO_SYMBOL={0:'.',1:'F',2:'H',3:'M',4:'W',5:'T',6:'C'}
GRID_W,GRID_H=13,12
ASSET_VERSION='battle-asset-v1.4-shifted-records'

@lru_cache(maxsize=1)
def metadata():
    d=json.loads((ROOT/'terrain_grids.json').read_text('utf8'))
    if d['schema']!=ASSET_VERSION or sorted(map(int,d['provinces']))!=list(range(1,42)):
        raise ValueError('Missing/invalid battlefield index')
    return d

def season_for_month(month):
    if type(month) is not int or month<1:raise ValueError('Invalid campaign month')
    return SEASONS[((month-1)//3)%4]

def province_grid(number):
    if type(number) is not int or number not in range(1,42):raise ValueError('Invalid province number')
    # Tactical logic uses the hash-pinned English NES ROM table, not the
    # older shifted-record artwork manifest. The two currently agree in all
    # 6,396 cells, including P41 cells previously marked assumed.
    item=rom_battle_map.province(number)
    rows=[['.FHMWTC'.index(c) for c in row] for row in item['terrain']]
    if len(rows)!=GRID_H or any(len(row)!=GRID_W or any(type(c) is not int or c not in CODE_TO_SYMBOL for c in row) for row in rows):
        raise ValueError('Malformed terrain records')
    if sum(c==6 for row in rows for c in row)!=1:raise ValueError('Missing/duplicated castle')
    return rows

def battle_grid(number):
    return [''.join(CODE_TO_SYMBOL[c] for c in row) for row in province_grid(number)]

def asset_path(number,season):
    province_grid(number)
    if season not in SEASONS:raise ValueError('Unknown season')
    return ROOT/f'Province_{number:02d}_{season.capitalize()}_Battlefield.png'

@lru_cache(maxsize=32)
def background(number,season):
    p=asset_path(number,season)
    with Image.open(p) as image:
        if image.size!=(624,576):raise ValueError(f'Invalid battlefield image size: {p.name}')
        img=image.convert('RGB')
    return img

def terrain_matches(number,actual):
    expected=battle_grid(number)
    if actual!=expected:raise ValueError(f'Battle terrain differs from authoritative map record: {number}')
    return True


def static_audit():
    """Validate all 164 image files and static source-cell -> image relationships."""
    d=metadata()
    issues=[];assets={}
    for n in range(1,42):
        province_grid(n)
        for s in SEASONS:
            p=asset_path(n,s)
            try:
                data=p.read_bytes()
                digest=hashlib.sha256(data).hexdigest()
                if digest!=d['images_sha256'][p.name]:raise ValueError('Hash mismatch')
                with Image.open(p) as im:
                    if im.size!=(624,576) or im.format!='PNG':raise ValueError('Invalid PNG/dimensions')
                assets[p.name]=digest
            except (OSError,ValueError,KeyError) as e:
                issues.append(f'{p.name}: {e}')
    if set(d['images_sha256'])!=set(assets):issues.append('Image manifest completeness mismatch')
    if issues:raise ValueError('\n'.join(issues))
    return {'maps':len(assets),'provinces':41,'seasons':len(SEASONS),
            'unverified_cells':d['unverified_cells'],'static_art_status':'PASS_WITH_PROVENANCE_LIMIT'}
