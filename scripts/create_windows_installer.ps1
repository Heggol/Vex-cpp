
param(
    [string]$BuildDir = "build\Release"
)

$ErrorActionPreference = 'Stop'

try {
    Get-Command makensis.exe -ErrorAction Stop | Out-Null
}
catch {
    Write-Error "NSIS is not installed or not in your PATH. Please install it from https://nsis.sourceforge.io/Download"
    exit 1
}

$ProjectRoot = (Get-Item -Path $PSScriptRoot).Parent.FullName
$NsisScriptTemplate = Join-Path $PSScriptRoot "make_windows_installer.nsi"
$BuildPath = Join-Path $ProjectRoot $BuildDir

if (-not (Test-Path -Path $BuildPath -PathType Container)) {
    Write-Error "Build directory not found at '$BuildPath'. Please build the project in Release mode first."
    exit 1
}

$Version = "0.1.0"
if (Get-Command git -ErrorAction SilentlyContinue) {
    $GitVersion = git describe --tags --abbrev=0 2>$null
    if ($LASTEXITCODE -eq 0 -and $GitVersion) {
        $Version = $GitVersion -replace '^v', ''
    }
}
Write-Host "Using version: $Version"

Write-Host "Creating installer..."

$TempNsisScript = Join-Path $env:TEMP "temp_vex_installer.nsi"
(Get-Content $NsisScriptTemplate -Raw) -replace '!define VERSION ".*"', "!define VERSION `"$Version`"" | Set-Content -Path $TempNsisScript
(Get-Content $TempNsisScript -Raw) -replace '!define BUILD_DIR ".*"', "!define BUILD_DIR `"$BuildPath`"" | Set-Content -Path $TempNsisScript

$InstallerName = "Vex-${Version}-Installer.exe"
$InstallerPath = Join-Path $ProjectRoot "build" $InstallerName

$NsisPluginsDir = Join-Path $ProjectRoot "scripts\NSIS\Plugins"
$makensisCommand = "makensis.exe /V4 /DVERSION=$Version /DOUTFILE=`"$InstallerPath`" /X`"SetPluginFolder `"`"$NsisPluginsDir`"`"`" `"$TempNsisScript`""

Invoke-Expression $makensisCommand

Remove-Item $TempNsisScript

if (Test-Path $InstallerPath) {
    Write-Host "Successfully created installer: $InstallerPath"
}
else {
    Write-Error "Installer creation failed."
    exit 1
}
