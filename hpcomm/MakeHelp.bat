@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by HPCOMM.HPJ. >"hlp\HPComm.hm"
echo. >>"hlp\HPComm.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\HPComm.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\HPComm.hm"
echo. >>"hlp\HPComm.hm"
echo // Prompts (IDP_*) >>"hlp\HPComm.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\HPComm.hm"
echo. >>"hlp\HPComm.hm"
echo // Resources (IDR_*) >>"hlp\HPComm.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\HPComm.hm"
echo. >>"hlp\HPComm.hm"
echo // Dialogs (IDD_*) >>"hlp\HPComm.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\HPComm.hm"
echo. >>"hlp\HPComm.hm"
echo // Frame Controls (IDW_*) >>"hlp\HPComm.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\HPComm.hm"
REM -- Make help for Project HPCOMM


echo Building Win32 Help files
start /wait hcw /C /E /M "hlp\HPComm.hpj"
if errorlevel 1 goto :Error
if not exist "hlp\HPComm.hlp" goto :Error
if not exist "hlp\HPComm.cnt" goto :Error
echo.
if exist Debug\nul copy "hlp\HPComm.hlp" Debug
if exist Debug\nul copy "hlp\HPComm.cnt" Debug
if exist Release\nul copy "hlp\HPComm.hlp" Release
if exist Release\nul copy "hlp\HPComm.cnt" Release
echo.
goto :done

:Error
echo hlp\HPComm.hpj(1) : error: Problem encountered creating help file

:done
echo.
