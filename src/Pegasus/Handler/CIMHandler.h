//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
// Author: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef PegasusHandler_Handler_h
#define PegasusHandler_Handler_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Common/ContentLanguages.h>  
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Handler/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_HANDLER_LINKAGE CIMHandler
{
public:

    CIMHandler() { };

    virtual ~CIMHandler() { };

    // Abstract method which each and every handler needs to be implemented.
    // Indication processing will instantiate IndicationDispatcher which 
    // will look into Handler Table to load the appropriate handler. If 
    // handler is already loaded then IndicationDispatcher will call this
    // method implemented in handler.
// l10n 
     virtual void handleIndication(
        CIMInstance& indicationHandlerInstance,
        CIMInstance& indicationInstance,
	String nameSpace,
	ContentLanguages& contentLanguages) = 0;
    
    // These are the method to initialize and terminate handler. Actual need and
    // implementation way these methods are yet to be finalized.

    virtual void initialize(CIMRepository* repository) = 0;
    virtual void terminate() = 0;
};

PEGASUS_NAMESPACE_END

#endif /* PegasusHandler_Handler_h */
