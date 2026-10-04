# HPComm — PC connectivity software for HP graphing calculators

HPComm is a family of Windows programs for transferring files between a PC
and Hewlett-Packard graphing calculators over a serial cable. This repository
archives the source code, help files and website of the HPComm project
(1997–2003) by Jean-Pierre "James" Bergamin, with contributions by Mitch Davis
and Colin Croft.

Two programs were produced:

- **HPComm** talks to the HP48 series and the HP49G using the Kermit protocol.
  It presents the calculator's directory tree and the PC file system side by
  side in an Explorer-like window with drag-and-drop.
- **HPComm38**, later renamed **HPGComm**, talks to the HP38G, HP39G and HP40G
  (Kermit, plus XModem for uploading binary aplets to the HP39G/HP40G).

All code is released under the GNU General Public License, version 2.

## History

HPComm started in 1997 as an open-source replacement for HP's own PC link
software and grew into the HPComm Project at www.hpcomm.org, run by James
Bergamin and Mitch Davis. In 1999 Hewlett-Packard partnered with the project
to ship HPComm as the official connectivity software. The release notes of
HPComm 3.0r3 (`hpcomm/readme.txt`) put it this way:

> Hewlett Packard Graphing Calculator PC Connectivity Kit
> Release Notes         HPComm 3.0r3          23-Mar-2000
>
> Hello and thank you for purchasing or downloading this
> software.  It is the result of a partnership between
> Hewlett Packard and the HPComm Project, and is being
> released simultaneously by both parties.
>
> This is version 3.0r3, which fixes a number of defects in
> version 3.0r2.  Improvements continue to be made, so it
> is very likely that an improved version of this software
> will be available by the time you read this.

The same file lists the bugs fixed since HPComm 3.0r2:

> - HPComm now works correctly when connected to an HP49G
>   calculator set in French or Spanish mode. (Symptoms were
>   object types and icons not correctly displayed).
> - HPComm has been updated with new icons for every possible
>   official object type. No exceptions!
> - Removed the about box.  Instead, we reuse the splash
>   screen.
> - Fixed a bug in the copying of files where a failure of the
>   last item would cause a crash.
> - Added checking to see if an item can be dragged, to prevent
>   problems if a PC drive or the calc HOME dir are dragged.
> - If an object could not be fetched from the calc (and hence
>   it had no data) HPComm was creating a 0-byte file on the
>   PC.  Fixed.
> - Enhanced the GROB-handling dialog and menus to have the
>   same functionality as HPComm.  Also, fixed some bugs.
> - Added the ability to select the language of the help file.
> - Removed lots of unneeded files.
> - Oops, the home of HPComm is http://www.hpcomm.org, not
>   http://www.hpcalc.org!  (Hi Eric!)
> - When we send the end-of-file packet, hang around to pick up
>   the acknowledge packet from the calculator.
> - Fixed many bugs with renaming files.
> - Fixed some bugs with creating directories.
> - Added code to sort the checksum column on the calculator.
> - Added the hourglass cursor in more places.
> - If during a file transfer the connection gets lost and we
>   get a RETRIES error we must clean up first, otherwise
>   causes problems.
> - If a directory in the tree is renamed, ensure the list view
>   is updated as well.
> - Added code to scroll if dragging an object.

The hpcomm.org front page (archived in `website/index.html`) described the
project in mid-2000:

> HPComm and HPComm38 are free programs which allow you to transfer data
> between a PC, and HP graphing calculators. HPComm and HPComm38 were
> developed by James Bergamin and Mitch Davis. HPComm and HPComm38 are also
> distributed by Hewlett Packard, as the official software in the "HP
> Graphing Calculator PC Connectivity Kit".

The last versions published on hpcomm.org were HPComm 3.0r4 and
HPComm38 1.0r4 (2 June 2000). HPComm38 was then reworked for the HP39G and
HP40G and renamed HPGComm. Its README (`hpgcomm/README.TXT`) explains:

> HPGComm Version 2.01 (1.07.2001)
>
> It has been a long time since the last major update of HPComm38 and
> since then HP has introduced the HP39G/40G successors to the HP38G. To
> support the new calculators, HPComm38 has now been modified and been
> given a new name: HPGComm.
>
> Major changes to HPComm38
> - Support of HP 39G and 40G calculators added
> - Nicer layout :-)
> - The ability to create directories on the PC from within HPGComm
> - The last six directories used can be recalled for easy re-use
> - File transfers can be cancelled at the PC end
> - Better error handling of directory files
> - Better overall error handling
> - Other bug-fixes
> - XModem support (see below)
>
> Changes in 2.01
> - Fixed a bug where an aplet with many programs/libraries failed to
>   download completely

HPGComm went through eight betas between January and April 2001; version 2.0
was released on 12 June 2001 and 2.01 on 1 July 2001. The last edit to the
HPGComm sources dates from May 2003.

## Repository layout

| Path | Contents |
| --- | --- |
| `hpcomm/` | HPComm 3.0r3 for the HP48 series and HP49G. Working copy from the hpcomm.org CVS repository (branch `TR3_0_R2_FIX`), with the WinHelp source in `hlp/`, the project plan and issue list in `doc/`, and the original `readme.txt`, `readme-win95.txt` and `copying.txt`. |
| `hpgcomm/` | HPGComm 2.01 for the HP38G, HP39G and HP40G (project files are still named `HPComm38.*`). Includes the HTML Help source and compiled-help inputs in `help/`, the NSIS installer script `HPGComm.nsi`, `README.TXT`, `Warning.txt` and `license.txt`. |
| `hpgcomm/help-src/` | An earlier, separate copy of the HTML Help source (`HPGComm.hhp`). |
| `hpgcomm/DLLTest/` | A small MFC DLL test project (June 2001). |
| `hpcommshell/` | HPCommShell, an unfinished Windows Explorer namespace extension (June 2001) that was meant to show the calculator as a folder inside Explorer. Reuses the Kermit and protocol classes from HPComm. |
| `website/` | The www.hpcomm.org website as kept in CVS: HTML pages, the FAQ source (`faq.dat`, built by `makefaq`), the mailing-list CGI and the screenshots in `images/`. |
| `docs/screenshots/` | Screenshots from the website and the HPGComm help, as the original GIFs and as PNG conversions. |
| `LICENSE` | GNU General Public License, version 2 (copy of `hpcomm/copying.txt`). |

Compiled binaries, help files (`.HLP`, `.chm`), Visual C++ workspace state
(`.ncb`, `.opt`, `.plg`, `.aps`, `.clw`), help-compiler logs and the `CVS/`
metadata directories were left out of the archive. Line endings are stored
byte-for-byte as they were on disk (`.gitattributes` sets `* -text`); most
files use CRLF.

## Screenshots

HPComm 3.0 (HP48/HP49), from the hpcomm.org website:

![HPComm main window](docs/screenshots/ss1.png)

HPComm38 1.0 (HP38G) sending a file, with the file tree and two captured
calculator screens, from the hpcomm.org website:

![HPComm38 sending a file](docs/screenshots/ss2.png)

HPGComm 2.0 (HP38G/39G/40G), from the HPGComm online help:

![HPGComm main window](docs/screenshots/HPGComm1.png)

![HPGComm screen capture window](docs/screenshots/HPGComm2.png)

![HPGComm options dialog](docs/screenshots/options1.png)

![HPGComm start menu entries](docs/screenshots/strtmenu.png)

## Building

All three programs are Microsoft Visual C++ 6.0 / MFC projects. Open the
workspace in Visual C++ 6 and build the `Release` or `Debug` configuration:

| Program | Workspace |
| --- | --- |
| HPComm | `hpcomm/HPComm.dsw` |
| HPGComm | `hpgcomm/HPComm38.dsw` |
| HPCommShell | `hpcommshell/HPCommShell.dsw` |
| DLLTest | `hpgcomm/DLLTest/DLLTest.dsw` |

Notes:

- HPComm's WinHelp file is built by `hpcomm/MakeHelp.bat` from
  `hpcomm/hlp/HPComm.hpj`; it needs the Help Compiler (`hcw.exe`) that shipped
  with Visual C++ 6. The `.hpj` file contains an absolute include path to the
  Visual Studio installation that must be adapted.
- HPGComm's help is an HTML Help project (`hpgcomm/help/HPGComm.hhp`) compiled
  with Microsoft HTML Help Workshop; the installer is an NSIS script
  (`hpgcomm/HPGComm.nsi`).
- The code targets Windows 95/98/ME/NT 4/2000. It compiles with later
  Visual Studio versions only after porting work (MFC and Win32 API changes),
  which has not been done.

## History of this repository

The `hpgcomm/` and `website/` directories carry the original CVS history of
the HPGComm project on SourceForge (six commits from 6 January to
15 February 2001, tag `V_2b5`) and of the hpcomm.org website (two commits
from 4 January 2001). Those two CVS modules were converted to Git, moved into
their subdirectories and merged.

Everything else was imported from archived working copies, one snapshot
commit per tree, with the author date set to the newest file modification
time in the snapshot:

1. HPGComm 2.0 beta 5 working copy (February 2001)
2. HPComm 3.0r3 (working copy on the `TR3_0_R2_FIX` CVS branch, last edited
   September 2001)
3. HPCommShell (June 2001)
4. HPGComm 2.01 final, help source and DLLTest (July 2001 to May 2003)

The earlier CVS history of HPComm itself (1997–2000, hosted on hpcomm.org)
has not been recovered.

## Credits

- **Jean-Pierre "James" Bergamin** — author of HPComm, HPComm38, HPGComm and
  HPCommShell.
- **Mitch Davis** — co-developer of HPComm and HPComm38, project planning and
  release management for the HP partnership, and maintainer of the hpcomm.org
  website.
- **Colin Croft** — the HP38G HOME view in HPGComm, testing, and co-author of
  the HPGComm README.
- **Hewlett-Packard** — partnered with the HPComm Project to distribute HPComm
  as the "HP Graphing Calculator PC Connectivity Kit".

## License

GNU General Public License, version 2. See [LICENSE](LICENSE) and
the original license files `hpcomm/copying.txt` and `hpgcomm/license.txt`.
