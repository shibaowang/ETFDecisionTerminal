param(
    [string]$BuildDir = "build",
    [string]$QtRoot = "C:\Qt\6.9.3\msvc2022_64",
    [string]$DbPath = "runtime\dev\readonly_demo\ETFDecision.db",
    [string]$MigrationPath = "migrations\001_initial_schema.sql",
    [string]$SocketName = "ETFDataServiceReadonly",
    [switch]$KeepDataService,
    [switch]$SkipBuild,
    [switch]$NoShell,
    [switch]$ForceRecreateDb,
    [bool]$WritePidFile = $true
)

$ErrorActionPreference = "Stop"

$ProjectRoot = $null
$DataServiceProcess = $null
$PidFile = $null

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

function Remove-DemoDatabaseFiles {
    param([string]$DatabasePath)
    foreach ($path in @($DatabasePath, "$DatabasePath-wal", "$DatabasePath-shm")) {
        if (Test-Path $path) {
            Remove-Item -LiteralPath $path -Force
            Write-Step "Removed old demo database file: $path"
        }
    }
}

function Stop-StartedDataService {
    param(
        [System.Diagnostics.Process]$Process,
        [string]$PidFilePath
    )
    if ($null -eq $Process) {
        return
    }
    if ($Process.HasExited) {
        Write-Step "DataService process already exited. PID: $($Process.Id)"
    } else {
        Write-Step "Stopping DataService process started by this script. PID: $($Process.Id)"
        [void]$Process.CloseMainWindow()
        if (!$Process.WaitForExit(3000)) {
            Write-Step "DataService did not exit after terminate request; killing PID $($Process.Id)"
            $Process.Kill()
            $Process.WaitForExit()
        }
    }
    if (![string]::IsNullOrWhiteSpace($PidFilePath) -and (Test-Path $PidFilePath)) {
        Remove-Item -LiteralPath $PidFilePath -Force
        Write-Step "Removed pid file: $PidFilePath"
    }
}

try {
    $ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
    Set-Location $ProjectRoot

    $BuildRoot = Resolve-ProjectPath $BuildDir
    $QtBin = Join-Path $QtRoot "bin"
    $DatabasePath = Resolve-ProjectPath $DbPath
    $MigrationFile = Resolve-ProjectPath $MigrationPath
    $DataServiceExe = Join-Path $BuildRoot "apps\ETFDataService\Debug\ETFDataService.exe"
    $ShellExe = Join-Path $BuildRoot "apps\ETFDecisionShell\Debug\ETFDecisionShell.exe"
    $DatabaseDir = Split-Path -Parent $DatabasePath
    $PidFile = Join-Path $DatabaseDir "$SocketName.pid"

    Write-Step "Development-only read-only Shell demo."
    Write-Step "This script initializes a demo database, starts DataService in read-only mode, and opens the Shell."
    Write-Step "The Shell page is read-only: no trading, no accounting entry, and no write actions."
    Write-Step "Project root: $ProjectRoot"

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

    New-Item -ItemType Directory -Force -Path $DatabaseDir | Out-Null

    if ($ForceRecreateDb) {
        Remove-DemoDatabaseFiles -DatabasePath $DatabasePath
    } elseif (Test-Path $DatabasePath) {
        Write-Step "Reusing existing demo database: $DatabasePath"
        Write-Step "Pass -ForceRecreateDb to remove the old demo database before initialization."
    }

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
    $DataServiceProcess = Start-Process -FilePath $DataServiceExe -ArgumentList $dataServiceArgs -PassThru -WindowStyle Hidden
    Start-Sleep -Seconds 2
    if ($DataServiceProcess.HasExited) {
        throw "ETFDataService exited early with code $($DataServiceProcess.ExitCode)"
    }

    if ($WritePidFile) {
        Set-Content -LiteralPath $PidFile -Value $DataServiceProcess.Id -Encoding ASCII
        Write-Step "Wrote DataService pid file: $PidFile"
    }

    Write-Host ""
    Write-Step "Manual acceptance steps:"
    Write-Host "  1. In the Shell, open the left navigation item: Read-only data preview"
    Write-Host "  2. Select preset: DataService read-only service"
    Write-Host "  3. Confirm socketName: $SocketName"
    Write-Host "  4. Click Connect"
    Write-Host "  5. Click Refresh All"
    Write-Host "  6. Confirm default account, default portfolio, and CASH instrument are visible"
    Write-Host ""
    Write-Step "This demo uses database: $DatabasePath"
    Write-Step "This demo does not trade, account, or write business tables."

    if (!$NoShell) {
        Write-Step "Starting ETFDecisionShell"
        $shellProcess = Start-Process -FilePath $ShellExe -ArgumentList @("--diagnostics-mock") -PassThru
        Start-Sleep -Seconds 1
        if ($shellProcess.HasExited) {
            throw "ETFDecisionShell exited early with code $($shellProcess.ExitCode)"
        }
        Write-Step "Close the Shell window to finish the demo, or press Ctrl+C in this console."
        while (!$shellProcess.HasExited) {
            Start-Sleep -Milliseconds 500
        }
    } else {
        [void](Read-Host "Press Enter to stop the read-only DataService demo")
    }
} catch {
    Write-Host "[readonly-demo] ERROR: $($_.Exception.Message)" -ForegroundColor Red
    exit 1
} finally {
    if (!$KeepDataService) {
        Stop-StartedDataService -Process $DataServiceProcess -PidFilePath $PidFile
    } elseif ($null -ne $DataServiceProcess) {
        Write-Step "Keeping ETFDataService running. PID: $($DataServiceProcess.Id)"
        if ($WritePidFile -and ![string]::IsNullOrWhiteSpace($PidFile)) {
            Write-Step "Use tools/dev/stop_readonly_demo.ps1 to stop the pid recorded at: $PidFile"
        }
    }
}

Write-Step "Readonly demo finished."
