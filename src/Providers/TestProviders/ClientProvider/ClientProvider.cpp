//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Jenny Yu , Hewlett-Packard Company
//         (jenny_yu@hp.com)
//         Sushma Fernandes , Hewlett-Packard Company
//         (sushma_fernandes@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ClientProvider.h"
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/TLS.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/IPC.h>

PEGASUS_NAMESPACE_BEGIN

ClientProvider::ClientProvider(void)
{
}

ClientProvider::~ClientProvider(void)
{
}

void ClientProvider::initialize(CIMOMHandle & cimom)
{
}

void ClientProvider::terminate(void)
{
    delete this;
}

static Boolean verifyCertificate(SSLCertificateInfo &certInfo)
{
    return true;
}


void ClientProvider::invokeMethod(
	const OperationContext & context,
	const CIMObjectPath & objectReference,
	const CIMName & methodName,
	const Array<CIMParamValue> & inParameters,
	MethodResultResponseHandler & handler)
{
    handler.processing();

    if (objectReference.getClassName().equal ("Sample_ClientProviderClass"))
    {
        if (methodName.equal ("DoConnect"))
        {
            if( inParameters.size() > 0 )
	    {
	        String 		connectType = String::EMPTY;
                CIMValue 	paramVal;
                CIMClient 	client;

	        paramVal = inParameters[0].getValue();
		paramVal.get( connectType );

		if( connectType == "Local" )
		{
                   client.connectLocal();

                  // Enumerate Instances.
                  Array<CIMObjectPath> instanceNames = 
                          client.enumerateInstanceNames(  
                              "root/cimv2", 
                              "CIM_ManagedElement");

                  client.disconnect();
                  handler.deliver(CIMValue(0));
                }
                else if ( connectType == "Remote" )
                {
                    String HOST     = "localhost";
                    Uint32 PORT     = 5989;
                    String USER     = "guest";
                    String PASSWORD = "guest";
                    const char* pegasusHome = getenv("PEGASUS_HOME");

                    String certpath = FileSystem::getAbsolutePath(
                            pegasusHome, PEGASUS_SSLCLIENT_CERTIFICATEFILE);

                    String randFile = String::EMPTY;

                    randFile = FileSystem::getAbsolutePath(
                                pegasusHome, PEGASUS_SSLCLIENT_RANDOMFILE);

                    SSLContext sslcontext(
                                certpath, verifyCertificate, randFile);

                    client.connect( 
                                   HOST,
                                   PORT,
                                   sslcontext,
                                   USER,
                                   PASSWORD ); 
		  }
                  // Enumerate Instances.

                  Array<CIMObjectPath> instanceNames = client.enumerateInstanceNames( "root/cimv2", "CIM_ManagedElement");

                  client.disconnect();
		  handler.deliver(CIMValue(0));
	     }
	     else
	     {
		  handler.deliver(CIMValue(1));
             }
	}
    }
    handler.complete();
}


void ClientProvider::getInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const Boolean includeQualifiers,
	const Boolean includeClassOrigin,
	const CIMPropertyList & propertyList,
	InstanceResponseHandler & handler)
{
}

void ClientProvider::enumerateInstances(
	const OperationContext & context,
	const CIMObjectPath & classReference,
	const Boolean includeQualifiers,
	const Boolean includeClassOrigin,
	const CIMPropertyList & propertyList,
	InstanceResponseHandler & handler)
{
}

void ClientProvider::enumerateInstanceNames(
	const OperationContext & context,
	const CIMObjectPath & classReference,
	ObjectPathResponseHandler & handler)
{
}

void ClientProvider::modifyInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const CIMInstance & instanceObject,
	const Boolean includeQualifiers,
	const CIMPropertyList & propertyList,
	ResponseHandler & handler)
{
}

void ClientProvider::createInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const CIMInstance & instanceObject,
	ObjectPathResponseHandler & handler)
{
}

void ClientProvider::deleteInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	ResponseHandler & handler)
{
}

PEGASUS_NAMESPACE_END
