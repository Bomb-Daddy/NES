#!/usr/bin/env python3
"""EXEC0303: fail-closed Windows staging of inherited EXEC0302 C11 source.
Original NES ROM and gameplay states are never bundled or manufactured.
"""
import hashlib
import os
from pathlib import Path
import sys
import tempfile

SOURCE_DIRECTORY = Path(__file__).resolve().parent
EXPECTED_APP = {
    "gk1_app_state.c": "c4a6707d70f809d9cb3d3bfaa775cc1757c331c81b985a82ec97e3a17f560fc6",
    "gk1_app_state.h": "dbaf417047418e431fa08e2578a64cd506850e6dca079cb5ef6c6c8ae3d1a29e",
}
SOURCE_BLOBS = {
    "gk302_stock_seed_timeline.c": "20da425769cd6f234a450c1056706ae459ece28a",
    "gk302_stock_seed_timeline.h": "438171537f39b5e3b96bd9be5ccb82a589b841b3",
    "test_gk302_stock_seed_timeline.c": "e2381b5808b4250888ed0f69af5d0ce7d1c217d2",
}
WRAPPER = """
/* EXEC0303 original-stock interleaved event witness is a strict dry-run.
 * No source witness alone establishes NES execution or player orders. */
Gk302Status gk1_app_original_seed_timeline(const Gk1App *app,
 const char *private_rom,const uint8_t initial[6],
 const Gk302SeedEvent *events,size_t count,Gk302SeedResult *out){
 if(!app || app->rules_profile!=0u || app->active_world>1u)
   return GK302_STAGE;
 const uint8_t w=app->active_world;
 if(!app->world_ready[w] ||
    app->worlds[w].phase!=GK1_PRE_SETUP_TEMPLATE ||
    app->setup_stage[w]!=GK53_SETUP_AWAIT_SOURCE ||
    app->setup_difficulty[w]<1u || app->setup_difficulty[w]>5u)
   return GK302_STAGE;
 return gk302_replay_stock_seed_timeline(private_rom,initial,events,count,out);
}
"""
PROTO = """
/* EXEC0303 chronological authentic-source witness, no Spring unlock. */
Gk302Status gk1_app_original_seed_timeline(const Gk1App *app,
 const char *private_rom,const uint8_t initial[6],
 const Gk302SeedEvent *events,size_t count,Gk302SeedResult *out);
"""
CMAKE = """
# EXEC0303 linked authenticated EXEC0302 CPU event witness, Windows safe.
add_executable(test_gk302_stock_seed_timeline test_gk302_stock_seed_timeline.c)
target_link_libraries(test_gk302_stock_seed_timeline PRIVATE gk1_app_state)
add_test(NAME GK1_302_PublicTimelineRefusalNoROM
 COMMAND test_gk302_stock_seed_timeline
 \${CMAKE_CURRENT_SOURCE_DIR}/../execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)
if(GK302_PRIVATE_ROM)
 add_test(NAME GK1_302_OriginalStockInterleavedSeedTimelineAndAtomicRollback
  COMMAND test_gk302_stock_seed_timeline
  \${CMAKE_CURRENT_SOURCE_DIR}/../execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv
  \${GK302_PRIVATE_ROM})
endif()
"""

def norm(data):
    if b'\0' in data or b'\r' in data.replace(b'\r\n', b''):
        raise ValueError('noncanonical source bytes')
    return data.replace(b'\r\n', b'\n')

def source_blob(data):
    return hashlib.sha1(b'blob '+str(len(data)).encode()+b'\0'+data).hexdigest()

def replace_once(content, old, new):
    if content.count(old)!=1 or new in content:
        raise ValueError('unexpected source anchor: '+old[:80])
    return content.replace(old,new,1)

def prepare(root):
    root=Path(root)
    current={n:norm((root/n).read_bytes()) for n in (*EXPECTED_APP,'CMakeLists.txt')}
    for name,sha in EXPECTED_APP.items():
        if hashlib.sha256(current[name]).hexdigest()!=sha:
            raise ValueError('EXEC0301 source preimage mismatch: '+name)
    new_sources={}
    for name,blob in SOURCE_BLOBS.items():
        raw=norm((SOURCE_DIRECTORY/name).read_bytes())
        if source_blob(raw)!=blob:raise ValueError('EXEC0302 source blob mismatch: '+name)
        if (root/name).exists():raise ValueError('already staged: '+name)
        new_sources[name]=raw
    c=current['gk1_app_state.c'].decode('utf-8')
    h=current['gk1_app_state.h'].decode('utf-8')
    cm=current['CMakeLists.txt'].decode('utf-8')
    if 'gk302_stock_seed_timeline' in c+h+cm:
        raise ValueError('source is already integrated')
    c+='\n'+WRAPPER
    h=replace_once(h,'#include "gk300_stock_nmi_seed_clock.h"',
      '#include "gk300_stock_nmi_seed_clock.h"\n#include "gk302_stock_seed_timeline.h"')
    end=h.rfind('#endif')
    if end<0 or h[end:].strip()!='#endif':
        raise ValueError('header final guard unexpected')
    h=h[:end]+PROTO+'\n'+h[end:]
    cm=replace_once(cm,'gk300_stock_nmi_seed_clock.c',
       'gk300_stock_nmi_seed_clock.c gk302_stock_seed_timeline.c')
    for name in ('GK1_257_CurrentWin32Workflow_RejectsStaleSourceAndFalsePass',
                 'GK1_297_StockResetInitializerAndUnknownC289Preseed',
                 'GK1_300_OriginalStockNmiGateClockAndRollback'):
        if name not in cm:raise ValueError('inherited test missing: '+name)
    cm+='\n'+CMAKE
    return {'gk1_app_state.c':c.encode(),'gk1_app_state.h':h.encode(),
            'CMakeLists.txt':cm.encode(),**new_sources}

def stage(root):
    root=Path(root)
    changes=prepare(root)
    backups={}
    committed=[]
    try:
        for name,content in changes.items():
            target=root/name
            backups[name]=target.read_bytes() if target.exists() else None
            fd,temp=tempfile.mkstemp(prefix='.gk303-',dir=root)
            try:
                with os.fdopen(fd,'wb') as f:
                    f.write(content);f.flush();os.fsync(f.fileno())
                os.replace(temp,target)
            finally:
                if os.path.exists(temp):os.unlink(temp)
            committed.append(name)
    except Exception:
        for name in reversed(committed):
            old=backups[name]
            if old is None:(root/name).unlink(missing_ok=True)
            else:(root/name).write_bytes(old)
        raise
    return {n:hashlib.sha256(v).hexdigest() for n,v in changes.items()}

if __name__=='__main__':
    if len(sys.argv)!=2:sys.exit('Usage: stage303.py path/to/complete/execution_199')
    try:
        for n,h in stage(sys.argv[1]).items():print('EXEC0303 STAGED',n,h)
    except (ValueError,OSError) as exc:sys.exit('EXEC0303 REFUSED: '+str(exc))
