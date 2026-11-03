@echo off
setlocal EnableExtensions
cd /d "%~dp0"
where py >nul 2>&1
if errorlevel 1 (echo Python 3.11+ missing&exit /b 1)
py -3 -m pip install -r requirements-windows-build.txt
if errorlevel 1 exit /b 1
py -3 tools\bundle_mesen.py
if errorlevel 1 exit /b 1
py -3 -m PyInstaller --noconfirm --clean --onedir --windowed --name KOEI_Classic_Enhanced --add-data "third_branch\assets;third_branch\assets" --add-data "third_branch\data;third_branch\data" --add-data "config;config" --add-data "third_party;third_party" --collect-all PIL --hidden-import PIL.ImageTk --hidden-import tkinter --hidden-import third_branch.strategic_game windows_entry.py
if errorlevel 1 exit /b 1
echo Built dist\KOEI_Classic_Enhanced\KOEI_Classic_Enhanced.exe
exit /b 0
