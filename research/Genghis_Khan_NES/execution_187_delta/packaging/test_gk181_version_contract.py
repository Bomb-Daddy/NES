from pathlib import Path
import sys
shell=Path(sys.argv[1]).read_text();cmake=Path(sys.argv[2]).read_text()
assert '#define GK1_PORT_VERSION "1.87.0-dev1"' in shell
assert '--source-bank-seed-writers' in shell
assert 'gk181_bank_seed_trace.c' in cmake
assert 'test_gk181_bank_seed_trace' in cmake
assert '1.87.0~dev1_amd64' in cmake
print("GK181 version / source + deb contract PASS")
