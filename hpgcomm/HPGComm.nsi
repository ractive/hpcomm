Name "HPGComm"
LicenseText "Please read and agree to this license before continuing."
LicenseData license.txt
ComponentText "This will install HPGComm on your system. Select which options you want set up."
DirText "Select a directory to install HPGComm in."
UninstallText "This will uninstall HPGComm. Hit Next to uninstall, or Cancel to cancel."
UninstallExeName uninst-HPGComm.exe
OutFile HPGComm2.exe
InstType Typical
EnabledBitmap Install.bmp
DisabledBitmap DisableInstall.bmp

InstallDir "$PROGRAMFILES\Hewlett Packard\HPGComm"
SetOverwrite on

Section "HPGComm (required)"
	SetOutPath $INSTDIR
	File Release\HPGComm.exe
	File ReadMe.txt
	WindowsVersion Win32 WinNT
	Win32:
		File Warning.txt
		File readme-win95.txt
		ExecWait '"$WINDIR\notepad.exe" "$INSTDIR\Warning.txt"'	
	WinNT:
	WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\HPGComm" "DisplayName" "HPGComm (remove only)"
	WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\HPGComm" "UninstallString" '"$INSTDIR\uninst-HPGComm.exe"'
	WriteRegStr HKEY_CURRENT_USER "Software\www.hpcomm.org" "HPGComm" "HPGComm"
SectionEnd

Section "View Readme.txt"
	SectionIn 1
	Exec '"$WINDIR\notepad.exe" "$INSTDIR\readme.txt"'
SectionEnd

Section "Add Icon to Start Menu"
	SectionIn 1
	CreateShortCut "$SMPROGRAMS\HPGComm.lnk" "$INSTDIR\HPGComm.exe" "" "" 0
SectionEnd

Section "Add Icon to Desktop"
	CreateShortCut "$DESKTOP\HPGComm.lnk" "$INSTDIR\HPGComm.exe" "" "" 0
SectionEnd

Section "Add Icon to Quicklauch Bar"
	CreateShortCut "$QUICKLAUNCH\HPGComm.lnk" "$INSTDIR\HPGComm.exe" "" "" 0
SectionEnd

Section "Launch HPGComm after installation"
	Exec '"$INSTDIR\HPGComm.exe"'
SectionEnd

Section Uninstall
	DeleteRegKey HKEY_CURRENT_USER "Software\www.hpcomm.org\HPGComm"
	DeleteRegKey HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\HPGComm"

	Delete "$DESKTOP\HPGComm.lnk"
	Delete "$SMPROGRAMS\HPGComm.lnk"
	Delete "$QUICKLAUNCH\HPGComm.lnk"
	Delete $INSTDIR\*.*
	RMDir $INSTDIR
	
	Delete "$SMPROGRAMS\HPGComm.lnk"
	Delete "$DESKTOP\HPGComm.lnk"
	Delete "$QUICKLAUNCH\HPGComm.lnk"
SectionEnd