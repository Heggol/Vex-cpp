
$AppName = "Vex"
$InstallDir = "$env:LOCALAPPDATA\$AppName"
$Executable = "vex.exe"
$SourcePath = "build\$Executable"

if (-not (Test-Path -Path $InstallDir)) {
    New-Item -ItemType Directory -Path $InstallDir | Out-Null
}

Copy-Item -Path $SourcePath -Destination $InstallDir -Force

$CurrentUserPath = [System.Environment]::GetEnvironmentVariable('Path', 'User')
if (-not ($CurrentUserPath -like "*$InstallDir*")) {
    $NewPath = "$CurrentUserPath;$InstallDir"
    [System.Environment]::SetEnvironmentVariable('Path', $NewPath, 'User')
    Write-Host "$AppName has been installed to $InstallDir and added to your PATH."
    Write-Host "Please restart your terminal for the changes to take effect."
}
else {
    Write-Host "$AppName is already in your PATH."
}

Write-Host "Installation complete."