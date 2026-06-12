[CmdletBinding()]
param(
    [string]$TrialRoot = "",
    [string]$DbPath = "",
    [string]$SocketName = "ETFDTLocalTrialRc",
    [string]$BuildDir = "",
    [switch]$PassThru
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$repoRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot "..\.."))
if ([string]::IsNullOrWhiteSpace($TrialRoot)) {
    $TrialRoot = Join-Path $repoRoot ".demo\local_trial_rc"
} elseif (-not [System.IO.Path]::IsPathRooted($TrialRoot)) {
    $TrialRoot = Join-Path $repoRoot $TrialRoot
}
if ([string]::IsNullOrWhiteSpace($BuildDir)) {
    $BuildDir = Join-Path $repoRoot "build"
} elseif (-not [System.IO.Path]::IsPathRooted($BuildDir)) {
    $BuildDir = Join-Path $repoRoot $BuildDir
}

$trialRootFull = [System.IO.Path]::GetFullPath($TrialRoot)
$buildDirFull = [System.IO.Path]::GetFullPath($BuildDir)
if ([string]::IsNullOrWhiteSpace($DbPath)) {
    $DbPath = Join-Path $trialRootFull "data\etfdt_local_trial.sqlite"
} elseif (-not [System.IO.Path]::IsPathRooted($DbPath)) {
    $DbPath = Join-Path $repoRoot $DbPath
}
$dbPathFull = [System.IO.Path]::GetFullPath($DbPath)
$serviceExe = Join-Path $buildDirFull "apps\ETFDataService\Debug\ETFDataService.exe"
$logDir = Join-Path $trialRootFull "logs"
$pidFile = Join-Path $logDir "dataservice.pid"

if (-not (Test-Path -LiteralPath $serviceExe -PathType Leaf)) {
    throw "Local service executable not found: $serviceExe"
}
if ($dbPathFull -match "ETFDecision\.db$") {
    throw "Refusing to use the application DB name for local trial."
}
New-Item -ItemType Directory -Force -Path $logDir | Out-Null

$args = @("--serve-dev-audit", "--db", $dbPathFull, "--socket-name", $SocketName)
$process = Start-Process -FilePath $serviceExe -ArgumentList $args -PassThru -WindowStyle Hidden
if ($process.HasExited) {
    throw "Local service exited early with code $($process.ExitCode)."
}
Set-Content -LiteralPath $pidFile -Value $process.Id -Encoding ASCII

$summary = [ordered]@{
    task = "EPIC-282"
    localServiceStarted = $true
    pid = $process.Id
    db = $dbPathFull
    socketName = $SocketName
    pidFile = $pidFile
    remoteAccess = $false
    realExecution = $false
}
if ($PassThru) {
    $process
}
$summary | ConvertTo-Json -Depth 4
