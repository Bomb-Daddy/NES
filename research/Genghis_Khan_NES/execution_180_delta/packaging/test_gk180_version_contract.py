from pathlib import Path
import sys
source=Path(sys.argv[1]).read_text();cmake=Path(sys.argv[2]).read_text()
assert '#define GK1_PORT_VERSION "1.80.0-dev1"' in source
assert '1.80.0~dev1_amd64' in cmake
assert 'gk180_seed_writer_scan.c' in cmake
assert 'test_gk180_seed_writer_scan' in cmake
assert '--source-seed-writers' in source
print('EXEC0180 source, test and Debian version contract PASS')
