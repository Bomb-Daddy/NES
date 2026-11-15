#!/usr/bin/env python3
"""EXEC0304: fail-closed, exact-source native C11 integration on EXEC0302.

Usage: python stage304.py INHERITED_APP_ROOT
Stages only after verified complete native EXEC0302 state, not a toy fork.
Private stock ROM is an external test input and is NEVER in this tree.
"""
from __future__ import annotations
import hashlib
import os
from pathlib import Path
import sys
import tempfile

PRE = {
    'gk1_app_state.c': 'e8031320743d9637767b46688f2e695b67fa38dacad6c2783a64ecb1f88e4b85',
    'gk1_app_state.h': 'ae21c0a4e658cebaccaa6e57e46bd26b0c7c193f5d09df02e4bd01b1105a1945',
    'CMakeLists.txt': 'd10e753bf0b78d66f9e1caeac0af3a2f9919c475c1dbb1b26a488803ee874510',
}
POST = {
    'gk1_app_state.c': '08aa197a7fa2e5bb655c337c595a28015328365a2b46a553c0e353669434dc78',
    'gk1_app_state.h': '6d3b39bd428143f34666785ba1e46dd4195526f87d9efcadbdadcba5bc4f7407',
    'CMakeLists.txt': '8ee1c8e9df8475b2ebb99c49af0726fdbb8534550f1823881e8a06a3256f8e52',
}
NEW = {
    'gk304_stock_store_value.c': '315351065db80d9a4b466f4a1bd69ce452192f4988ecd5cf14d92a13ba1cf7e3',
    'gk304_stock_store_value.h': 'ee6603e159111e2376cad1b2a208d2dd43fae5b3e4bb59724ac1cc49bba15afa',
    'test_gk304_stock_store_value.c': 'c2ab30829b4e356138edf0dcfd4ffc24b5677279bc963544b8c43c76ff0af0df',
}


def checksum(b: bytes) -> str:
    return hashlib.sha256(b).hexdigest()


def normalize(b: bytes) -> bytes:
    if b'\r' in b.replace(b'\r\n', b''):
        raise ValueError('unsupported carriage return in source')
    return b.replace(b'\r\n', b'\n')


def require_once(text: str, anchor: str) -> None:
    if text.count(anchor) != 1:
        raise ValueError(f'non-unique or absent source anchor: {anchor[:70]!r}')


def stage(root: Path, source: Path) -> dict[str, bytes]:
    """Return complete staged byte map after all preconditions; no writes yet."""
    if not root.is_dir():
        raise ValueError('complete inherited native app directory missing')
    target = {}
    for name, sha in PRE.items():
        content = normalize((root / name).read_bytes())
        if checksum(content) != sha:
            raise ValueError(f'wrong native inherited {name} source or already staged: {checksum(content)}')
        target[name] = content.decode('utf-8')
    # No unverified dependency deletion: direct old chronological replay and both
    # campaign state functions must be in exact authenticated predecessor.
    for name in NEW:
        if (root / name).exists():
            raise ValueError(f'new source already present; duplicate refused: {name}')
        b = normalize((source / name).read_bytes())
        if checksum(b) != NEW[name]:
            raise ValueError(f'new source tampered: {name}')
        target[name] = b
    h = target['gk1_app_state.h']
    require_once(h, '#include "gk302_stock_seed_timeline.h"\n')
    require_once(h, 'Gk302Status gk1_app_original_seed_timeline(')
    require_once(h, '/* EXEC0302: original source-authenticated interleaved seed witness chronology;')
    h=h.replace('#include "gk302_stock_seed_timeline.h"\n',
                '#include "gk302_stock_seed_timeline.h"\n#include "gk304_stock_store_value.h"\n', 1)
    marker = 'Gk302Status gk1_app_original_seed_timeline('
    h=h.replace(marker, (source / 'app304.h.inc').read_text()+marker, 1)
    target['gk1_app_state.h'] = h
    c=target['gk1_app_state.c']
    require_once(c, 'return gk302_replay_stock_seed_timeline(rom,initial,events,count,out);')
    if not c.endswith('\n'):
        raise ValueError('unexpected C11 app file terminator')
    c += (source / 'app304.c.inc').read_text()
    target['gk1_app_state.c'] = c
    cm=target['CMakeLists.txt']
    require_once(cm, 'gk302_stock_seed_timeline.c gk282_stock_rng_trace.c')
    require_once(cm, 'GK1_302_PublicTimelineRefusalNoROM')
    cm=cm.replace('gk302_stock_seed_timeline.c gk282_stock_rng_trace.c',
                  'gk302_stock_seed_timeline.c gk304_stock_store_value.c gk282_stock_rng_trace.c',1)
    cm += '''# EXEC0304: original ROM STORE opcode must match CPU register witness.
add_executable(test_gk304_stock_store_value test_gk304_stock_store_value.c)
target_link_libraries(test_gk304_stock_store_value PRIVATE gk1_app_state)
add_test(NAME GK1_304_PublicRegisterStoreRefusalNoROM
 COMMAND test_gk304_stock_store_value
 ${CMAKE_CURRENT_SOURCE_DIR}/../execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)
if(GK304_PRIVATE_ROM)
 add_test(NAME GK1_304_OriginalRomOpcodeRegisterAndAtomicRollback
 COMMAND test_gk304_stock_store_value
 ${CMAKE_CURRENT_SOURCE_DIR}/../execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv
 ${GK304_PRIVATE_ROM})
endif()
'''
    target['CMakeLists.txt']=cm
    out={name:(v.encode('utf-8') if isinstance(v,str) else v) for name,v in target.items()}
    for name,sha in POST.items():
        if checksum(out[name])!=sha:
            raise ValueError(f'unexpected staged {name} postimage: {checksum(out[name])}')
    return out


def commit(root:Path, updates:dict[str,bytes]):
    # Validate complete content up front. Stage temporary files on same volume
    # before any renames. Rollback every file on any failed replacement.
    old={name:((root/name).read_bytes() if (root/name).exists() else None)
         for name in updates}
    staged={}
    try:
        for name,data in updates.items():
            fd,path=tempfile.mkstemp(prefix=f'.{name}.gk304-',dir=root)
            staged[name]=Path(path)
            with os.fdopen(fd,'wb') as f:
                f.write(data)
                f.flush();os.fsync(f.fileno())
        for name,tmp in staged.items():
            os.replace(tmp,root/name)
    except BaseException:
        for name,data in old.items():
            if data is None:
                (root/name).unlink(missing_ok=True)
            else:
                (root/name).write_bytes(data)
        raise
    finally:
        for tmp in staged.values():tmp.unlink(missing_ok=True)


def main(argv:list[str])->int:
    if len(argv)!=2:print('usage: stage304.py APP_ROOT',file=sys.stderr);return 2
    try:
        source=Path(__file__).resolve().parent
        root=Path(argv[1]).resolve()
        updates=stage(root,source)
        commit(root,updates)
        print('GK304 staged exact native C11 inherited source:', ', '.join(updates))
        return 0
    except (OSError,ValueError) as e:
        print(f'GK304 fail closed: {e}',file=sys.stderr)
        return 3

if __name__=='__main__':raise SystemExit(main(sys.argv))
