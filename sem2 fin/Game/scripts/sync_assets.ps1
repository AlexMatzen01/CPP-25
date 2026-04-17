param(
    [string]$ManifestPath = "assets/manifest.txt"
)

$ErrorActionPreference = "Stop"

if (-not (Test-Path $ManifestPath)) {
    Write-Error "Manifest not found: $ManifestPath"
}

$assetDir = Split-Path -Parent (Resolve-Path $ManifestPath)
$attribFile = Join-Path $assetDir "ATTRIBUTION.txt"
Set-Content -Path $attribFile -Value "# Auto-generated attribution list"
Add-Content -Path $attribFile -Value "# Generated from $(Split-Path -Leaf $ManifestPath)"

$downloaded = 0
$cached = 0
$failed = 0

Get-Content $ManifestPath | ForEach-Object {
    $line = $_.Trim()
    if ([string]::IsNullOrWhiteSpace($line) -or $line.StartsWith("#")) {
        return
    }

    $parts = $line.Split("|")
    if ($parts.Count -lt 1) {
        return
    }

    $type = $parts[0].Trim()
    if ($type -eq "version") {
        return
    }

    if ($type -ne "texture" -or $parts.Count -lt 6) {
        return
    }

    $id = $parts[1].Trim()
    $relPath = $parts[2].Trim()
    $urls = $parts[3].Trim().Split(",") | ForEach-Object { $_.Trim() } | Where-Object { $_ -ne "" }
    $attribution = $parts[5].Trim()

    $target = Join-Path $assetDir $relPath
    New-Item -ItemType Directory -Force -Path (Split-Path -Parent $target) | Out-Null

    if (Test-Path $target) {
        $cached += 1
        Add-Content -Path $attribFile -Value "$id | $attribution"
        return
    }

    $ok = $false
    foreach ($url in $urls) {
        try {
            Write-Host "Downloading $id from $url"
            Invoke-WebRequest -UseBasicParsing -Uri $url -OutFile $target
            $ok = $true
            break
        } catch {
            $ok = $false
        }
    }

    if ($ok) {
        $downloaded += 1
        Add-Content -Path $attribFile -Value "$id | $attribution"
    } else {
        Write-Warning "Failed to download $id"
        $failed += 1
    }
}

Write-Host "Asset sync completed: downloaded=$downloaded cached=$cached failed=$failed"
if ($failed -gt 0) {
    exit 2
}
