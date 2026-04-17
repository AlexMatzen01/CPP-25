#!/usr/bin/env bash
set -euo pipefail

MANIFEST_PATH="${1:-assets/manifest.txt}"
PROJECT_ROOT="${2:-$PWD}"

resolve_manifest() {
    if [[ -f "$MANIFEST_PATH" ]]; then
        echo "$MANIFEST_PATH"
        return
    fi

    if [[ -f "$PROJECT_ROOT/$MANIFEST_PATH" ]]; then
        echo "$PROJECT_ROOT/$MANIFEST_PATH"
        return
    fi

    echo "Manifest not found: $MANIFEST_PATH" >&2
    exit 1
}

trim() {
    local s="$1"
    s="${s#${s%%[![:space:]]*}}"
    s="${s%${s##*[![:space:]]}}"
    printf '%s' "$s"
}

MANIFEST_FILE="$(resolve_manifest)"
ASSET_DIR="$(cd "$(dirname "$MANIFEST_FILE")" && pwd)"
ATTRIB_FILE="$ASSET_DIR/ATTRIBUTION.txt"

download_with_curl() {
    local url="$1"
    local output="$2"
    curl -L --fail --retry 3 --connect-timeout 10 --max-time 120 -o "$output" "$url"
}

download_with_wget() {
    local url="$1"
    local output="$2"
    wget --tries=3 --timeout=30 -O "$output" "$url"
}

download_with_powershell() {
    local url="$1"
    local output="$2"
    powershell.exe -NoProfile -Command "try { Invoke-WebRequest -UseBasicParsing -Uri '$url' -OutFile '$output' } catch { exit 1 }"
}

sha256_of() {
    local file="$1"
    if command -v sha256sum >/dev/null 2>&1; then
        sha256sum "$file" | awk '{print $1}'
        return
    fi

    if command -v shasum >/dev/null 2>&1; then
        shasum -a 256 "$file" | awk '{print $1}'
        return
    fi

    echo ""
}

attempt_download() {
    local url="$1"
    local output="$2"

    if command -v curl >/dev/null 2>&1; then
        download_with_curl "$url" "$output"
        return
    fi

    if command -v wget >/dev/null 2>&1; then
        download_with_wget "$url" "$output"
        return
    fi

    if command -v powershell.exe >/dev/null 2>&1; then
        download_with_powershell "$url" "$output"
        return
    fi

    echo "No supported downloader found (curl/wget/powershell)." >&2
    exit 1
}

echo "# Auto-generated attribution list" > "$ATTRIB_FILE"
echo "# Generated from $(basename "$MANIFEST_FILE")" >> "$ATTRIB_FILE"

downloaded=0
cached=0
failed=0

while IFS='|' read -r raw_type raw_id raw_path raw_urls raw_sha raw_attr; do
    line_type="$(trim "${raw_type:-}")"

    if [[ -z "$line_type" || "${line_type:0:1}" == "#" ]]; then
        continue
    fi

    if [[ "$line_type" == "version" ]]; then
        continue
    fi

    if [[ "$line_type" != "texture" ]]; then
        continue
    fi

    asset_id="$(trim "${raw_id:-}")"
    rel_path="$(trim "${raw_path:-}")"
    urls_field="$(trim "${raw_urls:-}")"
    expected_sha="$(trim "${raw_sha:-}")"
    attribution="$(trim "${raw_attr:-}")"

    if [[ -z "$asset_id" || -z "$rel_path" || -z "$urls_field" ]]; then
        echo "Skipping invalid manifest row for asset '$asset_id'" >&2
        failed=$((failed + 1))
        continue
    fi

    target="$ASSET_DIR/$rel_path"
    mkdir -p "$(dirname "$target")"

    skip_download=0
    if [[ -f "$target" ]]; then
        if [[ -z "$expected_sha" || "$expected_sha" == "none" || "$expected_sha" == "-" ]]; then
            skip_download=1
        else
            current_sha="$(sha256_of "$target")"
            if [[ -n "$current_sha" && "$current_sha" == "$expected_sha" ]]; then
                skip_download=1
            fi
        fi
    fi

    if [[ "$skip_download" -eq 1 ]]; then
        cached=$((cached + 1))
        echo "$asset_id | $attribution" >> "$ATTRIB_FILE"
        continue
    fi

    tmp_file="$target.part"
    rm -f "$tmp_file"

    IFS=',' read -r -a urls <<< "$urls_field"
    success=0
    for url in "${urls[@]}"; do
        url="$(trim "$url")"
        if [[ -z "$url" ]]; then
            continue
        fi

        echo "Downloading $asset_id from $url"
        if attempt_download "$url" "$tmp_file"; then
            success=1
            break
        fi
    done

    if [[ "$success" -ne 1 || ! -f "$tmp_file" ]]; then
        echo "Failed to download asset '$asset_id'" >&2
        failed=$((failed + 1))
        rm -f "$tmp_file"
        continue
    fi

    if [[ -n "$expected_sha" && "$expected_sha" != "none" && "$expected_sha" != "-" ]]; then
        actual_sha="$(sha256_of "$tmp_file")"
        if [[ -z "$actual_sha" || "$actual_sha" != "$expected_sha" ]]; then
            echo "Checksum mismatch for '$asset_id'" >&2
            failed=$((failed + 1))
            rm -f "$tmp_file"
            continue
        fi
    fi

    mv "$tmp_file" "$target"
    downloaded=$((downloaded + 1))
    echo "$asset_id | $attribution" >> "$ATTRIB_FILE"
done < "$MANIFEST_FILE"

echo "Asset sync completed: downloaded=$downloaded cached=$cached failed=$failed"
if [[ "$failed" -gt 0 ]]; then
    exit 2
fi
