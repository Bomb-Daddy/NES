@echo off
setlocal EnableExtensions
cd /d "%~dp0"
set "PACKAGE=dist\KOEI_Classic_Enhanced"
if not exist "%PACKAGE%\KOEI_Classic_Enhanced.exe" (
  echo Build or obtain the complete native Windows package first.
  echo Run BUILD_WINDOWS_EXE.bat on Windows 10 or 11 to package Python, Pillow, Tk and game assets.
  exit /b 1
)
if not defined LOCALAPPDATA set "LOCALAPPDATA=%USERPROFILE%\AppData\Local"
set "DEST=%LOCALAPPDATA%\Programs\KOEI_Classic_Enhanced"
if not exist "%DEST%" mkdir "%DEST%"
robocopy "%PACKAGE%" "%DEST%" /MIR /NFL /NDL /NJH /NJS
if errorlevel 8 exit /b 1
powershell -NoProfile -Command "$x=(New-Object -ComObject WScript.Shell).CreateShortcut([IO.Path]::Combine([Environment]::GetFolderPath('Desktop'),'KOEI Classic Enhanced.lnk'));$x.TargetPath=[IO.Path]::Combine($env:LOCALAPPDATA,'Programs\KOEI_Classic_Enhanced\KOEI_Classic_Enhanced.exe');$x.WorkingDirectory=[IO.Path]::Combine($env:LOCALAPPDATA,'Programs\KOEI_Classic_Enhanced');$x.Save()"
if errorlevel 1 exit /b 1
echo Installed to %DEST%
exit /b 0
