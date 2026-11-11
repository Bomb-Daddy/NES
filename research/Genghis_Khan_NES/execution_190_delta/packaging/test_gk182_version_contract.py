from pathlib import Path
import sys
shell=Path(sys.argv[1]).read_text();cmake=Path(sys.argv[2]).read_text()
assert '#define GK1_PORT_VERSION "1.90.0-dev1"' in shell
assert '--source-archived-bank' in shell
assert 'gk182_archived_source_bank.c' in cmake
assert 'test_gk182_archived_source_bank' in cmake
assert '1.90.0~dev1_amd64' in cmake
print('GK182 version, saved-state original provenance, source + Debian contract PASS')
