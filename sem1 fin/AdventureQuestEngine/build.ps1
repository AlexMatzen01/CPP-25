#!/usr/bin/env pwsh

# Usage: .\build.ps1 [fast|debug|release|clean]

if ($args.Count -gt 0) {
    $buildType = $args[0]
} else {
    $buildType = "fast"
}

$jobs = 6

Write-Host "Building AdventureQuestEngine ($buildType)..."

if (Get-Command mingw32-make -ErrorAction SilentlyContinue) {
    $makeCmd = "mingw32-make"
} else {
    $makeCmd = "make"
}

& $makeCmd "-j$jobs" $buildType

if ($LASTEXITCODE -eq 0) {
    Write-Host "Build successful!"
    Write-Host "Run with: .\bin\AdventureQuestEngine.exe"
} else {
    Write-Host "Build failed"
    exit 1
}