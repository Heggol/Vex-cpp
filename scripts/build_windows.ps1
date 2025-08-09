# Usage: Run from project root or scripts folder
# Builds the project using CMake
# Optionally creates the package from built executable

param(
    [string]$ProjectRoot = (Resolve-Path "$PSScriptRoot\.."),
    [string]$BuildDir = "build",
    [Parameter(HelpMessage = "Enable packaging of the built executable")]
    [switch]$Package
)

Set-Location $ProjectRoot
Set-Location $BuildDir
cmake .. -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE="conan_toolchain.cmake" -DCMAKE_BUILD_TYPE="Release"

if ($Package) {
    Write-Host "Packaging enabled. Running installer script..."
    .\scripts\installer_windows.ps1
}

Set-Location $ProjectRoot