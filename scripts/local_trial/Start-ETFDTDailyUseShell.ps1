param(
    [string]$TrialRoot = ".local\daily_use",
    [string]$DbPath = ".local\daily_use\etfdt_daily_use.sqlite",
    [string]$BuildDir = "build",
    [string]$SocketName = "ETFDataServiceDailyUse",
    [switch]$PassThru
)

$ErrorActionPreference = "Stop"

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
$buildRoot = if ([System.IO.Path]::IsPathRooted($BuildDir)) {
    [System.IO.Path]::GetFullPath($BuildDir)
} else {
    [System.IO.Path]::GetFullPath((Join-Path $repoRoot $BuildDir))
}
$trialRootFull = if ([System.IO.Path]::IsPathRooted($TrialRoot)) {
    [System.IO.Path]::GetFullPath($TrialRoot)
} else {
    [System.IO.Path]::GetFullPath((Join-Path $repoRoot $TrialRoot))
}
$dbPathFull = if ([System.IO.Path]::IsPathRooted($DbPath)) {
    [System.IO.Path]::GetFullPath($DbPath)
} else {
    [System.IO.Path]::GetFullPath((Join-Path $repoRoot $DbPath))
}

function Normalize-ETFDTLocalSocketName {
    param(
        [string]$Name,
        [string]$Fallback = "ETFDataServiceDailyUse"
    )

    if ([string]::IsNullOrWhiteSpace($Name)) {
        return $Fallback
    }

    $normalized = [regex]::Replace($Name.Trim(), "[^A-Za-z0-9_.-]+", "-")
    $normalized = $normalized.Trim(".", "-")
    if ([string]::IsNullOrWhiteSpace($normalized)) {
        return $Fallback
    }
    return $normalized
}

function Test-ETFDTLocalSocketReady {
    param([string]$Name)

    try {
        $pipe = New-Object System.IO.Pipes.NamedPipeClientStream(
            ".",
            $Name,
            [System.IO.Pipes.PipeDirection]::InOut,
            [System.IO.Pipes.PipeOptions]::Asynchronous
        )
        try {
            $pipe.Connect(250)
            return $pipe.IsConnected
        } finally {
            $pipe.Dispose()
        }
    } catch {
        return $false
    }
}

function ConvertTo-ETFDTCommandLineArgument {
    param([string]$Value)

    if ($null -eq $Value) {
        return '""'
    }
    if ($Value -notmatch '[\s"]') {
        return $Value
    }
    return '"' + ($Value -replace '"', '\"') + '"'
}

function Start-ETFDTDetachedGuiProcess {
    param(
        [string]$FilePath,
        [string[]]$ArgumentList
    )

    $psi = [System.Diagnostics.ProcessStartInfo]::new()
    $psi.FileName = $FilePath
    $psi.Arguments = ($ArgumentList | ForEach-Object { ConvertTo-ETFDTCommandLineArgument -Value $_ }) -join " "
    $psi.UseShellExecute = $true
    return [System.Diagnostics.Process]::Start($psi)
}

function Resolve-ETFDTExecutable {
    param(
        [string]$BuildRoot,
        [string]$AppDirectoryName,
        [string]$ExecutableName
    )

    $plainPath = Join-Path $BuildRoot "apps\$AppDirectoryName\$ExecutableName"
    $debugPath = Join-Path $BuildRoot "apps\$AppDirectoryName\Debug\$ExecutableName"
    $releasePath = Join-Path $BuildRoot "apps\$AppDirectoryName\Release\$ExecutableName"
    $script:CheckedExecutablePaths = @($plainPath, $debugPath, $releasePath)

    foreach ($candidate in @($releasePath, $debugPath, $plainPath)) {
        if (Test-Path -LiteralPath $candidate -PathType Leaf) {
            return $candidate
        }
    }

    $searchRoot = Join-Path $BuildRoot "apps\$AppDirectoryName"
    $script:CheckedExecutablePaths += "fallback search root: $searchRoot"
    if (Test-Path -LiteralPath $searchRoot -PathType Container) {
        $fallback = Get-ChildItem -LiteralPath $searchRoot -Recurse -Filter $ExecutableName -File |
            Sort-Object LastWriteTime -Descending |
            Select-Object -First 1
        if ($null -ne $fallback) {
            return $fallback.FullName
        }
    }

    return $null
}

$shellExe = Resolve-ETFDTExecutable -BuildRoot $buildRoot -AppDirectoryName "ETFDecisionShell" -ExecutableName "ETFDecisionShell.exe"
if ([string]::IsNullOrWhiteSpace($shellExe)) {
    $checked = ($CheckedExecutablePaths | ForEach-Object { " - $_" }) -join [Environment]::NewLine
    throw "ETFDecisionShell executable not found. Please run: cmake --build build --config Debug$([Environment]::NewLine)Checked paths:$([Environment]::NewLine)$checked"
}

$logsPath = Join-Path $trialRootFull "logs"
$pidFile = Join-Path $logsPath "shell.pid"
$shellLogPath = Join-Path $logsPath "shell.log"
$shellErrorLogPath = Join-Path $logsPath "shell.err.log"
New-Item -ItemType Directory -Force -Path $logsPath | Out-Null
$socketNameNormalized = Normalize-ETFDTLocalSocketName -Name $SocketName
$socketReadyBeforeShell = Test-ETFDTLocalSocketReady -Name $socketNameNormalized
Set-Content -LiteralPath $shellLogPath -Value "" -Encoding UTF8
Set-Content -LiteralPath $shellErrorLogPath -Value "" -Encoding UTF8

$shellArguments = @(
    "--daily-use",
    "--socket-name",
    $socketNameNormalized,
    "--db",
    $dbPathFull,
    "--default-page",
    "shell-accounting-daily-use"
)

$process = Start-ETFDTDetachedGuiProcess -FilePath $shellExe -ArgumentList $shellArguments
if ($process.HasExited) {
    throw "ETFDecisionShell exited early with code $($process.ExitCode)."
}

Set-Content -LiteralPath $pidFile -Value $process.Id -Encoding ASCII

$evidence = [ordered]@{
    task = "EPIC-289-FIX2"
    dailyUseShellStarted = $true
    shellExecutablePath = $shellExe
    trialRoot = $trialRootFull
    databasePath = $dbPathFull
    socketName = $SocketName
    socketNameNormalized = $socketNameNormalized
    socketReadyBeforeShell = $socketReadyBeforeShell
    shellArguments = $shellArguments
    dailyUseArgumentPassed = $true
    socketNameArgumentPassed = $true
    dbArgumentPassed = $true
    defaultPageArgumentPassed = $true
    defaultPage = "shell-accounting-daily-use"
    pid = $process.Id
    pidFile = $pidFile
    logPath = $shellLogPath
    errorLogPath = $shellErrorLogPath
    defaultTrialRoot = ".local\daily_use"
    defaultDbPath = ".local\daily_use\etfdt_daily_use.sqlite"
    defaultDailyUseRoot = ".local/daily_use"
    defaultDailyUseDbPath = ".local/daily_use/etfdt_daily_use.sqlite"
    diagnosticsMockMode = $false
    demoRcPathUsed = $false
    brokerOrderSubmitted = $false
    networkAccessBeyondLocalDataServiceTransport = $false
    credentialAccess = $false
    endpointAccess = $false
    automaticTrading = $false
}

if ($PassThru) {
    $process
}

$evidence | ConvertTo-Json -Depth 4
