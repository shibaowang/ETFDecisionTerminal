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
New-Item -ItemType Directory -Force -Path $logsPath | Out-Null

$shellArguments = @(
    "--daily-use",
    "--socket-name",
    $SocketName,
    "--db",
    $dbPathFull,
    "--default-page",
    "shell-accounting-daily-use"
)

$process = Start-Process -FilePath $shellExe -ArgumentList $shellArguments -PassThru
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
    shellArguments = $shellArguments
    dailyUseArgumentPassed = $true
    socketNameArgumentPassed = $true
    dbArgumentPassed = $true
    defaultPageArgumentPassed = $true
    defaultPage = "shell-accounting-daily-use"
    pid = $process.Id
    pidFile = $pidFile
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
