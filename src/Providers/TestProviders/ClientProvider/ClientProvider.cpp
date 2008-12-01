//%LICENSE////////////////////////////////////////////////////////////////
// 
// Licensed to The Open Group (TOG) under one or more contributor license
// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
// this work for additional information regarding copyright ownership.
// Each contributor licenses this file to you under the OpenPegasus Open
// Source License; you may not use this file except in compliance with the
// License.
// 
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// 
//////////////////////////////////////////////////////////////////////////
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ClientProvider.h"
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/TLS.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/Constants.h>

PEGASUS_NAMESPACE_BEGIN

ClientProvider::ClientProvider()
{
}

ClientProvider::~ClientProvider()
{
}

void ClientProvider::initialize(CIMOMHandle& cimom)
{
}

void ClientProvider::terminate()
{
    delete this;
}

static Boolean verifyCertificate(SSLCertificateInfo& certInfo)
{
    return true;
}


void ClientProvider::invokeMethod(
    const OperationContext& context,
    const CIMObjectPath& objectReference,
    const CIMName& methodName,
    const Array<CIMParamValue>& inParameters,
    MethodResultResponseHandler& handler)
{
   handler.processing();

   if (objectReference.getClassName().equal("Sample_ClientProviderClass"))
   {
       if (methodName.equal("DoConnect"))
       {
           if (inParameters.size() > 0)
           {
               String connectType = String::EMPTY;
               CIMValue paramVal;
               CIMClient client;

               paramVal = inParameters[0].getValue();
               paramVal.get(connectType);

               if (connectType == "Local")
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
               else if (connectType == "Remote")
               {
                   String HOST     = "localhost";
                   Uint32 PORT     = 5989;
                   String USER     = "guest";
                   String PASSWORD = "guest";

#ifdef PEGASUS_HAS_SSL

                   const char* pegasusHome = getenv("PEGASUS_HOME");

                   String certpath = FileSystem::getAbsolutePath(
                       pegasusHome, PEGASUS_SSLCLIENT_CERTIFICATEFILE);

                   String randFile = String::EMPTY;

#ifdef PEGASUS_SSL_RANDOMFILE
                   randFile = FileSystem::getAbsolutePath(
                       pegasusHome, PEGASUS_SSLCLIENT_RANDOMFILE);
#endif

                   SSLContext sslcontext(certpath, verifyCertificate, randFile);

                   client.connect(HOST, PORT, sslcontext, USER, PASSWORD);
#else
                   client.connect(HOST, PORT, USER, PASSWORD);
#endif
               }
               // Enumerate Instances.

               Array<CIMObjectPath> instanceNames =
                   client.enumerateInstanceNames(
                       "root/cimv2", "CIM_ManagedElement");

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
    const OperationContext& context,
    const CIMObjectPath& instanceReference,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList& propertyList,
    InstanceResponseHandler& handler)
{
}

void ClientProvider::enumerateInstances(
    const OperationContext& context,
    const CIMObjectPath& classReference,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList& propertyList,
    InstanceResponseHandler& handler)
{
}

void ClientProvider::enumerateInstanceNames(
    const OperationContext& context,
    const CIMObjectPath& classReference,
    ObjectPathResponseHandler& handler)
{
}

void ClientProvider::modifyInstance(
    const OperationContext& context,
    const CIMObjectPath& instanceReference,
    const CIMInstance& instanceObject,
    const Boolean includeQualifiers,
    const CIMPropertyList& propertyList,
    ResponseHandler& handler)
{
}

void ClientProvider::createInstance(
    const OperationContext& context,
    const CIMObjectPath& instanceReference,
    const CIMInstance& instanceObject,
    ObjectPathResponseHandler& handler)
{
}

void ClientProvider::deleteInstance(
    const OperationContext& context,
    const CIMObjectPath& instanceReference,
    ResponseHandler& handler)
{
}

PEGASUS_NAMESPACE_END
