/*
	SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
	SPDX-License-Identifier: GPL-3.0-or-later
*/

//
// gstrmacros.h
//

#ifndef G_STR_MACROS_H
#define G_STR_MACROS_H

#ifdef G_STR_IMP
#undef G_STR_IMP
#endif
#ifdef G_STR
#undef G_STR
#endif
#ifdef G_STR_PASTE_IMP
#undef G_STR_PASTE_IMP
#endif
#ifdef G_STR_PASTE
#undef G_STR_PASTE
#endif

#define G_STR_IMP(a) #a
#define G_STR(a) G_STR_IMP(a)
#define G_STR_PASTE_IMP(a,b) a##b
#define G_STR_PASTE(a,b) G_STR_PASTE_IMP(a,b)

#endif
