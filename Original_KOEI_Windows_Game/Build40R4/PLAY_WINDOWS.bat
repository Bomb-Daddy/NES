@echo off
setlocal EnableExtensions
cd /d "%~dp0"
if exist "dist\KOEI_Classic_Enhanced\KOEI_Classic_Enhanced.exe" (
  start "" "%~dp0dist\KOEI_Classic_Enhanced\KOEI_Classic_Enhanced.exe" %*
  exit /b 0
)
where py >nul 2>&1
if errorlevel 1 (
  echo Python 3.11+ is required when no bundled executable is installed.
  exit /b 1
)
py -3 -c "import tkinter; from PIL import Image" >nul 2>&1
if errorlevel 1 (
  echo Missing source runtime dependencies. Run: py -3 -m pip install -r requirements.txt
  exit /b 1
)
py -3 -m launcher %*
exit /b %ERRORLEVEL%
