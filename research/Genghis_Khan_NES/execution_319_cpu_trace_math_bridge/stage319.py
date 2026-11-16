#!/usr/bin/env python3
"""EXEC0319: append CPU-claim arithmetic reconciliation to the SAME native app.

Requires staged 0316/0317 source. No original ROM, save, licensed image or
Mesen event records are published or copied. Inherited game source untouched.
"""
from pathlib import Path
import hashlib
import os
import sys
import tempfile

HERE = Path(__file__).resolve().parent
SOURCES = ('gk319_cpu_trace_math.c', 'gk319_cpu_trace_math.h',
           'test_gk319_cpu_trace_math.c')
SOURCE_SHA256 = {
    'gk319_cpu_trace_math.c': '1319766048a5783ea6e17cde6b953855fec3e307a8e803a1717c742073c430e3',
    'gk319_cpu_trace_math.h': '4e51efb7018cf9f1acba354220420f9fe40f05728300879e52be79e4559357c9',
    'test_gk319_cpu_trace_math.c': '629a1a750c954630d471dfa8b08276700dee972b299952891a3f3b5081e82be3',
}
CMAKE_ANCHOR = 'gk316_cpu_claims_ingest.c'
CMAKE_AFTER = 'gk316_cpu_claims_ingest.c gk319_cpu_trace_math.c'
CMAKE_TEST = '''\n# EXEC0319: model complete interleaved original-stock CPU CLAIM arithmetic.
# This NEVER initializes original game first Spring or grants orders.
add_executable(test_gk319_cpu_trace_math test_gk319_cpu_trace_math.c)
target_link_libraries(test_gk319_cpu_trace_math PRIVATE gk1_app_state)
add_test(NAME GK1_319_SourceCpuNmiStoreC289MathClaimsNoOrders
 COMMAND test_gk319_cpu_trace_math)
'''

def digest(data):return hashlib.sha256(data).hexdigest()
def norm(data):
    if b'\0' in data or b'\r\r\n' in data or b'\r' in data.replace(b'\r\n', b''):
        raise ValueError('noncanonical source bytes')
    return data.replace(b'\r\n',b'\n')

def prepare(root):
    root=Path(root)
    cm=norm((root/'CMakeLists.txt').read_bytes()).decode('utf-8')
    libstart=cm.find('add_library(gk1_app_state STATIC ')
    libend=cm.find(')',libstart)
    if libstart<0 or libend<0:raise ValueError('same native gk1_app_state library absent')
    lib=cm[libstart:libend]
    for dependency in ('gk300_stock_nmi_seed_clock.c', 'gk316_cpu_claims_ingest.c'):
        if lib.count(dependency)!=1:raise ValueError('actual inherited app source absent: '+dependency)
    if cm.count('GK1_317_OriginalCpuClaimsAreWorldBoundWithoutOrders')!=1:
        raise ValueError('EXEC0317 current CTest registry not present')
    if 'GK1_319_SourceCpuNmiStoreC289MathClaimsNoOrders' in cm or 'gk319_cpu_trace_math.c' in cm:
        raise ValueError('duplicate EXEC0319')
    lib=lib.replace(CMAKE_ANCHOR,CMAKE_AFTER,1)
    cm=cm[:libstart]+lib+cm[libend:]+CMAKE_TEST
    changes={'CMakeLists.txt':cm.encode()}
    for name in SOURCES:
        src=(HERE/name).read_bytes()
        if digest(src)!=SOURCE_SHA256[name]:raise ValueError('reviewed source changed: '+name)
        if (root/name).exists():raise ValueError('duplicate source: '+name)
        changes[name]=src
    return changes

def stage(root):
    root=Path(root)
    changes=prepare(root)
    originals={n:(root/n).read_bytes() if (root/n).exists() else None for n in changes}
    done=[]
    try:
        for name,data in changes.items():
            fd,tmp=tempfile.mkstemp(prefix='.gk319-',dir=root)
            try:
                with os.fdopen(fd,'wb') as f:
                    f.write(data);f.flush();os.fsync(f.fileno())
                os.replace(tmp,root/name)
                done.append(name)
            finally:
                if os.path.exists(tmp):os.unlink(tmp)
    except BaseException:
        for name in reversed(done):
            if originals[name] is None:(root/name).unlink(missing_ok=True)
            else:(root/name).write_bytes(originals[name])
        raise
    return {n:digest(v) for n,v in changes.items()}

if __name__=='__main__':
    if len(sys.argv)!=2:sys.exit('Usage: stage319.py EXISTING_COMPLETE_GK1_NATIVE_APP_DIR')
    try:
        for k,v in stage(sys.argv[1]).items(): print('GK319 STAGED',k,v)
    except (ValueError,OSError) as e:sys.exit('GK319 FAIL CLOSED: '+str(e))
