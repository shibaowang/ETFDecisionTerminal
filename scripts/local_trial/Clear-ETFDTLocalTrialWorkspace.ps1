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

function Stop-LocalTrialProcess {
    param(
        [string]$PidFile,
        [string[]]$AllowedProcessNames
    )

    if (-not (Test-Path -LiteralPath $PidFile -PathType Leaf)) {
        return $null
    }

    $rawPid = (Get-Content -LiteralPath $PidFile -Raw).Trim()
    $localPid = 0
    if (-not [int]::TryParse($rawPid, [ref]$localPid)) {
        throw "Invalid local trial pid file: $PidFile"
    }

    $process = Get-Process -Id $localPid -ErrorAction SilentlyContinue
    if ($null -eq $process) {
        return [ordered]@{
            pid = $localPid
            alreadyExited = $true
        }
    }

    $processPath = ""
    try {
        $processPath = [string]$process.Path
    } catch {
        $processPath = ""
    }
    if ([string]::IsNullOrWhiteSpace($processPath) -or
        -not $processPath.StartsWith($repoRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
        throw "Refusing to stop a process outside the repository local trial boundary."
    }
    if ($AllowedProcessNames -notcontains $process.ProcessName) {
        throw "Refusing to stop unexpected process from local trial pid file: $($process.ProcessName)"
    }

    Stop-Process -Id $localPid -Force
    $process.WaitForExit(5000) | Out-Null
    Start-Sleep -Milliseconds 250
    return [ordered]@{
        pid = $localPid
        processName = $process.ProcessName
        stopped = $true
    }
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

$logs = Join-Path $trialRootFull "logs"
$stoppedProcesses = @()
foreach ($item in @(
    @{ PidFile = (Join-Path $logs "dataservice.pid"); Names = @("ETFDataService") },
    @{ PidFile = (Join-Path $logs "shell.pid"); Names = @("ETFDecisionShell") }
)) {
    $stopped = Stop-LocalTrialProcess -PidFile $item.PidFile -AllowedProcessNames $item.Names
    if ($null -ne $stopped) {
        $stoppedProcesses += $stopped
    }
}

if ($PreserveLogs) {
    $archive = Join-Path $repoRoot ".demo\local_trial_rc_logs_preserved"
    if (Test-Path -LiteralPath $logs) {
        New-Item -ItemType Directory -Force -Path $archive | Out-Null
        Copy-Item -Path (Join-Path $logs "*") -Destination $archive -Force -ErrorAction SilentlyContinue
    }
}

$removed = $false
for ($attempt = 1; $attempt -le 10; ++$attempt) {
    try {
        Remove-Item -LiteralPath $trialRootFull -Recurse -Force
        $removed = $true
        break
    } catch {
        if ($attempt -eq 10) {
            throw
        }
        Start-Sleep -Milliseconds 250
    }
}
[ordered]@{
    task = "EPIC-282"
    cleanupCompleted = $removed
    trialRoot = $trialRootFull
    preservedLogs = [bool]$PreserveLogs
    stoppedLocalProcesses = $stoppedProcesses
    remoteAccess = $false
    realExecution = $false
} | ConvertTo-Json -Depth 5
