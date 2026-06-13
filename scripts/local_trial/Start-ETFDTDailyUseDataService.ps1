param(
    [string]$DbPath = ".local/daily_use/etfdt_daily_use.sqlite",
    [string]$BuildDir = "build",
    [string]$SocketName = "ETFDataServiceDailyUse",
    [int]$ReadyTimeoutSeconds = 10,
    [switch]$EnablePublicMarketRefresh,
    [switch]$NoNetworkFixtureMode
)

$ErrorActionPreference = "Stop"

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
$buildRoot = if ([System.IO.Path]::IsPathRooted($BuildDir)) {
    [System.IO.Path]::GetFullPath($BuildDir)
} else {
    [System.IO.Path]::GetFullPath((Join-Path $repoRoot $BuildDir))
}
$resolvedDbPath = Join-Path $repoRoot $DbPath
$cachePath = Join-Path $repoRoot ".local/daily_use/cache/market_cache.json"
$dailyUseRoot = Join-Path $repoRoot ".local\daily_use"
$logsPath = Join-Path $dailyUseRoot "logs"
$pidFile = Join-Path $logsPath "dataservice.pid"
$logPath = Join-Path $logsPath "dataservice.log"
$errorLogPath = Join-Path $logsPath "dataservice.err.log"

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

function Start-ETFDTDetachedProcess {
    param(
        [string]$FilePath,
        [string[]]$ArgumentList,
        [string]$StandardOutputPath,
        [string]$StandardErrorPath
    )

    # Keep stdout/stderr redirected off the caller's console pipe so the startup
    # script can return while the daily-use service keeps running.
    $psi = [System.Diagnostics.ProcessStartInfo]::new()
    $psi.FileName = $FilePath
    $psi.Arguments = ($ArgumentList | ForEach-Object { ConvertTo-ETFDTCommandLineArgument -Value $_ }) -join " "
    $psi.UseShellExecute = $false
    $psi.CreateNoWindow = $true
    $psi.RedirectStandardOutput = $true
    $psi.RedirectStandardError = $true

    Set-Content -LiteralPath $StandardOutputPath -Value "" -Encoding UTF8
    Set-Content -LiteralPath $StandardErrorPath -Value "" -Encoding UTF8

    return [System.Diagnostics.Process]::Start($psi)
}

$serviceExe = Resolve-ETFDTExecutable -BuildRoot $buildRoot -AppDirectoryName "ETFDataService" -ExecutableName "ETFDataService.exe"
$socketNameNormalized = Normalize-ETFDTLocalSocketName -Name $SocketName

if (-not (Test-Path -LiteralPath $resolvedDbPath)) {
    throw "Daily-use DB does not exist. Run New-ETFDTDailyUseWorkspace.ps1 first: $resolvedDbPath"
}

New-Item -ItemType Directory -Force -Path $logsPath | Out-Null

if ([string]::IsNullOrWhiteSpace($serviceExe)) {
    $checked = ($CheckedExecutablePaths | ForEach-Object { " - $_" }) -join [Environment]::NewLine
    throw "ETFDataService executable not found. Please run: cmake --build build --config Debug$([Environment]::NewLine)Checked paths:$([Environment]::NewLine)$checked"
}

$migrationPath = Join-Path $repoRoot "migrations\001_initial_schema.sql"
if (-not (Test-Path -LiteralPath $migrationPath -PathType Leaf)) {
    throw "Daily-use DB migration file not found: $migrationPath"
}

$schemaInitialized = $false
$previousErrorActionPreference = $ErrorActionPreference
$ErrorActionPreference = "Continue"
try {
    $schemaCheckOutput = & $serviceExe --check-db --db $resolvedDbPath 2>&1
    $schemaCheckExitCode = $LASTEXITCODE
} finally {
    $ErrorActionPreference = $previousErrorActionPreference
}
if ($schemaCheckExitCode -ne 0) {
    $schemaCheckText = ($schemaCheckOutput | Out-String)
    $canInitializeSchema =
        ($schemaCheckText -match "schema_version table is missing") -or
        ($schemaCheckText -match "001_initial_schema migration is missing") -or
        ((Get-Item -LiteralPath $resolvedDbPath).Length -eq 0)
    if (-not $canInitializeSchema) {
        throw "Daily-use DB health check failed before service startup: $schemaCheckText"
    }

    $previousErrorActionPreference = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    try {
        $initOutput = & $serviceExe --init-db --db $resolvedDbPath --migration $migrationPath 2>&1
        $initExitCode = $LASTEXITCODE
    } finally {
        $ErrorActionPreference = $previousErrorActionPreference
    }
    if ($initExitCode -ne 0) {
        $initText = ($initOutput | Out-String)
        throw "Daily-use DB initialization failed: $initText"
    }
    $schemaInitialized = $true
}

$args = @(
    "--serve-readonly",
    "--db", $resolvedDbPath,
    "--socket-name", $socketNameNormalized
)

if ($EnablePublicMarketRefresh -and -not $NoNetworkFixtureMode) {
    $args += "--daily-use-public-market-refresh"
}

$evidence = [ordered]@{
    task = "EPIC-289"
    command = "ETFDataService " + ($args -join " ")
    databasePath = $resolvedDbPath
    cachePath = $cachePath
    buildRoot = $buildRoot
    executablePath = $serviceExe
    socketName = $SocketName
    socketNameNormalized = $socketNameNormalized
    pidFile = $pidFile
    logPath = $logPath
    errorLogPath = $errorLogPath
    migrationPath = $migrationPath
    schemaInitialized = $schemaInitialized
    readyTimeoutSeconds = $ReadyTimeoutSeconds
    startupAutoRefreshEnabled = [bool]$EnablePublicMarketRefresh
    publicMarketRefreshRequested = [bool]$EnablePublicMarketRefresh
    noNetworkFixtureMode = [bool]$NoNetworkFixtureMode
    liveNetworkUsed = ([bool]$EnablePublicMarketRefresh -and -not [bool]$NoNetworkFixtureMode)
    testNetworkAccess = $false
    brokerOrderSubmitted = $false
    networkAccess = ([bool]$EnablePublicMarketRefresh -and -not [bool]$NoNetworkFixtureMode)
    credentialAccess = $false
    endpointAccess = $false
    automaticTrading = $false
}

Write-Output ($evidence | ConvertTo-Json -Depth 4)

$process = Start-ETFDTDetachedProcess `
    -FilePath $serviceExe `
    -ArgumentList $args `
    -StandardOutputPath $logPath `
    -StandardErrorPath $errorLogPath

Set-Content -LiteralPath $pidFile -Value $process.Id -Encoding ASCII

$socketReady = $false
$processExited = $false
$exitCode = $null
$deadline = (Get-Date).AddSeconds($ReadyTimeoutSeconds)
while ((Get-Date) -lt $deadline) {
    Start-Sleep -Milliseconds 250
    $process.Refresh()
    if ($process.HasExited) {
        $processExited = $true
        $exitCode = $process.ExitCode
        break
    }
    if (Test-ETFDTLocalSocketReady -Name $socketNameNormalized) {
        $socketReady = $true
        break
    }
}

if (-not $socketReady -and -not $processExited) {
    $process.Refresh()
    $processExited = $process.HasExited
    if ($processExited) {
        $exitCode = $process.ExitCode
    }
}

$readyEvidence = [ordered]@{
    task = "EPIC-289-FIX4"
    dataServiceStarted = (-not $processExited)
    socketReady = $socketReady
    socketName = $SocketName
    socketNameNormalized = $socketNameNormalized
    pid = $process.Id
    pidFile = $pidFile
    processExited = $processExited
    exitCode = $exitCode
    logPath = $logPath
    errorLogPath = $errorLogPath
    checkedSocketName = $socketNameNormalized
}

Write-Output ($readyEvidence | ConvertTo-Json -Depth 4)

if (-not $socketReady) {
    throw "Daily-use DataService socket is not ready. checkedSocketName=$socketNameNormalized logPath=$logPath errorLogPath=$errorLogPath exitCode=$exitCode"
}
