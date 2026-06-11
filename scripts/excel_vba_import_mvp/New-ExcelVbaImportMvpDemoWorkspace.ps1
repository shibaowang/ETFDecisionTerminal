[CmdletBinding()]
param(
    [string]$DemoRoot = "",
    [string]$BuildDir = "",
    [switch]$InitializeDatabase,
    [switch]$Force
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$repoRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot "..\.."))
if ([string]::IsNullOrWhiteSpace($DemoRoot)) {
    $DemoRoot = Join-Path $repoRoot ".demo\excel_vba_import_mvp"
} elseif (-not [System.IO.Path]::IsPathRooted($DemoRoot)) {
    $DemoRoot = Join-Path $repoRoot $DemoRoot
}

if ([string]::IsNullOrWhiteSpace($BuildDir)) {
    $BuildDir = Join-Path $repoRoot "build"
} elseif (-not [System.IO.Path]::IsPathRooted($BuildDir)) {
    $BuildDir = Join-Path $repoRoot $BuildDir
}

$demoRootFull = [System.IO.Path]::GetFullPath($DemoRoot)
$buildDirFull = [System.IO.Path]::GetFullPath($BuildDir)
$sampleSource = Join-Path $repoRoot "samples\excel_vba_import\TASK272_sanitized_excel_vba_import_mvp_sample.json"
$sampleDir = Join-Path $demoRootFull "samples"
$dataDir = Join-Path $demoRootFull "data"
$logsDir = Join-Path $demoRootFull "logs"
$demoDb = Join-Path $dataDir "excel_vba_import_mvp_demo.sqlite"
$sampleTarget = Join-Path $sampleDir "TASK272_sanitized_excel_vba_import_mvp_sample.json"

if (-not (Test-Path -LiteralPath $sampleSource -PathType Leaf)) {
    throw "Missing sanitized sample export: $sampleSource"
}

if ($demoDb -match "data[\\/]+ETFDecision\.db$") {
    throw "Refusing to use production database path as demo DB."
}

New-Item -ItemType Directory -Force -Path $sampleDir | Out-Null
New-Item -ItemType Directory -Force -Path $dataDir | Out-Null
New-Item -ItemType Directory -Force -Path $logsDir | Out-Null
Copy-Item -LiteralPath $sampleSource -Destination $sampleTarget -Force

$trialCommands = @"
`$DemoDb = "$demoDb"
`$SocketName = "ETFDataServiceExcelVbaImportMvpDemo"
build\apps\ETFDataService\Debug\ETFDataService.exe --serve-dev-audit --db `$DemoDb --socket-name `$SocketName
build\apps\ETFDecisionShell\Debug\ETFDecisionShell.exe --diagnostics-mock
"@
Set-Content -LiteralPath (Join-Path $demoRootFull "trial_commands.ps1") -Value $trialCommands -Encoding UTF8

$readme = @"
Excel/VBA Import MVP Demo Workspace

Sample:
$sampleTarget

Demo DB:
$demoDb

This workspace is for sanitized JSON/TXT exports only. Direct .xlsx import is
not supported. Do not point the trial at production database paths. No broker,
network service, real order placement, or automatic trading is started here.
"@
Set-Content -LiteralPath (Join-Path $demoRootFull "README.txt") -Value $readme -Encoding UTF8

if ($InitializeDatabase) {
    $dataServiceExe = Join-Path $buildDirFull "apps\ETFDataService\Debug\ETFDataService.exe"
    $migration = Join-Path $repoRoot "migrations\001_initial_schema.sql"
    if (-not (Test-Path -LiteralPath $dataServiceExe -PathType Leaf)) {
        throw "Build ETFDataService before initializing the demo DB: $dataServiceExe"
    }
    if (-not (Test-Path -LiteralPath $migration -PathType Leaf)) {
        throw "Missing initial schema migration: $migration"
    }
    & $dataServiceExe --init-db --db $demoDb --migration $migration
    if ($LASTEXITCODE -ne 0) {
        throw "Demo DB initialization failed."
    }
}

[ordered]@{
    task = "EPIC-273"
    demoWorkspaceCreated = $true
    demoRoot = $demoRootFull
    demoDb = $demoDb
    sample = $sampleTarget
    productionDbTouched = $false
    networkAccess = $false
    credentialAccess = $false
    endpointAccess = $false
    brokerOrderSubmitted = $false
    automaticTrading = $false
    directXlsxImportSupported = $false
    sanitizedJsonTxtOnly = $true
} | ConvertTo-Json -Depth 3
