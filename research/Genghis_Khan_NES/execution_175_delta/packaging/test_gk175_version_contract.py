#!/usr/bin/env python3
"""EXEC0175 reproducible package/launcher consistency (not Win32 runtime QA)."""
import pathlib,re,subprocess,sys
if len(sys.argv)!=5: raise SystemExit('need binary, CMakeLists, desktop, CPackConfig')
binfile,cmfile,desktopfile,configfile=map(pathlib.Path,sys.argv[1:])
source=cmfile.read_text(); desk=desktopfile.read_text(); pkg=configfile.read_text()
version='1.75.0'; display=version+'-dev1'
assert all(p.is_file() for p in (binfile,cmfile,desktopfile,configfile))
assert source.count('set(CPACK_PACKAGE_VERSION "'+version+'")') == 2 # Windows and Debian
assert 'set(CPACK_NSIS_DISPLAY_NAME "Genghis Khan I Research '+display+'")' in source
assert 'set(CPACK_PACKAGE_FILE_NAME "gk1-native-research_'+version+'~dev1_amd64")' in source
assert 'Name=Genghis Khan I Research '+display+'\n' in desk
assert 'Exec=gk1-native-research\n' in desk
assert 'CPACK_PACKAGE_VERSION:STRING' not in pkg # do not infer CMakeCache format
assert 'CPACK_PACKAGE_VERSION "'+version+'"' in pkg
assert 'CPACK_PACKAGE_FILE_NAME "gk1-native-research_'+version+'~dev1_amd64"' in pkg
r=subprocess.run([str(binfile),'--version'],capture_output=True,text=True,timeout=20)
assert r.returncode==0 and ('research prototype '+display) in r.stdout
assert 'not full game' in r.stdout
assert not re.search(r'Name=Genghis Khan I Research 1\.(?:73|74)\.0',desk)
print('GK175 launcher, Linux runtime, DEB, authored NSIS version: PASS; actual NSIS/Win32 NOT RUN')