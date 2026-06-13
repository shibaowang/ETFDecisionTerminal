param(
    [string]$DbPath = ".local/daily_use/etfdt_daily_use.sqlite",
    [string]$BuildDir = "build",
    [string]$SocketName = "ETFDataServiceDailyUse",
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

$serviceExe = Resolve-ETFDTExecutable -BuildRoot $buildRoot -AppDirectoryName "ETFDataService" -ExecutableName "ETFDataService.exe"

if (-not (Test-Path -LiteralPath $resolvedDbPath)) {
    throw "Daily-use DB does not exist. Run New-ETFDTDailyUseWorkspace.ps1 first: $resolvedDbPath"
}

$args = @(
    "--serve-readonly",
    "--db", $resolvedDbPath,
    "--socket-name", $SocketName
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

Write-Host ($evidence | ConvertTo-Json -Depth 4)

if ([string]::IsNullOrWhiteSpace($serviceExe)) {
    $checked = ($CheckedExecutablePaths | ForEach-Object { " - $_" }) -join [Environment]::NewLine
    throw "ETFDataService executable not found. Please run: cmake --build build --config Debug$([Environment]::NewLine)Checked paths:$([Environment]::NewLine)$checked"
}

Start-Process -FilePath $serviceExe -ArgumentList $args -WindowStyle Hidden
