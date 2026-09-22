//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gfilterfactory.h
///

#ifndef G_FILTER_FACTORY_H
#define G_FILTER_FACTORY_H

#include "gdef.h"
#include "gfilterfactorybase.h"
#include "gfilestore.h"
#include "gpath.h"
#include "gstringview.h"
#include <string>
#include <utility>
#include <memory>
#include <cstddef> // std::nullptr_t

namespace GFilters
{
	class FilterFactory ;
}

//| \class GFilters::FilterFactory
/// A FilterFactory implementation. It holds a GSmtp::FileStore reference
/// so that it can instantiate filters that operate on messages stored as
/// files.
///
class GFilters::FilterFactory : public GSmtp::FilterFactoryBase
{
public:
	explicit FilterFactory( GStore::FileStore & ) ;
		///< Constructor. The FileStore reference is retained and passed
		///< to new filter objects so that they can derive the paths of
		///< the content and envelope files that they process.

	static Spec parse( std::string_view spec , const G::Path & base_dir = {} ,
		const G::Path & app_dir = {} , G::StringArray * warnings_p = nullptr ) ;
			///< Parses and validates the filter specification string returning
			///< the type and value in a Spec tuple, eg. ("file","/usr/bin/foo")
			///< or ("net","127.0.0.1:99").
			///<
			///< The specification string can be a comma-separated list with
			///< the component parts checked separately and the returned Spec
			///< is like ("chain","file:foo,net:bar").
			///<
			///< Any relative file paths are made absolute using the given
			///< base directory, if given. (This is normally from
			///< G::Process::cwd() called at startup).
			///<
			///< Any "@app" sub-strings in file paths are substituted with
			///< the given application directory, if given.
			///<
			///< Returns 'first' empty if a fatal parsing error, with the
			///< reason in 'second'.
			///<
			///< Returns warnings by reference for non-fatal errors, such
			///< as missing files.

public:
	~FilterFactory() override = default ;
	FilterFactory( const FilterFactory & ) = delete ;
	FilterFactory( FilterFactory && ) = delete ;
	FilterFactory & operator=( const FilterFactory & ) = delete ;
	FilterFactory & operator=( FilterFactory && ) = delete ;

protected: // overrides
	std::unique_ptr<GSmtp::Filter> newFilter( GNet::EventState ,
		GSmtp::Filter::Type , const GSmtp::Filter::Config & ,
		const Spec & ) override ;

private:
	static void checkNumber( Spec & ) ;
	static void checkNet( Spec & ) ;
	static void checkRange( Spec & ) ;
	static void checkFile( Spec & , G::StringArray * ) ;
	static void fixFile( Spec & , const G::Path & , const G::Path & ) ;

private:
	GStore::FileStore & m_file_store ;
} ;

#endif
