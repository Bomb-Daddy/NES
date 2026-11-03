@echo off
setlocal EnableExtensions
cd /d "%~dp0"
rem One step: runs the one-click installer. It needs nothing installed on this PC.
if exist "KOEI_Classic_Enhanced_Setup.exe" (
  start "" "%~dp0KOEI_Classic_Enhanced_Setup.exe"
  exit /b 0
)
echo KOEI_Classic_Enhanced_Setup.exe must be in the same folder as this file.
exit /b 1
