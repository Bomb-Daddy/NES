# EXEC0091 — Original stock MesenCE AllZeros power-on source trace (0.91.0-dev1)

Source-provenance correction to authenticated original USA NES CPU reset research. The **archived fresh stock Mesen launcher** in the private EXEC0076 evidence sets `Nes.RamPowerOnState=1`. MesenCE 2.2.1 public `Core/Shared/SettingTypes.h` explicitly defines `Random=0, AllZeros=1, AllOnes=2`; `Core/Shared/EmuSettings.cpp` implements `AllZeros` with `memset(data,0,length)`, called from `NesMemoryManager::Reset(false)`. Therefore **the archived fresh Mesen experiment's initial 2KiB CPU RAM is zero** under the specified setting. This does not establish arbitrary NES hardware power-on RAM or the archived-restored `.mss` state.

C11 `gk91_trace_reset_mesen_allzeros` executes the exact independently hash-verified original ROM's existing bounded 6502/MMC1 trace with all 2KiB of CPU RAM *known zero* under that exact recorded emulator setup. Old `gk83_trace_original_reset` remains intentionally unknown at first `$00F5` read, and `gk84_trace_reset_single_ram_hypothesis` stays explicitly counterfactual. Distinct API prevents one particular Mesen setting being turned into a universal original first Spring constant. Under the exact verified original ROM and this all-zero power-on mode, the bounded trace executes **142 CPU instructions** and stops before the first unmodeled PPUSTATUS read: `LDA $2002` at PC `$C5BE`. This is source-derived interpretation, **not** a new Mesen runtime replay, complete CPU/PPU emulator, or first Spring parity proof. Source and tests contain no ROM or proprietary RAM bytes.

Reproduce: `GK1_ORIGINAL_STOCK_ROM=/private/stock.nes ctest --test-dir build --output-on-failure -R GK1_091`; without private ROM the deterministic rejection tests run. Prior Mesen Spring captures remain prior observations, not newly run emulation. Original Spring generation and all 12 locked strategic commands remain open.

Public MesenCE evidence:
- https://github.com/nesdev-org/MesenCE/blob/master/Core/Shared/SettingTypes.h (`RamState` enum)
- https://github.com/nesdev-org/MesenCE/blob/master/Core/Shared/EmuSettings.cpp (`InitializeRam`)
- https://github.com/nesdev-org/MesenCE/blob/master/Core/NES/NesMemoryManager.cpp (`Reset(false)`).
