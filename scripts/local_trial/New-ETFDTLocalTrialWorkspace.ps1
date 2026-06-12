[CmdletBinding()]
param(
    [string]$TrialRoot = "",
    [string]$BuildDir = "",
    [switch]$InitializeDatabase,
    [switch]$Force
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$repoRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot "..\.."))
if ([string]::IsNullOrWhiteSpace($TrialRoot)) {
    $TrialRoot = Join-Path $repoRoot ".demo\local_trial_rc"
} elseif (-not [System.IO.Path]::IsPathRooted($TrialRoot)) {
    $TrialRoot = Join-Path $repoRoot $TrialRoot
}
if ([string]::IsNullOrWhiteSpace($BuildDir)) {
    $BuildDir = Join-Path $repoRoot "build"
} elseif (-not [System.IO.Path]::IsPathRooted($BuildDir)) {
    $BuildDir = Join-Path $repoRoot $BuildDir
}

$trialRootFull = [System.IO.Path]::GetFullPath($TrialRoot)
$buildDirFull = [System.IO.Path]::GetFullPath($BuildDir)
$dataDir = Join-Path $trialRootFull "data"
$sampleDir = Join-Path $trialRootFull "samples"
$logDir = Join-Path $trialRootFull "logs"
$dbPath = Join-Path $dataDir "etfdt_local_trial.sqlite"
$sampleSource = Join-Path $repoRoot "samples\local_trial"

if ($trialRootFull -eq [System.IO.Path]::GetPathRoot($trialRootFull)) {
    throw "Refusing to use a drive root as trial root."
}
if ($dbPath -match "ETFDecision\.db$") {
    throw "Refusing to use the application DB name for local trial."
}
if (-not (Test-Path -LiteralPath $sampleSource -PathType Container)) {
    throw "Missing local trial sample pack: $sampleSource"
}
if ((Test-Path -LiteralPath $trialRootFull) -and -not $Force) {
    throw "Trial root already exists. Pass -Force to refresh it: $trialRootFull"
}

New-Item -ItemType Directory -Force -Path $dataDir | Out-Null
New-Item -ItemType Directory -Force -Path $sampleDir | Out-Null
New-Item -ItemType Directory -Force -Path $logDir | Out-Null
Copy-Item -Path (Join-Path $sampleSource "*.json") -Destination $sampleDir -Force

$paths = [ordered]@{
    task = "EPIC-282"
    trialRoot = $trialRootFull
    demoDb = $dbPath
    samples = $sampleDir
    logs = $logDir
    defaultTrialRootRepoLocal = $true
    demoDbPathExplicit = $true
    scriptsRequireAdmin = $false
    scriptsDownloadExternalDependencies = $false
    remoteAccess = $false
    realExecution = $false
}
$paths | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $trialRootFull "trial_paths.json") -Encoding UTF8

if ($InitializeDatabase) {
    $serviceExe = Join-Path $buildDirFull "apps\ETFDataService\Debug\ETFDataService.exe"
    $migration = Join-Path $repoRoot "migrations\001_initial_schema.sql"
    if (-not (Test-Path -LiteralPath $serviceExe -PathType Leaf)) {
        throw "Build local service before initializing the demo DB: $serviceExe"
    }
    if (-not (Test-Path -LiteralPath $migration -PathType Leaf)) {
        throw "Missing schema file: $migration"
    }
    & $serviceExe --init-db --db $dbPath --migration $migration
    if ($LASTEXITCODE -ne 0) {
        throw "Demo DB initialization failed."
    }
}

$paths | ConvertTo-Json -Depth 4
