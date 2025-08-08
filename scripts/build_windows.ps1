# build_windows.ps1

param(
    [string]$ProjectRoot = (Resolve-Path "$PSScriptRoot\.."),
    [string]$BuildDir = "build",
    [switch]$Package
)

Set-Location $ProjectRoot
Set-Location $BuildDir
cmake .. -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE="conan_toolchain.cmake"

if ($Package) {
    Write-Host "Packaging enabled. Running installer script..."
    .\scripts\installer_windows.ps1
}
