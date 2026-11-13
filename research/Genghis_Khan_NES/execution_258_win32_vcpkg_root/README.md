# GK1 EXEC0258 — actual Win32 CI failure reproduction and vcpkg host fix (2026-11-13)

Predecessor: published EXEC0257 (commit f0ab6b998720631eecd3b32a88b46f37e2b05628).
Windows-2022 Actions run https://github.com/Bomb-Daddy/NES/actions/runs/43492931587 passed checkout, SHA-pinned full source expansion, patch application and NSIS prerequisite, then failed before compilation: runner exports VCPKG_INSTALLATION_ROOT=C:\vcpkg, but the workflow assumed VCPKG_ROOT was set.

This EXEC0258 commit changes ONLY the actual root GitHub Actions Windows workflow, preserving the exact 793-file source ZIP, SHA256 97421aa2ef45288684bf22d868ad110b850396c316bce9253b0fc8a05166b619, and 0257 source patch. It resolves vcpkg via VCPKG_ROOT, VCPKG_INSTALLATION_ROOT or C:\vcpkg; checks both vcpkg.exe and scripts/buildsystems/vcpkg.cmake; installs static zlib; and exports VCPKG_ROOT through GITHUB_ENV for the subsequent full C11 MSVC build.

Static regression: python research/Genghis_Khan_NES/execution_258_win32_vcpkg_root/test_gk258_windows_vcpkg_root.py .github/workflows/gk1-native-windows.yml . The test includes six hostile mutations. Native Win32/NSIS must be classified TESTED_PORT_WIN32 only after a successful real Actions run, not from this commit alone. Physical Xbox and full campaigns remain NOT RUN. Original ROM and licensed captures remain private. First authentic NES Spring RNG, AI, Trade and other gameplay completion remain OPEN.
