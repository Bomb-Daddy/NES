from pathlib import Path
import sys
shell=Path(sys.argv[1]).read_text();cmake=Path(sys.argv[2]).read_text()
assert '#define GK1_PORT_VERSION "1.87.0-dev1"' in shell
assert 'gk183_checkpoint_isolation.c' in cmake
assert 'test_gk183_checkpoint_isolation' in cmake
assert '1.87.0~dev1_amd64' in cmake
assert 'README_EXEC0183.md' in cmake
print('GK183 checkpoint isolation and Linux/NSIS package version contract PASS')
