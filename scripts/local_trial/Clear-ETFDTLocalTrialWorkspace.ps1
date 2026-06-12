[CmdletBinding()]
param(
    [string]$TrialRoot = "",
    [switch]$PreserveLogs
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$repoRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot "..\.."))
if ([string]::IsNullOrWhiteSpace($TrialRoot)) {
    $TrialRoot = Join-Path $repoRoot ".demo\local_trial_rc"
} elseif (-not [System.IO.Path]::IsPathRooted($TrialRoot)) {
    $TrialRoot = Join-Path $repoRoot $TrialRoot
}
$trialRootFull = [System.IO.Path]::GetFullPath($TrialRoot)
if ($trialRootFull -eq [System.IO.Path]::GetPathRoot($trialRootFull)) {
    throw "Refusing to clear a drive root."
}
if (-not (Test-Path -LiteralPath $trialRootFull)) {
    [ordered]@{
        task = "EPIC-282"
        cleanupCompleted = $true
        trialRoot = $trialRootFull
        alreadyAbsent = $true
    } | ConvertTo-Json -Depth 3
    exit 0
}

if ($PreserveLogs) {
    $logs = Join-Path $trialRootFull "logs"
    $archive = Join-Path $repoRoot ".demo\local_trial_rc_logs_preserved"
    if (Test-Path -LiteralPath $logs) {
        New-Item -ItemType Directory -Force -Path $archive | Out-Null
        Copy-Item -Path (Join-Path $logs "*") -Destination $archive -Force -ErrorAction SilentlyContinue
    }
}

Remove-Item -LiteralPath $trialRootFull -Recurse -Force
[ordered]@{
    task = "EPIC-282"
    cleanupCompleted = $true
    trialRoot = $trialRootFull
    preservedLogs = [bool]$PreserveLogs
    remoteAccess = $false
    realExecution = $false
} | ConvertTo-Json -Depth 3
