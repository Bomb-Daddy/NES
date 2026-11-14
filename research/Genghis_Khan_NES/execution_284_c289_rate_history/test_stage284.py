#!/usr/bin/env python3
"""Hostile source stage acceptance and rollback with source-specific test fixture."""
import hashlib
from pathlib import Path
import tempfile
import stage284 as st

def run():
    checks = 0
    with tempfile.TemporaryDirectory() as d:
        root = Path(d); dst = root/'game'; src = root/'source'; dst.mkdir();src.mkdir()
        old_c = b'old-c\n'; old_h = b'old-h\n'
        new = {'gk282_stock_rng_trace.c': b'new-c\n',
               'gk282_stock_rng_trace.h': b'new-h\n',
               'test_gk284_rate_history.c': b'new-test\n'}
        st.PRE = {k:hashlib.sha256(v).hexdigest() for k,v in
                  [('gk282_stock_rng_trace.c',old_c),('gk282_stock_rng_trace.h',old_h)]}
        st.POST_GIT_BLOB = {k:st.blob_id(v) for k,v in new.items()}
        (dst/'gk282_stock_rng_trace.c').write_bytes(old_c)
        (dst/'gk282_stock_rng_trace.h').write_bytes(old_h)
        cm=b'gk282_stock_rng_trace.c\nadd_test(NAME gk282_original_c289_trace COMMAND x)\ntarget_link_libraries(test_gk282_stock_rng_trace PRIVATE gk1_app_state)\n'
        (dst/'CMakeLists.txt').write_bytes(cm)
        for k,v in new.items():(src/k).write_bytes(v)
        def refuse():
            try:st.stage(dst,src)
            except ValueError:pass
            else:raise AssertionError('invalid stage accepted')
            assert (dst/'CMakeLists.txt').read_bytes()==cm
            assert (dst/'gk282_stock_rng_trace.c').read_bytes()==old_c
            assert (dst/'gk282_stock_rng_trace.h').read_bytes()==old_h
            assert not (dst/'test_gk284_rate_history.c').exists()
        (src/'test_gk284_rate_history.c').write_bytes(b'bad')
        refuse();checks+=1
        (src/'test_gk284_rate_history.c').write_bytes(new['test_gk284_rate_history.c'])
        (dst/'gk282_stock_rng_trace.c').write_bytes(b'stale')
        try:st.stage(dst,src)
        except ValueError:pass
        else:raise AssertionError('stale preimage accepted')
        assert (dst/'gk282_stock_rng_trace.c').read_bytes()==b'stale'
        checks+=1
        (dst/'gk282_stock_rng_trace.c').write_bytes(old_c)
        st.stage(dst,src)
        for k,v in new.items(): assert (dst/k).read_bytes()==v
        assert (dst/'CMakeLists.txt').read_bytes().count(b'GK1_284_OriginalC289AllHistoryRateCandidateRollback')==1
        checks+=1
        try:st.stage(dst,src)
        except ValueError:checks+=1
        else:raise AssertionError('double staging accepted')
    print(f'EXEC0284 Win32 staging hostile QA {checks}/{checks} PASS')
if __name__ == '__main__':run()
