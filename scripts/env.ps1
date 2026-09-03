# Adds the toolchain (CMake / Ninja / Git / MSVC) to the current PowerShell
# session PATH. The tools are installed with Visual Studio but not on PATH.
#
# Usage:  . ./scripts/env.ps1

$ErrorActionPreference = 'Stop'

function Add-ToPathFront([string[]]$Paths) {
    $existing = @()
    foreach ($p in $Paths) {
        if ($p -and (Test-Path $p) -and ($env:Path -notlike "*$p*")) {
            $existing += $p
        }
    }
    if ($existing.Count -gt 0) {
        $env:Path = ($existing -join ';') + ';' + $env:Path
    }
    return $existing
}

$vsEditions = @('Professional', 'Enterprise', 'Community', 'Preview')
$vsRoot = $null
foreach ($edition in $vsEditions) {
    $candidate = "C:\Program Files\Microsoft Visual Studio\2022\$edition"
    if (Test-Path $candidate) {
        $vsRoot = $candidate
        break
    }
}

$added = @()

if ($vsRoot) {
    $added += Add-ToPathFront @(
        "$vsRoot\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin",
        "$vsRoot\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja"
    )
    $env:ARPG_VS_ROOT = $vsRoot
    $env:ARPG_VCVARSALL = "$vsRoot\VC\Auxiliary\Build\vcvarsall.bat"
} else {
    Write-Warning 'Visual Studio 2022 not found; MSVC environment not configured.'
}

$added += Add-ToPathFront @('C:\Program Files\Git\cmd')

if ($added.Count -gt 0) {
    Write-Host 'Added to PATH:' -ForegroundColor Green
    $added | ForEach-Object { Write-Host "  $_" }
} else {
    Write-Host 'Toolchain already on PATH.'
}

Write-Host "cmake : $((Get-Command cmake -ErrorAction SilentlyContinue).Source)"
Write-Host "ninja : $((Get-Command ninja -ErrorAction SilentlyContinue).Source)"
Write-Host "git   : $((Get-Command git   -ErrorAction SilentlyContinue).Source)"
