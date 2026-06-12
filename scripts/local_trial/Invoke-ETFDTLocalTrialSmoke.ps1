[CmdletBinding()]
param(
    [string]$BuildDir = ""
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$repoRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot "..\.."))
if ([string]::IsNullOrWhiteSpace($BuildDir)) {
    $BuildDir = Join-Path $repoRoot "build"
} elseif (-not [System.IO.Path]::IsPathRooted($BuildDir)) {
    $BuildDir = Join-Path $repoRoot $BuildDir
}
$buildDirFull = [System.IO.Path]::GetFullPath($BuildDir)
$targets = @(
    "local_trial_release_candidate_full_delivery",
    "dashboard_mvp_full_delivery",
    "market_data_historical_high_refresh_engine_full_delivery",
    "otcmap_ac_multichannel_draft_engine_full_delivery",
    "tradedraft_manual_recommendation_flow_full_delivery",
    "strategy_recommendation_engine_vba_parity_full_delivery",
    "shell_accounting_portfolio_replay_vba_parity_full_delivery",
    "shell_accounting_excel_vba_import_mvp_local_service_e2e_acceptance"
)

foreach ($target in $targets) {
    ctest --test-dir $buildDirFull -R "^$target$" --output-on-failure
    if ($LASTEXITCODE -ne 0) {
        throw "Smoke target failed: $target"
    }
}

[ordered]@{
    task = "EPIC-282"
    scriptedSmokeCreated = $true
    smokePassed = $true
    remoteAccess = $false
    realExecution = $false
} | ConvertTo-Json -Depth 4
