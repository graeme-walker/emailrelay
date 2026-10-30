//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gqt.h
///

#ifndef G_MAIN_GUI_QT_H
#define G_MAIN_GUI_QT_H

#include "gdef.h"
#include "gpath.h"
#include <string>
#include <cstring>

#ifdef MemoryBarrier
#undef MemoryBarrier
#endif

#include <QtCore/QtCore>
#if QT_VERSION < 0x050000
#error Qt is too old
#endif
#include <QtGui/QtGui>
#include <QtWidgets/QtWidgets>
#include <QtCore/QtPlugin>

namespace GQt
{
	inline std::string u8string_from_qstring( const QString & q )
	{
		QByteArray a = q.toUtf8() ;
		return std::string( a.constData() , a.length() ) ;
	}

	inline QString qstring_from_u8string( const std::string & s )
	{
		return QString::fromUtf8( s.data() , static_cast<int>(s.size()) ) ;
	}

	inline QString qstring_from_path( const G::Path & p )
	{
		return QString::fromUtf8( p.cstr() ) ;
	}

	inline G::Path path_from_qstring( const QString & q )
	{
		return {u8string_from_qstring(q)} ;
	}
}

#endif
