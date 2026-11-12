cmake_minimum_required(VERSION 3.15)
project(GK1NativeShell199 C)
find_package(ZLIB QUIET)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()
add_library(gk1_app_state STATIC gk61_market.c gk64_manual_rules.c gk65_battle_terminal.c gk68_observed_rest.c gk74_spring_archive.c gk75_spring_economy.c gk110_spring_stage.c gk111_stock_spring_audit.c gk112_stock_setup_deltas.c gk212_stock_raw_variation.c gk213_stock_spring_boundary.c gk214_spring_batch.c gk222_enhanced_spring_lab.c gk224_practice_save.c gk225_practice_resume.c gk226_practice_checkpoint.c gk217_finance_preview.c gk219_finance_compare.c gk220_labor_history.c gk215_original_spring_gap.c gk216_fresh_cpu_pc.c gk218_stock_write_trace.c gk114_stock_spring_timeline.c gk115_market_gate.c gk116_enhanced_assign.c gk158_enhanced_summer.c gk159_enhanced_fall.c gk160_enhanced_winter.c gk161_enhanced_year2.c gk176_enhanced_cycle.c gk177_enhanced_receipt.c gk178_source_rng.c gk180_seed_writer_scan.c gk181_bank_seed_trace.c gk182_archived_source_bank.c gk183_checkpoint_isolation.c gk185_stock_hotkey.c gk187_classic_assign.c gk188_classic_receipt.c gk189_classic_cold_save.c gk162_original_entry.c gk164_mss_entry.c gk165_fresh_json.c gk166_fresh_entry.c gk193_auto_source_entry.c gk194_classic_storage_gate.c gk195_classic_view.c gk196_classic_trade_status.c gk198_tax_forecast.c gk199_classic_tax_undo.c gk202_irq_rng_dispatch.c gk203_rng_selector_writers.c gk204_selector_store_inventory.c gk205_selector_runtime_watch.c gk206_rng_source_parity.c gk207_seed_write_watch.c gk208_archived_rng.c gk209_archived_rate_ambiguity.c gk211_fresh_sequence.c gk167_private_cold_save.c gk170_slot_preview.c gk171_enhanced_title_resume.c gk172_rng_source_xref.c gk173_rng_control.c gk174_setup_boundary.c gk119_order_bridge.c gk117_stock_pristine_gate.c gk77_stock_spring.c gk78_spring_world.c gk79_stock_checkpoint.c gk80_full_checkpoint.c gk81_stock_attach.c gk82_stock_cartridge.c gk83_reset_trace.c gk105_openbus.c gk106_ppu_bus.c gk107_ppu_ports.c gk108_cpu_rmw_bus.c gk109_rmw_families.c gk121_bit_flags.c gk122_cpu_logic.c gk123_cpu_loadstore.c gk124_cpu_compare.c gk125_cpu_arith.c gk126_io_barrier.c gk127_io_transaction.c gk128_oam.c gk129_dma_source.c gk130_dma_bus.c gk131_boot_gate.c gk132_f5_setup.c gk133_boot_order.c gk134_stock_command_route.c gk137_stock_rng.c gk138_rng_reverse.c gk139_mesen_state.c gk140_rng_lineage.c gk141_dispatch_scan.c gk142_fixed_cfg.c gk143_bank_cfg.c gk144_archived_mapper.c gk145_fresh_progress.c gk146_assign_quantity.c gk147_stock_goods.c gk148_fresh_bank_trace.c gk149_ppu_phase_scan.c gk150_render_window.c gk151_even_source.c gk152_spring_writer_map.c gk153_country_write_watch.c gk94_prgram.c gk98_controller_port.c gk98_cpu_rotate.c gk100_cpu_cycles.c gk101_mmc1_cycles.c gk103_ntsc_ppu.c gk95_stock_trade_reference.c gk96_ppu_warmup.c gk91_reset_zero.c gk92_ppustatus_hypothesis.c gk89_spring_reconcile.c gk90_verified_finance.c gk71_order_budget.c gk72_battle_pass.c ../execution_049/gk1_scenario_factory.c ../execution_045/gk1_observed_turn.c ../execution_047/gk1_divide_contract.c gk1_app_state.c gk1_source_identity.c ../execution_039/gk1_battle_session.c ../execution_034/gk1_tactical_slice.c)
target_include_directories(gk1_app_state PUBLIC . ../execution_039 ../execution_034 ../execution_045 ../execution_047 ../execution_049)
add_library(gk1_pre_setup_store STATIC gk1_pre_setup_store.c)
add_library(gk1_setup_draft_store STATIC gk1_setup_draft_store.c)
add_library(gk1_manual_setup_store STATIC gk1_manual_setup_store.c)
target_link_libraries(gk1_manual_setup_store PUBLIC gk1_setup_draft_store)
add_library(gk1_manual_bonus_store STATIC gk1_manual_bonus_store.c)
target_link_libraries(gk1_manual_bonus_store PUBLIC gk1_manual_setup_store)
target_link_libraries(gk1_setup_draft_store PUBLIC gk1_pre_setup_store)
target_link_libraries(gk1_pre_setup_store PUBLIC gk1_replay_store)
add_library(gk1_replay_store STATIC gk1_replay_store.c)
add_library(gk1_path_store STATIC gk1_path_store.c)
add_library(gk1_divide_store STATIC gk1_divide_store.c)
target_link_libraries(gk1_divide_store PUBLIC gk1_replay_store)
target_link_libraries(gk1_path_store PUBLIC gk1_replay_store)
target_link_libraries(gk1_replay_store PUBLIC gk1_app_state)
add_library(gk69_mesen_ram_map STATIC gk69_mesen_ram_map.c)
target_link_libraries(gk69_mesen_ram_map PUBLIC gk1_app_state)
add_library(gk1_snapshot_view STATIC gk1_snapshot_view.c ../execution_040/gk1_battle_snapshot.c ../execution_035/gk1_native_ram_view.c)
target_include_directories(gk1_snapshot_view PUBLIC . ../execution_040 ../execution_035)
add_library(gk1_pad_router STATIC gk1_pad_router.c)
add_library(gk58_slots STATIC gk58_slots.c)
add_library(gk59_storage STATIC gk59_storage.c)
add_library(gk120_save_home STATIC gk120_save_home.c)
add_library(gk60_demobilize STATIC gk60_demobilize.c)
target_link_libraries(gk1_app_state PUBLIC gk60_demobilize)
add_executable(test_gk59_storage test_gk59_storage.c)
target_link_libraries(test_gk59_storage PRIVATE gk59_storage)
add_test(NAME GK1_059_WinUserDataPathNoCWDAndNoMutation COMMAND test_gk59_storage)
target_include_directories(gk1_pad_router PUBLIC .)
add_executable(test_gk1_pad_router test_gk1_pad_router.c)
target_link_libraries(test_gk1_pad_router PRIVATE gk1_pad_router)
add_executable(test_gk1_app_state test_gk1_app_state.c)
target_link_libraries(test_gk1_app_state PRIVATE gk1_app_state)
if(WIN32)
  add_executable(gk1_native_shell WIN32 gk1_native_shell.c)
  target_link_libraries(gk1_native_shell PRIVATE gk1_path_store gk1_divide_store gk1_manual_bonus_store gk1_manual_setup_store gk1_setup_draft_store gk1_pre_setup_store gk1_replay_store gk1_pad_router gk1_snapshot_view gk69_mesen_ram_map gk58_slots gk59_storage gk120_save_home gk60_demobilize user32 gdi32)
else()
  find_package(X11 REQUIRED)
  add_executable(gk1_native_shell gk1_native_shell.c)
  target_link_libraries(gk1_native_shell PRIVATE gk1_path_store gk1_divide_store gk1_manual_bonus_store gk1_manual_setup_store gk1_setup_draft_store gk1_pre_setup_store gk1_replay_store gk1_pad_router gk1_snapshot_view gk69_mesen_ram_map gk58_slots gk59_storage gk120_save_home gk60_demobilize X11::X11)
endif()
# Windows installer for the SAME Win32/XInput native executable, not an HTML shell.
# File-based static source packaging specification: full Windows compile,
# NSIS generator, installer execution and physical Xbox remain UNTESTED here.
if(WIN32)
  install(TARGETS gk1_native_shell RUNTIME DESTINATION .)
  install(FILES ../execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv DESTINATION .)
  install(FILES README_EXEC0059.md README_EXEC0060.md README_EXEC0062.md README_EXEC0065.md README_EXEC0066.md README_EXEC0067.md README_EXEC0068.md README_EXEC0069.md README_EXEC0071.md README_EXEC0072.md README_EXEC0073.md README_EXEC0074.md README_EXEC0075.md README_EXEC0077.md README_EXEC0078.md README_EXEC0079.md README_EXEC0080.md README_EXEC0081.md README_EXEC0083.md README_EXEC0084.md README_EXEC0085.md README_EXEC0086.md README_EXEC0087.md README_EXEC0088.md README_EXEC0089.md README_EXEC0090.md README_EXEC0091.md README_EXEC0092.md README_EXEC0093.md README_EXEC0094.md README_EXEC0095.md README_EXEC0097.md README_EXEC0098.md README_EXEC0099.md README_EXEC0100.md README_EXEC0101.md README_EXEC0102.md README_EXEC0103.md README_EXEC0104.md README_EXEC0105.md README_EXEC0106.md README_EXEC0107.md README_EXEC0108.md README_EXEC0109.md README_EXEC0110.md README_EXEC0111.md README_EXEC0112.md README_EXEC0114.md README_EXEC0115.md HANDOFF_EXEC0117.md HANDOFF_EXEC0118.md HANDOFF_EXEC0119.md HANDOFF_EXEC0120.md README_EXEC0120.md README_EXEC0121.md README_EXEC0123.md README_EXEC0125.md README_EXEC0126.md README_EXEC0127.md README_EXEC0128.md README_EXEC0129.md README_EXEC0132.md README_EXEC0133.md README_EXEC0135.md README_EXEC0136.md README_EXEC0138.md README_EXEC0139.md README_EXEC0140.md README_EXEC0142.md README_EXEC0143.md README_EXEC0144.md README_EXEC0145.md README_EXEC0147.md README_EXEC0148.md README_EXEC0149.md README_EXEC0151.md README_EXEC0152.md README_EXEC0153.md README_EXEC0154.md README_EXEC0155.md README_EXEC0156.md README_EXEC0157.md README_EXEC0158.md README_EXEC0159.md README_EXEC0160.md README_EXEC0161.md README_EXEC0162.md README_EXEC0163.md README_EXEC0164.md README_EXEC0165.md README_EXEC0166.md README_EXEC0167.md README_EXEC0168.md README_EXEC0170.md README_EXEC0171.md README_EXEC0172.md README_EXEC0173.md README_EXEC0174.md README_EXEC0175.md README_EXEC0176.md README_EXEC0177.md README_EXEC0178.md README_EXEC0179.md README_EXEC0180.md README_EXEC0181.md README_EXEC0182.md README_EXEC0183.md README_EXEC0184.md README_EXEC0185.md README_EXEC0186.md README_EXEC0187.md README_EXEC0188.md README_EXEC0189.md README_EXEC0190.md README_EXEC0191.md README_EXEC0192.md README_EXEC0193.md README_EXEC0194.md README_EXEC0195.md README_EXEC0196.md README_EXEC0197.md README_EXEC0198.md README_EXEC0199.md README_EXEC0209.md README_EXEC0210.md README_EXEC0211.md README_EXEC0212.md README_EXEC0213.md README_EXEC0224.md DESTINATION .)
  set(CPACK_GENERATOR "NSIS")
  set(CPACK_PACKAGE_NAME "GK1-Native-Research")
  set(CPACK_PACKAGE_VERSION "1.99.0")
  set(CPACK_PACKAGE_VENDOR "GK1 native research prototype")
  set(CPACK_PACKAGE_INSTALL_DIRECTORY "Genghis Khan I Native Research")
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Incomplete native Genghis Khan I research; original first Spring still unfinished")
  set(CPACK_NSIS_DISPLAY_NAME "Genghis Khan I Research 1.99.0-dev1")
  set(CPACK_NSIS_PACKAGE_NAME "Genghis Khan I Research 1.99.0-dev1")
  set(CPACK_NSIS_MUI_FINISHPAGE_RUN "gk1_native_shell.exe")
  set(CPACK_NSIS_MODIFY_PATH OFF)
  set(CPACK_PACKAGE_EXECUTABLES "gk1_native_shell" "Genghis Khan I Research 1.99.0-dev1")
  set(CPACK_CREATE_DESKTOP_LINKS "gk1_native_shell")
  include(CPack)
endif()
add_executable(test_gk100_cpu_timing test_gk100_cpu_timing.c)
target_link_libraries(test_gk100_cpu_timing PRIVATE gk1_app_state)
add_test(NAME GK1_100_AuthenticatedOriginalCpuCycleAndPPUCTRLSourceAudit COMMAND test_gk100_cpu_timing)
add_executable(test_gk104_ppu_origin test_gk104_ppu_origin.c)
target_link_libraries(test_gk104_ppu_origin PRIVATE gk1_app_state)
add_test(NAME GK1_104_NTSC_FullFrame_ResetOriginHypotheses COMMAND test_gk104_ppu_origin)
add_executable(test_gk103_ntsc_ppu test_gk103_ntsc_ppu.c)
target_link_libraries(test_gk103_ntsc_ppu PRIVATE gk1_app_state)
add_test(NAME GK1_103_NTSC_UnrenderedPPU_Phase_Scheduled_SourceGate COMMAND test_gk103_ntsc_ppu)
add_executable(test_gk102_controller_frames test_gk102_controller_frames.c)
target_link_libraries(test_gk102_controller_frames PRIVATE gk1_app_state)
add_test(NAME GK1_102_StockROM_ScriptedNMIControllerSourceTrace COMMAND test_gk102_controller_frames)
add_executable(test_gk101_mmc1_cycles test_gk101_mmc1_cycles.c)
target_link_libraries(test_gk101_mmc1_cycles PRIVATE gk1_app_state)
add_test(NAME GK1_101_NESMMC1_RMWConsecutiveCycleResetAndROMTrace COMMAND test_gk101_mmc1_cycles)
add_library(gk99_xinput_bridge STATIC gk99_xinput_bridge.c)
target_include_directories(gk99_xinput_bridge PUBLIC .)
target_link_libraries(gk1_native_shell PRIVATE gk99_xinput_bridge)
add_executable(test_gk99_xinput_bridge test_gk99_xinput_bridge.c)
target_link_libraries(test_gk99_xinput_bridge PRIVATE gk99_xinput_bridge gk1_pad_router)
add_test(NAME GK1_099_Win32ControllerMappingAndHotplug_HostileLinuxQA COMMAND test_gk99_xinput_bridge)
add_executable(test_gk1_replay_store test_gk1_replay_store.c)
target_link_libraries(test_gk1_replay_store PRIVATE gk1_replay_store)
enable_testing()
add_test(NAME GK1_044_C11_ControllerState COMMAND test_gk1_app_state)

add_test(NAME GK1_044_Exclusive_Research_Replay COMMAND test_gk1_replay_store)

add_test(NAME GK1_044_ControllerEdgeRepeat COMMAND test_gk1_pad_router)
add_executable(test_gk1_controller_pipeline test_gk1_controller_pipeline.c)
target_link_libraries(test_gk1_controller_pipeline PRIVATE gk1_pad_router gk1_app_state)
add_test(NAME GK1_044_ControllerToNativeApp COMMAND test_gk1_controller_pipeline)

add_executable(test_gk1_snapshot_view test_gk1_snapshot_view.c)
target_link_libraries(test_gk1_snapshot_view PRIVATE gk1_snapshot_view)
add_test(NAME GK1_044_IdentityGatedBattleViewer COMMAND test_gk1_snapshot_view)

add_executable(test_gk1_path_integration test_gk1_path_integration.c)
target_link_libraries(test_gk1_path_integration PRIVATE gk1_path_store gk1_pad_router)
add_test(NAME GK1_046_ModifiedROMPathControllerAndSave COMMAND test_gk1_path_integration)

add_executable(test_gk1_divide_in_app test_gk1_divide_in_app.c)
target_link_libraries(test_gk1_divide_in_app PRIVATE gk1_divide_store gk1_pad_router)
add_test(NAME GK1_048_ActualNativeAppDivideAndSave COMMAND test_gk1_divide_in_app)

# Stage the exact research CSV adjacent to the native executable in build output.
# The launched program resolves it next to the binary, never assumes a CWD.
configure_file(../execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv
  ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv COPYONLY)
add_executable(test_gk1_scenario_factory ../execution_049/test_gk1_scenario_factory.c)
target_link_libraries(test_gk1_scenario_factory PRIVATE gk1_app_state)
add_test(NAME GK1_049_OriginalCountryFactory COMMAND test_gk1_scenario_factory ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)
add_executable(test_gk1_scenario_integration test_gk1_scenario_integration.c)
target_link_libraries(test_gk1_scenario_integration PRIVATE gk1_app_state gk1_pad_router)
add_test(NAME GK1_050_NativeTwoScenarioControllerIntegration COMMAND test_gk1_scenario_integration ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

add_executable(test_gk1_source_identity test_gk1_source_identity.c)
target_link_libraries(test_gk1_source_identity PRIVATE gk1_app_state)
add_test(NAME GK1_050_VerifiedOriginalCSVHash COMMAND test_gk1_source_identity ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

add_executable(test_gk1_pre_setup_store test_gk1_pre_setup_store.c)
target_link_libraries(test_gk1_pre_setup_store PRIVATE gk1_pre_setup_store gk1_app_state)
add_test(NAME GK1_051_PresetupExclusiveColdLoadAndHostileMutation COMMAND test_gk1_pre_setup_store ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)
add_executable(test_gk1_setup_draft test_gk1_setup_draft.c)
target_link_libraries(test_gk1_setup_draft PRIVATE gk1_setup_draft_store gk1_pad_router)
add_test(NAME GK1_052_ProfileSeparatedCandidateInputAndColdLoad COMMAND test_gk1_setup_draft ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

add_executable(test_gk1_manual_setup test_gk1_manual_setup.c)
target_link_libraries(test_gk1_manual_setup PRIVATE gk1_manual_setup_store gk1_pad_router)
add_test(NAME GK1_053_ManualSetupWizardControllerAndSave COMMAND test_gk1_manual_setup ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0054: install the SAME inherited app and its source-identity-gated CSV.
# The CSV must be beside the executable because scenario_csv_path() resolves
# /proc/self/exe (or GetModuleFileNameA on Windows), never the launch CWD.
if(UNIX AND NOT APPLE)
  install(TARGETS gk1_native_shell RUNTIME DESTINATION opt/gk1-native-research)
  install(FILES ../execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv
          DESTINATION opt/gk1-native-research)
  install(PROGRAMS packaging/gk1-native-research DESTINATION usr/bin)
  install(FILES packaging/gk1-native-research.desktop DESTINATION usr/share/applications)
  install(FILES README_EXEC0059.md README_EXEC0060.md README_EXEC0062.md README_EXEC0065.md README_EXEC0066.md README_EXEC0067.md README_EXEC0068.md README_EXEC0069.md README_EXEC0071.md README_EXEC0072.md README_EXEC0073.md README_EXEC0074.md README_EXEC0075.md README_EXEC0077.md README_EXEC0078.md README_EXEC0079.md README_EXEC0080.md README_EXEC0081.md README_EXEC0083.md README_EXEC0084.md README_EXEC0085.md README_EXEC0086.md README_EXEC0087.md README_EXEC0088.md README_EXEC0089.md README_EXEC0090.md README_EXEC0091.md README_EXEC0092.md README_EXEC0093.md README_EXEC0094.md README_EXEC0095.md README_EXEC0097.md README_EXEC0098.md README_EXEC0099.md README_EXEC0100.md README_EXEC0101.md README_EXEC0102.md README_EXEC0103.md README_EXEC0104.md README_EXEC0105.md README_EXEC0106.md README_EXEC0107.md README_EXEC0108.md README_EXEC0109.md README_EXEC0110.md README_EXEC0111.md README_EXEC0112.md README_EXEC0114.md README_EXEC0115.md HANDOFF_EXEC0117.md HANDOFF_EXEC0118.md HANDOFF_EXEC0119.md HANDOFF_EXEC0120.md README_EXEC0120.md README_EXEC0121.md README_EXEC0123.md README_EXEC0125.md README_EXEC0126.md README_EXEC0127.md README_EXEC0128.md README_EXEC0129.md README_EXEC0132.md README_EXEC0133.md README_EXEC0135.md README_EXEC0136.md README_EXEC0138.md README_EXEC0139.md README_EXEC0140.md README_EXEC0142.md README_EXEC0143.md README_EXEC0144.md README_EXEC0145.md README_EXEC0147.md README_EXEC0148.md README_EXEC0149.md README_EXEC0153.md README_EXEC0154.md README_EXEC0155.md README_EXEC0156.md README_EXEC0157.md README_EXEC0158.md README_EXEC0159.md README_EXEC0160.md README_EXEC0161.md README_EXEC0162.md README_EXEC0163.md README_EXEC0164.md README_EXEC0165.md README_EXEC0166.md README_EXEC0167.md README_EXEC0168.md README_EXEC0170.md README_EXEC0171.md README_EXEC0172.md README_EXEC0173.md README_EXEC0174.md README_EXEC0175.md README_EXEC0176.md README_EXEC0177.md README_EXEC0178.md README_EXEC0179.md README_EXEC0180.md README_EXEC0181.md README_EXEC0182.md README_EXEC0183.md README_EXEC0184.md README_EXEC0185.md README_EXEC0187.md README_EXEC0188.md README_EXEC0189.md README_EXEC0190.md README_EXEC0191.md README_EXEC0192.md README_EXEC0198.md README_EXEC0199.md README_EXEC0209.md README_EXEC0210.md README_EXEC0211.md README_EXEC0222.md README_EXEC0224.md DESTINATION usr/share/doc/gk1-native-research)
  set(CPACK_GENERATOR "DEB")
  set(CPACK_PACKAGE_NAME "gk1-native-research")
  set(CPACK_PACKAGE_VERSION "1.99.0")
  set(CPACK_DEBIAN_PACKAGE_RELEASE "dev1")
  set(CPACK_PACKAGE_CONTACT "GK1 research prototype (offline source distribution)")
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "GK1 Native Research")
  if(ZLIB_FOUND)
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libx11-6, zlib1g")
  else()
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libx11-6")
  endif()
  set(CPACK_DEBIAN_PACKAGE_SECTION "games")
  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Genghis Khan I native research prototype, not completed game")
  set(CPACK_PACKAGE_FILE_NAME "gk1-native-research_1.99.0~dev1_amd64")
  set(CPACK_SET_DESTDIR OFF)
  set(CPACK_PACKAGING_INSTALL_PREFIX "/")
  find_package(Python3 COMPONENTS Interpreter)
  if(Python3_Interpreter_FOUND)
    add_test(NAME GK1_054_InstalledStyleSourceIdentityAndVersion
      COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/packaging/test_package_contract.py
      $<TARGET_FILE:gk1_native_shell>
      ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv
      ${CMAKE_CURRENT_SOURCE_DIR}/packaging/gk1-native-research.desktop)
  endif()
  include(CPack)
endif()

add_executable(test_gk1_manual_bonus test_gk1_manual_bonus.c)
target_link_libraries(test_gk1_manual_bonus PRIVATE gk1_manual_bonus_store gk1_pad_router)
add_test(NAME GK1_055_SixAbilityFiveCharacterBonusDraftAndColdLoad
  COMMAND test_gk1_manual_bonus ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0057 integrated controller View-driven PRE_SETUP workforce planner.
add_executable(test_gk1_labor_plan test_gk1_labor_plan.c)
target_link_libraries(test_gk1_labor_plan PRIVATE gk1_manual_bonus_store gk1_pad_router)
add_test(NAME GK1_057_IntegratedLaborPlanningStateAndSave
  COMMAND test_gk1_labor_plan ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0058: native binary separate-process three-slot saves, hostile rollback.
add_executable(test_gk58_slots test_gk58_slots.c)
target_link_libraries(test_gk58_slots PRIVATE gk58_slots gk1_pad_router)
add_test(NAME GK1_058_SlotRoutingPathAndEdges COMMAND test_gk58_slots)
if(Python3_Interpreter_FOUND)
 add_test(NAME GK1_058_ActualNativeMultiSlotColdLoad
  COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/packaging/test_slot_processes.py
  $<TARGET_FILE:gk1_native_shell>)
endif()

# EXEC0060: integrate source-verified Enhanced mod Demobilize arithmetic into
# SAME native controller-only shell; never assert Classic or first-Spring parity.
add_executable(test_gk60_demobilize test_gk60_demobilize.c)
target_link_libraries(test_gk60_demobilize PRIVATE gk1_app_state gk60_demobilize gk1_manual_bonus_store gk1_pad_router)
add_test(NAME GK1_060_EnhancedDemobilizeControllerSourceSaveRollback
  COMMAND test_gk60_demobilize ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0062: source-authentic original NES market math + live-game phase guard.
# No Trade controller menu and NO synthetic first-Spring gameplay unlock.
add_executable(test_gk61_market test_gk61_market.c)
target_link_libraries(test_gk61_market PRIVATE gk1_app_state)
add_test(NAME GK1_061_OriginalMerchantTransactionMath
         COMMAND test_gk61_market)
add_executable(test_gk62_game_market_guard test_gk62_game_market_guard.c)
target_link_libraries(test_gk62_game_market_guard PRIVATE gk1_app_state)
add_test(NAME GK1_062_NativeAppMarketStateGuardAndAtomicity
         COMMAND test_gk62_game_market_guard
         ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0065: manual-based terminal state and inherited source-only 0064 preview
# integrated into same native controller app, explicitly not a completed battle.
add_executable(test_gk65_battle_terminal test_gk65_battle_terminal.c)
target_link_libraries(test_gk65_battle_terminal PRIVATE gk1_app_state)
add_test(NAME GK1_065_ManualBattleDayTerminalSourceAndController COMMAND
  test_gk65_battle_terminal ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0066: direct controller what-if event/victim interface with profile/phase gate.
add_executable(test_gk66_manual_whatif test_gk66_manual_whatif.c)
target_link_libraries(test_gk66_manual_whatif PRIVATE gk1_app_state)
add_test(NAME GK1_066_ManualEventVictimControllerAndNoWorldMutation COMMAND
 test_gk66_manual_whatif ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0067 primary manual audit 0003 source-corrected geography and mobility.
add_executable(test_gk67_manual_audit_corrections test_gk67_manual_audit_corrections.c)
target_link_libraries(test_gk67_manual_audit_corrections PRIVATE gk1_app_state)
add_test(NAME GK1_067_ManualAudit003GeographyTerrainDivideAtomicity COMMAND
 test_gk67_manual_audit_corrections ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0068 real approved modified-ROM Rest checkpoint: controller A/B
# transaction with byte-for-byte no-mutation cancel. Not stock or Spring.
add_executable(test_gk68_observed_rest test_gk68_observed_rest.c)
target_link_libraries(test_gk68_observed_rest PRIVATE gk1_app_state gk1_pad_router)
add_test(NAME GK1_068_ObservedEnhancedRestControllerCancelAtomicity COMMAND
 test_gk68_observed_rest ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0069 source-verified raw 8192-byte native Mesen WorkRAM import.
# PRIVATE .workram needed for positive provenance test and graphical smoke;
# public tests still enforce negative rollback and classification protections.
add_executable(test_gk69_mesen_ram_map test_gk69_mesen_ram_map.c)
target_link_libraries(test_gk69_mesen_ram_map PRIVATE gk69_mesen_ram_map)
add_test(NAME GK1_069_MesenSourceIdentityMapBoundaryAndNoMutation
         COMMAND test_gk69_mesen_ram_map)

# EXEC0071 added MANUAL_DERIVED order budget engine / PRE_SETUP controller lock.
add_executable(test_gk71_order_budget test_gk71_order_budget.c)
target_link_libraries(test_gk71_order_budget PRIVATE gk1_app_state)
add_test(NAME GK1_071_ManualStrategicOrderBudgetPreSetupLockAtomicity
 COMMAND test_gk71_order_budget ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

add_test(NAME GK1_071_NativeControllerVIEWASafeGateAndBothWorldsImmutable
 COMMAND gk1_native_shell --order-budget-pad-check)

# EXEC0072 manual Pass battle controller research drill, PRE_SETUP isolated.
add_executable(test_gk72_battle_pass test_gk72_battle_pass.c)
target_link_libraries(test_gk72_battle_pass PRIVATE gk1_app_state gk1_pad_router)
add_test(NAME GK1_072_ManualPassControllerPhaseIsolationNegativeRollback
 COMMAND test_gk72_battle_pass ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

add_test(NAME GK1_073_MergedControllerBudgetAndBattlePassPreSetupWorldImmutable COMMAND gk1_native_shell --merged-pad-check)

# EXEC0074 restored read-only archived Spring crosswalk to SAME combined C11 app.
# The private RAM is an optional external input and MUST NOT be packaged.
add_executable(test_gk74_spring_archive test_gk74_spring_archive.c)
target_link_libraries(test_gk74_spring_archive PRIVATE gk1_app_state)
add_test(NAME GK1_074_SHAIdentityGuardArchivedSpringDecoder COMMAND test_gk74_spring_archive)
add_test(NAME GK1_074_MergedSpringControllerNoWorldMutation COMMAND gk1_native_shell --spring-pad-check)

# EXEC0075 exact original-6502 source arithmetic within a bounded spring
# component; source-mapped but NOT setup, hazard, AI, season, or live campaign.
add_executable(test_gk75_spring_economy test_gk75_spring_economy.c)
target_link_libraries(test_gk75_spring_economy PRIVATE gk1_app_state)
add_test(NAME GK1_075_OriginalSourceSpringFoodTaxPayrollAndNoWorldMutation
 COMMAND test_gk75_spring_economy ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)
add_test(NAME GK1_075_ActualNativeControllerSpringPreviewNoMutation
 COMMAND gk1_native_shell --spring-source-pad-check)

# EXEC0077 exact original-stock Spring first-turn checkpoint acceptance oracle.
# Raw original ROM/WorkRAM/savestates are deliberately external/private.
add_executable(test_gk77_stock_spring test_gk77_stock_spring.c)
target_link_libraries(test_gk77_stock_spring PRIVATE gk1_app_state)
add_test(NAME GK1_077_StockSpringExactBankOffsetAndAtomicProvenance COMMAND test_gk77_stock_spring)

# EXEC0078: full native world Spring source-bounded financial transaction,
# atomic guard against the still-unimplemented actual first Spring.
add_executable(test_gk78_spring_world test_gk78_spring_world.c)
target_link_libraries(test_gk78_spring_world PRIVATE gk1_app_state)
add_test(NAME GK1_078_RealWorldSpringFinanceAtomicAndLocked COMMAND test_gk78_spring_world ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0079 integrates an original-stock-SHA-gated CHECKPOINT session into the
# native app and controller. Only manual-derived Pass can change the session.
# Full first-Spring RNG, trade, future seasons and save remain open.
add_executable(test_gk79_stock_checkpoint test_gk79_stock_checkpoint.c)
target_link_libraries(test_gk79_stock_checkpoint PRIVATE gk1_app_state)
add_test(NAME GK1_079_SourceBoundCheckpointPlayerPassAndRollback COMMAND
  test_gk79_stock_checkpoint ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0080: all-country exact original source-bound snapshot save/restore.
add_executable(test_gk80_full_checkpoint test_gk80_full_checkpoint.c)
target_link_libraries(test_gk80_full_checkpoint PRIVATE gk1_app_state)
add_test(NAME GK1_080_SourceBoundWholeSpringColdLoadHostileQA COMMAND test_gk80_full_checkpoint ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0081 native offline controller source chooser, three exact original-SHA profiles.
add_executable(test_gk81_stock_attach test_gk81_stock_attach.c)
target_link_libraries(test_gk81_stock_attach PRIVATE gk1_app_state)
add_test(NAME GK1_081_PrivateStockSourceAttachNoCWDWrongHashAtomic COMMAND test_gk81_stock_attach)

# EXEC0083: owner-only original stock iNES source reset trace; no ROM in package.
add_executable(test_gk82_stock_cartridge test_gk82_stock_cartridge.c)
target_link_libraries(test_gk82_stock_cartridge PRIVATE gk1_app_state)
add_test(NAME GK1_082_OriginalExactStockResetTraceAndRollback COMMAND test_gk82_stock_cartridge)

# EXEC0083: bounded executable 6502 reset trace from exact private stock cartridge.
add_executable(test_gk83_reset_trace test_gk83_reset_trace.c)
target_link_libraries(test_gk83_reset_trace PRIVATE gk1_app_state)
add_test(NAME GK1_083_Original6502ResetTraceSourceGatesNoFabricatedIO COMMAND test_gk83_reset_trace)

add_executable(test_gk84_reset_hypothesis test_gk84_reset_hypothesis.c)
target_link_libraries(test_gk84_reset_hypothesis PRIVATE gk1_app_state)
add_test(NAME GK1_084_StockBootUnknownRAMExhaustiveHypotheses COMMAND test_gk84_reset_hypothesis)

add_executable(test_gk85_raw_source_binding test_gk85_raw_source_binding.c)
target_link_libraries(test_gk85_raw_source_binding PRIVATE gk1_app_state)
add_test(NAME GK1_085_FullOriginal83ByteSourcePreservedAndControllerViewRollback
 COMMAND test_gk85_raw_source_binding ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0086: V2 full source-byte provenance; V1 saved receipts remain recognized
# with explicit legacy status instead of silently claiming source authentication.
add_executable(test_gk86_source_save_binding test_gk86_source_save_binding.c)
target_link_libraries(test_gk86_source_save_binding PRIVATE gk1_app_state)
add_test(NAME GK1_086_SourceByteBoundV2SaveV1MigrationHostileRollback
 COMMAND test_gk86_source_save_binding ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0087: actual controller Spring 14-command menu. Only original
# SHA-verified VIEW and PASS have working effects; others remain source-gated.
add_executable(test_gk87_stock_command_menu test_gk87_stock_command_menu.c)
target_link_libraries(test_gk87_stock_command_menu PRIVATE gk1_app_state)
add_test(NAME GK1_087_StockFourteenCommandsPassViewRollbackAndSourceIsolation
 COMMAND test_gk87_stock_command_menu ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0088 authentic 83-byte numeric projection integrity: failed country tamper
# must reject native controller Pass/view and three-slot saves atomically.
add_executable(test_gk88_source_projection test_gk88_source_projection.c)
target_link_libraries(test_gk88_source_projection PRIVATE gk1_app_state)
add_test(NAME GK1_088_VerifiedSourceProjectionAndTamperRollback COMMAND test_gk88_source_projection ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0089: actual current C11 app source-verified Spring reconciliation view.
add_executable(test_gk89_spring_reconcile test_gk89_spring_reconcile.c)
target_link_libraries(test_gk89_spring_reconcile PRIVATE gk1_app_state)
add_test(NAME GK1_089_AuthenticStockFinanceResidualAndAtomicRefusals
 COMMAND test_gk89_spring_reconcile ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0090: the real native VIEW must reject forged PRE_SETUP scenario
# even if the private 83-byte stock profile itself remains valid.
add_executable(test_gk90_verified_finance test_gk90_verified_finance.c)
target_link_libraries(test_gk90_verified_finance PRIVATE gk1_app_state)
add_test(NAME GK1_090_SealedOriginalTemplateBeforeStockFinance
 COMMAND test_gk90_verified_finance ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0091 archived fresh MesenCE zero-RAM power-on provenance gate.
add_executable(test_gk91_reset_zero test_gk91_reset_zero.c)
target_link_libraries(test_gk91_reset_zero PRIVATE gk1_app_state)
add_test(NAME GK1_091_MesenCE_AllZeros_PowerOn_SourceGate COMMAND test_gk91_reset_zero)

# EXEC0092 source-ROM PPUSTATUS single/sequence hypothesis. No emulated vblank.
add_executable(test_gk92_ppustatus_hypothesis test_gk92_ppustatus_hypothesis.c)
target_link_libraries(test_gk92_ppustatus_hypothesis PRIVATE gk1_app_state)
add_test(NAME GK1_092_Counterfactual_PPUSTATUS_Source_Trace COMMAND test_gk92_ppustatus_hypothesis)

# EXEC0093: original NES reset interpreter now decodes the evidenced setup
# arithmetic/indirect/stack path. Private ROM is an external test input only.
add_executable(test_gk93_source_setup_opcode test_gk93_source_setup_opcode.c)
target_link_libraries(test_gk93_source_setup_opcode PRIVATE gk1_app_state)
add_test(NAME GK1_093_CounterfactualSetupOpcodeSourceAndRollback COMMAND test_gk93_source_setup_opcode)

# EXEC0094: source-banked CPU $6000-$7FFF cartridge save RAM, never an
# alias of the Mesen nesWorkRam mirror; unknown-by-default battery contents.
add_executable(test_gk94_prgram test_gk94_prgram.c)
target_link_libraries(test_gk94_prgram PRIVATE gk1_app_state)
add_test(NAME GK1_094_CartridgeSRAMBankVsWorkRAMUnknownByDefault COMMAND test_gk94_prgram)

# EXEC0095: exact original-stock Rate bound to existing market quote arithmetic.
# No offer RNG, merchant appearance, purchase or order consumption.
add_executable(test_gk95_stock_trade_reference test_gk95_stock_trade_reference.c)
target_link_libraries(test_gk95_stock_trade_reference PRIVATE gk1_app_state)
add_test(NAME GK1_095_SourceAuthenticatedSpringTradeQuotesAndControllerRollback
 COMMAND test_gk95_stock_trade_reference ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

add_executable(test_gk96_ppu_warmup test_gk96_ppu_warmup.c)
target_link_libraries(test_gk96_ppu_warmup PRIVATE gk1_app_state)
add_test(NAME GK1_096_AuthenticatedBootNMIHardwareWaitBoundary COMMAND test_gk96_ppu_warmup)

add_executable(test_gk97_nmi_trace test_gk97_nmi_trace.c)
target_link_libraries(test_gk97_nmi_trace PRIVATE gk1_app_state)
add_test(NAME GK1_097_HypotheticalNMIFrameAndPrivateSourceGate COMMAND test_gk97_nmi_trace)

add_executable(test_gk98_controller_port test_gk98_controller_port.c gk98_cpu_rotate.c)
target_link_libraries(test_gk98_controller_port PRIVATE gk1_app_state)
add_test(NAME GK1_098_BoundedPadSerialAndPrivateBootGate COMMAND test_gk98_controller_port)

add_executable(test_gk105_openbus test_gk105_openbus.c)
target_link_libraries(test_gk105_openbus PRIVATE gk1_app_state)
add_test(NAME GK1_105_PrivateOriginal_PPUSTATUS_LowerSeven_128WayHypothesis COMMAND test_gk105_openbus)

# EXEC0106 bounded CPU-written PPU I/O bus latch research (not Mesen parity).
add_executable(test_gk106_ppu_bus test_gk106_ppu_bus.c)
target_link_libraries(test_gk106_ppu_bus PRIVATE gk1_app_state)
add_test(NAME GK1_106_SourceDrivenPPURegisterOpenBusLatchAndRollback COMMAND test_gk106_ppu_bus)

# EXEC0107 read-only and write-only PPU port source-bounded mirror audit.
add_executable(test_gk107_ppu_ports test_gk107_ppu_ports.c)
target_link_libraries(test_gk107_ppu_ports PRIVATE gk1_app_state)
add_test(NAME GK1_107_PPU_WriteOnly_ReadOpenBusMirrorsAndDecayGate COMMAND test_gk107_ppu_ports)

add_executable(test_gk108_cpu_rmw_bus test_gk108_cpu_rmw_bus.c)
target_link_libraries(test_gk108_cpu_rmw_bus PRIVATE gk1_app_state)
add_test(NAME GK1_108_NMOS6502_RMW_OldNewBusWriteCycleAndAtomicRefusal COMMAND test_gk108_cpu_rmw_bus)

# EXEC0109: all documented NMOS 6502 memory RMW opcodes, old/new bus hook.
add_executable(test_gk109_rmw_families test_gk109_rmw_families.c)
target_link_libraries(test_gk109_rmw_families PRIVATE gk1_app_state)
add_test(NAME GK1_109_OriginalSource_CompleteRMWFamiliesOpcodeCyclesAndFlags COMMAND test_gk109_rmw_families)

# EXEC0110: source-verified all-country partial Spring finance stage.
add_executable(test_gk110_spring_stage test_gk110_spring_stage.c)
target_link_libraries(test_gk110_spring_stage PRIVATE gk1_app_state)
add_test(NAME GK1_110_Original41CountrySpringFinanceStageNoPlayableUnlock
 COMMAND test_gk110_spring_stage ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0111: 14/27-country SHA-authenticated Spring source/stock world gap.
add_executable(test_gk111_stock_spring_audit test_gk111_stock_spring_audit.c)
target_link_libraries(test_gk111_stock_spring_audit PRIVATE gk1_app_state)
add_test(NAME GK1_111_AuthenticStockSpring_AllCountryGapAuditAndRollback
 COMMAND test_gk111_stock_spring_audit ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0112 all 24 stock-country fields; paired independent original histories.
add_executable(test_gk112_stock_setup_deltas test_gk112_stock_setup_deltas.c)
target_link_libraries(test_gk112_stock_setup_deltas PRIVATE gk1_app_state)
add_test(NAME GK1_112_AuthenticatedStockFullFieldSetupAndHistoryDeltas_NoUnlock
 COMMAND test_gk112_stock_setup_deltas ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0114: six original authentic Mesen stage transitions, strict private-only proof.
add_executable(test_gk114_stock_spring_timeline test_gk114_stock_spring_timeline.c)
target_link_libraries(test_gk114_stock_spring_timeline PRIVATE gk1_app_state)
add_test(NAME GK1_114_OriginalStockFreshFirstSpringSixStageSourceProof
 COMMAND test_gk114_stock_spring_timeline ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

add_executable(test_gk115_market_gate test_gk115_market_gate.c)
target_link_libraries(test_gk115_market_gate PRIVATE gk1_app_state)
add_test(NAME GK1_115_SourceAuthenticMarketOfferGateAndAtomicRollback COMMAND test_gk115_market_gate "${CMAKE_CURRENT_SOURCE_DIR}/../execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv")

# EXEC0117: canonical SHA-verified original template + private record boundary
add_executable(test_gk117_stock_pristine_gate test_gk117_stock_pristine_gate.c)
target_link_libraries(test_gk117_stock_pristine_gate PRIVATE gk1_app_state)
add_test(NAME GK1_117_AuthenticStockPristineImportNegativeControls COMMAND test_gk117_stock_pristine_gate "${CMAKE_CURRENT_SOURCE_DIR}/../execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv")

# EXEC0118: recovered exact published EXEC0116 source integrated with EXEC0117
# plus strict authenticated source receipt tests.
add_executable(test_gk116_enhanced_assign test_gk116_enhanced_assign.c)
target_link_libraries(test_gk116_enhanced_assign PRIVATE gk1_app_state)
add_test(NAME GK1_118_EnhancedAssignIntegratedClassicLockedAndSourceBound
 COMMAND test_gk116_enhanced_assign)

add_executable(test_gk118_private_integration test_gk118_private_integration.c)
target_link_libraries(test_gk118_private_integration PRIVATE gk1_app_state)
add_test(NAME GK1_118_PrivateIntegrationRequiresRealSourceOrReportsNotRun COMMAND test_gk118_private_integration)

# EXEC0119: source-gated Enhanced Pass and backward-compatible cold-load.
add_executable(test_gk119_order_bridge test_gk119_order_bridge.c)
target_link_libraries(test_gk119_order_bridge PRIVATE gk1_app_state)
add_test(NAME GK1_119_SharedEnhancedThreeOrderBridgeAndReceiptCompatibility
 COMMAND test_gk119_order_bridge)

# EXEC0120 portable user-data policy is real native shell code, not a mock.
add_executable(test_gk120_save_home test_gk120_save_home.c)
target_link_libraries(test_gk120_save_home PRIVATE gk120_save_home gk58_slots gk59_storage)
add_test(NAME GK1_120_NativeLinuxXDGHomeAndWinEnhancedUserDataNoCWD
 COMMAND test_gk120_save_home)

# EXEC0121 original 6502 BIT/CLV/BVC/BVS source execution & full arithmetic test.
add_executable(test_gk121_bit_flags test_gk121_bit_flags.c)
target_link_libraries(test_gk121_bit_flags PRIVATE gk1_app_state)
add_test(NAME GK1_121_NES2A03_BitOverflowBranch_FullHostileQA
 COMMAND test_gk121_bit_flags)

# EXEC0123: official 6502 register load/store address and flag semantics.
add_executable(test_gk123_cpu_loadstore test_gk123_cpu_loadstore.c)
target_link_libraries(test_gk123_cpu_loadstore PRIVATE gk1_app_state)
add_test(NAME GK1_123_NMOS_LoadStore_All31_And_SourceGates COMMAND test_gk123_cpu_loadstore)
# EXEC0122 integration regression tests, ensuring no regression while adding 123.
add_executable(test_gk122_cpu_logic test_gk122_cpu_logic.c)
target_link_libraries(test_gk122_cpu_logic PRIVATE gk1_app_state)
add_test(NAME GK1_122_LogicParity_RetainedIn123 COMMAND test_gk122_cpu_logic)

# EXEC0124: every documented NMOS CMP/CPX/CPY addressing mode; a private
# cartridge is not bundled or required by the standalone exhaustive tests.
add_executable(test_gk124_cpu_compare test_gk124_cpu_compare.c)
target_link_libraries(test_gk124_cpu_compare PRIVATE gk1_app_state)
add_test(NAME GK1_124_NMOS_All14_CompareModes_Flags_Cycles_GatedSource
 COMMAND test_gk124_cpu_compare)

# EXEC0125: all official 2A03 ADC and SBC forms, including complete bus gates.
add_executable(test_gk125_cpu_arith test_gk125_cpu_arith.c)
target_link_libraries(test_gk125_cpu_arith PRIVATE gk1_app_state)
add_test(NAME GK1_125_StockNES_BinaryADCSBC_16Modes_UnknownRAMGate COMMAND test_gk125_cpu_arith)

# EXEC0126: hardware source-write barrier is an actual linked source gate.
add_executable(test_gk126_io_barrier test_gk126_io_barrier.c)
target_link_libraries(test_gk126_io_barrier PRIVATE gk1_app_state)
add_test(NAME GK1_126_StockROM_DMA_PPU_Render_SourceGates COMMAND test_gk126_io_barrier)

# EXEC0127: fail-closed two-write RMW transaction and CPU register rollback.
add_executable(test_gk127_io_transaction test_gk127_io_transaction.c)
target_link_libraries(test_gk127_io_transaction PRIVATE gk1_app_state)
add_test(NAME GK1_127_BusAtomic_RMW_Preflight_And_StockRollback COMMAND test_gk127_io_transaction)

add_executable(test_gk128_oam test_gk128_oam.c)
target_link_libraries(test_gk128_oam PRIVATE gk1_app_state)
add_test(NAME GK1_128_SourceGatedOAMRegisters_ExplicitAPUPhaseDMA COMMAND test_gk128_oam)

add_executable(test_gk129_dma_source test_gk129_dma_source.c)
target_link_libraries(test_gk129_dma_source PRIVATE gk1_app_state)
add_test(NAME GK1_129_SourceGatedCpuRamOamDmaAtomicAndVerifiedRomGate COMMAND test_gk129_dma_source)


add_executable(test_gk130_dma_bus test_gk130_dma_bus.c)
target_link_libraries(test_gk130_dma_bus PRIVATE gk1_app_state)
add_test(NAME GK1_130_SourceAuthenticatedCpuOamDmaRollback COMMAND test_gk130_dma_bus)
add_executable(test_gk131_boot_gate test_gk131_boot_gate.c)
target_link_libraries(test_gk131_boot_gate PRIVATE gk1_app_state)
add_test(NAME GK1_131_OriginalResetUnknownVersusArchivedAllZeros COMMAND test_gk131_boot_gate)
add_executable(test_gk132_f5_setup test_gk132_f5_setup.c)
target_link_libraries(test_gk132_f5_setup PRIVATE gk1_app_state)
add_test(NAME GK1_132_AuthenticConditionalF5SetupAndExhaustiveUnknowns COMMAND test_gk132_f5_setup)

add_executable(test_gk133_boot_order test_gk133_boot_order.c)
target_link_libraries(test_gk133_boot_order PRIVATE gk1_app_state)
add_test(NAME GK1_133_AuthenticatedBootReadBeforeWriteAndMesenPowerOnProfile COMMAND test_gk133_boot_order)

# EXEC0134: canonical original 14-command identity, TAX/ASSIGN routing guard.
add_executable(test_gk134_stock_command_route test_gk134_stock_command_route.c)
target_link_libraries(test_gk134_stock_command_route PRIVATE gk1_app_state)
add_test(NAME GK1_134_Stock14CommandTaxAssignRouteNeverCrosses COMMAND test_gk134_stock_command_route)

add_executable(test_gk135_ppu_atomic test_gk135_ppu_atomic.c)
target_link_libraries(test_gk135_ppu_atomic PRIVATE gk1_app_state)
add_test(NAME GK1_135_PPUStatusRaceAndRenderRefusalAtomicSourceTrace COMMAND test_gk135_ppu_atomic)

# EXEC0136 original-source NTSC frame accounting and long-horizon hostile QA.
add_executable(test_gk136_ppu_longspan test_gk136_ppu_longspan.c)
target_link_libraries(test_gk136_ppu_longspan PRIVATE gk1_app_state)
add_test(NAME GK1_136_PPUConstantTimeAndLongHorizonAtomicity COMMAND test_gk136_ppu_longspan)

# EXEC0137: authentic economic-mapping RNG source kernel, C11 native app.
add_executable(test_gk137_stock_rng test_gk137_stock_rng.c)
target_link_libraries(test_gk137_stock_rng PRIVATE gk1_app_state)
add_test(NAME GK1_137_SourceDerived48BitSpringRateIndependentOracleAndAppSafety COMMAND test_gk137_stock_rng)

# EXEC0138: backwards source RNG stream and noncausal Spring Rate alignment.
add_executable(test_gk138_rng_reverse test_gk138_rng_reverse.c)
target_link_libraries(test_gk138_rng_reverse PRIVATE gk1_app_state)
add_test(NAME GK1_138_OriginalStock48BitInverseAndSpringRateHistoryNoGameplayPromotion COMMAND test_gk138_rng_reverse)

add_executable(test_gk139_mesen_state test_gk139_mesen_state.c)
target_link_libraries(test_gk139_mesen_state PRIVATE gk1_app_state)
add_test(NAME GK1_139_MesenArchivedPostSetupRNGProvenanceWorkVersusSaveRAMAndAtomicRefusal COMMAND test_gk139_mesen_state)

add_executable(gk139_state_inspect gk139_state_inspect.c)
target_link_libraries(gk139_state_inspect PRIVATE gk1_app_state)

find_package(Python3 COMPONENTS Interpreter QUIET)
if(Python3_Interpreter_FOUND)
 add_test(NAME GK1_139_IndependentCompressedMesenStateParserSyntheticHostileQA
          COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test_gk139_archived_state_extract.py)
endif()

add_executable(test_gk140_rng_lineage test_gk140_rng_lineage.c)
target_link_libraries(test_gk140_rng_lineage PRIVATE gk1_app_state)
add_test(NAME GK1_140_AuthenticatedMesenStatePureRngLineageCompatNeverProvesGameChronology COMMAND test_gk140_rng_lineage)

add_executable(test_gk142_fixed_cfg test_gk142_fixed_cfg.c)
target_link_libraries(test_gk142_fixed_cfg PRIVATE gk1_app_state)
add_test(NAME GK1_142_FixedBankVectorRootedCFGNotRuntimeStock COMMAND test_gk142_fixed_cfg)

add_executable(test_gk143_bank_cfg test_gk143_bank_cfg.c)
target_link_libraries(test_gk143_bank_cfg PRIVATE gk1_app_state)
add_test(NAME GK1_143_ExplicitBankHypothesisNoSpringNorMesenRuntime COMMAND test_gk143_bank_cfg)

# EXEC0144: source-bound archived Mesen PRG-window mapping at SAVED instant.
add_executable(test_gk144_archived_mapper test_gk144_archived_mapper.c)
target_link_libraries(test_gk144_archived_mapper PRIVATE gk1_app_state)
add_test(NAME GK1_144_ArchivedMesenMappedPRGPageProofNotStartupOrGameplay COMMAND test_gk144_archived_mapper)

add_executable(test_gk145_fresh_progress test_gk145_fresh_progress.c)
target_link_libraries(test_gk145_fresh_progress PRIVATE gk1_app_state)
add_test(NAME GK1_145_VerifiedFreshSourceProgressNoInventedFirstSpring COMMAND test_gk145_fresh_progress)

# EXEC0146: live Enhanced controller quantity, optional authenticated original
# stock fresh/restored runtime snapshots never embedded in public QA.
add_executable(test_gk146_assign_quantity test_gk146_assign_quantity.c)
target_link_libraries(test_gk146_assign_quantity PRIVATE gk1_app_state)
add_test(NAME GK1_146_EnhancedControllerVariableQuantityOriginalSourceGuards COMMAND test_gk146_assign_quantity)

add_executable(test_gk147_stock_goods test_gk147_stock_goods.c)
target_link_libraries(test_gk147_stock_goods PRIVATE gk1_app_state)
add_test(NAME GK1_147_AuthenticatedSourceAll14and27CountryGoodsNoTradeOrOrders COMMAND test_gk147_stock_goods)

add_executable(test_gk148_fresh_bank_trace test_gk148_fresh_bank_trace.c gk149_ppu_phase_scan.c)
target_link_libraries(test_gk148_fresh_bank_trace PRIVATE gk1_app_state)
add_test(NAME GK1_148_OriginalFreshWorkRAMPhasedChronologyAndImmutableApp COMMAND test_gk148_fresh_bank_trace)

# EXEC0149 independent source-phase scan tests. Private original ROM supplied
# only by owner/environment, NEVER shipped in source, installer or public CI.
add_executable(test_gk149_ppu_phase_scan test_gk149_ppu_phase_scan.c)
target_link_libraries(test_gk149_ppu_phase_scan PRIVATE gk1_app_state)
add_test(NAME GK1_149_AuthenticatedOriginalBootPPUPhaseHypothesesNoSpringUnlock
 COMMAND test_gk149_ppu_phase_scan)

# EXEC0150 actual linked source and hostile PPU window / ROM provenance QA.
add_executable(test_gk150_render_window test_gk150_render_window.c)
target_link_libraries(test_gk150_render_window PRIVATE gk1_app_state)
add_test(NAME GK1_150_AuthenticatedBoundedRenderWindowOriginalSourceNoGameplay
 COMMAND test_gk150_render_window)

# EXEC0151 even-rendered-frame *hypothesis*, not authentic Mesen behavior.
add_executable(test_gk151_even_source test_gk151_even_source.c)
target_link_libraries(test_gk151_even_source PRIVATE gk1_app_state)
add_test(NAME GK1_151_OriginalStockEvenPreRenderHypothesisStillNotSpring
 COMMAND test_gk151_even_source)

add_executable(test_gk152_spring_writer_map test_gk152_spring_writer_map.c)
target_link_libraries(test_gk152_spring_writer_map PRIVATE gk1_app_state)
add_test(NAME GK1_152_OriginalStockSpringWritersStaticCandidatesNotExecuted COMMAND test_gk152_spring_writer_map)

# EXEC0153 source-executed memory watchpoint with private-cartridge opt-in.
add_executable(test_gk153_country_write_watch test_gk153_country_write_watch.c)
target_link_libraries(test_gk153_country_write_watch PRIVATE gk1_app_state)
add_test(NAME GK1_153_Executed6502PRGRAMCountryWatchNotRawCandidate COMMAND test_gk153_country_write_watch)

# EXEC0154: an explicitly counterfactual single PPUSTATUS race branch, tested
# on the complete linked native game, never a Classic first-Spring unlock.
add_executable(test_gk154_vblank_branch test_gk154_vblank_branch.c)
target_link_libraries(test_gk154_vblank_branch PRIVATE gk1_app_state)
add_test(NAME GK1_154_AtomicSingleRaceBranchAndNoCampaignUnlock
 COMMAND test_gk154_vblank_branch)

# GK155: strictly source-proved PPUSTATUS sign-only poll; unverified phase.
add_executable(test_gk155_sign_only_poll test_gk155_sign_only_poll.c)
target_link_libraries(test_gk155_sign_only_poll PRIVATE gk1_app_state)
add_test(NAME GK1_155_SignOnlySourceGuardAndRenderBoundaryRefusal
 COMMAND test_gk155_sign_only_poll)

# GK156 exhaustive source-low7 counterfactual audit
add_executable(test_gk156_sign_exit_matrix test_gk156_sign_exit_matrix.c)
target_link_libraries(test_gk156_sign_exit_matrix PRIVATE gk1_app_state)
add_test(NAME GK1_156_ExhaustiveLow7UnknownMatrixRefusal COMMAND test_gk156_sign_exit_matrix)
# GK157 symbolic taint alternative retains GK156 exhaustive low7 matrix test.
add_executable(test_gk157_symbolic_vblank test_gk157_symbolic_vblank.c)
target_link_libraries(test_gk157_symbolic_vblank PRIVATE gk1_app_state)
add_test(NAME GK1_157_SymbolicLow7SourceTaintRefusal COMMAND test_gk157_symbolic_vblank)

# EXEC0158: real controller-linked Enhanced Summer worker continuation,
# strict original checkpoint provenance and separate GS18 cold-load contract.
add_executable(test_gk158_enhanced_summer test_gk158_enhanced_summer.c)
target_link_libraries(test_gk158_enhanced_summer PRIVATE gk1_app_state)
add_test(NAME GK1_158_SourceAttestedEnhancedSummerAndColdLoad
 COMMAND test_gk158_enhanced_summer ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0159: Enhanced-only authenticated Fall after original Spring and Enhanced Summer.
add_executable(test_gk159_enhanced_fall test_gk159_enhanced_fall.c)
target_link_libraries(test_gk159_enhanced_fall PRIVATE gk1_app_state)
add_test(NAME GK1_159_SourceAttestedEnhancedFallControllerAndIndependentColdLoad
 COMMAND test_gk159_enhanced_fall ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0160: source-attested Enhanced-only Winter; original NES still pre-setup.
add_executable(test_gk160_enhanced_winter test_gk160_enhanced_winter.c)
target_link_libraries(test_gk160_enhanced_winter PRIVATE gk1_app_state)
add_test(NAME GK1_160_SourceAttestedEnhancedWinterAndColdLoad
 COMMAND test_gk160_enhanced_winter ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

add_executable(test_gk161_enhanced_year2 test_gk161_enhanced_year2.c)
target_link_libraries(test_gk161_enhanced_year2 PRIVATE gk1_app_state)
add_test(NAME gk161_enhanced_year2 COMMAND test_gk161_enhanced_year2 ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

add_executable(test_gk162_original_entry test_gk162_original_entry.c)
target_link_libraries(test_gk162_original_entry PRIVATE gk1_app_state)
add_test(NAME GK1_162_OriginalStockCheckpointEntry_ControllerRegression COMMAND test_gk162_original_entry ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0163: exact-original archived Mesen .mss importer. Optional ZLIB keeps
# existing Win32/Linux game builds valid even when zlib development files
# are unavailable; in that case the importer fails closed as UNAVAILABLE.
target_sources(gk1_app_state PRIVATE gk163_mesen_import.c)
if(ZLIB_FOUND)
  target_link_libraries(gk1_app_state PUBLIC ZLIB::ZLIB)
  target_compile_definitions(gk1_app_state PUBLIC GK163_HAS_ZLIB=1)
endif()
add_executable(test_gk163_mesen_import test_gk163_mesen_import.c)
target_link_libraries(test_gk163_mesen_import PRIVATE gk1_app_state)
add_test(NAME GK1_163_ExactArchivedOriginalMesenMssImportAndHostileQACases
 COMMAND test_gk163_mesen_import)

# EXEC0164 source-authenticated in-memory archived original Mesen continuation.
add_executable(test_gk164_mss_entry test_gk164_mss_entry.c)
target_link_libraries(test_gk164_mss_entry PRIVATE gk1_app_state)
add_test(NAME GK1_164_PrivateOriginalMssDirectControllerSourceGuard
 COMMAND test_gk164_mss_entry ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0166 canonical integration: archived .mss and fresh original WorkRAM JSON
# enter through the same source-authenticated atomic Gk162 native state.
add_executable(test_gk165_fresh_json test_gk165_fresh_json.c)
target_link_libraries(test_gk165_fresh_json PRIVATE gk1_app_state)
add_test(NAME GK1_165_OriginalFreshMesenJSONImport_AuthenticatedOwnerOnly COMMAND test_gk165_fresh_json)
add_executable(test_gk166_fresh_entry test_gk166_fresh_entry.c)
target_link_libraries(test_gk166_fresh_entry PRIVATE gk1_app_state)
add_test(NAME GK1_166_OriginalFreshMesenDirectInMemoryAndRollback COMMAND test_gk166_fresh_entry ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0167: private self-contained original-stock continuation (no licensed
# save file is installed or distributed). Public test uses negative controls;
# owner-private original Mesen evidence is tested out of band.
add_executable(test_gk167_private_cold_save test_gk167_private_cold_save.c)
target_link_libraries(test_gk167_private_cold_save PRIVATE gk1_app_state)
add_test(NAME GK1_167_StockPrivateColdSave_NoFakeSource_HostileQA
  COMMAND test_gk167_private_cold_save
    ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0169 port-only controller Save New / Cold Restore menu, source-locked.
add_executable(test_gk169_port_storage test_gk169_port_storage.c)
target_link_libraries(test_gk169_port_storage PRIVATE gk1_app_state)
add_test(NAME GK1_169_ControllerOTHER_SourceLockedPortStorage_AtomicCancel
 COMMAND test_gk169_port_storage)

# EXEC0170 — independently validated controller TITLE private-slot inventory.
add_executable(test_gk170_slot_preview test_gk170_slot_preview.c)
target_link_libraries(test_gk170_slot_preview PRIVATE gk1_app_state)
add_test(NAME GK1_170_TitleSlotPreview_NoMutationAndSourceIdentity
 COMMAND test_gk170_slot_preview
 "${CMAKE_CURRENT_SOURCE_DIR}/../execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv")

# EXEC0171 owner-private, source-attested Enhanced controller-title cold resume.
add_executable(test_gk171_enhanced_title_resume test_gk171_enhanced_title_resume.c)
target_link_libraries(test_gk171_enhanced_title_resume PRIVATE gk1_app_state)
add_test(NAME GK1_171_EnhancedControllerTitleColdResume_SourceGateAndRollback
 COMMAND test_gk171_enhanced_title_resume
 "${CMAKE_CURRENT_SOURCE_DIR}/../execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv")

# EXEC0172 authentic original stock initialization/RNG candidate inventory.
add_executable(test_gk172_rng_source_xref test_gk172_rng_source_xref.c)
target_link_libraries(test_gk172_rng_source_xref PRIVATE gk1_app_state)
add_test(NAME GK1_172_AuthenticatedStaticOriginalRngSitesNoGameplayClaim
 COMMAND test_gk172_rng_source_xref)

add_executable(test_gk173_rng_control test_gk173_rng_control.c)
target_link_libraries(test_gk173_rng_control PRIVATE gk1_app_state)
add_test(NAME GK1_173_SourceExecutedRngTransferOnlyNotOpcodeCandidates COMMAND test_gk173_rng_control)

add_executable(test_gk174_setup_boundary test_gk174_setup_boundary.c)
target_link_libraries(test_gk174_setup_boundary PRIVATE gk1_app_state)
add_test(NAME GK1_174_ExactMesenRAMSetupBoundaryNotRuntimeParity COMMAND test_gk174_setup_boundary)

# GK175: independently assert Linux launcher, CLI, CPack and both source
# packaging generator declarations share one version.
if(Python3_Interpreter_FOUND)
 add_test(NAME GK1_176_LauncherAndDualPackagingVersionContract
  COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/packaging/test_gk176_version_contract.py
   $<TARGET_FILE:gk1_native_shell> ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
   ${CMAKE_CURRENT_SOURCE_DIR}/packaging/gk1-native-research.desktop
   ${CMAKE_CURRENT_BINARY_DIR}/CPackConfig.cmake)
endif()

# EXEC0176 Enhanced-only source-attested five-year labor continuation; Classic untouched.
add_executable(test_gk176_enhanced_cycle test_gk176_enhanced_cycle.c)
target_link_libraries(test_gk176_enhanced_cycle PRIVATE gk1_app_state)
add_test(NAME GK1_176_EnhancedLaborContinuationAttestationRollback
 COMMAND test_gk176_enhanced_cycle ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0177 authenticated Enhanced continuation durable cross-session receipt.
add_executable(test_gk177_enhanced_receipt test_gk177_enhanced_receipt.c)
target_link_libraries(test_gk177_enhanced_receipt PRIVATE gk1_app_state)
add_test(NAME GK1_177_EnhancedContinuationDurableAtomicColdLoad
 COMMAND test_gk177_enhanced_receipt ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0178: source-authenticated original fixed-bank RNG primitive, not Spring.
add_executable(test_gk178_source_rng test_gk178_source_rng.c)
target_link_libraries(test_gk178_source_rng PRIVATE gk1_app_state)
add_test(NAME gk178_source_rng COMMAND test_gk178_source_rng)

# EXEC0179: refuse original source RNG arithmetic outside completed setup
# and when any prior loaded stock or Enhanced state is present.
add_executable(test_gk179_rng_setup_boundary test_gk179_rng_setup_boundary.c)
target_link_libraries(test_gk179_rng_setup_boundary PRIVATE gk1_app_state)
add_test(NAME GK1_179_OriginalRngCompleteSetupExclusivePhaseBoundary
 COMMAND test_gk179_rng_setup_boundary)

add_test(NAME GK1_179_SourceAndPackagingVersionContract
 COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/packaging/test_gk179_version_contract.py
 ${CMAKE_CURRENT_SOURCE_DIR}/gk1_native_shell.c ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)

# EXEC0180: static original fixed-bank reset/NMI/IRQ seed-writer map.
add_executable(test_gk180_seed_writer_scan test_gk180_seed_writer_scan.c)
target_link_libraries(test_gk180_seed_writer_scan PRIVATE gk1_app_state)
add_test(NAME GK1_180_AuthenticatedSourceSeedWritersAndAtomicRefusals
 COMMAND test_gk180_seed_writer_scan)
add_test(NAME GK1_180_SourceAndPackagingVersionContract
 COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/packaging/test_gk180_version_contract.py
 ${CMAKE_CURRENT_SOURCE_DIR}/gk1_native_shell.c ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)

# EXEC0181: selected low-bank conditional source graph (not actual NES execution).
add_executable(test_gk181_bank_seed_trace test_gk181_bank_seed_trace.c)
target_link_libraries(test_gk181_bank_seed_trace PRIVATE gk1_app_state)
add_test(NAME GK1_181_OriginalLowBankSeedWritersAndMapperBarrier
 COMMAND test_gk181_bank_seed_trace)
add_test(NAME GK1_181_LinuxVersionAndPackagingContract
 COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/packaging/test_gk181_version_contract.py
 ${CMAKE_CURRENT_SOURCE_DIR}/gk1_native_shell.c ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)

# EXEC0182: original private archived Mesen snapshot + exact ROM source provenance.
add_executable(test_gk182_archived_source_bank test_gk182_archived_source_bank.c)
target_link_libraries(test_gk182_archived_source_bank PRIVATE gk1_app_state)
add_test(NAME GK1_182_SourceAuthenticatedSavedInstantMapperAndSeed
 COMMAND test_gk182_archived_source_bank)
add_test(NAME GK1_182_LinuxVersionAndPackagingContract
 COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/packaging/test_gk182_version_contract.py
 ${CMAKE_CURRENT_SOURCE_DIR}/gk1_native_shell.c ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)

# EXEC0183: authentic checkpoint re-entry must clear ALL Enhanced stages,
# including the 20-season continuation, and refuse private Classic saves
# with a stale Enhanced chain. Not NES first-Spring gameplay.
add_executable(test_gk183_checkpoint_isolation test_gk183_checkpoint_isolation.c)
target_link_libraries(test_gk183_checkpoint_isolation PRIVATE gk1_app_state)
add_test(NAME GK1_183_PrivateCheckpointEnhancedLineageRefusalAndReset
 COMMAND test_gk183_checkpoint_isolation
 ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)
add_test(NAME GK1_183_VersionAndPackagingContract
 COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/packaging/test_gk183_version_contract.py
 ${CMAKE_CURRENT_SOURCE_DIR}/gk1_native_shell.c ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)

# EXEC0184: no synthetic unverified original-stock checkpoint may be turned
# into a playable Classic command merely by changing the controller screen.
add_executable(test_gk184_controller_source_gate test_gk184_controller_source_gate.c)
target_link_libraries(test_gk184_controller_source_gate PRIVATE gk1_app_state)
add_test(NAME GK1_184_AuthenticSourceControllerCommandAndPassBoundary
 COMMAND test_gk184_controller_source_gate
 ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)
add_test(NAME GK1_184_VersionAndPackagingContract
 COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/packaging/test_gk184_version_contract.py
 ${CMAKE_CURRENT_SOURCE_DIR}/gk1_native_shell.c ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)

add_executable(test_gk185_stock_hotkey test_gk185_stock_hotkey.c)
target_link_libraries(test_gk185_stock_hotkey PRIVATE gk1_app_state)
add_test(NAME GK1_185_StockControllerSnapshotHotkeyProfileAndSourceGate COMMAND test_gk185_stock_hotkey)

add_executable(test_gk186_original_rng_call_roots test_gk186_original_rng_call_roots.c)
target_link_libraries(test_gk186_original_rng_call_roots PRIVATE gk1_app_state)
add_test(NAME GK1_186_OriginalAuthenticatedRNGCallsiteStaticRootReachability COMMAND test_gk186_original_rng_call_roots)

# EXEC0187: source-attested Classic ASSIGN, manual-derived, NOT stock execution parity.
add_executable(test_gk187_classic_assign test_gk187_classic_assign.c)
target_link_libraries(test_gk187_classic_assign PRIVATE gk1_app_state)
add_test(NAME GK1_187_ClassicSourceGatedManualAssignAndRollback COMMAND test_gk187_classic_assign
 ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

add_executable(test_gk188_classic_receipt test_gk188_classic_receipt.c)
target_link_libraries(test_gk188_classic_receipt PRIVATE gk1_app_state)
add_test(NAME GK1_188_ClassicReceiptRejectsForgedAndUnverifiedSource COMMAND test_gk188_classic_receipt)

# EXEC0189: owner-private self-contained ORIGINAL stock checkpoint + manual Classic
# ASSIGN and PASS; cold-load from TITLE with exact source SHA and atomic replay.
add_executable(test_gk189_classic_cold_save test_gk189_classic_cold_save.c)
target_link_libraries(test_gk189_classic_cold_save PRIVATE gk1_app_state)
add_test(NAME GK1_189_PrivateClassicColdLoadAtomicRefusal COMMAND test_gk189_classic_cold_save
 ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0189 private two-process proof executable; positive tests require owner-private
# authentic 83-byte records and MUST NOT be run from public CTest fixtures.
add_executable(test_gk189_process_restart test_gk189_process_restart.c)
target_link_libraries(test_gk189_process_restart PRIVATE gk1_app_state)

# EXEC0190: port-only Y undo of LAST unpassed manual-derived source-attested
# Classic ASSIGN, with atomic source replay and inherited private GC19 restore.
add_executable(test_gk190_classic_undo test_gk190_classic_undo.c)
target_link_libraries(test_gk190_classic_undo PRIVATE gk1_app_state)
add_test(NAME GK1_190_ControllerClassicUndoAtomicReplayAndColdSave COMMAND test_gk190_classic_undo
 "${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv")

# EXEC0191: source-only inline $EF85 dispatch-table target graph, not NES execution.
add_executable(test_gk191_dispatch_rng_tables test_gk191_dispatch_rng_tables.c)
target_link_libraries(test_gk191_dispatch_rng_tables PRIVATE gk1_app_state)
add_test(NAME GK1_191_ConditionalDispatcherRngTableAndAtomicNegativeTests
 COMMAND test_gk191_dispatch_rng_tables)

# EXEC0192: unpassed third Classic labor order may be undone after automatic
# budget exhaustion; Pass-ended and forged source states remain irreversible.
add_executable(test_gk192_final_order_undo test_gk192_final_order_undo.c)
target_link_libraries(test_gk192_final_order_undo PRIVATE gk1_app_state)
add_test(NAME GK1_192_ControllerThirdOrderUndoAndSourceReplay
 COMMAND test_gk192_final_order_undo
 "${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv")

# EXEC0193: authentic private Mesen bank/profile auto-identification in the
# existing original-stock C11 game; no guessed Spring initialization.
add_executable(test_gk193_auto_source_entry test_gk193_auto_source_entry.c)
target_link_libraries(test_gk193_auto_source_entry PRIVATE gk1_app_state)
add_test(NAME GK1_193_OriginalBankAwarePrivateSourceAutoEntryAtomicRollback
 COMMAND test_gk193_auto_source_entry
 "${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv")

# Original private stock CLI/controller OTHER -> exclusive GC19 save/load, with
# source/replay gating; all authentic stock records remain owner-private.
add_executable(test_gk194_classic_storage test_gk194_classic_storage.c)
target_link_libraries(test_gk194_classic_storage PRIVATE gk1_app_state)
add_test(NAME GK1_194_ControllerOtherClassicColdSaveSourceGate
 COMMAND test_gk194_classic_storage ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

add_executable(test_gk195_classic_view test_gk195_classic_view.c)
target_link_libraries(test_gk195_classic_view PRIVATE gk1_app_state)
add_test(NAME GK1_195_AuthenticClassicDerivedCountryViewAndPassBudget
 COMMAND test_gk195_classic_view ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0196: source-attested Classic Trade quote order/status accounting only.
add_executable(test_gk196_classic_trade_status test_gk196_classic_trade_status.c)
target_link_libraries(test_gk196_classic_trade_status PRIVATE gk1_app_state)
add_test(NAME GK1_196_ClassicSourceBoundTradeQuoteLiveBudgetAndTamperRollback
 COMMAND test_gk196_classic_trade_status ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

add_executable(test_gk198_tax_forecast test_gk198_tax_forecast.c)
target_link_libraries(test_gk198_tax_forecast PRIVATE gk1_app_state)
add_test(NAME GK1_198_AuthenticatedClassicTaxWhatIfAndAtomicRollback COMMAND test_gk198_tax_forecast)

add_executable(test_gk197_classic_tax test_gk197_classic_tax.c)
target_link_libraries(test_gk197_classic_tax PRIVATE gk1_app_state)
add_test(NAME GK1_197_ManualDerivedClassicTaxRateRangeAndRollback COMMAND test_gk197_classic_tax ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

add_executable(test_gk199_classic_tax_undo test_gk199_classic_tax_undo.c)
target_link_libraries(test_gk199_classic_tax_undo PRIVATE gk1_app_state)
add_test(NAME GK1_199_ControllerOnlyReplayVerifiedTaxUndo COMMAND test_gk199_classic_tax_undo)

# EXEC0201-CANDIDATE: reusable native Windows XInput + Linux controller
# anti-phantom action fix. Full release remains blocked by exact EXEC0197.
add_executable(test_gk201_chord_rearm test_gk201_chord_rearm.c)
target_link_libraries(test_gk201_chord_rearm PRIVATE gk1_pad_router gk99_xinput_bridge)
add_test(NAME GK1_201_Win32XInputLinuxJoystick_ChordPartialReleaseCannotPhantomSaveConfirm
 COMMAND test_gk201_chord_rearm)

# EXEC0202: original private-stock IRQ/BRK table RNG source attestation.
add_executable(test_gk202_irq_rng_dispatch test_gk202_irq_rng_dispatch.c)
target_link_libraries(test_gk202_irq_rng_dispatch PRIVATE gk1_app_state)
add_test(NAME GK1_202_OriginalRngBrkIndirectDispatchNoFalseSpring
 COMMAND test_gk202_irq_rng_dispatch)
# EXEC0202: regression for a CLI parser branch that previously rejected the
# graphical --smoke-frames route (code 4) despite successful compilation.
if(UNIX AND NOT APPLE)
 find_program(GK202_XVFB_RUN xvfb-run)
 if(GK202_XVFB_RUN)
  add_test(NAME GK1_202_LinuxX11_SmokeFlagsRemainReachable
   COMMAND ${GK202_XVFB_RUN} -a -s "-screen 0 1280x720x24"
    $<TARGET_FILE:gk1_native_shell> --smoke-frames 120)
 endif()
endif()

# EXEC0203 original stock selector provenance; private positive ROM excluded from public CTest.
add_executable(test_gk203_rng_selector_writers test_gk203_rng_selector_writers.c)
target_link_libraries(test_gk203_rng_selector_writers PRIVATE gk1_app_state)
add_test(NAME GK1_203_AuthenticatedSelectorWriterSourceOnlyNoPhantomFirstSpring COMMAND test_gk203_rng_selector_writers)

# EXEC0204: source-authenticated raw selector writes; never gameplay parity.
add_executable(test_gk204_selector_store_inventory test_gk204_selector_store_inventory.c)
target_link_libraries(test_gk204_selector_store_inventory PRIVATE gk1_app_state)
add_test(NAME GK1_204_OriginalSelectorStoreCandidatesStaticOnly COMMAND test_gk204_selector_store_inventory)

# EXEC0205 source-executed 6502 selector-write telemetry; no source ROM in public artifacts.
add_executable(test_gk205_selector_runtime_watch test_gk205_selector_runtime_watch.c)
target_link_libraries(test_gk205_selector_runtime_watch PRIVATE gk1_app_state)
add_test(NAME GK1_205_BoundedStockCPUSelectorWriteTelemetry COMMAND test_gk205_selector_runtime_watch)

# EXEC0206: independent original fixed-bank 6502 vs native 48-bit RNG parity.
add_executable(test_gk206_rng_source_parity test_gk206_rng_source_parity.c)
target_link_libraries(test_gk206_rng_source_parity PRIVATE gk1_app_state)
add_test(NAME GK1_206_StockRng6502CoefficientParityFailClosed COMMAND test_gk206_rng_source_parity)

# EXEC0207: source-executed seed RAM write provenance, not original Mesen runtime.
add_executable(test_gk207_seed_write_watch test_gk207_seed_write_watch.c)
target_link_libraries(test_gk207_seed_write_watch PRIVATE gk1_app_state)
add_test(NAME GK1_207_Bounded6502SourceCompletedSeedWriteProvenance COMMAND test_gk207_seed_write_watch)

# EXEC0208: exact-original archived Mesen source-verified RNG state; no ROM bundled.
add_executable(test_gk208_archived_rng test_gk208_archived_rng.c)
target_link_libraries(test_gk208_archived_rng PRIVATE gk1_app_state)
add_test(NAME GK1_208_ArchiveSourceSeedIdentityAndAtomicFailureGates
 COMMAND test_gk208_archived_rng)

add_executable(test_gk209_archived_rate_ambiguity test_gk209_archived_rate_ambiguity.c)
target_link_libraries(test_gk209_archived_rate_ambiguity PRIVATE gk1_app_state)
add_test(NAME GK1_209_ArchivedStockRngRateAmbiguity_MustNotInferSpring COMMAND test_gk209_archived_rate_ambiguity)

# EXEC0210: exact-original fresh WorkRAM/nesMemory/save-bank gate.
add_executable(test_gk210_fresh_bank_provenance test_gk210_fresh_bank_provenance.c)
target_link_libraries(test_gk210_fresh_bank_provenance PRIVATE gk1_app_state)
add_test(NAME GK1_210_FreshOriginalMemoryBankProvenanceAndRollback COMMAND test_gk210_fresh_bank_provenance)

# EXEC0211: 14 authenticated fresh reset source stage events, no stock gameplay unlocked.
add_executable(test_gk211_fresh_sequence test_gk211_fresh_sequence.c)
target_link_libraries(test_gk211_fresh_sequence PRIVATE gk1_app_state)
add_test(NAME GK1_211_OriginalFreshStock14StageSourceChronologyAndNegativeGates COMMAND test_gk211_fresh_sequence)

# EXEC0212: independently authenticated original-stock byte variation across
# two different history/choice paths. Always run refusal tests; positive proof
# uses user-private originals out of tree, never published as assets.
add_executable(test_gk212_stock_raw_variation test_gk212_stock_raw_variation.c)
target_link_libraries(test_gk212_stock_raw_variation PRIVATE gk1_app_state)
add_test(NAME GK1_212_OriginalMongolRawVariationAndSourceRefusal
 COMMAND test_gk212_stock_raw_variation ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

add_executable(test_gk213_stock_spring_boundary test_gk213_stock_spring_boundary.c)
target_link_libraries(test_gk213_stock_spring_boundary PRIVATE gk1_app_state)
add_test(NAME GK1_213_OriginalSpring1175BoundedFoodTaxPayrollConsistencyNoPhantomRng COMMAND test_gk213_stock_spring_boundary)

# GK214: source-bounded, all-14/27-country atomic Spring economic batch.
add_executable(test_gk214_spring_batch test_gk214_spring_batch.c)
target_link_libraries(test_gk214_spring_batch PRIVATE gk1_app_state)
add_test(NAME GK1_214_BothCampaignsAtomicSpringFinanceNoInventedSetup
 COMMAND test_gk214_spring_batch ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0215: three independently SHA-verified original NES Spring histories
# versus original source finance-only baseline. Public regression refuses fake
# stock data; private positive control invoked separately in QA.
add_executable(test_gk215_original_spring_gap test_gk215_original_spring_gap.c)
target_link_libraries(test_gk215_original_spring_gap PRIVATE gk1_app_state)
add_test(NAME GK1_215_OriginalSpringThreeProfilesGapAndForgeryRefusal
 COMMAND test_gk215_original_spring_gap
 ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0216: private original stock CPU sample provenance, never published ROM.
add_executable(test_gk217_finance_preview test_gk217_finance_preview.c)
target_link_libraries(test_gk217_finance_preview PRIVATE gk1_app_state)
add_test(NAME GK1_EXEC0217_ControllerClassicSourceFinanceNoSpringUnlock
 COMMAND test_gk217_finance_preview ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)
add_executable(test_gk216_fresh_cpu_pc test_gk216_fresh_cpu_pc.c)
target_link_libraries(test_gk216_fresh_cpu_pc PRIVATE gk1_app_state)
add_test(NAME GK1_216_OriginalFreshCpuCaptureProvenanceNegativeSourceRefusals
 COMMAND test_gk216_fresh_cpu_pc)

# EXEC0217: actual graphical controller-route integration test. Distinct from
# ordinary standalone batch math: Classic Start twice reaches real X11 view.
if(UNIX AND NOT APPLE)
 find_program(GK217_XVFB_RUN xvfb-run)
 if(GK217_XVFB_RUN)
  add_test(NAME GK1_217_Linux1280x720_ClassicControllerFinanceActualRendered
   COMMAND ${GK217_XVFB_RUN} -a -s "-screen 0 1280x720x24"
    $<TARGET_FILE:gk1_native_shell> --smoke-classic-finance 120)
 endif()
endif()

# EXEC0218: owner-private Mesen original reset write-capture intake; not startup replay.
add_executable(test_gk218_stock_write_trace test_gk218_stock_write_trace.c)
target_link_libraries(test_gk218_stock_write_trace PRIVATE gk1_app_state)
add_test(NAME GK1_218_ExactROM_MesenTraceSchemaAndAdversarialRefusals
 COMMAND test_gk218_stock_write_trace
 "${CMAKE_CURRENT_BINARY_DIR}/GK218_Synthetic_NonOriginal.csv")

# EXEC0219: native controller Classic source-versus-conserved-labor finance comparison.
add_executable(test_gk219_finance_compare test_gk219_finance_compare.c)
target_link_libraries(test_gk219_finance_compare PRIVATE gk1_app_state)
add_test(NAME GK1_219_ClassicFinanceSourceVsControllerLaborDraftAtomic
 COMMAND test_gk219_finance_compare ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)
if(UNIX AND NOT APPLE)
 find_program(GK219_XVFB_RUN xvfb-run)
 if(GK219_XVFB_RUN)
  add_test(NAME GK1_219_Linux1280x720_ControllerSourceVersusLaborDraftOverlayRendered
   COMMAND ${GK219_XVFB_RUN} -a -s "-screen 0 1280x720x24" $<TARGET_FILE:gk1_native_shell> --smoke-classic-finance-delta 120)
 endif()
endif()

# EXEC0220 source-attested, reversible, controller-operated PORT-only labor edits.
add_executable(test_gk220_labor_history test_gk220_labor_history.c)
target_link_libraries(test_gk220_labor_history PRIVATE gk1_pre_setup_store gk1_app_state)
add_test(NAME GK1_220_PortOnlyLaborUndo_SourceIdentityAndAtomicControls
 COMMAND test_gk220_labor_history ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

if(UNIX AND NOT APPLE)
  find_program(XVFB_RUN_EXECUTABLE xvfb-run)
  if(XVFB_RUN_EXECUTABLE)
    add_test(NAME GK1_220_Linux1280x720_ControllerLaborUndoActualX11
      COMMAND ${XVFB_RUN_EXECUTABLE} -a -s "-screen 0 1280x720x24"
        $<TARGET_FILE:gk1_native_shell> --smoke-classic-labor-undo 120)
  endif()
endif()

# EXEC0221: actual MesenCE original-ROM capture-runner HOSTILE QA.  Twelve
# synthetic runner cases test process/identity/privacy failure without claiming
# any original runtime execution.  The real capture requires owner's exact
# matched MesenCE executable, core and original ROM outside source checkout.
if(Python3_Interpreter_FOUND)
 add_test(NAME GK1_221_PrivateStockMesenCaptureRunnerAdversarialHarness
  COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/packaging/test_gk221_original_stock_capture.py -q)
endif()

# EXEC0222: Enhanced-only 3-order finance practice with strictly isolated Classic.
add_executable(test_gk222_enhanced_spring_lab test_gk222_enhanced_spring_lab.c)
target_link_libraries(test_gk222_enhanced_spring_lab PRIVATE gk1_app_state)
add_test(NAME GK1_222_EnhancedFinancePractice_ThreeOrders_ClassicIsolation
 COMMAND test_gk222_enhanced_spring_lab ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

if(UNIX AND NOT APPLE)
 find_program(GK222_XVFB_RUN xvfb-run)
 if(GK222_XVFB_RUN)
  add_test(NAME GK1_222_Linux1280x720_ControllerEnhancedSpringThreeOrders
   COMMAND ${GK222_XVFB_RUN} -a -s "-screen 0 1280x720x24"
     $<TARGET_FILE:gk1_native_shell> --smoke-enhanced-spring-lab 120)
 endif()
endif()

# EXEC0223: independent explicit undo/replay audit of isolated Enhanced practice.
add_executable(test_gk223_practice_history test_gk223_practice_history.c)
target_link_libraries(test_gk223_practice_history PRIVATE gk1_app_state)
add_test(NAME GK1_223_EnhancedPractice_ThreeOrderUndoReplayAndTamperAtomic
 COMMAND test_gk223_practice_history ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0224: versioned Enhanced-only practice receipt persistence.
add_executable(test_gk224_practice_save test_gk224_practice_save.c)
target_link_libraries(test_gk224_practice_save PRIVATE gk1_app_state)
add_test(NAME GK1_224_EnhancedPracticeReceiptColdLoadAndTamperRollback
 COMMAND test_gk224_practice_save ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)
if(UNIX AND NOT APPLE AND Python3_Interpreter_FOUND)
 find_program(GK224_XVFB_RUN xvfb-run)
 if(GK224_XVFB_RUN)
  add_test(NAME GK1_224_Linux1280x720_ControllerXSaveYLoadAndExclusiveFile
   COMMAND ${Python3_EXECUTABLE}
     ${CMAKE_CURRENT_SOURCE_DIR}/packaging/test_gk224_enhanced_practice_shell.py
     $<TARGET_FILE:gk1_native_shell>)
 endif()
endif()

# EXEC0225: real two-process title Y Enhanced practice replay (not NES).
add_executable(test_gk225_title_resume test_gk225_title_resume.c)
target_link_libraries(test_gk225_title_resume PRIVATE gk1_app_state)
add_test(NAME GK1_225_EnhancedTitleColdResumeBothCampaignsAndFailClosed
 COMMAND test_gk225_title_resume ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)

# EXEC0225 distinct real X11 processes: authenticated Enhanced practice save,
# title-controller cold load and malicious on-disk mutation rejection.
add_test(NAME GK1_225_TwoProcessTitleControllerColdLoad
 COMMAND ${Python3_EXECUTABLE}
   ${CMAKE_CURRENT_SOURCE_DIR}/packaging/test_gk225_two_process_resume.py
   $<TARGET_FILE:gk1_native_shell>)

# EXEC0226: real source-locked Enhanced X overwrite with atomic same-directory replacement.
add_executable(test_gk226_practice_checkpoint test_gk226_practice_checkpoint.c)
target_link_libraries(test_gk226_practice_checkpoint PRIVATE gk1_app_state)
add_test(NAME GK1_226_Enhanced_CrashSafeRecheckpoint_SourceRollback_14_27
 COMMAND test_gk226_practice_checkpoint ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)
add_test(NAME GK1_226_FourSeparateX11_ControllerTitleYSafeUpdate
 COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/packaging/test_gk226_multi_process_checkpoint.py $<TARGET_FILE:gk1_native_shell>)
