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

#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/ExportClient/CIMExportClient.h>
#include <Pegasus/Handler/CIMHandler.h>
#include <Pegasus/Repository/CIMRepository.h>

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;

//#define DDD(X) X
#define DDD(X) // X

DDD(static const char* _SNMPINDICATIONHANDLER = "snmpIndicationHandler::";)

class PEGASUS_HANDLER_LINKAGE snmpIndicationHandler: public CIMHandler
{
public:

    CIMRepository* _repository;

    snmpIndicationHandler()
    {
        DDD(cout << _SNMPINDICATIONHANDLER << "snmpIndicationHandler()" << endl;)
    }

    virtual ~snmpIndicationHandler()
    {
        DDD(cout << _SNMPINDICATIONHANDLER << "~snmpIndicationHandler()" << endl;)
    }

    void initialize(CIMRepository* repository);

    void terminate()
    {
        DDD(cout << _SNMPINDICATIONHANDLER << "terminate()" << endl;)
    }

    void handleIndication(
	CIMInstance& handler, 
	CIMInstance& indication, 
	String nameSpace);
};

PEGASUS_NAMESPACE_END
