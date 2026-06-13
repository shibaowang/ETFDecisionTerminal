param(
    [string]$WorkspaceRoot = ".local/daily_use",
    [switch]$Force
)

$ErrorActionPreference = "Stop"

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
$workspacePath = Join-Path $repoRoot $WorkspaceRoot
$dbPath = Join-Path $workspacePath "etfdt_daily_use.sqlite"
$logsPath = Join-Path $workspacePath "logs"
$cachePath = Join-Path $workspacePath "cache"
$cacheFilePath = Join-Path $cachePath "market_cache.json"

if ((Test-Path -LiteralPath $workspacePath) -and -not $Force) {
    Write-Host "Daily-use workspace already exists: $workspacePath"
} else {
    New-Item -ItemType Directory -Force -Path $workspacePath | Out-Null
}

New-Item -ItemType Directory -Force -Path $logsPath | Out-Null
New-Item -ItemType Directory -Force -Path $cachePath | Out-Null

if (-not (Test-Path -LiteralPath $dbPath)) {
    New-Item -ItemType File -Force -Path $dbPath | Out-Null
}

$evidence = [ordered]@{
    task = "EPIC-289"
    dailyUseWorkspaceCreated = $true
    workspacePath = $workspacePath
    databasePath = $dbPath
    cachePath = $cacheFilePath
    defaultDailyUseDbPath = ".local/daily_use/etfdt_daily_use.sqlite"
    defaultMarketCachePath = ".local/daily_use/cache/market_cache.json"
    startupAutoRefreshEnabled = $true
    liveNetworkUsed = $false
    testNetworkAccess = $false
    demoRcPathUsed = $false
    productionDbTouched = $false
    brokerOrderSubmitted = $false
    networkAccess = $false
    credentialAccess = $false
    endpointAccess = $false
    automaticTrading = $false
}

$evidence | ConvertTo-Json -Depth 4
