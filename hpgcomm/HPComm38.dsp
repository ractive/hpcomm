# Microsoft Developer Studio Project File - Name="HPComm38" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=HPComm38 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "HPComm38.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "HPComm38.mak" CFG="HPComm38 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "HPComm38 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "HPComm38 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "HPComm38 - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 imagehlp.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "HPComm38 - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 imagehlp.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "HPComm38 - Win32 Release"
# Name "HPComm38 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AboutDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GrobDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\HelpFileDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\HPComm38.cpp
# End Source File
# Begin Source File

SOURCE=.\HPComm38.rc
# End Source File
# Begin Source File

SOURCE=.\HPComm38Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\HyperLink.cpp
# End Source File
# Begin Source File

SOURCE=.\Image.cpp
# End Source File
# Begin Source File

SOURCE=.\Kermit.cpp
# End Source File
# Begin Source File

SOURCE=.\NewVDNameDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PicHolder.cpp
# End Source File
# Begin Source File

SOURCE=.\Prot.cpp
# End Source File
# Begin Source File

SOURCE=.\ProtException.cpp
# End Source File
# Begin Source File

SOURCE=.\RegBinding.cpp
# End Source File
# Begin Source File

SOURCE=.\ResizeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Serial.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TransferInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\VD.cpp
# End Source File
# Begin Source File

SOURCE=.\VDDlg.cpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\38g_image.bmp
# End Source File
# Begin Source File

SOURCE=.\res\calc.ico
# End Source File
# Begin Source File

SOURCE=.\res\changedi.ico
# End Source File
# Begin Source File

SOURCE=.\res\hideinfo.ico
# End Source File
# Begin Source File

SOURCE=.\res\hp38dir.ico
# End Source File
# Begin Source File

SOURCE=.\res\hp39dir.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\magni.cur
# End Source File
# Begin Source File

SOURCE=.\res\newcalcimage.bmp
# End Source File
# Begin Source File

SOURCE=.\res\newdir.bmp
# End Source File
# Begin Source File

SOURCE=.\res\newdir.ico
# End Source File
# Begin Source File

SOURCE=.\res\nocalc.bmp
# End Source File
# Begin Source File

SOURCE=.\res\spot38.ico
# End Source File
# Begin Source File

SOURCE=.\res\vditems.bmp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\AboutDlg.h
# End Source File
# Begin Source File

SOURCE=.\GrobDlg.h
# End Source File
# Begin Source File

SOURCE=.\HelpFileDlg.h
# End Source File
# Begin Source File

SOURCE=.\HPComm38.h
# End Source File
# Begin Source File

SOURCE=.\HPComm38Dlg.h
# End Source File
# Begin Source File

SOURCE=.\HyperLink.h
# End Source File
# Begin Source File

SOURCE=.\Image.h
# End Source File
# Begin Source File

SOURCE=.\Kermit.h
# End Source File
# Begin Source File

SOURCE=.\NewVDNameDlg.h
# End Source File
# Begin Source File

SOURCE=.\PicHolder.h
# End Source File
# Begin Source File

SOURCE=.\Prot.h
# End Source File
# Begin Source File

SOURCE=.\ProtException.h
# End Source File
# Begin Source File

SOURCE=.\RegBinding.h
# End Source File
# Begin Source File

SOURCE=.\ResizeDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Serial.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TransferInfo.h
# End Source File
# Begin Source File

SOURCE=.\VD.h
# End Source File
# Begin Source File

SOURCE=.\VDDlg.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\calctrans.avi
# End Source File
# Begin Source File

SOURCE=.\res\dog.avi
# End Source File
# Begin Source File

SOURCE=.\res\dog.wav
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\res\swap.avi
# End Source File
# End Target
# End Project
