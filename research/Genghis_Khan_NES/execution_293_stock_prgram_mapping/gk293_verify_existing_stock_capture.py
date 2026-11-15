#!/usr/bin/env python3
"""Independently audit already authenticated private EXEC0076 Mesen evidence.
No ROM/checkpoints/screenshots or licensed imagery in this executable source.
This validates historical capture consistency, NOT new emulator execution.
"""
import hashlib
import json
import pathlib
import zipfile

ORIGINAL_ROM_SHA256='ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3'
EVIDENCE_SHA256='8b757e11b12aeb3efe93e36c10628599ca5f12e91b15c02354cd3ebbd11bf52a'
STAGES=('fresh_09_after_a.json','fresh_14_after_wait.json')

class EvidenceError(ValueError): pass

def check_views(stage, label):
    views=stage['views']
    cpu,work,save=tuple(views[k] for k in ('nesMemory','nesWorkRam','nesSaveRam'))
    fields=('year','gold','food','troops','rate','orders','country_count')
    if any(cpu.get(k)!=work.get(k) for k in fields):
        raise EvidenceError(f'{label} CPU bus != physical WorkRAM mapping')
    if any(save.get(k)!=0 for k in fields):
        raise EvidenceError(f'{label} incorrectly mapped stock fresh to archived SaveRAM')
    return cpu

def verify_private_evidence(rom, zipname):
    rom=pathlib.Path(rom).read_bytes()
    if len(rom)!=262160 or hashlib.sha256(rom).hexdigest()!=ORIGINAL_ROM_SHA256:
        raise EvidenceError('private original stock ROM mismatch')
    zpath=pathlib.Path(zipname)
    if hashlib.sha256(zpath.read_bytes()).hexdigest()!=EVIDENCE_SHA256:
        raise EvidenceError('private source-capture ZIP mismatch')
    with zipfile.ZipFile(zpath) as archive:
        if archive.testzip() is not None:raise EvidenceError('private evidence CRC mismatch')
        older,newer=(json.loads(archive.read('private/'+x)) for x in STAGES)
    early=check_views(older,'pre-Spring')
    later=check_views(newer,'first-Spring')
    if (early['year'],early['gold'],early['food'],early['troops'],early['rate'],early['orders'],early['country_count'])!=(1174,425,258,60,0,0,14):
        raise EvidenceError('pre-Spring observed values not preserved')
    if (later['year'],later['gold'],later['food'],later['troops'],later['rate'],later['orders'],later['country_count'])!=(1175,430,208,61,121,3,14):
        raise EvidenceError('authentic fresh Spring 1175 changed')
    records=bytes.fromhex(newer['original_country_records_workram'])
    if len(records)!=14*83:raise EvidenceError('not 14 original 83-byte records')
    if newer['frame']<=older['frame'] or newer['cpu']['cycleCount']<=older['cpu']['cycleCount']:
        raise EvidenceError('fresh source chronology inconsistent')
    return {'source_identity':True,'authentic_historical_records':14,
            'pre_spring_orders':early['orders'],'fresh_spring_orders':later['orders'],
            'cpu_6dcd_prgram_mapping_supported':True,
            'cpu_0dcd_internal_mirror_is_not_stock_country_ram':True,
            'new_mesen_runtime_executed':False,'native_first_spring_implemented':False}

if __name__=='__main__':
    import argparse
    parser=argparse.ArgumentParser()
    parser.add_argument('private_stock_rom')
    parser.add_argument('private_source_capture_zip')
    a=parser.parse_args()
    print(verify_private_evidence(a.private_stock_rom,a.private_source_capture_zip))
