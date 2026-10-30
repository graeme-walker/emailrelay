//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file guilink.h
///

#ifndef GUI_LINK_H
#define GUI_LINK_H

#include "gdef.h"
#include "gpath.h"
#include "gexception.h"
#include "gstringarray.h"
#include <string>
#include <memory>

namespace Gui
{
	class Link ;
	class LinkImp ;
}

//| \class Gui::Link
/// A class for creating desktop links (aka "shortcuts") and
/// application menu items.
///
class Gui::Link
{
public:
	G_EXCEPTION( SaveError , tx("error saving desktop or menu link") )

	enum class Show { Default , Hide } ;

	Link( const G::Path & target_path , const std::string & name , const std::string & description ,
		const G::Path & working_dir , const G::StringArray & args = G::StringArray() ,
		const G::Path & icon_source = G::Path() , Show show = Show::Default ,
		const std::string & internal_comment_1 = {} ,
		const std::string & internal_comment_2 = {} ,
		const std::string & internal_comment_3 = {} ) ;
			///< Constructor. Note that the path of the link itself
			///< is specified in saveAs(), not the constructor.
			///< The "working_dir" is the current-working-directory
			///< when the link is used.

	static std::string filename( const std::string & name ) ;
		///< Returns a normalised filename including an extension like ".lnk" or ".desktop".

	void saveAs( const G::Path & link_path ) ;
		///< Saves the link.

	~Link() ;
		///< Destructor.

	static bool remove( const G::Path & link_path ) ;
		///< Removes a link. Returns true if removed.

	static bool exists( const G::Path & link_path ) ;
		///< Returns true if the link exists.

	static bool exists( const G::Path & dir , const std::string & link_name ) ;
		///< Returns true if the link exists.

public:
	Link( const Link & ) = delete ;
	Link( Link && ) = delete ;
	Link & operator=( const Link & ) = delete ;
	Link & operator=( Link && ) = delete ;

private:
	std::unique_ptr<LinkImp> m_imp ;
} ;

inline
bool Gui::Link::exists( const G::Path & dir , const std::string & link_name )
{
	return !dir.empty() && !link_name.empty() && exists( dir / link_name ) ;
}

#endif
