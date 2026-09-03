# One-shot configure + build + test for ARPG Runtime Lab.
#
# Usage:
#   ./scripts/build.ps1
#   ./scripts/build.ps1 -Configuration Debug
#   ./scripts/build.ps1 -Clean -RunBenchmarks

[CmdletBinding()]
param(
    [ValidateSet('Debug', 'RelWithDebInfo', 'Release')]
    [string]$Configuration = 'RelWithDebInfo',

    [switch]$Clean,
    [switch]$SkipTests,
    [switch]$RunBenchmarks,
    [switch]$RunApp,
    [switch]$ConfigureOnly
)

$ErrorActionPreference = 'Stop'

$repoRoot = Split-Path -Parent $PSScriptRoot
$buildDir = Join-Path $repoRoot "build/$($Configuration.ToLower())"

# ---------------------------------------------------------------------------
# 1. Locate the Visual Studio toolchain
# ---------------------------------------------------------------------------
function Get-VisualStudioRoot {
    foreach ($edition in @('Professional', 'Enterprise', 'Community', 'Preview')) {
        $candidate = "C:\Program Files\Microsoft Visual Studio\2022\$edition"
        if (Test-Path $candidate) {
            return $candidate
        }
    }
    return $null
}

function Import-VcVarsEnvironment([string]$VcVarsAll) {
    # vcvarsall.bat is a batch script; run it and capture the resulting
    # environment so MSVC is usable without a dedicated developer shell.
    $output = & cmd /c "`"$VcVarsAll`" x64 >nul && set" 2>&1
    foreach ($line in $output) {
        if ($line -match '^([^=]+)=(.*)$') {
            [System.Environment]::SetEnvironmentVariable($matches[1], $matches[2], 'Process')
        }
    }
}

$cmake = (Get-Command cmake -ErrorAction SilentlyContinue).Source
if (-not $cmake) {
    $vsRoot = Get-VisualStudioRoot
    if (-not $vsRoot) {
        throw 'cmake not found and Visual Studio 2022 is not installed.'
    }
    $cmakeCandidates = @(
        "$vsRoot\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe",
        'C:\Program Files\CMake\bin\cmake.exe'
    )
    $cmake = $cmakeCandidates | Where-Object { Test-Path $_ } | Select-Object -First 1
    if (-not $cmake) {
        throw 'cmake.exe not found. Install CMake or the VS "C++ CMake tools" component.'
    }
    $ninja = "$vsRoot\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe"
    if (Test-Path $ninja) {
        $env:Path = (Split-Path $ninja) + ';' + $env:Path
    }
}

if (-not (Get-Command cl -ErrorAction SilentlyContinue)) {
    $vsRoot = if ($vsRoot) { $vsRoot } else { Get-VisualStudioRoot }
    if ($vsRoot -and (Test-Path "$vsRoot\VC\Auxiliary\Build\vcvarsall.bat")) {
        Write-Host "Initialising MSVC environment from $vsRoot" -ForegroundColor Cyan
        Import-VcVarsEnvironment "$vsRoot\VC\Auxiliary\Build\vcvarsall.bat"
    } else {
        throw 'MSVC (cl.exe) not available. Run this script from a VS developer shell, or install the VS C++ workload.'
    }
}

# ---------------------------------------------------------------------------
# 2. Configure
# ---------------------------------------------------------------------------
if ($Clean -and (Test-Path $buildDir)) {
    Write-Host "Cleaning $buildDir" -ForegroundColor Yellow
    Remove-Item -Recurse -Force $buildDir
}

Write-Host "Configuring ($Configuration) -> $buildDir" -ForegroundColor Cyan
& $cmake -S $repoRoot -B $buildDir -G Ninja "-DCMAKE_BUILD_TYPE=$Configuration"
if ($LASTEXITCODE -ne 0) { throw 'CMake configure failed.' }

if ($ConfigureOnly) {
    Write-Host 'Configure-only requested, stopping here.' -ForegroundColor Green
    return
}

# ---------------------------------------------------------------------------
# 3. Build
# ---------------------------------------------------------------------------
Write-Host 'Building' -ForegroundColor Cyan
& $cmake --build $buildDir --config $Configuration
if ($LASTEXITCODE -ne 0) { throw 'Build failed.' }

# ---------------------------------------------------------------------------
# 4. Test
# ---------------------------------------------------------------------------
if (-not $SkipTests) {
    Write-Host 'Running tests' -ForegroundColor Cyan
    & $cmake --build $buildDir --target test --config $Configuration
    if ($LASTEXITCODE -ne 0) { throw 'Tests failed.' }
}

if ($RunApp) {
    $app = Join-Path $buildDir 'Apps/RuntimeApp/arpg_runtime_app.exe'
    if (Test-Path $app) {
        Write-Host 'Running arpg_runtime_app' -ForegroundColor Cyan
        & $app
    }
}

if ($RunBenchmarks) {
    $bench = Join-Path $buildDir 'Benchmarks/arpg_benchmarks.exe'
    if (Test-Path $bench) {
        Write-Host 'Running benchmarks' -ForegroundColor Cyan
        & $bench --benchmark_min_time=0.05s
    }
}

Write-Host 'Done.' -ForegroundColor Green
