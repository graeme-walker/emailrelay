//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file glog.h
///

#ifndef G_LOG_H
#define G_LOG_H

#include "gdef.h"
#include "glogstream.h"
#include "glogoutput.h"
#include "gloggroup.h"
#include "gstrmacros.h"

// NOLINTBEGIN(*-macro-perentheses)
#define G_LOG_IMP( expr , severity , mvarg , cond ) do { if(G::LogOutput::Instance::at(severity,mvarg,cond)) { auto log_stream = G::LogOutput::Instance::start((severity),__FILE__,__LINE__) ; log_stream << expr ; G::LogOutput::Instance::output(log_stream) ; } } while(0)
#define G_LOG_IMP_ONCE( expr , severity , mvarg ) do { static bool once__ = true ; if(G::LogOutput::Instance::at(severity,mvarg,&once__)) { auto log_stream = G::LogOutput::Instance::start((severity),__FILE__,__LINE__) ; log_stream << expr ; G::LogOutput::Instance::output(log_stream) ; } } while(0)
// NOLINTEND(*-macro-perentheses)

#if defined(G_WITH_DEBUG) || ( defined(_DEBUG) && ! defined(G_NO_DEBUG) )
#define G_DEBUG( expr ) G_LOG_IMP( expr , G::LogOutput::Severity::Debug , 0 , true )
#define G_DEBUG_IF( cond , expr ) G_LOG_IMP( expr , G::LogOutput::Severity::Debug , 0 , cond )
#else
#define G_DEBUG( expr )
#define G_DEBUG_IF( cond , expr )
#endif

#if ! defined(G_NO_LOG)

#define G_LOG( expr ) G_LOG_IMP( expr , G::LogOutput::Severity::InfoVerbose , 0 , true )
#define G_LOG_IF( cond , expr ) G_LOG_IMP( expr , G::LogOutput::Severity::InfoVerbose , 0 , cond )
#define G_LOG_ONCE( expr ) G_LOG_IMP_ONCE( expr , G::LogOutput::Severity::InfoVerbose , 0 )
#define G_LOG_V( verbosity , expr ) G_LOG_IMP( expr , G::LogOutput::Severity::InfoVerbose , verbosity , true )
#define G_LOG_G( log_group_name , expr ) G_LOG_IMP( expr , G::LogOutput::Severity::InfoVerbose , G_STR_PASTE(#log_group_name,_id) , true )

#define G_LOG_ACTIVE() G::LogOutput::Instance::at( G::LogOutput::Severity::InfoVerbose , 0 , true )
#define G_LOG_ACTIVE_V( verbosity ) G::LogOutput::Instance::at( G::LogOutput::Severity::InfoVerbose , verbosity , true )
#define G_LOG_ACTIVE_G( log_group_name ) G::LogOutput::Instance::at( G::LogOutput::Severity::InfoVerbose , G_STR_PASTE(#log_group_name,_id) , true )

#define G_LOG_MORE( expr ) G_LOG_IMP( expr , G::LogOutput::Severity::InfoMoreVerbose , 0 , true )
#define G_LOG_MORE_IF( cond , expr ) G_LOG_IMP( expr , G::LogOutput::Severity::InfoMoreVerbose , 0 , cond )
#define G_LOG_MORE_ONCE( expr ) G_LOG_IMP_ONCE( expr , G::LogOutput::Severity::InfoMoreVerbose , 0 )
#define G_LOG_MORE_V( verbosity , expr ) G_LOG_IMP( expr , G::LogOutput::Severity::InfoMoreVerbose , verbosity , true )
#define G_LOG_MORE_G( log_group_name , expr ) G_LOG_IMP( expr , G::LogOutput::Severity::InfoMoreVerbose , G_STR_PASTE(#log_group_name,_id) , true )

#define G_LOG_MORE_ACTIVE() G::LogOutput::Instance::at( G::LogOutput::Severity::InfoMoreVerbose , 0 , true )
#define G_LOG_MORE_ACTIVE_V( verbosity ) G::LogOutput::Instance::at( G::LogOutput::Severity::InfoMoreVerbose , verbosity , true )
#define G_LOG_MORE_ACTIVE_G( log_group_name ) G::LogOutput::Instance::at( G::LogOutput::Severity::InfoMoreVerbose , G_STR_PASTE(#log_group_name,_id) , true )

#define G_LOG_S( expr ) G_LOG_IMP( expr , G::LogOutput::Severity::InfoSummary , 0 , true )
#define G_LOG_S_IF( cond , expr ) G_LOG_IMP( expr , G::LogOutput::Severity::InfoSummary , 0 , cond )
#define G_LOG_S_ONCE( expr ) G_LOG_IMP_ONCE( expr , G::LogOutput::Severity::InfoSummary , 0 )
#define G_LOG_S_V( verbosity , expr ) G_LOG_IMP( expr , G::LogOutput::Severity::InfoSummary , verbosity , true )
#define G_LOG_S_G( log_group_name , expr ) G_LOG_IMP( expr , G::LogOutput::Severity::InfoSummary , G_STR_PASTE(#log_group_name,_id) , true )

#define G_LOG_S_ACTIVE() G::LogOutput::Instance::at( G::LogOutput::Severity::InfoSummary , 0 , true )
#define G_LOG_S_ACTIVE_V( verbosity ) G::LogOutput::Instance::at( G::LogOutput::Severity::InfoSummary , verbosity , true )
#define G_LOG_S_ACTIVE_G( log_group_name ) G::LogOutput::Instance::at( G::LogOutput::Severity::InfoSummary , G_STR_PASTE(#log_group_name,_id) , true )

#else

#define G_LOG( expr )
#define G_LOG_IF( cond , expr )
#define G_LOG_ONCE( expr )
#define G_LOG_V( verbosity , expr )
#define G_LOG_G( log_group_name , expr )

#define G_LOG_MORE( expr )
#define G_LOG_MORE_IF( cond , expr )
#define G_LOG_MORE_ONCE( expr )
#define G_LOG_MORE_V( verbosity , expr )
#define G_LOG_MORE_G( log_group_name )

#define G_LOG_S( expr )
#define G_LOG_S_IF( cond , expr )
#define G_LOG_S_ONCE( expr )
#define G_LOG_S_V( verbosity , expr )
#define G_LOG_S_G( log_group_name , expr )

#endif

#if ! defined(G_NO_WARNING)
#define G_WARNING( expr ) G_LOG_IMP( expr , G::LogOutput::Severity::Warning , 0 , true )
#define G_WARNING_IF( cond , expr ) G_LOG_IMP( expr , G::LogOutput::Severity::Warning , 0 , cond )
#define G_WARNING_ONCE( expr ) G_LOG_IMP_ONCE( expr , G::LogOutput::Severity::Warning , 0 )
#else
#define G_WARNING( expr )
#define G_WARNING_IF( cond , expr )
#define G_WARNING_ONCE( expr )
#endif

#if ! defined(G_NO_ERROR)
#define G_ERROR( expr ) G_LOG_IMP( expr , G::LogOutput::Severity::Error , 0 , true )
#else
#define G_ERROR( expr )
#endif

#endif
