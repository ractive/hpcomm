======================================================================
HPGComm Version 2.01 (1.07.2001)
======================================================================

Dear HP calculator users

It has been a long time since the last major update of HPComm38 and
since then HP has introduced the HP39G/40G successors to the HP38G. To
support the new calculators, HPComm38 has now been modified and been
given a new name: HPGComm.

HPGComm has become even more user-friendly and easier to use. New
features have been added and some bugs were fixed.

After a quite long beta-test phase we decided to release the final
version 2.0.

If you find any bugs, please report them (address below).

======================================================================
Major changes to HPComm38
======================================================================
- Support of HP 39G and 40G calculators added
- Nicer layout :-)
- The ability to create directories on the PC from within HPGComm
- The last six directories used can be recalled for easy re-use
- File transfers can be cancelled at the PC end
- Better error handling of directory files
- Better overall error handling
- Other bug-fixes
- XModem support (see below)

======================================================================
Changes in 2.01
======================================================================
- Fixed a bug where an aplet with many programs/libraries failed to 
  download completely


======================================================================
XModem  (for HP39G and HP40G only)
======================================================================
HPGComm now supports the uploading of Aplets to the calculator by
XModem for HP39G and HP40G calculators. Only specially prepared aplets 
can be tranferred by XModem. The aplets must be present in binary 
format. This is the case for most of the games out there ;-)

To upload an aplet by XModem, choose "Recv" on the calculator, then
"HP39/40 (Wire)". HPGComm opens a dialog where you can choose your
aplet. When you click on OK, the XModem transfer starts.
If the aplet is not in binary format the calculator reports "Transfer
Failed". In this case you have to upload in the normal way.

We (Colin and I) decided not to support XModem beyond this basic level
(i.e. downloading stuff to the PC etc.), because of lack of support
from HP and lack of any feedback from users of the need for anything 
more. You can at least upload most of the games (which is probably 
the main reason to use XModem :-)

======================================================================
!!!!! ATTENTION !!!!!
======================================================================

THERE IS A BUG LEFT IN THE PROGRAM WHICH COULD NOT BE FIXED YET. IF YOU
ARE RUNNING WINDOWS 95/98/ME IT MAY HAPPEN THAT HPGCOMM OR EVEN THE
OPERATING SYSTEM CRASHES UNDER CERTAIN RARE CIRCUMSTANCES!
Windows NT/2000 is not affected by this!

So make sure you saved all open documents or, even better, close all
other programs. Otherwise, data may possibly be lost!

NOTE: This ONLY HAPPENS if an ongoing file transfer is interrupted, 
i.e. by pulling out the cable, or by pressing CANCEL on the calculator 
in mid transfer.

HPGComm detects this situation and advises you to close the application
before doing any further file transfers. To avoid any risk of a crash, hit 
"Yes" and restart the program. If you do this, the bug won't show up!

We'd be glad to hear if this also happens on your computer! It will
help us to find out if this is a problem with HPGComm or a bug in Windows
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

- Did the operating system crash?
- Operating system in use (Windows 95/98/ME)
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


Regards

Jean-Pierre Bergamin
Colin Croft