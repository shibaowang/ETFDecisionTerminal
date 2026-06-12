[CmdletBinding()]
param(
    [string]$BuildDir = "",
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

$shellExe = Join-Path ([System.IO.Path]::GetFullPath($BuildDir)) "apps\ETFDecisionShell\Debug\ETFDecisionShell.exe"
if (-not (Test-Path -LiteralPath $shellExe -PathType Leaf)) {
    throw "Shell executable not found: $shellExe"
}

$process = Start-Process -FilePath $shellExe -ArgumentList @("--diagnostics-mock") -PassThru
if ($process.HasExited) {
    throw "Shell exited early with code $($process.ExitCode)."
}
$summary = [ordered]@{
    task = "EPIC-282"
    shellStarted = $true
    pid = $process.Id
    remoteAccess = $false
    realExecution = $false
}
if ($PassThru) {
    $process
}
$summary | ConvertTo-Json -Depth 4
