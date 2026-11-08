cmake_minimum_required(VERSION 3.15)
project(GK1NativeShell090 C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()
add_library(gk1_app_state STATIC gk61_market.c gk64_manual_rules.c gk65_battle_terminal.c gk68_observed_rest.c gk74_spring_archive.c gk75_spring_economy.c gk77_stock_spring.c gk78_spring_world.c gk79_stock_checkpoint.c gk80_full_checkpoint.c gk81_stock_attach.c gk82_stock_cartridge.c gk83_reset_trace.c gk91_reset_zero.c gk89_spring_reconcile.c gk90_verified_finance.c gk71_order_budget.c gk72_battle_pass.c ../execution_049/gk1_scenario_factory.c ../execution_045/gk1_observed_turn.c ../execution_047/gk1_divide_contract.c gk1_app_state.c gk1_source_identity.c ../execution_039/gk1_battle_session.c ../execution_034/gk1_tactical_slice.c)
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
  target_link_libraries(gk1_native_shell PRIVATE gk1_path_store gk1_divide_store gk1_manual_bonus_store gk1_manual_setup_store gk1_setup_draft_store gk1_pre_setup_store gk1_replay_store gk1_pad_router gk1_snapshot_view gk69_mesen_ram_map gk58_slots gk59_storage gk60_demobilize user32 gdi32)
else()
  find_package(X11 REQUIRED)
  add_executable(gk1_native_shell gk1_native_shell.c)
  target_link_libraries(gk1_native_shell PRIVATE gk1_path_store gk1_divide_store gk1_manual_bonus_store gk1_manual_setup_store gk1_setup_draft_store gk1_pre_setup_store gk1_replay_store gk1_pad_router gk1_snapshot_view gk69_mesen_ram_map gk58_slots gk59_storage gk60_demobilize X11::X11)
endif()
# Windows installer for the SAME Win32/XInput native executable, not an HTML shell.
# File-based static source packaging specification: full Windows compile,
# NSIS generator, installer execution and physical Xbox remain UNTESTED here.
if(WIN32)
  install(TARGETS gk1_native_shell RUNTIME DESTINATION .)
  install(FILES ../execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv DESTINATION .)
  install(FILES README_EXEC0059.md README_EXEC0060.md README_EXEC0062.md README_EXEC0065.md README_EXEC0066.md README_EXEC0067.md README_EXEC0068.md README_EXEC0069.md README_EXEC0071.md README_EXEC0072.md README_EXEC0073.md README_EXEC0074.md README_EXEC0075.md README_EXEC0077.md README_EXEC0078.md README_EXEC0079.md README_EXEC0080.md README_EXEC0081.md README_EXEC0083.md README_EXEC0084.md README_EXEC0085.md README_EXEC0086.md README_EXEC0087.md README_EXEC0088.md README_EXEC0089.md README_EXEC0090.md README_EXEC0091.md DESTINATION .)
  set(CPACK_GENERATOR "NSIS")
  set(CPACK_PACKAGE_NAME "GK1-Native-Research")
  set(CPACK_PACKAGE_VERSION "0.91.0")
  set(CPACK_PACKAGE_VENDOR "GK1 native research prototype")
  set(CPACK_PACKAGE_INSTALL_DIRECTORY "Genghis Khan I Native Research")
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Incomplete native Genghis Khan I research; original first Spring still unfinished")
  set(CPACK_NSIS_DISPLAY_NAME "Genghis Khan I Research 0.91.0-dev1")
  set(CPACK_NSIS_PACKAGE_NAME "Genghis Khan I Research 0.91.0-dev1")
  set(CPACK_NSIS_MUI_FINISHPAGE_RUN "gk1_native_shell.exe")
  set(CPACK_NSIS_MODIFY_PATH OFF)
  set(CPACK_PACKAGE_EXECUTABLES "gk1_native_shell" "Genghis Khan I Research 0.91.0-dev1")
  set(CPACK_CREATE_DESKTOP_LINKS "gk1_native_shell")
  include(CPack)
endif()
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
  install(FILES README_EXEC0059.md README_EXEC0060.md README_EXEC0062.md README_EXEC0065.md README_EXEC0066.md README_EXEC0067.md README_EXEC0068.md README_EXEC0069.md README_EXEC0071.md README_EXEC0072.md README_EXEC0073.md README_EXEC0074.md README_EXEC0075.md README_EXEC0077.md README_EXEC0078.md README_EXEC0079.md README_EXEC0080.md README_EXEC0081.md README_EXEC0083.md README_EXEC0084.md README_EXEC0085.md README_EXEC0086.md README_EXEC0087.md README_EXEC0088.md README_EXEC0089.md README_EXEC0090.md README_EXEC0091.md DESTINATION usr/share/doc/gk1-native-research)
  set(CPACK_GENERATOR "DEB")
  set(CPACK_PACKAGE_NAME "gk1-native-research")
  set(CPACK_PACKAGE_VERSION "0.91.0")
  set(CPACK_DEBIAN_PACKAGE_RELEASE "dev1")
  set(CPACK_PACKAGE_CONTACT "GK1 research prototype (offline source distribution)")
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "GK1 Native Research")
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "libx11-6")
  set(CPACK_DEBIAN_PACKAGE_SECTION "games")
  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Genghis Khan I native research prototype, not completed game")
  set(CPACK_PACKAGE_FILE_NAME "gk1-native-research_0.91.0~dev1_amd64")
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
