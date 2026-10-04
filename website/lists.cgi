#! /usr/local/bin/perl -w

use integer;
use strict;

use CGI;

# ------------------------------------------------------------

my(%formField) =
(
	name	=> '',
	email	=> '',
	list	=> '',
);

# ------------------------------------------------------------

# The next line is needed inside Cookie.pm.
$ENV{'SCRIPT_NAME'}	= $0;
my($cgi)			= new CGI;
# my($action)			= $cgi -> param('action');

# Recover the data, if any, from the current form.
my($key, %formData);

for $key (keys(%formField) )
{
	$formData{$key} = $cgi -> param($key);
}

my($name)	= $formData{'name'};
my($email)	= $formData{'email'};
my($list)	= $formData{'list'};

print <<EOF;
Content-type: text/html

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<html>
  <head>
    <title>www.HPComm.org easy-subscribe</title>
  </head>

<BODY bgcolor="#ffffff" text="#000000" link="#0000ff" vlink="#aa00aa">
<table width="100%"><tr><td bgcolor="#6699ff"><font size="-2" color="#6699ff">_</font></td></tr></table>
<font size="+4">www.HPComm.org easy-subscribe</font>
<table width="100%"><tr><td bgcolor="#6699ff" align="right"><font size="-2" color="#3300cc">
<!-- Created: Fri Sep 17 12:16:14 EST 1999 -->
<!-- hhmts start -->
Last modified: Fri Oct 15 11:13:41 EST 1999
<!-- hhmts end -->
</font></td></tr></table>

EOF

if ($email eq '' || $list eq '')
{
  print "not allowed.";

  exit(1);
}

if ($name ne '')
{
  print "<p> Hi $name,";
}

print <<EOF;

<p> Thanks for clicking this link.  If you like, I
can subscribe you to the "$list" list, using "$email"
as your email address.

<p> If you choose to subscribe, you'll shortly get a confirmation email.
To complete subscription, you <em>must</em> reply to that email.

<table>
<tr>
 <td>
  <form action="http://www.complete.org/cgi-bin/listargate.cgi" method=POST>
   <input type=hidden name="user" value="$email">
   <input type=hidden name="lsg-mode" value="subscribe">
   <input type=hidden name="lsg-list" value="$list">
   <input type=submit value="Subscribe">
  </form>
 </td>
 <td>
  <form action="http://www.hpcomm.org/lists.html">
   <input type=submit value="Visit HPComm lists page">
  </form>
 </td>
 <td>
  <form action="http://www.hpcomm.org/">
   <input type=submit value="Visit HPComm main page.">
  </form>
 </td>
</tr>
</table>

After subscribing, send your mail to 
<a href="mailto:$list\@complete.org">$list\@complete.org</a>, and it will
appear on the list.

<p> See you soon!

<p> Regards,

<p> <a href="mailto:mjd\@hpcomm.org">Mitch Davis</a>.
<hr>
</body>
</html>
EOF
