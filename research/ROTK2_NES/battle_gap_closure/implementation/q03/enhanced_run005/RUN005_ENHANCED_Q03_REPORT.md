# ROTK II Enhanced Run005 Q03 — stationed, nondeployed defenders consume rice
Date: 2026-11-06. Source project: separate Original-KOEI Enhanced game. **NOT** NES Classic canonical source.

## Exact code ancestry
Parent: `ROTK2_Enhanced46_BPF_Run004_Q14_Remote_Refuge_Source_v1.03.zip`, SHA256 `7b2891d3c7d7b6ef371b2e06926fce59bda1b0f480160ccb8fc32c9e286ec229`, 517 entries; verified against 18 original Drive part manifest before patch.
Child: `ROTK2_Enhanced46_BPF_Run005_Q03_ResidentDefenders_FullSource_v1.00.zip`, SHA256 `fc747de9b10a7e14683acdde416197eb413b64c5abd3b044fb2fb130cf490fce`, size 148704302, 518 CRC-clean members; 515 parent members unchanged, 2 engine modules changed, 1 regression file added. Rebuilt twice to identical child SHA. No DOS source touched; no changes applied to NES canonical Run014.

## Behavioral correction
The original NES/SNES Q03 defender food tick includes *stationed defending officers not represented on the tactical field* as well as the on-field and reserve list. Enhanced Run004 previously counted only deployed and reserve defenders. Run005 uses actual owned/stationed army groups at the contested province, excludes already fielded and queued officers, excludes remote/off-faction officers, and preserves numeric `/30` food accounting. Conservation validation now compares the actual province garrison with deployed defender strength PLUS the real undeployed resident army strengths. No synthetic troops are created.
Reference original observed P19-to-P20 example: 19,000 fielded plus 1,000 resident: 666 hundredths/day; zero resident: 633. Enhanced controlled reference at starting rice 45,000 changes 44,367 to 44,334 (inclusive of 1,000 resident). Exact arithmetic independently matched Classic port for six normalized examples.

## Executed checks
- Untouched parent Run004 against new tests: **RED 3 failed, 1 passed**. Raw output `RED_OriginalEnhancedRun004.log`; test command code 1.
- New candidate focused Q03: 4 passed. `GREEN_Q03.log`.
- Nine explicit combat suites: 36 passed, 1 skipped. `GREEN_BATTLE_SCOPED.log`.
- Selected Build/Run004/Q03 broad suite in Xvfb: **196 passed, 2 skipped**. `GREEN_BROAD_XVFB.log`.
- New full ZIP, fresh cold extraction and same broad suite: **196 passed, 2 skipped**. `COLD_GREEN.log`.
- Actual Linux Tk software-controller Xvfb: 1 passed. `GREEN_XVFB.log`. Not a real Xbox controller.
- Actual `save_bytes/load_bytes` campaign cold-load fingerprint and battle ledger validation pass in focused tests.
- Negative guards: remote same-faction troops excluded, deployed officer not double charged, conservation requires stationed totals.
- Unfiltered collection remains **BLOCKED**: `ROTK2_TEST_ROM` unset in `test_build24_q04_original_rom_deployment.py`. Broad suite deliberately omits that ROM-gated module. Raw first attempts also show headless Tk display :0 failure, which passes under Xvfb; those failures are preserved as trace.

## Status and limits
- Original Q03 19000+1000 arithmetic: **OBSERVED_RUNTIME historical / REPORTED_UNREPRODUCED fresh**, and approved original SNES source **VERIFIED_STATIC via prior matrix**, not newly rerun in Mesen.
- Actual source candidate and tests: **TESTED_PORT**, scoped to Enhanced Q03 resident defenders and ledger.
- Full Classic-vs-Enhanced state-controlled gameplay: **OPEN**, only six arithmetic source fixtures compared; four-slot NES list behavior in Enhanced remains approximate.
- Native Windows / real Xbox controller / full NES Q01–Q22 battle parity / Director acceptance: **NOT RUN / OPEN**.
- Full 41-province gameplay and cross-system release not established.

## Next executable integration
Keep canonical NES Run014 exact SHA `ddb6ee5693e6dce140f4e216d14d241e8c1239aa8ddfb66fd1b7257d46ade398`. For its separate next Q09 patch derive `hsovl2 $D6AC–$D6C8` failed-Flee capture-list slot and live unlink, then implement fail-before/green-after with source fixtures and actual saved battles. For Enhanced, develop normalized Classic/Enhanced controlled-RNG Q03 battle fixture with exactly the same deployed+resident records; only then claim cross-mode parity. Verify publication before promoting the Enhanced candidate.
