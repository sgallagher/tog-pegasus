//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
//
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Yi Zhou, Hewlett-Packard Company (yi.zhou@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/PegasusVersion.h>

#include <iostream>
#include <Pegasus/Handler/CIMHandler.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/CIMType.h>
#include <Pegasus/Common/IndicationFormatter.h>

#include "SystemLogListenerDestination.h"

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;

void SystemLogListenerDestination::initialize(CIMRepository* repository)
{
}

void SystemLogListenerDestination::handleIndication(
    const OperationContext& context,
    const String nameSpace,
    CIMInstance& indication, 
    CIMInstance& handler, 
    CIMInstance& subscription, 
    ContentLanguages & contentLanguages)
{
    PEG_METHOD_ENTER (TRC_IND_HANDLER, 
        "SystemLogListenerDestination::handleIndication");

    String ident_name = "CIM Indication";
    String indicationText;

    try
    {
	// gets formatted indication message
	indicationText = IndicationFormatter::getFormattedIndText(
	    subscription, indication, contentLanguages);

        // default severity
        Uint32 severity = Logger::INFORMATION;

        // If an indication contains severity information, gets the value
        // and maps it to Pegasus logger severity. Otherwise, default value 
        // is used.

        Uint32 severityPos = indication.findProperty(CIMName 
	    ("PerceivedSeverity")); 

        if (severityPos != PEG_NOT_FOUND)
        {
            Uint16 perceivedSeverity;
            CIMValue perceivedSeverityValue = 
                indication.getProperty(severityPos).getValue();

            if (!perceivedSeverityValue.isNull())
            {
                perceivedSeverityValue.get(perceivedSeverity);

                switch (perceivedSeverity)
                {
                    case (_SEVERITY_FATAL):
                    case (_SEVERITY_CRITICAL):
                    {
                        severity = Logger::FATAL;
                        break;
                    }

                    case (_SEVERITY_MAJOR):
                    {
                        severity = Logger::SEVERE;
                        break;
                    }

                    case (_SEVERITY_MINOR):
                    case (_SEVERITY_WARNING):
                    {
                        severity = Logger::WARNING;
                        break;
                    }

                    case (_SEVERITY_INFORMATION):
                    case (_SEVERITY_OTHER):
                    case (_SEVERITY_UNKNOWN):
                    {
                        severity = Logger::INFORMATION;
                        break;
                    }

                    default:
                    {
			Tracer::trace(TRC_IND_HANDLER, Tracer::LEVEL4,
			    "PerceivedSeverity = %d is not a valid value." 
			    " Using default severity.", perceivedSeverity);
                        break;
                    }
                }
            }
        }

	// writes the formatted indication to a system log file
	_writeToSystemLog(ident_name, severity, indicationText);

    }
    catch (CIMException & c)
    {
        PEG_TRACE_STRING(TRC_IND_HANDLER, Tracer::LEVEL4, c.getMessage());
        PEG_METHOD_EXIT();

        throw PEGASUS_CIM_EXCEPTION (CIM_ERR_FAILED, c.getMessage());
    }
    catch (Exception& e)
    {
        PEG_TRACE_STRING(TRC_IND_HANDLER, Tracer::LEVEL4, e.getMessage());
        PEG_METHOD_EXIT();

        throw PEGASUS_CIM_EXCEPTION (CIM_ERR_FAILED, e.getMessage());
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_IND_HANDLER, Tracer::LEVEL4,
            "Failed to deliver indication to system log file.");
        PEG_METHOD_EXIT();
   
        throw PEGASUS_CIM_EXCEPTION_L (CIM_ERR_FAILED,
            MessageLoaderParms("Handler.SystemLogListenerDestination."
	    "SystemLogListenerDestination.FAILED_TO_DELIVER_INDICATION_TO_SYSTEM_LOG",
            "Failed to deliver indication to system log file."));
    }

    PEG_METHOD_EXIT();
}

void SystemLogListenerDestination::_writeToSystemLog(
    const String & identifier,
    Uint32 severity,
    const String & formattedText)
{
    PEG_METHOD_ENTER (TRC_IND_HANDLER,
	"SystemLogListenerDestination::_writeToSystemLog");

#if defined(PEGASUS_USE_SYSLOGS)

    System::syslog(identifier, severity, formattedText.getCString());

#else

    // PEGASUS_USE_SYSLOGS is not defined, writes the formatted 
    // indications into PegasusStandard.log file
    Logger::put (Logger::STANDARD_LOG , identifier, severity, 
		 (const char *)formattedText.getCString());

#endif

    PEG_METHOD_EXIT();

}

// This is the dynamic entry point into this dynamic module. The name of
// this handler is "SystemLogListenerDestination" which is appended to "PegasusCreateHandler_"
// to form a symbol name. This function is called by the HandlerTable
// to load this handler.

extern "C" PEGASUS_EXPORT CIMHandler* 
    PegasusCreateHandler_SystemLogListenerDestination() {
    return new SystemLogListenerDestination;
}

PEGASUS_NAMESPACE_END
