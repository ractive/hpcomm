======================================================================
 HPGComm Version 2b5 (15.02.2001)
======================================================================

Dear HP calculator users

It has been a long time since the last major update of HPComm38 and
since then HP has introduced the HP39G/40G successors to the HP38G. To
support the new calculators, HPComm38 has now been modified and been
given a new name: HPGComm.

HPGComm has become even more user-friendly and easier to use. New
features have been added and some bugs were fixed. After internal
testing we now want to see how the new program works for you, the user.
For this reason we are lauching this beta-test.

We encourage you to take part and give us your feedback! First of all
it's not possible for us to test everything on every system. Even more
important, we want to know if this software fits your needs.

======================================================================
How to take part
======================================================================

Download the program file and put it anywhere you want, but preferably
in the same directory as HPComm38.exe

Play around with the program and report to us any errors and/or bugs
that you find. Please also give us feedback about the functionality,
the look and feel and how user-friendly it is.
Are you missing a feature? No problem. Drop us a note about what ideas
you have and what you would like to see in the program.

Subscribe yourself to the HPComm mailinglist at:
http://gpl.ractive.ch/mailinglist.html to get the latest news about
this beta test.

======================================================================
!!!!! ATTENTION !!!!!
======================================================================

THERE IS A BUG LEFT IN THE PROGRAM WHICH COULD NOT BE FIXED YET. IF YOU
ARE RUNNING WINDOWS 95/98 IT CAN HAPPEN THAT HPGCOMM OR EVEN THE
OPERATING SYSTEM CRASHES!

So make sure you saved all open documents or ever better, close all
other programs. Otherwise, data may be lost!

This only happens if an ongoing file transfer is interrupted, i.e. by
pulling out the cable, or by pressing CANCL on the calculator in mid
transfer. Windows NT/2000 is not affected by this.

We'd be glad to hear if this also happens on your computer! It will
help us to find out if this is a problem of HPGComm or a bug in Windows
95/98!

So if you know what you are doing and you are using a Windows 95/98
system, you may wish to take these steps to reproduce the bug and help
us to fix this:

- Start the program.
- Start the download of a file to your calculator.
- During the file transfer, pull out the cable.
- HPGComm will report that the calc does not answer any more and ask
  you if you want to close the HPGComm. Choose "No".
- Plug in the cable and start the file transfer again.
- If the bug is present then the second attempt at file transfer will
  also immediately fail despite the cable being plugged in.
- Close HPGComm by clicking on the close button at the top right of the
  window.
- Now, a bluescreen with a "Fatal Exception (0E)" may appear

AGAIN! If you do this, your operating system may crash! We won't take
any responsibility if you lose data!

If you tried it, please report this to us. Include the following
information:

- Did the operation system crash?
- Operating system (Windows 95/98)
- Language of the operating system
- Processor used
- Do you have other software installed that accesses the COM-Port,
  like voice mail software etc.?
 
======================================================================
How to contact us
======================================================================

You can write us at:

Jean-Pierre Bergamin (the author):   james@ractive.ch
Colin Croft (The HP38G HOME view):   ccroft@iinet.net.au
             http://members.iinet.net.au/~ccroft

More information can be found here:
http://www.hpcomm.org


======================================================================
Major changes to HPComm38
======================================================================
- Support of HP 39G and 40G calculators
- Nicer layout :-)
- The ability to create directories on the PC from within HPGComm
- The last six used directories with calculator files are stored
  for easy access
- File transfers can be cancelled
- Better error handling of directory files
- Better overall error handling
- Other bug-fixes
======================================================================


======================================================================
Changes to Beta 2 (06.01.2001)
======================================================================
- Tooltips for disabled buttons
- Menu enries grayed out when not needed
- Crash when trying to create a directory via menu. Fixed.
======================================================================
Changes to Beta 3 (09.01.2001)
======================================================================
- Progress bar added
- Nicer about-box ;-)
- Tried to implement XModem (sending to calc only). See Below...
======================================================================
Changes to Beta 4 (15.01.2001)
======================================================================
- Improved multithreading. This speeds things up to 30% ;-)
  The program changed quite a bit internally. I hope I did not intro-
  duce new bugs...
- Transfer details like BPS, CPS etc. added
- XModem also multithreaded
======================================================================
Changes to Beta 4 (15.01.2001)
======================================================================
- If you created (many :-) screenshot, they could be created out of
  scope. This is fixed now
- To copy a screenshot to the clipboard you can now press Ctrl-C
- A new option File-Open is introduced in the screenshot menu.
  It opens the current-screenshot in your registered drawing program.
======================================================================


======================================================================
XModem
======================================================================
We (me and Colin) decided not to support XModem with more effort.
You can at least download (some special) aplets to your calc.
The aplets must be present in binary format. This is the case for most
all for the games out there ;-)
======================================================================


We hope to hear from you! Take the opportunity to co-design your
software...

Regards

Jean-Pierre Bergamin
Colin Croft