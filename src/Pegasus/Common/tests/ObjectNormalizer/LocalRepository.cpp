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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "LocalRepository.h"

PEGASUS_USING_PEGASUS;

LocalRepository::LocalRepository(void) : context(0)
{
    // the SimpleDeclContext object does not handle inheritance so it is necessary to create super-class
    // qualifiers and properties in subclasses.
    context = new SimpleDeclContext;

    // create essential qualifiers
    CIMQualifierDecl abstractQualifier("Abstract", Boolean(true), CIMScope::CLASS, CIMFlavor::NONE);
    CIMQualifierDecl keyQualifier("Key", Boolean(true), (CIMScope::PROPERTY + CIMScope::REFERENCE), CIMFlavor::TOSUBCLASS);

    context->addQualifierDecl("test_namespace", abstractQualifier);
    context->addQualifierDecl("test_namespace", keyQualifier);

    // create base class
    {
        CIMClass classA("ClassA");

        classA.addQualifier(CIMQualifier("Abstract", Boolean(true)));

        CIMProperty property1("property1", CIMValue(CIMTYPE_UINT32, 0, 0), 0, CIMName(), "ClassA");

        property1.addQualifier(CIMQualifier("Key", Boolean(true)));

        classA.addProperty(property1);

        CIMProperty property2("property2", CIMValue(String("default_value")), 0, CIMName(), "ClassA");

        property2.addQualifier(CIMQualifier("Key", Boolean(true)));

        classA.addProperty(property2);

        CIMProperty property3("property3", CIMValue(CIMTYPE_DATETIME, 0, 0), 0, CIMName(), "ClassA");

        property3.addQualifier(CIMQualifier("Key", Boolean(true)));

        classA.addProperty(property3);

        // add class to namespace
        context->addClass("test_namespace", classA);
    }

    // create sub-class
    {
        CIMClass classB("ClassB", "ClassA");

        classB.addQualifier(CIMQualifier("Abstract", Boolean(true)));

        CIMProperty property1("property1", CIMValue(CIMTYPE_UINT32, 0, 0), 0, CIMName(), "ClassA");

        property1.addQualifier(CIMQualifier("Key", Boolean(true)));

        classB.addProperty(property1);

        CIMProperty property2("property2", CIMValue(String("default_value")), 0, CIMName(), "ClassA");

        property2.addQualifier(CIMQualifier("Key", Boolean(true)));

        classB.addProperty(property2);

        CIMProperty property3("property3", CIMValue(CIMTYPE_DATETIME, 0, 0), 0, CIMName(), "ClassA");

        property3.addQualifier(CIMQualifier("Key", Boolean(true)));

        classB.addProperty(property3);

        // create default value for property2
        CIMProperty property4("property4", CIMValue(String("default_value")), 0, CIMName(), "ClassB");

        classB.addProperty(property4);

        // add class to namespace
        context->addClass("test_namespace", classB);
    }

    // creae sub-class
    {
        CIMClass classC("ClassC", "ClassB");

        CIMProperty property1("property1", CIMValue(CIMTYPE_UINT32, 0, 0), 0, CIMName(), "ClassA");

        property1.addQualifier(CIMQualifier("Key", Boolean(true)));

        classC.addProperty(property1);

        CIMProperty property2("property2", CIMValue(String("default_value")), 0, CIMName(), "ClassA");

        property2.addQualifier(CIMQualifier("Key", Boolean(true)));

        classC.addProperty(property2);

        CIMProperty property3("property3", CIMValue(CIMTYPE_DATETIME, 0, 0), 0, CIMName(), "ClassA");

        property3.addQualifier(CIMQualifier("Key", Boolean(true)));

        classC.addProperty(property3);

        // create default value for property2
        CIMProperty property4("property4", CIMValue(String("default_value")), 0, CIMName(), "ClassB");

        classC.addProperty(property4);

        // add class to namespace
        context->addClass("test_namespace", classC);
    }

    // TODO: add association object

    // TODO: add indication object

    // TODO: add object with embedded object
}

LocalRepository::~LocalRepository(void)
{
    delete context;
}

CIMClass LocalRepository::getClass(
    const String & nameSpace,
    const String & className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList & propertyList)
{
    CIMClass cimClass = context->lookupClass("test_namespace", className);


    if(localOnly == true)
    {
        // remove parent properties
        for(Uint32 i = 0, n = cimClass.getPropertyCount(); i < n; i++)
        {
            if(String::equalNoCase(cimClass.getProperty(i).getClassOrigin().getString(), className))
            {
                cimClass.removeProperty(i);
                i--;
            }
        }
    }

    if(includeQualifiers == false)
    {
        // remove qualifiers from class
        for(Uint32 i = 0, n = cimClass.getQualifierCount(); i < n; i++)
        {
            cimClass.removeQualifier(i);
        }

        // remove qualifiers from properties
        for(Uint32 i = 0, n = cimClass.getPropertyCount(); i < n; i++)
        {
            CIMProperty cimProperty = cimClass.getProperty(i);

            for(Uint32 j = 0, m = cimProperty.getQualifierCount(); j < m; j++)
            {
                cimProperty.removeQualifier(j);
            }
        }
    }

    if(includeClassOrigin == false)
    {
        // remove class origin
        for(Uint32 i = 0, n = cimClass.getPropertyCount(); i < n; i++)
        {
            cimClass.getProperty(i).setClassOrigin(CIMName());
        }
    }

    // ATTN: patch the class object path because the repository does not set the host or namespace elements.
    cimClass.setPath(CIMObjectPath("localhost", "test_namespace", cimClass.getClassName()));

    return(cimClass);
}
