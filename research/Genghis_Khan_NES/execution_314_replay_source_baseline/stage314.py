#!/usr/bin/env python3
"""EXEC0314: fail-closed integration into inherited complete Gk1App source.

Use AFTER the latest verified EXEC0313 staging. No ROM is accessed or published.
"""
from pathlib import Path
import os
import sys
import tempfile

SOURCE_ANCHOR = (
    ' memcpy(next->worlds,old->worlds,sizeof next->worlds);\n'
    ' memcpy(next->world_ready,old->world_ready,sizeof next->world_ready);'
)
SOURCE_PATCH = (
    ' memcpy(next->worlds,old->worlds,sizeof next->worlds);\n'
    ' /* EXEC0314: isolated tactical replay must retain the original 14/27\n'
    '  * immutable CSV baselines for source/finance/labor checks. These are\n'
    '  * not NES first-Spring initialization or playable orders. */\n'
    ' memcpy(next->finance_source_baseline,old->finance_source_baseline,\n'
    '        sizeof next->finance_source_baseline);\n'
    ' memcpy(next->world_ready,old->world_ready,sizeof next->world_ready);'
)
CMAKE_ANCHOR = 'add_library(gk1_app_state STATIC '
CMAKE_APPEND = '''\n# EXEC0314: preserve 14/27 source baselines after isolated tactical replay.\nadd_executable(test_gk314_replay_source_baseline test_gk314_replay_source_baseline.c)\ntarget_link_libraries(test_gk314_replay_source_baseline PRIVATE gk1_app_state)\nadd_test(NAME GK1_314_ReplayPreservesSourceBaseline COMMAND test_gk314_replay_source_baseline ${CMAKE_CURRENT_SOURCE_DIR}/../execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)\n'''


def integrate(source_dir: Path, inject_failure_after: int = 0):
    source_dir = Path(source_dir).resolve()
    paths = {
        'app': source_dir / 'gk1_app_state.c',
        'cmake': source_dir / 'CMakeLists.txt',
        'test': source_dir / 'test_gk314_replay_source_baseline.c',
    }
    if any(not paths[k].is_file() for k in ('app', 'cmake')) or paths['test'].exists():
        raise ValueError('missing inherited complete sources or duplicate test')
    app = paths['app'].read_bytes()
    cmake = paths['cmake'].read_bytes()
    if app.count(SOURCE_ANCHOR.encode()) != 1 or b'EXEC0314' in app:
        raise ValueError('original app replay-source anchor missing, drift or duplicate')
    if cmake.count(CMAKE_ANCHOR.encode()) != 1 or b'test_gk314_replay_source_baseline' in cmake:
        raise ValueError('native CMake target anchor missing, drift or duplicate')
    # Prevalidate every input before touching any inherited file.
    test = Path(__file__).with_name('test_gk314_replay_source_baseline.c').read_bytes()
    if b'gk1_app_restore_session' not in test or b'gk1_app_restore_divide' not in test:
        raise ValueError('regression test missing critical replay paths')
    desired = {
        paths['app']: app.replace(SOURCE_ANCHOR.encode(), SOURCE_PATCH.encode()),
        paths['cmake']: cmake + CMAKE_APPEND.encode(),
        paths['test']: test,
    }
    originals = {paths['app']: app, paths['cmake']: cmake, paths['test']: None}
    committed = []
    def replace_atomic(path, payload):
        fd, tmp = tempfile.mkstemp(dir=str(path.parent), prefix='.gk314-')
        try:
            with os.fdopen(fd, 'wb') as stream:
                stream.write(payload)
                stream.flush()
                os.fsync(stream.fileno())
            os.replace(tmp, path)
        finally:
            if os.path.exists(tmp):
                os.unlink(tmp)
    try:
        for path, payload in desired.items():
            replace_atomic(path, payload)
            committed.append(path)
            if inject_failure_after == len(committed):
                raise OSError('injected staging failure for rollback verification')
    except BaseException:
        for path in reversed(committed):
            original = originals[path]
            if original is None:
                path.unlink(missing_ok=True)
            else:
                replace_atomic(path, original)
        raise
    return paths


if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.exit('Usage: stage314.py /path/to/latest/staged/execution_199')
    integrate(Path(sys.argv[1]))
    print('EXEC0314 integrated inherited native C11 app; original Spring still OPEN')
