
# conan_install_windows.ps1
# Usage: Run from project root or scripts folder
# Checks for conan, creates build dir, runs conan install

param(
    [string]$ProjectRoot = (Resolve-Path "$PSScriptRoot\.."),
    [string]$BuildDir = "build"
)

function Check-Conan {
    if (-not (Get-Command conan -ErrorAction SilentlyContinue)) {
        Write-Error "Conan is not installed. Please install Conan before proceeding."
        exit 1
    }
}

Check-Conan

Set-Location $ProjectRoot
if (-not (Test-Path $BuildDir)) {
    New-Item -ItemType Directory -Path $BuildDir | Out-Null
}
conan profile detect --force
conan install . --output-folder=$BuildDir --build=missing
