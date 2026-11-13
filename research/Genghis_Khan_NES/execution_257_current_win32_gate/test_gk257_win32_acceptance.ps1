param([Parameter(Mandatory=$true)][string]$Executable)
$ErrorActionPreference = 'Stop'
if (-not (Test-Path -LiteralPath $Executable)) { throw "Native WinMain EXE missing: $Executable" }
$Executable = (Resolve-Path -LiteralPath $Executable).Path
$cwd = Split-Path -Parent $Executable
$trace = Join-Path $env:RUNNER_TEMP 'gk257_winmain_trace.log'
if (Test-Path $trace) { Remove-Item $trace -Force }
$env:GK1_SMOKE_TRACE = $trace
function Invoke-NativeExit([string]$arg, [int]$expected) {
  $p = Start-Process -FilePath $Executable -WorkingDirectory $cwd -ArgumentList $arg -Wait -PassThru
  if ($p.ExitCode -ne $expected) { throw "Native WinMain [$arg] exit $($p.ExitCode), expected $expected" }
}
try {
  # Real Windows GUI WM_PAINT path with software-injected XInput mapping only.
  Invoke-NativeExit '--smoke-frames=15' 0
  if (-not (Test-Path $trace)) { throw 'WM_PAINT trace was not produced by native EXE' }
  $content = Get-Content -LiteralPath $trace -Raw
  if (-not $content.Contains('WM_PAINT completed')) { throw 'Native Windows WM_PAINT did not complete' }
  Invoke-NativeExit '--smoke-gamepad-nav' 0
  # Incorrect or excessively long frame arguments must be refused before GUI execution.
  Invoke-NativeExit '--smoke-frames=121' 4
  Invoke-NativeExit '--smoke-frames=4294967297' 4
  Write-Output 'EXEC0257 WinMain GUI rendering / software controller / malformed args PASS; hardware Xbox NOT RUN'
} finally {
  Remove-Item Env:GK1_SMOKE_TRACE -ErrorAction SilentlyContinue
  if (Test-Path $trace) { Remove-Item $trace -Force }
}
