# GK1 EXEC0147 IMPLEMENTER / HOSTILE QA HANDOFF

- Predecessor: exact complete EXEC0146, `1.46.0-dev1`, unchanged historical project files.
- New version: `1.47.0-dev1`; same C11 native game, X11/Linux and Win32-target source.
- Files: `gk147_stock_goods.c/.h` and `test_gk147_stock_goods.c`, plus guarded `gk1_app_state.c/.h`, `gk1_native_shell.c`, `CMakeLists.txt`, desktop/package version contract, this handoff and README.
- Critical positive authority: original stock ROM SHA256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`; private Mesen evidence ZIP SHA256 `8b757e11b12aeb3efe93e36c10628599ca5f12e91b15c02354cd3ebbd11bf52a`. No ROM or original bytes in public source.
- Exact country record goods start `+0x22`, Food `+0x36`. Do not confuse goods+0x24 with first good; this is an authenticated correction.
- Hostile QA tests: fake source, absent attestation, invalid country/count/profile, source byte corruption, Enhanced mutable-world tamper, Classic/Enhanced mismatch, every good against raw original source across 14+14+27 countries, controller cycle, no orders or source mutations.
- Important limitations: the capability is **a source-authenticated original read-only goods page**, not Trade, merchant presence, source RNG, player Tax/Assign, autonomous AI, source-independent first Spring or completed campaign.
- Launch: `GK1_STOCK_SPRING_PROFILE=fresh GK1_STOCK_SPRING_RECORDS=<owner private valid raw 1162B> xvfb-run -a build/gk1_native_shell --smoke-stock-goods 30`; archived `mongol` 1162B or `world` 2241B likewise. Source authenticating exact SHA is required; no fallback.
- Repro: see README_EXEC0147; tests must distinguish `VERIFIED_STATIC`, prior `OBSERVED_RUNTIME_STOCK`, `TESTED_PORT_LINUX`, and `NOT RUN`.
