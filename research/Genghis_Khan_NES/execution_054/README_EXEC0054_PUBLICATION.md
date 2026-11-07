# Genghis Khan I — EXEC0054 native research package

**Status:** Linux research prototype, not a completed NES-faithful Windows game.

This is an additive continuation of the same native C11 game in EXEC0053. The copyright-clean **complete integrated 303-file source archive** (including prior source history), and the amd64 Debian research installer, are available in the canonical **Retro Gaming / Collaboration** Drive folder. This GitHub entry is a provenance pointer only, **not the full EXEC0054 source tree**. Do not try to build from this README alone.

| Deliverable | Drive file ID | Bytes | SHA-256 |
|---|---|---:|---|
| GK1_EXEC_0054_Native_Research_Installer_Source_v1.00.zip | `1l1fLPz3_GVEPzRJX8XoetBe511sBDzIb` | 571684 | `4abd043b0c6a9a1dabc738f7fdf697a967a724e7c3b9ce71af85f1600d0f08a6` |
| gk1-native-research_0.54.0~dev1_amd64.deb | `1ryoTW17CNU9_6dkYY7Dej9zx3uwD4pSp` | 58254 | `47bd09a5d0c9e05a7e7223d5aaf859c45d9ec6724985b839998ceb926882ecce` |

Sources and package were uploaded and independently downloaded from the Collaboration folder, matching byte-for-byte. CPack Debian initially installed under `/usr/local/opt`, which failed the launcher-root contract; it was fixed to `/opt/gk1-native-research`, and the final package was extracted and tested.

Native Linux source Release CTest **14/14**, independent source clean rebuild CTest **14/14**. Extracted final Debian payload validated: the versioned desktop launcher, executable, original 41-country CSV, source-identity check, XDG save directory with restrictive file modes, cross-process manual draft cold load, invalid/overwrite rejection, and 1280x720 Xvfb rendering. No native Windows build, real Xbox acceptance, original NES gameplay trace, complete playable Spring or full campaign verified in EXEC0054.

**Existing profiles remain distinct:** Classic NES stock and approved Enhanced combined-modified evidence are not interchangeable; source templates remain PRE_SETUP, not first Spring. User campaigns, ROMs and older research evidence must never be overwritten.

- [Full master v2.36](https://docs.google.com/document/d/1OzjpnKcM06TVTyNF7QYY5Lq18q6Cnykl0UZuTtaK10c/edit)
- [EXEC0054 implementer handoff](https://docs.google.com/document/d/1oJDyVpbbirHAeDyHgxf3vbyzRJdtWjStvZngcJKYZU8/edit)
- [EXEC0054 completion ledger](https://docs.google.com/document/d/1KtuKgcnBF-aGOWLt2Q37P5pwaln9gipnyRR3DtyCFSw/edit)
