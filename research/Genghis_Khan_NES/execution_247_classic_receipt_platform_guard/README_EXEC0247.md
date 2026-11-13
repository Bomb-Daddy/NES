# GK1 EXEC0247 — Owner-private Classic receipt security / Windows fixture portability

This is a real delta to the inherited **complete EXEC0246 v1.01 C11 native game**, not a replacement prototype. Apply [the exact integration patch](./GK1_EXEC0247_Integration.patch) to the complete prior `source/` tree. Exact predecessor/postpatch hashes are in [the manifest](./GK1_EXEC0247_Changed_Source_SHA256.json). This directory contains the newly linked source and its C11 test; the patch includes the changes to the original CMake, Classic GC18 loader and prior Tax fixture.

## Actually executed November 13, 2026

- Linux native Release build: **PASS; 210/210 CTests PASS**, including four focused Classic receipt, cold-load and filesystem-guard tests.
- On POSIX, private receipt loads reject foreign ownership, group/world-readable mode, multiple hard links, wrong length and nonregular files. Win32 implementation checks opened HANDLE type, reparse attributes, link count and exact size, opening with write sharing denied.
- Optional owner-private Win32 Tax receipt fixture now creates a valid Windows temp path, instead of unconditional failure. The original three private-stock evidence profiles remain confidential.
- **NOT RUN:** EXEC0247 actual native Win32/MSVC/NSIS, physical Xbox controller, full campaigns and new original Mesen simulation. This does not complete first-Spring generation or original command parity.

## Full source and authority

[Complete integrated EXEC0247 C11 source, evidence and QA archive](https://drive.google.com/file/d/1fkc_J10SvtdpOeeCeBOSdoCx_3gPiZ0u/view); [implementer/QA handoff](https://drive.google.com/file/d/1b06JeQQiw6HVcKl7eL_1ODMNf6t_jcXU/view); [cumulative master v4.01](https://drive.google.com/file/d/1ZtylAapa5cElb3PO_SpjXwdpkoePdkez/view).

Reproduce from repository/archive root with: `patch -p1 < GK1_EXEC0247_Integration.patch`; `cmake -S source/execution_199 -B build -DCMAKE_BUILD_TYPE=Release`; `cmake --build build --parallel 8`; `ctest --test-dir build --parallel 8 --output-on-failure`.

No original ROM, RAM captures, save states, screenshots or licensed game assets are included.
