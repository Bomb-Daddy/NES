"""Source-verified NES fixed images for private-use original third-branch presentation.

Events only display for exact matching committed event kinds; this registry never
generates story events or alters simulation/save state. 28 historical ruler faces
are by NES officer ID, not faction slot or role placeholders.
"""
from __future__ import annotations
from functools import lru_cache
from pathlib import Path
import json
import hashlib
from PIL import Image

ROOT = Path(__file__).resolve().parent
INDEX = ROOT / 'assets' / 'nes_illustrations' / 'manifest.json'

EVENT_KIND_TO_IMAGE = {
    'locust_plague':'event_01_locust_plague',
    'typhoon':'event_02_typhoon',
    'flood':'event_03_flood',
    'epidemic':'event_04_epidemic',
    'riot':'event_05_riot',
    'rebellion':'event_06_rebellion',
    'falling_star':'event_07_falling_star_evil_omen',
    'ruler_died':'event_08_ruler_died_funeral',
    'secret_letter_delivered':'event_09_secret_letter_delivered',
    'messenger_caught_secret_letter':'event_10_messenger_caught_with_secret_letter',
    'messenger_caught_gold_rice':'event_11_caught_transporting_gold_rice',
}

@lru_cache(maxsize=1)
def manifest():
    return json.loads(INDEX.read_text(encoding='utf-8'))

def _load(category, key, size=None):
    record=manifest().get(category,{}).get(str(key))
    if record is None:return None
    target=ROOT / record['path']
    if not target.resolve().is_relative_to((ROOT/'assets/nes_illustrations').resolve()):return None
    try:
        if hashlib.sha256(target.read_bytes()).hexdigest()!=record['sha256']:
            return None
        with Image.open(target) as image:
            if list(image.size)!=record['dimensions']:return None
            return image.convert('RGB')
    except (OSError,ValueError):
        return None

def ruler_portrait(nes_officer_id):
    if type(nes_officer_id) is not int:return None
    return _load('rulers',nes_officer_id)

def seasonal_still(season):
    key={'Spring':'season_1_spring','Summer':'season_2_summer',
         'Autumn':'season_3_autumn','Winter':'season_4_winter'}.get(season)
    return _load('seasons',key) if key else None

def event_still_for_committed_record(record,province_id=None):
    if not isinstance(record,dict):return None
    event_key=EVENT_KIND_TO_IMAGE.get(record.get('kind'))
    if not event_key:return None
    target=record.get('province_id',record.get('province'))
    if target is not None and province_id is not None and target not in (province_id,f'p{province_id}'):
        return None
    return _load('events',event_key)
