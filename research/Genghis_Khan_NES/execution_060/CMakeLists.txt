cmake_minimum_required(VERSION 3.15)
project(GK1NativeShell060 C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()
add_library(gk1_app_state STATIC ../execution_049/gk1_scenario_factory.c ../execution_045/gk1_observed_turn.c ../execution_047/gk1_divide_contract.c gk1_app_state.c gk1_source_identity.c ../execution_039/gk1_battle_session.c ../execution_034/gk1_tactical_slice.c)
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
  target_link_libraries(gk1_native_shell PRIVATE gk1_path_store gk1_divide_store gk1_manual_bonus_store gk1_manual_setup_store gk1_setup_draft_store gk1_pre_setup_store gk1_replay_store gk1_pad_router gk1_snapshot_view gk58_slots gk59_storage gk60_demobilize user32 gdi32)
else()
  find_package(X11 REQUIRED)
  add_executable(gk1_native_shell gk1_native_shell.c)
  target_link_libraries(gk1_native_shell PRIVATE gk1_path_store gk1_divide_store gk1_manual_bonus_store gk1_manual_setup_store gk1_setup_draft_store gk1_pre_setup_store gk1_replay_store gk1_pad_router gk1_snapshot_view gk58_slots gk59_storage gk60_demobilize X11::X11)
endif()
# Windows installer for the SAME Win32/XInput native executable, not an HTML shell.
# File-based static source packaging specification: full Windows compile,
# NSIS generator, installer execution and physical Xbox remain UNTESTED here.
if(WIN32)
  install(TARGETS gk1_native_shell RUNTIME DESTINATION .)
  install(FILES ../execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv DESTINATION .)
  install(FILES README_EXEC0059.md README_EXEC0060.md DESTINATION .)
  set(CPACK_GENERATOR "NSIS")
  set(CPACK_PACKAGE_NAME "GK1-Native-Research")
  set(CPACK_PACKAGE_VERSION "0.60.0")
  set(CPACK_PACKAGE_VENDOR "GK1 native research prototype")
  set(CPACK_PACKAGE_INSTALL_DIRECTORY "Genghis Khan I Native Research")
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Incomplete standalone Genghis Khan I port; pre-setup only")
  set(CPACK_NSIS_DISPLAY_NAME "Genghis Khan I Research 0.60.0-dev1")
  set(CPACK_NSIS_PACKAGE_NAME "Genghis Khan I Research 0.60.0-dev1")
  set(CPACK_NSIS_MUI_FINISHPAGE_RUN "gk1_native_shell.exe")
  set(CPACK_NSIS_MODIFY_PATH OFF)
  set(CPACK_PACKAGE_EXECUTABLES "gk1_native_shell" "Genghis Khan I Research 0.60.0-dev1")
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
  install(FILES README_EXEC0059.md README_EXEC0060.md DESTINATION usr/share/doc/gk1-native-research)
  set(CPACK_GENERATOR "DEB")
  set(CPACK_PACKAGE_NAME "gk1-native-research")
  set(CPACK_PACKAGE_VERSION "0.60.0")
  set(CPACK_DEBIAN_PACKAGE_RELEASE "dev1")
  set(CPACK_PACKAGE_CONTACT "GK1 research prototype (offline source distribution)")
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "GK1 Native Research")
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "libx11-6")
  set(CPACK_DEBIAN_PACKAGE_SECTION "games")
  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Genghis Khan I native research prototype, not completed game")
  set(CPACK_PACKAGE_FILE_NAME "gk1-native-research_0.60.0~dev1_amd64")
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
