#!/usr/bin/env python3
"""Hostile staging negative and reproducible positive integration tests."""
import tempfile, hashlib, sys, os
from pathlib import Path
from stage285 import stage,OLD_SHA,NEW_SHA
HERE=Path(__file__).resolve().parent
ORIGIN=HERE/'execution_199'
# Explicit local predecessor archive required for host-independent reproducibility.
# Never fetch, bundle, or infer the private original-stock ROM here.
OLD_ARCHIVE=Path(sys.argv[1] if len(sys.argv)>1 else os.environ.get('GK1_EXEC0284_SOURCE_ZIP',''))
if not OLD_ARCHIVE.is_file():
    raise SystemExit('usage: python test_stage285.py <verified-EXEC0284-source.zip>')
# For independent tests, reconstruct only staging-required immutable predecessor
# files from the verified predecessor source ZIP.
import zipfile
with zipfile.ZipFile(OLD_ARCHIVE) as z:
    member_names=z.namelist()
    def find_member(name):
        vals=[n for n in member_names if n.endswith('/execution_199/'+name) or n=='execution_199/'+name]
        assert len(vals)==1,(name,vals)
        return z.read(vals[0])
    with tempfile.TemporaryDirectory(prefix='gk285_') as tmp:
        p=Path(tmp)
        app=p/'execution_199'; (app/'packaging').mkdir(parents=True)
        for f in (*OLD_SHA,'CMakeLists.txt'):
            (app/f).write_bytes(find_member(f))
        original_cmake=(app/'CMakeLists.txt').read_bytes()
        gate=app/'packaging/test_gk265_current_win32_source.py'
        gate.write_text("EXPECTED_APP_SHA = '"+OLD_SHA['gk1_app_state.c']+"'\n")
        before={f:(app/f).read_bytes() for f in OLD_SHA}
        stage(app)
        for f,digest in NEW_SHA.items():
            assert hashlib.sha256((app/f).read_bytes()).hexdigest()==digest,f
        assert 'GK1_285_SelectedDifficulty_OriginalC289_CausalAndAtomic' in (app/'CMakeLists.txt').read_text()
        try: stage(app)
        except ValueError: pass
        else: raise AssertionError('duplicate stage accepted')
        # Every deliberately corrupt predecessor needs zero writes and fail.
        for name in OLD_SHA:
            for f in OLD_SHA:
                (app/f).write_bytes(before[f])
            (app/'test_gk285_source_difficulty.c').unlink(missing_ok=True)
            (app/'CMakeLists.txt').write_bytes(original_cmake)
            gate.write_text("EXPECTED_APP_SHA = '"+OLD_SHA['gk1_app_state.c']+"'\n")
            t=app/name
            t.write_bytes(t.read_bytes()+b'\n/* corrupt preimage */')
            originals={f:(app/f).read_bytes() for f in OLD_SHA}
            try:stage(app)
            except ValueError:pass
            else:raise AssertionError('corrupt '+name+' accepted')
            assert all((app/f).read_bytes()==originals[f] for f in OLD_SHA),'non-atomic rollback'
            assert not (app/'test_gk285_source_difficulty.c').exists(),'test leaked'
        print('EXEC0285 hostile exact preimage/duplicate/rollback/positive source stage PASS')
