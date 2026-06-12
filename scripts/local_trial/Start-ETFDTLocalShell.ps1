[CmdletBinding()]
param(
    [string]$BuildDir = "",
    [string]$TrialRoot = "",
    [switch]$PassThru
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$repoRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot "..\.."))
if ([string]::IsNullOrWhiteSpace($BuildDir)) {
    $BuildDir = Join-Path $repoRoot "build"
} elseif (-not [System.IO.Path]::IsPathRooted($BuildDir)) {
    $BuildDir = Join-Path $repoRoot $BuildDir
}
if ([string]::IsNullOrWhiteSpace($TrialRoot)) {
    $TrialRoot = Join-Path $repoRoot ".demo\local_trial_rc"
} elseif (-not [System.IO.Path]::IsPathRooted($TrialRoot)) {
    $TrialRoot = Join-Path $repoRoot $TrialRoot
}

$shellExe = Join-Path ([System.IO.Path]::GetFullPath($BuildDir)) "apps\ETFDecisionShell\Debug\ETFDecisionShell.exe"
if (-not (Test-Path -LiteralPath $shellExe -PathType Leaf)) {
    throw "Shell executable not found: $shellExe"
}
$trialRootFull = [System.IO.Path]::GetFullPath($TrialRoot)
$logDir = Join-Path $trialRootFull "logs"
$pidFile = Join-Path $logDir "shell.pid"
New-Item -ItemType Directory -Force -Path $logDir | Out-Null

$process = Start-Process -FilePath $shellExe -ArgumentList @("--diagnostics-mock") -PassThru
if ($process.HasExited) {
    throw "Shell exited early with code $($process.ExitCode)."
}
Set-Content -LiteralPath $pidFile -Value $process.Id -Encoding ASCII
$summary = [ordered]@{
    task = "EPIC-282"
    shellStarted = $true
    pid = $process.Id
    pidFile = $pidFile
    remoteAccess = $false
    realExecution = $false
}
if ($PassThru) {
    $process
}
$summary | ConvertTo-Json -Depth 4
