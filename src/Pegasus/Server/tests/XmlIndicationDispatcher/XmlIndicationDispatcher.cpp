//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
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

#include <cassert>
#include <Pegasus/Common/Monitor.h>
#include <Pegasus/Common/HTTPConnector.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/ExportServer/CIMExportRequestDispatcher.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const String NAMESPACE = "root/cimv2";

int main()
{
    //CIMRepository r("/PegasusRun/repository");

    Monitor* monitor = new Monitor;
    HTTPConnector* httpConnector = new HTTPConnector(monitor);
    CIMClient r(monitor, httpConnector);

    r.connect("localhost:5988");

    //Consumer
    CIMClass cClass = r.getClass(NAMESPACE, "PG_ConsumerRegistration", false);
    CIMInstance cInstance("PG_ConsumerRegistration");
    cInstance.addProperty(CIMProperty("url", "localhost:5988"));
    cInstance.addProperty(CIMProperty("consumerName", "DisplayConsumer"));
    CIMReference instanceName3 = cInstance.getInstanceName(cClass);
    r.createInstance(NAMESPACE, cInstance);

    
    //Handler
    CIMClass handlerClass = r.getClass(NAMESPACE, "CIM_IndicationHandlerXMLHTTP", false);
    CIMInstance handlerInstance("CIM_IndicationHandlerXMLHTTP");
    handlerInstance.addQualifier(CIMQualifier("Handler", "CIMxmlIndicationHandler"));
    handlerInstance.addProperty(CIMProperty("Destination", "localhost:5988"));
    //handlerInstance.addProperty(CIMProperty("Destination", "arches.cup.hp.com:5988"));
    handlerInstance.addProperty(CIMProperty("SystemCreationClassName", "NU744781"));
    handlerInstance.addProperty(CIMProperty("SystemName", "NU744781"));
    handlerInstance.addProperty(CIMProperty("CreationClassName", "CIM_IndciationHandlerXMLHTTP"));
    handlerInstance.addProperty(CIMProperty("Name", "snmpIndicationHandler"));
    CIMReference instanceName1 = handlerInstance.getInstanceName(handlerClass);
    r.createInstance(NAMESPACE, handlerInstance);

    //Indication
    CIMClass cimClass = r.getClass(NAMESPACE, "TestSoftwarePkg", false);
    CIMInstance cimInstance("TestSoftwarePkg");
    cimInstance.addProperty(CIMProperty("PkgName", "WBEM"));
    cimInstance.addProperty(CIMProperty("PkgIndex", Uint32(101)));
    cimInstance.addProperty(CIMProperty("trapOid", "1.3.6.1.4.1.11.2.3.1.7.0.4"));
    cimInstance.addProperty(CIMProperty("computerName", "NU744781"));
    CIMReference instanceName2 = cimInstance.getInstanceName(cimClass);
    r.createInstance(NAMESPACE, cimInstance);

    CIMRepository* repository = new CIMRepository("/PegasusRun/repository");
    CIMExportRequestDispatcher* dispatcher = new CIMExportRequestDispatcher(repository);

    dispatcher->handleIndication(handlerInstance, cimInstance, NAMESPACE);

    r.deleteInstance(NAMESPACE, instanceName1);
    r.deleteInstance(NAMESPACE, instanceName2);
    r.deleteInstance(NAMESPACE, instanceName3);

    return 0;
}

