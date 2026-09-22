//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gsize.h
///

#ifndef G_GUI_SIZE_H
#define G_GUI_SIZE_H

#include "gdef.h"
#include <iostream>

namespace GGui
{
	class Size ;
}

//| \class GGui::Size
/// A structure representing the size of a rectangle (typically
/// a GUI window).
///
class GGui::Size
{
public:
	unsigned long dx ;
	unsigned long dy ;
	Size() ;
	Size( unsigned long dx , unsigned long dy ) ;
	void streamOut( std::ostream & ) const ;
} ;

inline
GGui::Size::Size() :
	dx(0) ,
	dy(0)
{
}

inline
GGui::Size::Size( unsigned long dx_ , unsigned long dy_ ) :
	dx(dx_) ,
	dy(dy_)
{
}

inline
void GGui::Size::streamOut( std::ostream & s ) const
{
	s << "(" << dx << "," << dy << ")" ;
}

namespace GGui
{
	inline
	std::ostream & operator<<( std::ostream & stream , const Size & size )
	{
		size.streamOut( stream ) ;
		return stream ;
	}
}

#endif
