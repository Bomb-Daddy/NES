"""Standalone, pinned source-repair and 41x4 seasonal image builder for game v1.4."""
from pathlib import Path
import argparse,hashlib,zipfile,json,csv,io
from PIL import Image
ROOT=Path(__file__).resolve().parent
SOURCE=ROOT/'battlefield_source_inputs'
ap=argparse.ArgumentParser(description=__doc__)
ap.add_argument('--output',type=Path,default=ROOT.parent/'third_branch'/'assets'/'battlefields')
ap.add_argument('--terrain-zip',type=Path,default=SOURCE/'ROTK2_41_Provinces_Original_Static_Terrain_Maps_and_Art_Prototypes_SOURCE.zip')
ap.add_argument('--tile-zip',type=Path,default=SOURCE/'PM_REVIEW_Seasonal_Battlefield_28_Tile_Source_Pack_2026-10-28.zip')
args=ap.parse_args()
OUT=args.output
TERR=args.terrain_zip
TILES=args.tile_zip
assert hashlib.sha256(TERR.read_bytes()).hexdigest()=='04561f76a38deb612fee03b0dcd8834f0936eda96f344bea72e600902538151a'
assert hashlib.sha256(TILES.read_bytes()).hexdigest()=='f87cf3ae0113dc19fef9bff129055f8e582dded7a95572169eeebe63e9b1f10a'
A=zipfile.ZipFile(TERR);B=zipfile.ZipFile(TILES)
raw=[]
for i in range(1,42):
 j=json.loads(A.read(f'Province_Capitals/Province_{i:02d}/Province_{i:02d}_Terrain_Raw.json'))
 assert j['province_id']==i and j['source_sha256']=='11e312691b516e5e29dade3c8ce5434df3561cc69e1aecad9c948acdfddc517a'
 assert int(j['record_offset'],16)==0x26FF7+(i-1)*78
 r=j['row_major'];assert len(r)==12 and all(len(q)==13 for q in r)
 old=[c for row in r for c in row];assert len(old)==156 and all(0<=c<=6 for c in old)
 raw.append(old)
reg=json.loads(B.read('seasonal_battlefield_tile_registry.json'))['tiles']
tiles={}
for season in ('spring','summer','autumn','winter'):
 for i in range(7):
  p=reg[season][str(i)];img=Image.open(io.BytesIO(B.read(p)));img.load();tiles[(season,i)]=img.convert('RGB').resize((48,48),Image.Resampling.NEAREST)
metadata={'schema':'battle-asset-v1.4-shifted-records','source_legacy_terrain_sha256':hashlib.sha256(TERR.read_bytes()).hexdigest(),
 'user_seasonal_tiles_sha256':hashlib.sha256(TILES.read_bytes()).hexdigest(),
 'rom_sha256_as_recorded_in_legacy_source':'11e312691b516e5e29dade3c8ce5434df3561cc69e1aecad9c948acdfddc517a',
 'legacy_byte_offset':'0x26FF7','corrected_byte_offset':'0x26FF8',
 'derivation':'record i at correct offset is old_flat[i][2:] + old_flat[i+1][:2]; original terrain codec packs two sequential cells per ROM byte',
 'limitations':['Original English NES ROM not present in this workspace for byte-perfect oracle','P41 final record byte absent; positions (11,11) and (12,11) inferred as water, not original-ROM validated','User-supplied tiles not certified as final seamless game art','Imported original-game visuals are independent of NES Classic parity'],
 'unverified_cells':[{'province':41,'x':11,'y':11,'assumed':4},{'province':41,'x':12,'y':11,'assumed':4}],
 'provinces':{},'images_sha256':{}}
OUT.mkdir(exist_ok=True,parents=True)
for i,old in enumerate(raw,1):
 arr=old[2:] +(raw[i][:2] if i<41 else [4,4])
 assert len(arr)==156
 castle=[(j%13,j//13) for j,v in enumerate(arr) if v==6]
 assert len(castle)==1, (i,castle)
 # Oracle source reported p1 castle (9,2) from original, independently recomputed here from archived cells.
 if i==1:assert castle==[(9,2)]
 rows=[arr[j*13:(j+1)*13] for j in range(12)]
 metadata['provinces'][str(i)]={'rows':rows,'castle_xy':list(castle[0]),'cell_provenance':'shifted_byte_records' if i<41 else 'last_two_water_inferred'}
 for season in ('spring','summer','autumn','winter'):
  img=Image.new('RGB',(13*48,12*48))
  for index,code in enumerate(arr):img.paste(tiles[(season,code)],((index%13)*48,(index//13)*48))
  p=OUT/f'Province_{i:02d}_{season.capitalize()}_Battlefield.png';img.save(p,optimize=True)
  metadata['images_sha256'][p.name]=hashlib.sha256(p.read_bytes()).hexdigest()
(OUT/'terrain_grids.json').write_text(json.dumps(metadata,sort_keys=True,indent=2)+'\n')
print(json.dumps({'province_count':len(metadata['provinces']),'map_count':len(metadata['images_sha256']),
                  'p01_castle':metadata['provinces']['1']['castle_xy'],'unverified_cells':metadata['unverified_cells'],
                  'size_bytes':sum(p.stat().st_size for p in OUT.glob('*.png'))}))
