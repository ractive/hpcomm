v1.0: 8-Sep-1999

Many computers running Windows 95 have a system library
which has bugs.  Symptoms of this are a wierd-looking
toolbar, and not being able to double-click on files or get
right-mousebutton menus.

### DO NOT APPLY THIS PATCH UNLESS YOUR SYSTEM ###
### EXACTLY MEETS THE DESCRIPTION BELOW.       ###

The following text is reproduced from the Microsoft
website, and will help you determine whether your system
needs upgrading.  It is provided here in case you do not
have access to the Internet.

<start of Microsoft text>

  http://support.microsoft.com/support/kb/articles/q186/1/76.asp

  o To use the latest versions of the common controls with
    Windows, you must have the latest version of
    Comctl32.dll installed. You can obtain and distribute
    the latest version of Comctl32.dll appropriate to a
    particular operating system in either one of the
    following ways:

        In all cases, you can upgrade Comctl32.dll by
        installing the latest version of Internet Explorer
        (currently 5) intended for the particular platform:

           http://www.microsoft.com/windows/ie/download/windows.htm

    This is the preferred method of upgrading Comctl32.dll
    for all platforms.

  o You can use the file 401comupd.exe to install
    Comctl32.dll (version 4.72.3611.1900) on Windows 95,
    Windows 98, and Windows NT 4.0. You cannot use
    401comupd.exe for Windows 3.x, Windows for Workgroups
    3.x, Windows NT 3.x, Windows 98 SE, or Windows 2000. Do
    not attempt to run 401comupd.exe on any system with
    Internet Explorer 5.x installed. You must install
    Service Pack 3 or later on Windows NT 4.0 before
    installing 401comupd.exe. As with all operating system
    changes, administrator privileges are required to use
    401comupd.exe on Windows NT 4.0.

<end of Microsoft text>

Note: We suggest applying the patch below, not 401comupd.exe.

<start of Microsoft text>

  http://support.microsoft.com/support/kb/articles/Q165/4/87.asp

  This issue is resolved by the following updated file for Windows 95: 

     Comctl32.dll version 4.70 (dated 5/19/97) and later 

  Microsoft does not allow the separate redistribution of
  the Comctl32.dll file at this time. Note that Microsoft
  does allow the redistribution of the complete
  self-extracting archive file Com32upd.exe.

<end of Microsoft text>

If you have Internet access, we suggest you visit the
Microsoft website at the above addresses to find full
information on correcting this problem.


END
