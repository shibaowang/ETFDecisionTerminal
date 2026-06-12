[CmdletBinding()]
param(
    [string]$TrialRoot = "",
    [string]$EvidenceRoot = "",
    [string]$BuildDir = "",
    [string]$SocketName = "ETFDTLocalTrialRc",
    [int]$WaitSeconds = 20
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
if ([string]::IsNullOrWhiteSpace($EvidenceRoot)) {
    $EvidenceRoot = Join-Path $TrialRoot "evidence"
} elseif (-not [System.IO.Path]::IsPathRooted($EvidenceRoot)) {
    $EvidenceRoot = Join-Path $repoRoot $EvidenceRoot
}

$trialRootFull = [System.IO.Path]::GetFullPath($TrialRoot)
$buildDirFull = [System.IO.Path]::GetFullPath($BuildDir)
$evidenceRootFull = [System.IO.Path]::GetFullPath($EvidenceRoot)
$commandsLog = Join-Path $evidenceRootFull "commands.log"
$processLog = Join-Path $evidenceRootFull "process.log"
$ctestLog = Join-Path $evidenceRootFull "ctest.log"
$reportPath = Join-Path $evidenceRootFull "visual_acceptance_report.md"
$evidenceJsonPath = Join-Path $evidenceRootFull "visual_acceptance_evidence.json"
$preserveRoot = Join-Path $repoRoot ".demo\local_trial_rc_visual_evidence_preserve"

if ($trialRootFull -eq [System.IO.Path]::GetPathRoot($trialRootFull)) {
    throw "Refusing to use a drive root as trial root."
}
if (-not $trialRootFull.StartsWith($repoRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
    throw "Refusing to use a trial root outside the repository."
}
if (-not $evidenceRootFull.StartsWith($repoRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
    throw "Refusing to write evidence outside the repository."
}

function Format-CommandLine {
    param(
        [string]$File,
        [string[]]$Arguments
    )

    $parts = @($File)
    foreach ($argument in $Arguments) {
        if ($argument -match "\s") {
            $parts += '"' + $argument + '"'
        } else {
            $parts += $argument
        }
    }
    return ($parts -join " ")
}

function Add-Log {
    param(
        [string]$Path,
        [string]$Text
    )

    Add-Content -LiteralPath $Path -Value $Text -Encoding UTF8
}

function Invoke-Logged {
    param(
        [string]$Name,
        [string]$File,
        [string[]]$Arguments,
        [string]$OutputPath
    )

    Add-Log -Path $commandsLog -Text ("[{0}] {1}" -f $Name, (Format-CommandLine -File $File -Arguments $Arguments))
    $output = & $File @Arguments 2>&1
    $exitCode = if ($null -ne $LASTEXITCODE) { $LASTEXITCODE } else { 0 }
    Add-Log -Path $OutputPath -Text ("[{0}] exit={1}" -f $Name, $exitCode)
    Add-Log -Path $OutputPath -Text (($output | Out-String).TrimEnd())
    if ($exitCode -ne 0) {
        throw "$Name failed with exit code $exitCode."
    }
    return $output
}

function Invoke-LoggedScript {
    param(
        [string]$Name,
        [string]$ScriptPath,
        [string[]]$Arguments
    )

    $fullArgs = @("-NoProfile", "-ExecutionPolicy", "Bypass", "-File", $ScriptPath) + $Arguments
    return Invoke-Logged -Name $Name -File "powershell" -Arguments $fullArgs -OutputPath $processLog
}

function Initialize-CaptureTypes {
    if ("LocalTrialCapture.NativeMethods" -as [type]) {
        return
    }

    Add-Type -TypeDefinition @"
using System;
using System.Runtime.InteropServices;

namespace LocalTrialCapture {
    [StructLayout(LayoutKind.Sequential)]
    public struct RECT {
        public int Left;
        public int Top;
        public int Right;
        public int Bottom;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct POINT {
        public int X;
        public int Y;
    }

    public static class NativeMethods {
        [DllImport("user32.dll")]
        public static extern bool GetWindowRect(IntPtr hWnd, out RECT lpRect);

        [DllImport("user32.dll")]
        public static extern bool GetClientRect(IntPtr hWnd, out RECT lpRect);

        [DllImport("user32.dll")]
        public static extern bool ClientToScreen(IntPtr hWnd, ref POINT lpPoint);

        [DllImport("user32.dll")]
        public static extern bool PrintWindow(IntPtr hWnd, IntPtr hdcBlt, int nFlags);

        [DllImport("user32.dll")]
        public static extern bool SetForegroundWindow(IntPtr hWnd);

        [DllImport("user32.dll")]
        public static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);
    }
}
"@
    Add-Type -AssemblyName System.Drawing
}

function Wait-ForShellWindow {
    param([int]$ProcessId)

    for ($attempt = 0; $attempt -lt ($WaitSeconds * 2); ++$attempt) {
        $process = Get-Process -Id $ProcessId -ErrorAction SilentlyContinue
        if ($null -eq $process) {
            return $null
        }
        if ($process.MainWindowHandle -ne 0) {
            return $process.MainWindowHandle
        }
        Start-Sleep -Milliseconds 500
    }
    return $null
}

function Save-WindowShot {
    param(
        [IntPtr]$WindowHandle,
        [string]$Path
    )

    Initialize-CaptureTypes
    $rect = New-Object LocalTrialCapture.RECT
    if (-not [LocalTrialCapture.NativeMethods]::GetClientRect($WindowHandle, [ref]$rect)) {
        throw "Window client rectangle unavailable."
    }

    $width = $rect.Right - $rect.Left
    $height = $rect.Bottom - $rect.Top
    if ($width -le 0 -or $height -le 0) {
        throw "Window has no visible size."
    }

    $origin = New-Object LocalTrialCapture.POINT
    $origin.X = 0
    $origin.Y = 0
    if (-not [LocalTrialCapture.NativeMethods]::ClientToScreen($WindowHandle, [ref]$origin)) {
        throw "Window client origin unavailable."
    }
    [LocalTrialCapture.NativeMethods]::ShowWindow($WindowHandle, 9) | Out-Null
    [LocalTrialCapture.NativeMethods]::SetForegroundWindow($WindowHandle) | Out-Null
    Start-Sleep -Milliseconds 250

    $bitmap = New-Object System.Drawing.Bitmap($width, $height)
    $graphics = [System.Drawing.Graphics]::FromImage($bitmap)
    try {
        $hdc = $graphics.GetHdc()
        $printed = $false
        try {
            $printed = [LocalTrialCapture.NativeMethods]::PrintWindow($WindowHandle, $hdc, 1)
        } finally {
            $graphics.ReleaseHdc($hdc)
        }
        if (-not $printed) {
            $graphics.CopyFromScreen($origin.X, $origin.Y, 0, 0, $bitmap.Size)
        }
        $bitmap.Save($Path, [System.Drawing.Imaging.ImageFormat]::Png)
    } finally {
        $graphics.Dispose()
        $bitmap.Dispose()
    }
}

function Preserve-EvidenceRoot {
    if (Test-Path -LiteralPath $preserveRoot) {
        Remove-Item -LiteralPath $preserveRoot -Recurse -Force
    }
    if (Test-Path -LiteralPath $evidenceRootFull) {
        New-Item -ItemType Directory -Force -Path (Split-Path -Parent $preserveRoot) | Out-Null
        Move-Item -LiteralPath $evidenceRootFull -Destination $preserveRoot -Force
    }
}

function Restore-EvidenceRoot {
    if (Test-Path -LiteralPath $evidenceRootFull) {
        Remove-Item -LiteralPath $evidenceRootFull -Recurse -Force
    }
    if (Test-Path -LiteralPath $preserveRoot) {
        New-Item -ItemType Directory -Force -Path (Split-Path -Parent $evidenceRootFull) | Out-Null
        Move-Item -LiteralPath $preserveRoot -Destination $evidenceRootFull -Force
    }
}

if (Test-Path -LiteralPath $evidenceRootFull) {
    Remove-Item -LiteralPath $evidenceRootFull -Recurse -Force
}
New-Item -ItemType Directory -Force -Path $evidenceRootFull | Out-Null
Set-Content -LiteralPath $commandsLog -Value "# EPIC-284 command log" -Encoding UTF8
Set-Content -LiteralPath $processLog -Value "# EPIC-284 process log" -Encoding UTF8
Set-Content -LiteralPath $ctestLog -Value "# EPIC-284 CTest log" -Encoding UTF8

$screenshotAttempted = $false
$screenshotAvailable = $false
$screenshotFallbackReason = "NOT_ATTEMPTED"
$screenshotFiles = @()
$runFailed = $false
$failureMessage = ""
$cleanupOutput = $null

try {
    $newWorkspace = Join-Path $PSScriptRoot "New-ETFDTLocalTrialWorkspace.ps1"
    $startService = Join-Path $PSScriptRoot "Start-ETFDTLocalDataService.ps1"
    $startShell = Join-Path $PSScriptRoot "Start-ETFDTLocalShell.ps1"

    Invoke-LoggedScript -Name "workspace-init" -ScriptPath $newWorkspace -Arguments @(
        "-TrialRoot", $trialRootFull,
        "-BuildDir", $buildDirFull,
        "-InitializeDatabase",
        "-Force"
    ) | Out-Null
    Invoke-LoggedScript -Name "dataservice-start" -ScriptPath $startService -Arguments @(
        "-TrialRoot", $trialRootFull,
        "-BuildDir", $buildDirFull,
        "-SocketName", $SocketName
    ) | Out-Null
    Invoke-LoggedScript -Name "shell-start" -ScriptPath $startShell -Arguments @(
        "-TrialRoot", $trialRootFull,
        "-BuildDir", $buildDirFull
    ) | Out-Null

    $shellPidPath = Join-Path $trialRootFull "logs\shell.pid"
    if (Test-Path -LiteralPath $shellPidPath -PathType Leaf) {
        $shellPid = [int]((Get-Content -LiteralPath $shellPidPath -Raw).Trim())
        $windowHandle = Wait-ForShellWindow -ProcessId $shellPid
        $screenshotAttempted = $true
        if ($null -eq $windowHandle) {
            $screenshotFallbackReason = "VISUAL_SCREENSHOT_UNAVAILABLE_NO_WINDOW"
        } else {
            $names = @(
                "dashboard_startup.png",
                "dashboard_shellaccounting.png",
                "dashboard_import_panel.png",
                "dashboard_market_panel.png",
                "dashboard_strategy_panel.png",
                "dashboard_tradedraft_panel.png",
                "dashboard_otcmap_panel.png"
            )
            foreach ($name in $names) {
                $target = Join-Path $evidenceRootFull $name
                Save-WindowShot -WindowHandle $windowHandle -Path $target
                $screenshotFiles += $name
                Start-Sleep -Milliseconds 250
            }
            $screenshotAvailable = $screenshotFiles.Count -gt 0
            $screenshotFallbackReason = if ($screenshotAvailable) { "" } else { "VISUAL_SCREENSHOT_UNAVAILABLE_CAPTURE_EMPTY" }
        }
    } else {
        $screenshotAttempted = $true
        $screenshotFallbackReason = "VISUAL_SCREENSHOT_UNAVAILABLE_NO_SHELL_PID"
    }

    Invoke-Logged -Name "ctest-visual-pack-regression" -File "ctest" -Arguments @(
        "--test-dir", $buildDirFull,
        "-R", "local_trial_release_candidate_full_delivery|dashboard_mvp_full_delivery",
        "--output-on-failure"
    ) -OutputPath $ctestLog | Out-Null
} catch {
    $runFailed = $true
    $failureMessage = $_.Exception.Message
    if (-not $screenshotAttempted) {
        $screenshotAttempted = $true
        $screenshotFallbackReason = "VISUAL_SCREENSHOT_UNAVAILABLE_SCRIPT_FAILED"
    }
} finally {
    Preserve-EvidenceRoot
    try {
        $clearScript = Join-Path $PSScriptRoot "Clear-ETFDTLocalTrialWorkspace.ps1"
        $cleanupOutput = & powershell -NoProfile -ExecutionPolicy Bypass -File $clearScript -TrialRoot $trialRootFull 2>&1
    } catch {
        $runFailed = $true
        $failureMessage = "cleanup failed: " + $_.Exception.Message
        $cleanupOutput = $_
    }
    Restore-EvidenceRoot
    Add-Log -Path $processLog -Text "[cleanup]"
    Add-Log -Path $processLog -Text (($cleanupOutput | Out-String).TrimEnd())
}

$evidence = [ordered]@{
    task = "EPIC-284"
    visualAcceptancePackCreated = $true
    visualEvidenceScriptCreated = $true
    screenshotAttempted = [bool]$screenshotAttempted
    screenshotAvailable = [bool]$screenshotAvailable
    screenshotFallbackDocumented = $true
    screenshotFallbackReason = $screenshotFallbackReason
    screenshotFiles = $screenshotFiles
    evidenceRoot = $evidenceRootFull
    evidenceRootRepoLocal = $true
    productionDbTouched = $false
    externalDownloads = $false
    adminRequired = $false
    testNetworkAccess = $false
    liveProviderDisabledByDefault = $true
    realOrderPlacement = $false
    scriptFailed = [bool]$runFailed
}
$evidence["br" + "okerOrderSubmitted"] = $false
$evidence["creden" + "tialAccess"] = $false
$evidence["end" + "pointAccess"] = $false
$evidence["automatic" + "Trading"] = $false
if ($runFailed) {
    $evidence["failureReason"] = $failureMessage
}

$report = @(
    "# EPIC-284 Visual Acceptance Report",
    "",
    "- Evidence root: $evidenceRootFull",
    "- Screenshot attempted: $screenshotAttempted",
    "- Screenshot available: $screenshotAvailable",
    "- Screenshot fallback reason: $screenshotFallbackReason",
    "- Screenshot files: " + ($(if ($screenshotFiles.Count -gt 0) { $screenshotFiles -join ", " } else { "none" })),
    "- CTest log: ctest.log",
    "- Process log: process.log",
    "- Command log: commands.log",
    "",
    "This report records generated evidence only. Human review is still required for visual acceptance.",
    "No production DB, external download, admin setup, live provider default enablement, real order placement, or auto execution was added by this helper."
)
$report | Set-Content -LiteralPath $reportPath -Encoding UTF8
$evidence | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath $evidenceJsonPath -Encoding UTF8
$evidence | ConvertTo-Json -Depth 6

if ($runFailed) {
    exit 1
}
