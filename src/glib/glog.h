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
#include "gstrmacros.h"
#include "gmodule.h"

#define G_LOG_IMP( cond , expr , severity , verbosity ) do { if(G::LogOutput::Instance::at(severity,verbosity)&&(cond)) { auto log_stream = G::LogOutput::Instance::start((severity),__FILE__,__LINE__) ; log_stream << expr ; G::LogOutput::Instance::output(log_stream) ; } } while(0) /* NOLINT bugprone-macro-parentheses */
#define G_LOG_IMP_ONCE( cond , expr , severity , verbosity ) do { if(cond) { static bool done__ = false ; if(!done__&&G::LogOutput::Instance::at(severity,verbosity)) { auto log_stream = G::LogOutput::Instance::start((severity),__FILE__,__LINE__) ; log_stream << expr ; G::LogOutput::Instance::output(log_stream) ; } done__ = true ; } } while(0) /* NOLINT bugprone-macro-parentheses */

#if defined(G_WITH_DEBUG) || ( defined(_DEBUG) && ! defined(G_NO_DEBUG) )
#define G_DEBUG( expr ) G_LOG_IMP( true , expr , G::LogOutput::Severity::Debug , 0 )
#define G_DEBUG_IF( cond , expr ) G_LOG_IMP( cond , expr , G::LogOutput::Severity::Debug , 0 )
#define G_DEBUG_ONCE( expr ) G_LOG_IMP_ONCE( true , expr , G::LogOutput::Severity::Debug , 0 )
#else
#define G_DEBUG( expr )
#define G_DEBUG_IF( cond , expr )
#define G_DEBUG_ONCE( group , expr )
#endif

#if ! defined(G_NO_LOG)
#define G_LOG( expr ) G_LOG_IMP( true , expr , G::LogOutput::Severity::InfoVerbose , 0 )
#define G_LOG_IF( cond , expr ) G_LOG_IMP( cond , expr , G::LogOutput::Severity::InfoVerbose , 0 )
#define G_LOG_ONCE( expr ) G_LOG_IMP_ONCE( true , expr , G::LogOutput::Severity::InfoVerbose , 0 )
#define G_LOG_MORE( expr ) G_LOG_IMP( true , expr , G::LogOutput::Severity::InfoMoreVerbose , 0 )
#define G_LOG_MORE_IF( cond , expr ) G_LOG_IMP( cond , expr , G::LogOutput::Severity::InfoMoreVerbose , 0 )
#define G_LOG_MORE_ONCE( expr ) G_LOG_IMP_ONCE( true , expr , G::LogOutput::Severity::InfoMoreVerbose , 0 )
#define G_LOG_S( expr ) G_LOG_IMP( true , expr , G::LogOutput::Severity::InfoSummary , 0 )
#define G_LOG_S_IF( cond , expr ) G_LOG_IMP( cond , expr , G::LogOutput::Severity::InfoSummary , 0 )
#define G_LOG_S_ONCE( expr ) G_LOG_IMP_ONCE( true , expr , G::LogOutput::Severity::InfoSummary , 0 )
#define G_LOG_V( verbosity , expr ) G_LOG_IMP( true , expr , G::LogOutput::Severity::InfoVerbose , verbosity )
#define G_LOG_S_V( verbosity , expr ) G_LOG_IMP( true , expr , G::LogOutput::Severity::InfoSummary , verbosity )
#define G_LOG_M( module_name , expr ) G_LOG_IMP( true , expr , G::LogOutput::Severity::InfoVerbose , G::LogOutput::Instance::moduleVerbosity(G_STR_PASTE(#module_name,_id.value)) )
#define G_LOG_S_M( module_name , expr ) G_LOG_IMP( true , expr , G::LogOutput::Severity::InfoSummary , G::LogOutput::Instance::moduleVerbosity(G_STR_PASTE(#module_name,_id.value)) )
#else
#define G_LOG( expr )
#define G_LOG_IF( cond , expr )
#define G_LOG_ONCE( expr )
#define G_LOG_MORE( expr )
#define G_LOG_MORE_IF( cond , expr )
#define G_LOG_MORE_ONCE( expr )
#define G_LOG_S( expr )
#define G_LOG_S_IF( cond , expr )
#define G_LOG_S_ONCE( expr )
#define G_LOG_V( verbosity , expr )
#define G_LOG_S_V( verbosity , expr )
#define G_LOG_M( module_name , expr )
#endif

#if ! defined(G_NO_WARNING)
#define G_WARNING( expr ) G_LOG_IMP( true , expr , G::LogOutput::Severity::Warning , 0 )
#define G_WARNING_IF( cond , expr ) G_LOG_IMP( cond , expr , G::LogOutput::Severity::Warning , 0 )
#define G_WARNING_ONCE( expr ) G_LOG_IMP_ONCE( true , expr , G::LogOutput::Severity::Warning , 0 )
#else
#define G_WARNING( expr )
#define G_WARNING_IF( cond , expr )
#define G_WARNING_ONCE( expr )
#endif

#if ! defined(G_NO_ERROR)
#define G_ERROR( expr ) G_LOG_IMP( true , expr , G::LogOutput::Severity::Error , 0 )
#else
#define G_ERROR( expr )
#endif

#endif
