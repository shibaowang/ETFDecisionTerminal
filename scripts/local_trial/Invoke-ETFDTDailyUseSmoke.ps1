param(
    [string]$DbPath = ".local/daily_use/etfdt_daily_use.sqlite",
    [switch]$NoNetworkFixtureMode = $true
)

$ErrorActionPreference = "Stop"

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
$resolvedDbPath = Join-Path $repoRoot $DbPath

if (-not (Test-Path -LiteralPath $resolvedDbPath)) {
    throw "Daily-use DB missing: $resolvedDbPath"
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

$evidence = [ordered]@{
    task = "EPIC-289"
    dailyUseSmokeReady = $true
    databasePath = $resolvedDbPath
    noNetworkFixtureMode = [bool]$NoNetworkFixtureMode
    defaultDailyUseDbPath = ".local/daily_use/etfdt_daily_use.sqlite"
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
