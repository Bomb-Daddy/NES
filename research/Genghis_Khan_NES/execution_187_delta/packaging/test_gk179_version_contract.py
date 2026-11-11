from pathlib import Path
import sys
shell=Path(sys.argv[1]).read_text()
cmake=Path(sys.argv[2]).read_text()
assert 'EXEC0179 source-authentic RNG only after validated setup' in shell
assert '1.87.0~dev1_amd64' in cmake
assert 'test_gk179_rng_setup_boundary' in cmake
print('GK179 version source contract PASS')
