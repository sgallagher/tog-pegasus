//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett Packard, IBM, The Open Group,
// Tivoli Systems
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

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/PegasusVersion.h>

#include <iostream>
#include <Pegasus/ExportClient/CIMExportClient.h>
#include <Pegasus/Handler/CIMHandler.h>
#include <Pegasus/Repository/CIMRepository.h>

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;

//#define DDD(X) X
#define DDD(X) // X

DDD(static const char* _CIMXMLINDICATIONHANDLER = "CIMxmlIndicationHandler::";)

class PEGASUS_HANDLER_LINKAGE CIMxmlIndicationHandler: public CIMHandler
{
public:

    CIMxmlIndicationHandler()
    {
        DDD(cout << _CIMXMLINDICATIONHANDLER << "CIMxmlIndicationHandler()" << endl;)
    }

    virtual ~CIMxmlIndicationHandler()
    {
        DDD(cout << _CIMXMLINDICATIONHANDLER << "~CIMxmlIndicationHandler()" << endl;)
    }

    void initialize(CIMRepository* repository)
    {
        DDD(cout << _CIMXMLINDICATIONHANDLER << "initialize()" << endl;)
    }

    void terminate()
    {
        DDD(cout << _CIMXMLINDICATIONHANDLER << "terminate()" << endl;)
    }

    void handleIndication(
	CIMInstance& indicationHandlerInstance, 
	CIMInstance& indicationInstance, 
	String nameSpace)
    {
	//get destination for the indication
	Uint32 pos = indicationHandlerInstance.findProperty("destination");
        if (pos == PEG_NOT_FOUND)
        {
            // ATTN: Deal with a malformed handler instance
        }

	CIMProperty prop = indicationHandlerInstance.getProperty(pos);

        String dest;
        try
        {
            prop.getValue().get(dest);
        }
        catch (TypeMismatch& e)
        {
            // ATTN: Deal with a malformed handler instance
        }
	
	try
        {
	    Monitor* monitor = new Monitor;
	    HTTPConnector* httpConnector = new HTTPConnector(monitor);
	    CIMExportClient exportclient(monitor, httpConnector);
	    exportclient.connect(dest.subString(0, dest.find("/")));
	    exportclient.exportIndication(
                dest.subString(dest.find("/")+1), indicationInstance);
	}
	catch(Exception& e)
        {
            PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
        }
    }
};

// This is the dynamic entry point into this dynamic module. The name of
// this handler is "CIMxmlIndicationHandler" which is appened to "PegasusCreateHandler_"
// to form a symbol name. This function is called by the HandlerTable
// to load this handler.

extern "C" PEGASUS_EXPORT CIMHandler* 
    PegasusCreateHandler_CIMxmlIndicationHandler() {
    DDD(cout << "Called PegasusCreateHandler_CIMxmlIndicationHandler" << endl;)
    return new CIMxmlIndicationHandler;
}

PEGASUS_NAMESPACE_END
