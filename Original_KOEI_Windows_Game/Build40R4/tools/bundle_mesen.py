"""Supply official MesenCE 2.2.1 GPL emulator binaries to two-mode builds.

ROMs are copyrighted user assets and are NEVER embedded or fetched.
The selected native build includes runtime dependencies rather than requiring
.NET. The hash is pinned to the upstream release assets, not a mirror.
"""
from __future__ import annotations
import hashlib, io, shutil, sys, urllib.request, zipfile
from pathlib import Path

RELEASE='https://github.com/nesdev-org/MesenCE/releases/download/2.2.1/'
BUNDLES={
 'linux':('Mesen_2.2.1_Linux_x64.zip','c88ff4d251b407515c43d3332d641927655cd69fb538996b6a21da4509dbb58f'),
 'windows':('Mesen_2.2.1_Windows.zip','c19e10b3b3865eb0e86e4d01bf09a5027a4452081c04b5bd5b6cbbf76e346fd6'),
}
ROOT=Path(__file__).resolve().parents[1]

def install(platform=None,download=None):
    target=('windows' if sys.platform=='win32' else 'linux') if platform is None else platform
    if target not in BUNDLES:raise RuntimeError('Mesen supported target is linux/windows x64')
    name,digest=BUNDLES[target]
    url=RELEASE+name
    data=urllib.request.urlopen(url, timeout=180).read() if download is None else download(url)
    actual=hashlib.sha256(data).hexdigest()
    if actual!=digest:raise RuntimeError('Upstream Mesen release SHA-256 mismatch '+actual)
    folder=ROOT/'third_party'/'Mesen'/target
    folder.mkdir(parents=True,exist_ok=True)
    with zipfile.ZipFile(io.BytesIO(data)) as z:
        for i in z.infolist():
            if i.is_dir():continue
            dest=folder / i.filename
            if not dest.resolve().is_relative_to(folder.resolve()):raise RuntimeError('Unsafe package entry')
            dest.parent.mkdir(parents=True,exist_ok=True)
            with z.open(i) as src,dest.open('wb') as dst:shutil.copyfileobj(src,dst)
            if target=='linux' and dest.name in ('Mesen','Mesen2'):dest.chmod(0o755)
    names=('Mesen.exe','Mesen2.exe') if target=='windows' else ('Mesen','Mesen2')
    if not any(p.is_file() for n in names for p in folder.rglob(n)):
        raise RuntimeError('Mesen archive lacked expected executable')
    # Preserve the GPL text and source location required by upstream licensing.
    license_path=folder/'GPL-3.0.txt'
    local=Path('/usr/share/common-licenses/GPL-3')
    if not license_path.exists() and local.is_file():shutil.copy2(local,license_path)
    (folder/'SOURCE_AND_LICENSE.txt').write_text(
        'MesenCE 2.2.1 (GPL-3.0). Copyright the MesenCE contributors.\n'
        'Upstream source: https://github.com/nesdev-org/MesenCE\n'
        'Official release: '+url+'\nNo copyrighted NES game ROM is included.\n',encoding='utf-8')
    print(f'Bundled {target} MesenCE: {url} sha256 {actual}')
    return folder

if __name__=='__main__':install()
