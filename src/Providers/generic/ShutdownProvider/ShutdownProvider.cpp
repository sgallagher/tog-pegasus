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
// Author: Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//
// Modified By: Sushma Fernandes, Hewlett-Packard Company 
//                                (sushma_fernandes@hp.com)
//
//%////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//  Shutdown Provider
///////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMMethodProvider.h>
#include <Pegasus/Server/ServiceCIMOMHandle.h>
#include <Pegasus/Server/ShutdownService.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

/**
    The ShutdownProvider is a method provider that implements the shutdown
    method in the PG_ShutdownService class.  It provides response to the
    shutdown request from clients to shutdown cimom gracefully.
*/

class ShutdownProvider : public CIMMethodProvider
{
public:

    ShutdownProvider()
    {
    }

    virtual ~ShutdownProvider()
    {
    }

    /**
     Standard initialization function for the provider.
    */
    void initialize(CIMOMHandle& cimomHandle)
    {
        //
        // get an instance of Shutdown Service
        //
        _shutdownService = ShutdownService::getInstance();

        _cimomHandle = dynamic_cast<ServiceCIMOMHandle *>(&cimomHandle);
        _cimserver = _cimomHandle->getServer();
    }

    void terminate(void)
    {
    }

    /**
        Invoke Method.  Used to shutdown cimom.
    */
    void invokeMethod(
            const OperationContext & context,
            const CIMObjectPath & objectReference,
            const String & methodName,
            const Array<CIMParamValue> & inParameters,
            Array<CIMParamValue> & outParameters,
            ResponseHandler<CIMValue> & handler)
    {
        // Begin processing the request
        handler.processing();

        // Check if the input parameters are passed.
        if ( inParameters.size() < 2 )
        {
            throw PEGASUS_CIM_EXCEPTION( CIM_ERR_INVALID_PARAMETER,
                                        "Input parameters are not valid.");
        }

        Boolean force = false;
        Uint32 timeoutValue = 0;

        // Get the input parameter values
        for (Uint32 i = 0; i < inParameters.size(); i++)
        {
            String parmName = inParameters[i].getParameterName();
            if (String::equalNoCase(parmName, "force"))
            {
                //
                // get the force parameter
                //
                inParameters[i].getValue().get(force);
            }
            else
            {
                if (String::equalNoCase(parmName, "timeout"))
                {
                    //
                    // get the timeout value
                    //
                    inParameters[i].getValue().get(timeoutValue);
                }
                else
                {
                    throw PEGASUS_CIM_EXCEPTION( CIM_ERR_INVALID_PARAMETER,
                                        "Input parameters are not valid.");
                }
            }
        }

        try
        {
            _shutdownService->shutdown(_cimserver, force, timeoutValue);
        }
        catch (Exception& e)
        {
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
        }

        handler.deliver(CIMValue(0));
        handler.complete();
        return;
    };

    private:

        //
        // Shutdown Service Instance variable
        //
        ShutdownService*      _shutdownService;
        ServiceCIMOMHandle*   _cimomHandle;
        CIMServer*            _cimserver;
};

// This is the dynamic entry point into this dynamic module. The name of
// this provider is "UserManagerProvider" which is appened to
// "PegasusCreateProvider_" to form a symbol name. This function is called
// by the ProviderModule to load this provider.
//
// NOTE: The name of the provider must be correct to be loadable.

extern "C" PEGASUS_EXPORT CIMBaseProvider *
PegasusCreateProvider(const String &providerName)
{
    const String PROVIDER_NAME = "ShutdownProvider";
    if (String::equalNoCase(providerName, PROVIDER_NAME))
    {
        return(new ShutdownProvider());
    }
    return(0);
}

PEGASUS_NAMESPACE_END

