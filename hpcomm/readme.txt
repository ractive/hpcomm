### WINDOWS 95 USERS **MUST** READ THIS FILE ###

=======================================================
Hewlett Packard Graphing Calculator PC Connectivity Kit
=======================================================
Release Notes         HPComm 3.0r3          23-Mar-2000
=======================================================

Welcome:
========

Hello and thank you for purchasing or downloading this
software.  It is the result of a partnership between
Hewlett Packard and the HPComm Project, and is being
released simultaneously by both parties.

This is version 3.0r3, which fixes a number of defects in
version 3.0r2.  Improvements continue to be made, so it
is very likely that an improved version of this software
will be available by the time you read this. 

A list of enhancements and bug fixes since HPComm 3.0r2
may be found at the end of this document.

You may download source code and free updates over the
Internet from the Hewlett Packard calculator website
(http://www.hp.com/calculators/) or from the HPComm
website (http://www.hpcomm.org).  The HPComm web site
also includes troubleshooting information, mailing lists,
answers to frequently asked questions, source code,
and more.

License, copying and warranty:
==============================

This software is licensed under the GNU General Public
License, and may be freely copied under certain
conditions.  Please see the COPYING.TXT file for the
license agreement.


WARNING: HEWLETT PACKARD PROVIDES THIS PROGRAM
         "AS IS" WITHOUT WARRANTY OF ANY KIND,
         EITHER EXPRESSED OR IMPLIED, INCLUDING,
         BUT NOT LIMITED TO, THE IMPLIED
         WARRANTIES OF MERCHANTABILITY AND
         FITNESS FOR A PARTICULAR PURPOSE, TO
         THE EXTENT PERMITTED BY APPLICABLE
         LAW.  THE ENTIRE RISK AS TO THE
         QUALITY AND PERFORMANCE OF THE PROGRAM
         IS WITH YOU.  SHOULD THE PROGRAM PROVE
         DEFECTIVE, YOU ASSUME THE COST OF ALL
         NECESSARY SERVICING, REPAIR OR
         CORRECTION.

         REGULAR BACKUPS OF BOTH YOUR PC AND
         CALCULATOR ARE STRONGLY ADVISED.


Having said that, Hewlett Packard and the HPComm Project
regularly update HPComm, and are very interested in fixing
all reported bugs.  See the HPComm website (http://www.hpcomm.org)
for more information.


System Requirements:
====================

 - An HP Graphing Calculator.  All calculators in
   the HP48 series, and the HP49G are supported.  The
   HP38G is not supported at this time.

 - This software is designed to run under Windows 95,
   Windows 98, Windows NT 4.0 and Windows 2000.  No other
   OS versions are supported at present.

 - Your display must be set to 256 colors or higher.
   Most modern displays are.

 - Your computer must have an unused 9-pin serial port,
   into which the transfer cable is plugged.
   Alternatively, you must have an unused 25-pin serial
   port and a 9-to-25-pin adaptor (not supplied).  

 - More details can be found in the online help.  Help
   files are provided in English, French, German,
   Portuguese and Spanish.



Known Problems with Windows 95:
===============================

Many computers running Windows 95 have a system library
which has bugs.  Symptoms of this are a wierd-looking
toolbar, and not being able to double-click on files or get
right-mousebutton menus.  If you're running Windows 95, we
recommend you upgrade this library.

This web page describes how to upgrade this library:

  http://support.microsoft.com/support/kb/articles/q186/1/76.asp

(More information can also be found here:)

  http://support.microsoft.com/support/downloads/dp2722.asp
  http://support.microsoft.com/support/kb/articles/Q165/4/87.asp

As a convenience to you, this CD includes the official
Microsoft upgrade for this system library.  The patch can be
found in the \msw95patch directory of the CD, and is called
"com32upd.exe".  We strongly suggest you read the README.TXT
file in that directory before you install it, as it's not
compatible with all versions of Microsoft Windows.

Contributions:
==============

This program is being released as "Open Source".  If you
like, you may download the source code that was used to
create it.  Also, you are very welcome to help improve
the program or report bugs.  Please visit the HPComm
website (http://www.hpcomm.org) for more information.

Bugs fixed since HPComm 3.0r2:
==============================


 - HPComm now works correctly when connected to an HP49G
   calculator set in French or Spanish mode. (Symptoms were
   object types and icons not correctly displayed).

 - HPComm has been updated with new icons for every possible
   official object type. No exceptions!

 - Removed the about box.  Instead, we reuse the splash
   screen.

 - Fixed a bug in the copying of files where a failure of the
   last item would cause a crash.

 - Added checking to see if an item can be dragged, to prevent
   problems if a PC drive or the calc HOME dir are dragged.

 - If an object could not be fetched from the calc (and hence
   it had no data) HPComm was creating a 0-byte file on the
   PC.  Fixed.

 - Enhanced the GROB-handling dialog and menus to have the
   same functionality as HPComm.  Also, fixed some bugs.

 - Added the ability to select the language of the help file.

 - Removed lots of unneeded files.

 - Oops, the home of HPComm is http://www.hpcomm.org, not
   http://www.hpcalc.org!  (Hi Eric!)

 - When we send the end-of-file packet, hang around to pick up
   the acknowledge packet from the calculator.

 - Fixed many bugs with renaming files.

 - Fixed some bugs with creating directories.

 - Added code to sort the checksum column on the calculator.

 - Added the hourglass cursor in more places.

 - If during a file transfer the connection gets lost and we
   get a RETRIES error we must clean up first, otherwise
   causes problems.

 - If a directory in the tree is renamed, ensure the list view 
   is updated as well.

 - Added code to scroll if dragging an object.

 END
