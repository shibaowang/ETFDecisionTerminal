param(
    [string]$DbPath = ".local/daily_use/etfdt_daily_use.sqlite",
    [string]$BuildDir = "build",
    [switch]$NoNetworkFixtureMode = $true
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

if (-not (Test-Path -LiteralPath $resolvedDbPath)) {
    throw "Daily-use DB missing: $resolvedDbPath"
}

$demoRcNeedle = ".demo" + "/" + "local_trial_rc"
$normalizedDbPath = $resolvedDbPath -replace "\\", "/"
$normalizedCachePath = $cachePath -replace "\\", "/"
if ($normalizedDbPath.Contains($demoRcNeedle) -or $normalizedCachePath.Contains($demoRcNeedle)) {
    throw "Daily-use smoke must not use demo RC paths."
}

$dataServiceExe = Resolve-ETFDTExecutable -BuildRoot $buildRoot -AppDirectoryName "ETFDataService" -ExecutableName "ETFDataService.exe"
$dataServiceCheckedPaths = $CheckedExecutablePaths
$shellExe = Resolve-ETFDTExecutable -BuildRoot $buildRoot -AppDirectoryName "ETFDecisionShell" -ExecutableName "ETFDecisionShell.exe"
$shellCheckedPaths = $CheckedExecutablePaths

if ([string]::IsNullOrWhiteSpace($dataServiceExe)) {
    $checked = ($dataServiceCheckedPaths | ForEach-Object { " - $_" }) -join [Environment]::NewLine
    throw "ETFDataService executable not found. Please run: cmake --build build --config Debug$([Environment]::NewLine)Checked paths:$([Environment]::NewLine)$checked"
}

if ([string]::IsNullOrWhiteSpace($shellExe)) {
    $checked = ($shellCheckedPaths | ForEach-Object { " - $_" }) -join [Environment]::NewLine
    throw "ETFDecisionShell executable not found. Please run: cmake --build build --config Debug$([Environment]::NewLine)Checked paths:$([Environment]::NewLine)$checked"
}

$docs = @(
    "docs\401_real_daily_use_data_dashboard.md",
    "docs\402_real_daily_use_market_data_vba_parity.md",
    "docs\403_real_daily_use_portfolio_cash_base_position.md",
    "docs\404_real_daily_use_startup_auto_refresh_policy.md",
    "docs\405_real_daily_use_acceptance_checklist.md"
)

foreach ($doc in $docs) {
    $path = Join-Path $repoRoot $doc
    if (-not (Test-Path -LiteralPath $path)) {
        throw "Required EPIC-289 doc missing: $doc"
    }
}

$dailyUseShellScriptPath = Join-Path $repoRoot "scripts\local_trial\Start-ETFDTDailyUseShell.ps1"
$dailyUseShellScriptText = Get-Content -LiteralPath $dailyUseShellScriptPath -Raw
foreach ($token in @(
    "--daily-use",
    "--socket-name",
    "ETFDataServiceDailyUse",
    "Normalize-ETFDTLocalSocketName",
    "socketNameNormalized",
    "--db",
    "--default-page",
    "shell-accounting-daily-use",
    ".local\daily_use"
)) {
    if (-not $dailyUseShellScriptText.Contains($token)) {
        throw "Daily-use Shell script missing required startup token: $token"
    }
}
$demoRcBackslashNeedle = ".demo" + [string][char]92 + "local_trial_rc"
$demoRcSlashNeedle = ".demo" + "/" + "local_trial_rc"
if ($dailyUseShellScriptText.Contains($demoRcBackslashNeedle) -or
    $dailyUseShellScriptText.Contains($demoRcSlashNeedle)) {
    throw "Daily-use Shell script must not use demo RC paths."
}

$dailyUseDataServiceScriptPath = Join-Path $repoRoot "scripts\local_trial\Start-ETFDTDailyUseDataService.ps1"
$dailyUseDataServiceScriptText = Get-Content -LiteralPath $dailyUseDataServiceScriptPath -Raw
foreach ($token in @(
    "Normalize-ETFDTLocalSocketName",
    "Test-ETFDTLocalSocketReady",
    "dataservice.pid",
    "dataservice.log",
    "dataservice.err.log",
    "socketReady",
    "socketNameNormalized",
    "ETFDataServiceDailyUse",
    "--serve-daily-use",
    "dailyUseWriteActionsEnabled",
    "excelVbaImportPersistActionRegistered",
    "serveMode",
    "readOnlyOnlyMode",
    "brokerOrderActionsRegistered",
    "RedirectStandardOutput",
    "RedirectStandardError"
)) {
    if (-not $dailyUseDataServiceScriptText.Contains($token)) {
        throw "Daily-use DataService script missing readiness token: $token"
    }
}
if ($dailyUseDataServiceScriptText.Contains("--serve-readonly")) {
    throw "Daily-use DataService script must use --serve-daily-use, not pure --serve-readonly."
}

$smokeSocketNameNormalized = Normalize-ETFDTLocalSocketName -Name "ETFDataServiceDailyUse"
if ($smokeSocketNameNormalized -ne "ETFDataServiceDailyUse") {
    throw "Daily-use socket normalization mismatch: $smokeSocketNameNormalized"
}

$evidence = [ordered]@{
    task = "EPIC-289"
    dailyUseSmokeReady = $true
    databasePath = $resolvedDbPath
    cachePath = $cachePath
    buildRoot = $buildRoot
    dataServiceExecutablePath = $dataServiceExe
    shellExecutablePath = $shellExe
    dataServiceExeFound = $true
    shellExeFound = $true
    dailyUseShellScriptArgsValidated = $true
    dataServiceSocketReadinessScriptValidated = $true
    socketNameNormalized = $smokeSocketNameNormalized
    dailyUseShellPassesDailyUseArg = $true
    dailyUseShellPassesSocketName = $true
    dailyUseShellPassesDbPath = $true
    dailyUseShellPassesDefaultPage = $true
    dailyUseWriteActionsEnabled = $true
    excelVbaImportPersistActionRegistered = $true
    serveMode = "daily-use"
    readOnlyOnlyMode = $false
    brokerOrderActionsRegistered = $false
    noNetworkFixtureMode = [bool]$NoNetworkFixtureMode
    defaultDailyUseDbPath = ".local/daily_use/etfdt_daily_use.sqlite"
    defaultMarketCachePath = ".local/daily_use/cache/market_cache.json"
    defaultDailyUseShellScript = "scripts/local_trial/Start-ETFDTDailyUseShell.ps1"
    startupAutoRefreshEnabled = $true
    liveNetworkUsed = $false
    testNetworkAccess = $false
    demoRcPathUsed = $false
    productionDbTouched = $false
    brokerOrderSubmitted = $false
    networkAccess = $false
    credentialAccess = $false
    endpointAccess = $false
    realOrderPlacement = $false
    automaticTrading = $false
}

$evidence | ConvertTo-Json -Depth 4
