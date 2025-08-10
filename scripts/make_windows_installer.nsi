

!define APPNAME "Vex"
!define COMPANYNAME "Heggo"
!define DESCRIPTION "A C++ command-line tool for package management."
!define VERSION "0.1.0"
!define EXECUTABLE "vex.exe"

!define MUI_ICON "assets\icon.ico"
!define MUI_UNICON "assets\icon.ico"

!include "MUI2.nsh"

Name "${APPNAME}"
OutFile "Vex-Installer.exe"
InstallDir "$PROGRAMFILES\${APPNAME}"
RequestExecutionLevel admin

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "LICENSE"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

Section "Install"
    SetOutPath $INSTDIR
    File /r "build\Release\*"

    EnVar::SetHKLM "PATH" "$INSTDIR"

    WriteUninstaller "$INSTDIR\uninstall.exe"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$INSTDIR\uninstall.exe"
SectionEnd

Section "Uninstall"
    EnVar::DeleteValue "PATH" "$INSTDIR"

    Delete "$INSTDIR\uninstall.exe"
    RMDir /r "$INSTDIR"

    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
SectionEnd
