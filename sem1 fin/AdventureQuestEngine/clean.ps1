#powershell
# Clean up the build artifacts
# This script removes all files in the `build` directory and the `bin` directory.

# Remove all files in the build directory
Get-ChildItem -Path "build/*" -Recurse | Remove-Item -Force
#Text output for nice formatting
Write-Host "Cleaned up build artifacts in the 'build' directory." -ForegroundColor Green
# Remove all files in the bin directory
Get-ChildItem -Path "bin/*" -Recurse | Remove-Item -Force
Write-Host "Cleaned up build artifacts in the 'bin' directory." -ForegroundColor Green
#Error handling for any issues that may arise during the cleanup process
try {
    Get-ChildItem -Path "build/*" -Recurse | Remove-Item -Force
    Get-ChildItem -Path "bin/*" -Recurse | Remove-Item -Force
} catch {
    Write-Host "An error occurred while cleaning up build artifacts: $_" -ForegroundColor Red
}
