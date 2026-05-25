param(
    [string]$BuildDir = "build",
    [string]$QtRoot = "C:\Qt\6.9.3\msvc2022_64",
    [string]$DbPath = "runtime\dev\readonly_demo\ETFDecision.db",
    [string]$MigrationPath = "migrations\001_initial_schema.sql",
    [string]$SocketName = "ETFDataServiceReadonly",
    [switch]$KeepDataService,
    [switch]$SkipBuild,
    [switch]$NoShell
)

$ErrorActionPreference = "Stop"

function Write-Step {
    param([string]$Message)
    Write-Host "[readonly-demo] $Message"
}

function Resolve-ProjectPath {
    param([string]$Path)
    if ([System.IO.Path]::IsPathRooted($Path)) {
        return $Path
    }
    return (Join-Path $ProjectRoot $Path)
}

function Invoke-Checked {
    param(
        [string]$FilePath,
        [string[]]$Arguments,
        [string]$Description
    )
    Write-Step $Description
    & $FilePath @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "$Description failed with exit code $LASTEXITCODE"
    }
}

$ProjectRoot = Resolve-Path (Join-Path $PSScriptRoot "..\..")
Set-Location $ProjectRoot

$BuildRoot = Resolve-ProjectPath $BuildDir
$QtBin = Join-Path $QtRoot "bin"
$DatabasePath = Resolve-ProjectPath $DbPath
$MigrationFile = Resolve-ProjectPath $MigrationPath
$DataServiceExe = Join-Path $BuildRoot "apps\ETFDataService\Debug\ETFDataService.exe"
$ShellExe = Join-Path $BuildRoot "apps\ETFDecisionShell\Debug\ETFDecisionShell.exe"

Write-Step "Development-only read-only Shell demo."
Write-Step "This script initializes a demo database, starts DataService in read-only mode, and opens the Shell."
Write-Step "The Shell page is read-only: no trading, no accounting entry, and no write actions."

if (!(Test-Path $MigrationFile)) {
    throw "Migration file not found: $MigrationFile"
}

if (Test-Path $QtBin) {
    $env:PATH = "$QtBin;$env:PATH"
    Write-Step "Added Qt bin to PATH for this process: $QtBin"
} else {
    Write-Step "Qt bin path not found; continuing with current PATH: $QtBin"
}

if (!$SkipBuild) {
    Invoke-Checked -FilePath "cmake" -Arguments @(
        "-S", ".",
        "-B", $BuildRoot,
        "-DETFDT_QT6_ROOT=$QtRoot"
    ) -Description "Configuring project"
    Invoke-Checked -FilePath "cmake" -Arguments @(
        "--build", $BuildRoot
    ) -Description "Building project"
}

if (!(Test-Path $DataServiceExe)) {
    throw "ETFDataService executable not found: $DataServiceExe"
}
if (!$NoShell -and !(Test-Path $ShellExe)) {
    throw "ETFDecisionShell executable not found: $ShellExe"
}

$DatabaseDir = Split-Path -Parent $DatabasePath
New-Item -ItemType Directory -Force -Path $DatabaseDir | Out-Null

Invoke-Checked -FilePath $DataServiceExe -Arguments @(
    "--init-db",
    "--db", $DatabasePath,
    "--migration", $MigrationFile
) -Description "Initializing demo database"

Invoke-Checked -FilePath $DataServiceExe -Arguments @(
    "--check-db",
    "--db", $DatabasePath
) -Description "Checking demo database"

$dataServiceArgs = @(
    "--serve-readonly",
    "--db", $DatabasePath,
    "--socket-name", $SocketName
)

Write-Step "Starting ETFDataService read-only host on socket '$SocketName'"
$dataServiceProcess = Start-Process -FilePath $DataServiceExe -ArgumentList $dataServiceArgs -PassThru -WindowStyle Hidden
Start-Sleep -Seconds 2
if ($dataServiceProcess.HasExited) {
    throw "ETFDataService exited early with code $($dataServiceProcess.ExitCode)"
}

try {
    Write-Host ""
    Write-Step "Manual acceptance steps:"
    Write-Host "  1. In the Shell, open the left navigation item: 只读数据预览"
    Write-Host "  2. Select preset: DataService read-only service"
    Write-Host "  3. Confirm socketName: $SocketName"
    Write-Host "  4. Click Connect"
    Write-Host "  5. Click Refresh All"
    Write-Host "  6. Confirm default account, default portfolio, and CASH instrument are visible"
    Write-Host ""
    Write-Step "This demo uses database: $DatabasePath"
    Write-Step "Close the Shell or press Enter in this window to stop the demo."

    if (!$NoShell) {
        Write-Step "Starting ETFDecisionShell"
        $shellProcess = Start-Process -FilePath $ShellExe -ArgumentList @("--diagnostics-mock") -PassThru
        while (!$shellProcess.HasExited) {
            Start-Sleep -Milliseconds 500
            if ([Console]::KeyAvailable) {
                [void][Console]::ReadLine()
                break
            }
        }
    } else {
        [void](Read-Host "Press Enter to stop the read-only DataService demo")
    }
} finally {
    if (!$KeepDataService -and $dataServiceProcess -and !$dataServiceProcess.HasExited) {
        Write-Step "Stopping ETFDataService read-only host"
        $dataServiceProcess.CloseMainWindow() | Out-Null
        if (!$dataServiceProcess.WaitForExit(3000)) {
            $dataServiceProcess.Kill()
            $dataServiceProcess.WaitForExit()
        }
    } elseif ($KeepDataService) {
        Write-Step "Keeping ETFDataService running. PID: $($dataServiceProcess.Id)"
    }
}

Write-Step "Readonly demo finished."
