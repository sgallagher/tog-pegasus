//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
// Author: Muni s Reddy (mreddy@in.ibm.com)   for bug #4792
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////
#include "CLITestProvider.h"
//#include <fstream>
#include <Pegasus/Common/PegasusAssert.h>
PEGASUS_USING_PEGASUS;
//PEGASUS_USING_STD;


CLITestProvider::CLITestProvider()
{
}

CLITestProvider::~CLITestProvider()
{
}

void CLITestProvider::initialize(CIMOMHandle & cimom)
{
}

void CLITestProvider::terminate()
{
    delete this;
}



void CLITestProvider::invokeMethod(
    const OperationContext & context,
    const CIMObjectPath & objectReference,
    const CIMName & methodName,
    const Array<CIMParamValue> & inParameters,
    MethodResultResponseHandler & handler)
{
    // convert a fully qualified reference into a local reference
    // (class name and keys only).

    CIMObjectPath localReference = CIMObjectPath(
        String(),
        CIMNamespaceName(),
        objectReference.getClassName(),
        objectReference.getKeyBindings());
    handler.processing();

    String outString = "CLITestProvider  Tests : ";

    if (objectReference.getClassName().equal("Test_CLITestProviderClass"))
    {
        if (methodName.equal("ReferenceParamTest"))
        {
            if (inParameters.size() > 0)
            {
	            for(Uint32 i = 0; i < inParameters.size(); ++i)
    	        {
                    CIMValue paramVal = inParameters[i].getValue();
                    if (!paramVal.isNull())
                    {
                    	if(paramVal.getType() == CIMTYPE_REFERENCE)
                        {
                        	CIMObjectPath cop,cop1("test/Testprovider:class.k1=\"v1\",k2=\"v2\",k3=\"v3\"");
                        	paramVal.get(cop);
                        	PEGASUS_TEST_ASSERT(cop.identical(cop1) == true);
                        	outString.append("	\n Passed Reference params Test1	");
                        	PEGASUS_TEST_ASSERT(!cop.identical(cop1) == false);
                        	outString.append("	\n Passed Reference params Test2	");
                        }
                        else
                        {
                        	//This code gets excuted for non reference parameters.
                        	String replyName;
                        	paramVal.get(replyName);
                      	    if (replyName != String::EMPTY)
                    	    {
                     		    outString.append(replyName);
                     		    outString.append("\n");
                     		    outString.append("Passed String Param Test\n");
                    	    }
               	        }

            	    outString.append("\n");
                    }
                    else
                    {
                        outString.append("Param Value is NULL");
                    }
                }

                handler.deliver(CIMValue(outString));
            }
            else
            {
                outString.append("Empty Parameters");
                handler.deliver(CIMValue(outString));
            }
        }
    }
    handler.complete();
}


