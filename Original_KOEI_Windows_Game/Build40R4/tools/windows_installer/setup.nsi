Unicode true
!define APP "KOEI Classic / Enhanced"
Name "${APP}"
OutFile "KOEI_Classic_Enhanced_Setup.exe"
RequestExecutionLevel user
InstallDir "$LOCALAPPDATA\KOEI_Classic_Enhanced"
SetCompressor /SOLID lzma
SetCompressorDictSize 64
ShowInstDetails nevershow
BrandingText "KOEI Classic / Enhanced"
!include MUI2.nsh
!define MUI_FINISHPAGE_RUN ""
!define MUI_FINISHPAGE_RUN_FUNCTION LaunchGame
!define MUI_FINISHPAGE_RUN_TEXT "Play now"
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_LANGUAGE "English"
Function LaunchGame
  SetOutPath "$INSTDIR\app"
  Exec '"$INSTDIR\python\pythonw.exe" run_game.pyw'
FunctionEnd
Section "Install"
  nsExec::Exec 'powershell.exe -NoProfile -WindowStyle Hidden -Command "Get-Process pythonw,python,Mesen -ErrorAction SilentlyContinue | Where-Object { $$_.Path -like \"$INSTDIR\*\" } | Stop-Process -Force"'
  Sleep 1500
  RMDir /r "$INSTDIR\app"
  RMDir /r "$INSTDIR\python"
  SetOutPath "$INSTDIR"
  File /r "stage\python"
  File /r "stage\app"
  SetOutPath "$INSTDIR\app"
  CreateDirectory "$SMPROGRAMS\KOEI Classic Enhanced"
  CreateShortcut "$SMPROGRAMS\KOEI Classic Enhanced\KOEI Classic Enhanced.lnk" "$INSTDIR\python\pythonw.exe" "run_game.pyw" "$INSTDIR\python\pythonw.exe" 0
  CreateShortcut "$SMPROGRAMS\KOEI Classic Enhanced\Uninstall KOEI Classic Enhanced.lnk" "$INSTDIR\Uninstall.exe"
  CreateShortcut "$DESKTOP\KOEI Classic Enhanced.lnk" "$INSTDIR\python\pythonw.exe" "run_game.pyw" "$INSTDIR\python\pythonw.exe" 0
  WriteUninstaller "$INSTDIR\Uninstall.exe"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\KOEIClassicEnhanced" "DisplayName" "${APP}"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\KOEIClassicEnhanced" "UninstallString" '"$INSTDIR\Uninstall.exe"'
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\KOEIClassicEnhanced" "InstallLocation" "$INSTDIR"
SectionEnd
Section "Uninstall"
  nsExec::Exec 'powershell.exe -NoProfile -WindowStyle Hidden -Command "Get-Process pythonw,python,Mesen -ErrorAction SilentlyContinue | Where-Object { $$_.Path -like \"$INSTDIR\*\" } | Stop-Process -Force"'
  Sleep 1500
  Delete "$DESKTOP\KOEI Classic Enhanced.lnk"
  RMDir /r "$SMPROGRAMS\KOEI Classic Enhanced"
  RMDir /r "$INSTDIR\python"
  RMDir /r "$INSTDIR\app"
  Delete "$INSTDIR\Uninstall.exe"
  RMDir "$INSTDIR"
  DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\KOEIClassicEnhanced"
SectionEnd
