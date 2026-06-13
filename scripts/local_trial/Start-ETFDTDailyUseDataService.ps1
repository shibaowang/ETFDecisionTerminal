param(
    [string]$DbPath = ".local/daily_use/etfdt_daily_use.sqlite",
    [string]$SocketName = "ETFDataServiceDailyUse",
    [switch]$EnablePublicMarketRefresh,
    [switch]$NoNetworkFixtureMode
)

$ErrorActionPreference = "Stop"

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
$resolvedDbPath = Join-Path $repoRoot $DbPath
$cachePath = Join-Path $repoRoot ".local/daily_use/cache/market_cache.json"
$serviceExe = Join-Path $repoRoot "build\apps\ETFDataService\ETFDataService.exe"

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

if (-not (Test-Path -LiteralPath $serviceExe)) {
    throw "ETFDataService executable not found: $serviceExe"
}

Start-Process -FilePath $serviceExe -ArgumentList $args -WindowStyle Hidden
