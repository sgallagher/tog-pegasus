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
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Server/CIMExportRequestDispatcher.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const String NAMESPACE = "root/cimv2";

int main()
{
    CIMRepository r("./testrepository");

    try
    {   
        r.createNameSpace(NAMESPACE);
    }
    catch (AlreadyExists&)
    {
        // Ignore this!
    }

    r.setQualifier(
        NAMESPACE, CIMQualifierDecl("Handler", "CIMxmlIndicationHandler", CIMScope::CLASS));

    r.setQualifier(
        NAMESPACE, CIMQualifierDecl("key", true, CIMScope::PROPERTY));

    //Handler
    CIMClass handlerClass("CIMxmlIndicationHandler");

    handlerClass
        .addProperty(CIMProperty("destination", String())
            .addQualifier(CIMQualifier("key", true)));
    handlerClass.addQualifier(CIMQualifier("Handler", "CIMxmlIndicationHandler"));

    r.createClass(NAMESPACE, handlerClass);

    //--------------------------------------------------------------------------
    // Create Instance (of SuperClass):
    //--------------------------------------------------------------------------

    CIMInstance handlerInstance("CIMxmlIndicationHandler");
    handlerInstance.addQualifier(CIMQualifier("Handler", "CIMxmlIndicationHandler"));
    handlerInstance.addProperty(CIMProperty("destination", "localhost:5988"));
    
    r.createInstance(NAMESPACE, handlerInstance);

    //Indication
    CIMClass indicationClass("MyIndication");

    indicationClass
        .addProperty(CIMProperty("Company", String())
            .addQualifier(CIMQualifier("key", true)))
        .addProperty(CIMProperty("City", String())
            .addQualifier(CIMQualifier("key", true)))
        .addProperty(CIMProperty("Building", Uint8(0))
            .addQualifier(CIMQualifier("key", true)));

    r.createClass(NAMESPACE, indicationClass);

    //--------------------------------------------------------------------------
    // Create Instance (of SuperClass):
    //--------------------------------------------------------------------------

    CIMInstance indicationInstance("MyIndication");
    indicationInstance.addProperty(CIMProperty("Company", "Hewlett-Packard Company"));
    indicationInstance.addProperty(CIMProperty("City", "Cupertino, CA"));
    indicationInstance.addProperty(CIMProperty("Building", Uint8(44)));

    r.createInstance(NAMESPACE, indicationInstance);

    CIMRepository* repository = new CIMRepository("./testrepository");
    CIMExportRequestDispatcher* dispatcher = new CIMExportRequestDispatcher(repository);

    dispatcher->handleIndication(handlerInstance, indicationInstance, NAMESPACE);

    //Removing everything
#ifdef PEGASUS_OS_TYPE_UNIX
    system("rm -r testrepository");
#endif

    cout << "+++++ passed all tests" << endl;

    return 0;
}

