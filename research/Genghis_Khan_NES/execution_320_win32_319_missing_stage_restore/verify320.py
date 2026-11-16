#!/usr/bin/env python3
"""EXEC0320: source-identity gate for EXEC0319 native Win32 source staging.

The published EXEC0319 C11 code is preserved without edits. Source files must
be present AND byte-authentic. On Windows, git may check out CRLF; conversion
is permitted only after an atomic six-file, normalized-SHA validation.

No ROM, licensed assets, emulator claims, game turn, or campaign is involved.
"""
from pathlib import Path
import hashlib
import os
import sys
import tempfile

EXPECTED = {
    'gk319_cpu_trace_math.c': '1319766048a5783ea6e17cde6b953855fec3e307a8e803a1717c742073c430e3',
    'gk319_cpu_trace_math.h': '4e51efb7018cf9f1acba354220420f9fe40f05728300879e52be79e4559357c9',
    'test_gk319_cpu_trace_math.c': '629a1a750c954630d471dfa8b08276700dee972b299952891a3f3b5081e82be3',
    'stage319.py': '3be8eb2fca8a6b00f15a7d84117c3f00676959a233ebcb89a0c6da6e301de816',
    'test_stage319.py': '9336f64517ffe4e8f979509b2a987594c1c494a19dd6e5aeea2be82abc5ce1bd',
    'SHA256_MANIFEST.json': 'f3e07b250b130de2dc7bb32aff2f7278578ed0195b1b191e1db7d42fdfddc155',
}
TEST_ID = 'GK1_319_SourceCpuNmiStoreC289MathClaimsNoOrders'


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def canonical(raw: bytes) -> bytes:
    if not raw or b'\0' in raw or b'\r\r\n' in raw:
        raise ValueError('missing/empty, NUL or doubled CR line ending')
    norm = raw.replace(b'\r\n', b'\n')
    if b'\r' in norm:
        raise ValueError('lone CR line ending')
    norm.decode('utf-8')
    return norm


def audit(source: Path) -> dict[str, bytes]:
    """Read/check all six files before any write. Never silently accept empties."""
    files = {}
    for name, digest in EXPECTED.items():
        data = canonical((Path(source)/name).read_bytes())
        if sha256(data) != digest:
            raise ValueError(f'EXEC0319 SHA mismatch: {name}')
        files[name] = data
    return files


def normalize_checkout(source: Path) -> int:
    """Normalize exact SHA-matching CRLF checkout to LF, with rollback."""
    root = Path(source)
    files = audit(root)
    original = {name: (root/name).read_bytes() for name in files}
    replacements = [name for name in files if original[name] != files[name]]
    changed = []
    try:
        for name in replacements:
            fd, temporary = tempfile.mkstemp(prefix='.gk320-', dir=root)
            try:
                with os.fdopen(fd, 'wb') as stream:
                    stream.write(files[name])
                    stream.flush()
                    os.fsync(stream.fileno())
                os.replace(temporary, root/name)
                changed.append(name)
            finally:
                if os.path.exists(temporary): os.unlink(temporary)
    except BaseException:
        for name in reversed(changed):
            (root/name).write_bytes(original[name])
        raise
    if audit(root) != files:
        raise ValueError('post-normalization identity mismatch')
    return len(replacements)


def verify_staged(source: Path, app: Path) -> None:
    files = audit(source)
    app = Path(app)
    cmake = canonical((app/'CMakeLists.txt').read_bytes()).decode('utf-8')
    libstart = cmake.find('add_library(gk1_app_state STATIC ')
    if libstart < 0:
        raise ValueError('complete inherited app C11 library absent')
    libend = cmake.find(')', libstart)
    if libend < 0:
        raise ValueError('CMake library structure invalid')
    lib = cmake[libstart:libend]
    if lib.count('gk319_cpu_trace_math.c') != 1:
        raise ValueError('EXEC0319 C11 module not linked exactly once to native app')
    if cmake.count(TEST_ID) != 1:
        raise ValueError('EXEC0319 native CTest absent or duplicated')
    for name in ('gk319_cpu_trace_math.c','gk319_cpu_trace_math.h','test_gk319_cpu_trace_math.c'):
        if canonical((app/name).read_bytes()) != files[name]:
            raise ValueError('staged native C11 source mismatch: ' + name)
    for dependency in ('gk300_stock_nmi_seed_clock.c','gk316_cpu_claims_ingest.c'):
        if lib.count(dependency) != 1:
            raise ValueError('inherited full game dependency absent: '+ dependency)


def main(argv: list[str]) -> int:
    if len(argv) not in (2, 3):
        print('Usage: verify320.py EXEC0319_SOURCE_DIR [STAGED_NATIVE_APP_DIR]',file=sys.stderr)
        return 2
    try:
        count = normalize_checkout(Path(argv[1]))
        if len(argv) == 3:
            verify_staged(Path(argv[1]),Path(argv[2]))
        print('GK320 VERIFIED_STATIC 6/6 authentic source files; normalized',count,
              'checkout files; integrated native CMake validated' if len(argv)==3
              else 'pre-stage source validated')
        return 0
    except (OSError,ValueError,UnicodeError) as error:
        print('GK320 FAIL CLOSED:',error,file=sys.stderr)
        return 3

if __name__ == '__main__':
    raise SystemExit(main(sys.argv))
