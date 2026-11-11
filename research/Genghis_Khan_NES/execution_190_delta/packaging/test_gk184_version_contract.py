from pathlib import Path
import sys
assert len(sys.argv)==3
shell=Path(sys.argv[1]).read_text()
cmake=Path(sys.argv[2]).read_text()
assert '#define GK1_PORT_VERSION "1.90.0-dev1"' in shell
assert '1.90.0~dev1_amd64' in cmake
assert 'README_EXEC0184.md' in cmake
assert 'test_gk184_controller_source_gate' in cmake
print('EXEC0184 version/package and controller gate contract PASS')
