param(
    [string]$PidFile = "runtime\dev\readonly_demo\ETFDataServiceReadonly.pid",
    [switch]$ForceAll
)

$ErrorActionPreference = "Stop"

function Write-Step {
    param([string]$Message)
    Write-Host "[readonly-demo-stop] $Message"
}

$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
Set-Location $ProjectRoot

if (![System.IO.Path]::IsPathRooted($PidFile)) {
    $PidFile = Join-Path $ProjectRoot $PidFile
}

if (Test-Path $PidFile) {
    $pidText = (Get-Content -LiteralPath $PidFile -ErrorAction Stop | Select-Object -First 1).Trim()
    $processId = 0
    if (![int]::TryParse($pidText, [ref]$processId)) {
        throw "Invalid pid file content: $PidFile"
    }

    $process = Get-Process -Id $processId -ErrorAction SilentlyContinue
    if ($null -eq $process) {
        Write-Step "No process exists for pid $processId; removing stale pid file."
    } elseif ($process.ProcessName -ne "ETFDataService") {
        Write-Step "Pid $processId is '$($process.ProcessName)', not ETFDataService. Leaving the process running."
    } else {
        Write-Step "Stopping DataService demo process from pid file. PID: $processId"
        Stop-Process -Id $processId -Force
    }
    Remove-Item -LiteralPath $PidFile -Force
    Write-Step "Removed pid file: $PidFile"
    exit 0
}

if (!$ForceAll) {
    Write-Step "Pid file not found: $PidFile"
    Write-Step "No process was stopped. Pass -ForceAll only when you explicitly want to stop every ETFDataService process."
    exit 0
}

Write-Step "ForceAll requested. Stopping all ETFDataService processes."
$processes = Get-Process -Name "ETFDataService" -ErrorAction SilentlyContinue
if ($null -eq $processes) {
    Write-Step "No ETFDataService process found."
    exit 0
}

foreach ($process in $processes) {
    Write-Step "Stopping ETFDataService PID: $($process.Id)"
    Stop-Process -Id $process.Id -Force
}
