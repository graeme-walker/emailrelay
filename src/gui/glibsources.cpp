//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file glibsources.cpp
///
// These are the source files that are shared between the gui and the
// other executables, so this file can be compiled with Qt-friendly
// compiler flags without affecting the rest of the build.
//

#include "gdef.h"

#ifdef G_WINDOWS
#include "genvironment.cpp"
#include "genvironment_win32.cpp"
#include "gdatetime.cpp"
#include "ggettext_win32.cpp"
#else
#include "genvironment.cpp"
#include "genvironment_unix.cpp"
#include "gdatetime.cpp"
#include "ggettext_unix.cpp"
#endif

#include "garg.cpp"
#include "gbase64.cpp"
#include "gbatchfile.cpp"
#include "gcodepage.cpp"
#include "gconvert.cpp"
#include "gdate.cpp"
#include "gdirectory.cpp"
#include "gexception.cpp"
#include "gexecutablecommand.cpp"
#include "gfile.cpp"
#include "gformat.cpp"
#include "ggetopt.cpp"
#include "ghash.cpp"
#include "glog.cpp"
#include "glogstream.cpp"
#include "glogoutput.cpp"
#include "gmapfile.cpp"
#include "gmd5.cpp"
#include "goption.cpp"
#include "goptionmap.cpp"
#include "goptionparser.cpp"
#include "goptionreader.cpp"
#include "goptions.cpp"
#include "goptionsusage.cpp"
#include "gpath.cpp"
#include "groot.cpp"
#include "gstr.cpp"
#include "gstringview.cpp"
#include "gstringwrap.cpp"
#include "gtest.cpp"
#include "gtime.cpp"
#include "gxtext.cpp"
#include "options.cpp"
#ifdef G_WINDOWS
#include "gcleanup_win32.cpp"
#include "gdirectory_win32.cpp"
#include "gfile_win32.cpp"
#include "gidentity_win32.cpp"
#include "glogoutput_win32.cpp"
#include "gnewprocess_win32.cpp"
#include "gprocess_win32.cpp"
#include "servicecontrol_win32.cpp"
#else
#include "gcleanup_unix.cpp"
#include "gdirectory_unix.cpp"
#include "gfile_unix.cpp"
#include "gidentity_unix.cpp"
#include "glogoutput_unix.cpp"
#include "gnewprocess_unix.cpp"
#include "gprocess_unix.cpp"
#include "servicecontrol_unix.cpp"
#endif
