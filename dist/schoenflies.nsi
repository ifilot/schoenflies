/**
 * Schoenflies
 * Copyright (c) 2022 Luuk Kempen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

; includes
!include "x64.nsh"
!include "FileFunc.nsh"
!include "MUI2.nsh"

; settings
Unicode true
SetCompressor /SOLID LZMA
ManifestDPIAware true
RequestExecutionLevel admin

; information
!searchparse /file "..\..\src\program.h" ` * Copyright (c) ` YEAR ` ` AUTHOR
!searchparse /file "..\..\src\program.h" `#define PROGRAM_NAME "` NAME `"`
!searchparse /file "..\..\src\program.h" `#define PROGRAM_DESC "` DESC `"`
!searchparse /file "..\..\src\program.h" `#define PROGRAM_VERSION "` VER_STR `"`
!searchparse /file "..\..\src\program.h" `#define PROGRAM_VERSION "` VER_MAJOR `.` VER_MINOR `.` VER_PATCH `"`

Name "${NAME}"
OutFile "schoenflies-windows-installer-${VER_STR}.exe"
InstallDir "$PROGRAMFILES64\${NAME}"

BrandingText "${NAME}"

VIProductVersion ${VER_MAJOR}.${VER_MINOR}.${VER_PATCH}.0
VIAddVersionKey "ProductName" "${NAME}"
VIAddVersionKey "CompanyName" "${AUTHOR}"
VIAddVersionKey "LegalCopyright" "Copyright (c) ${YEAR} ${AUTHOR}"
VIAddVersionKey "FileDescription" "${NAME}: ${DESC}"
VIAddVersionKey "FileVersion" "${VER_STR}"
VIAddVersionKey "ProductVersion" "${VER_STR}"

!define UninstallKey "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}"

; check x64
Function .onInit
  ${IfNot} ${RunningX64}
    MessageBox MB_OK|MB_ICONEXCLAMATION "${NAME} can only be installed on 64-bit systems."
    Abort
  ${EndIf}
FunctionEnd

; interface settings
!define MUI_ABORTWARNING

!define MUI_ICON "..\..\assets\icons\schoenflies.ico"
!define MUI_UNICON "..\..\assets\icons\schoenflies.ico"

; pages
Var StartMenuFolder
!define MUI_FINISHPAGE_RUN "$INSTDIR\schoenflies.exe"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

; languages
!insertmacro MUI_LANGUAGE "English"

; installer sections
Section "Schoenflies"

  SetOutPath "$INSTDIR"
  SetRegView 64

  ; Schoenflies files
  File "schoenflies.exe"
  File /r "*.dll"

  WriteRegStr HKCU "Software\${NAME}" "" $INSTDIR  ; store installation folder
  WriteUninstaller "$INSTDIR\uninstall.exe"  ; create uninstaller

  !uninstfinalize 'python3 ../sign.py --cert "../../cert.p12" --pass "$%CERT_KEY_PASSWORD%" "%1"' = 0

  ; create start menu shortcuts (this block only runs when desired)
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application

    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortcut "$SMPROGRAMS\$StartMenuFolder\${NAME}.lnk" "$INSTDIR\schoenflies.exe"
    CreateShortcut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\uninstall.exe"

  !insertmacro MUI_STARTMENU_WRITE_END

  ; add to Add/Remove Programs
  ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
  IntFmt $0 "0x%08X" $0
  WriteRegStr HKLM "${UninstallKey}" "DisplayName" "${NAME}"
  WriteRegStr HKLM "${UninstallKey}" "DisplayIcon" "$\"$INSTDIR\schoenflies.exe$\""
  WriteRegStr HKLM "${UninstallKey}" "DisplayVersion" "${VER_STR}"
  WriteRegStr HKLM "${UninstallKey}" "Publisher" "${AUTHOR}"
  WriteRegStr HKLM "${UninstallKey}" "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
  WriteRegDWORD HKLM "${UninstallKey}" "EstimatedSize" "$0"
  WriteRegDWORD HKLM "${UninstallKey}" "NoModify" "1"
  WriteRegDWORD HKLM "${UninstallKey}" "NoRepair" "1"

SectionEnd

; uninstaller section
Section "Uninstall"

  SetRegView 64

  ; Schoenflies files
  Delete "$INSTDIR\schoenflies.exe"
  Delete "$INSTDIR\uninstall.exe"
  RMDir /r "$INSTDIR"

  ; start menu shortcuts
  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
  Delete "$SMPROGRAMS\$StartMenuFolder\${NAME}.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
  RMDir "$SMPROGRAMS\$StartMenuFolder"

  ; registry keys
  DeleteRegKey /ifempty HKCU "Software\${NAME}"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}"

SectionEnd
