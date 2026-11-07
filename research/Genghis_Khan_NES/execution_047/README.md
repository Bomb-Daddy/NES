# Genghis Khan I — EXEC0047: Native Divide completion and source-bounded C11 tests

**Research and code, November 6 2026.** The original approved combined Shared BP/Rest modified NES ROM was executed under matched Mesen from a hash-pinned first-battle checkpoint. This adds real controller-driven Divide quantity/target/commit behavior beyond EXEC0045's quantity prompt; does **not** complete the Genghis Windows game.

## Captured gameplay and exact profiles

Initial unit0 soldiers40 at (7,7), type0, controlled Mobility 20 (the only initial RAM intervention), source seed c7c3f28f2363. Real controller D-pad/A/B selected q=1,6,7,8 troops, moved one forest tile to (8,7). Source remaining39,34,33,32; new slot3 survivors0,0,1,2. q1/q6 displayed unit3 death message, first B dismisses, second B finishes; q7/q8 one B finishes. Source Mobility refills20→10, completion word0→1. Slot3 zero records retain coordinates/type/Mobility; 18 other units unaltered. Negative: B before legal target produced "Can't move there"; second B returned to quantity prompt without troop/RNG/word changes. Hazard seed advanced a31e4ecd8fd1. Exploratory ten-Up numeric attempt was NOT a successful q10 split.

Reused already source-verified forest attrition: new group q first floor(q*98/100), then minus min(remainder,5) at the fixed captured RNG. Source loses exactly requested q, not q-survivors. These are four **bounded controlled OBSERVED_RUNTIME_MOD** cases and do not establish universal Divide rules.

## Executable source, QA and private separation

[**Copyright-clean full source ZIP — 18 entries**](GK1_EXEC_0047_Divide_NES_Source_v1.00.zip), SHA-256 `c6d0927854e52dcf47f18932020adabf84ee75a81b1d84504eca005c02f804a8`, 12,574 bytes. Contains C11 state machine, test, CMake, original-capture audit, numeric manifest, README, and complete QA output. **128 C11 checks passed** for each of Debug, Release, and AddressSanitizer+UBSan; separately extracted clean source Release passed; **58 native-capture checks** passed. Not yet integrated into EXEC0046 1280x720 game shell or built/tested on Windows or physical Xbox.

Original ROM, emulator, checkpoint, SRAM and 175 native screenshot/trace files are NOT published in GitHub; private authorized [Drive native evidence](https://drive.google.com/file/d/1_l36f8pm8IC-wt6fwe_-GpNCqQxuTT6e/view). Matching source [Drive copy](https://drive.google.com/file/d/1emN_6UTR6Hx17S3gDfGAE-rgXXooKP7_/view).

[Numbered implementer handoff (Director/Nobunaga/Cao Cao)](https://drive.google.com/file/d/1lJWJ_iCFs5q73_H2jhvtwVq4WA_cLq38/view) · [Cumulative master guide v2.29](https://docs.google.com/document/d/1BEUs9WYkLhI9S4kUsuMle5lBnxOTFSKjdmPWgSYLgoA/edit) · [Completion ledger](https://docs.google.com/document/d/19j65dIsTkmKcani5bbkuQLkMcjyIzrSePFv8xxcTMsQ/edit).

Next: implement Divide into the **same existing** Windows/Linux C11 game source and controller menu, not another standalone game; finish general slot allocation, battle victory/defeat and AI. The project's full two campaigns, authentic audio/graphics, save/cold-load, real Windows package and physical Xbox test remain open.
