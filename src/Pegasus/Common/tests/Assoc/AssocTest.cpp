//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/DeclContext.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

CIMClass CreateGraphic()
{
    CIMClass graphic("Graphic");

    CIMProperty color("color", String());
    color.addQualifier(CIMQualifier("key", true));
    graphic.addProperty(color);

    return graphic;
}

CIMClass CreateAssocClass()
{
    CIMClass assocClass("Assoc");
    assocClass.addQualifier(CIMQualifier("Association", true));

    CIMProperty ref1("ref1", CIMReference(), 0, "Graphic");
    ref1.addQualifier(CIMQualifier("key", true));
    assocClass.addProperty(ref1);

    CIMProperty ref2("ref2", CIMReference(), 0, "Graphic");
    ref2.addQualifier(CIMQualifier("key", true));
    assocClass.addProperty(ref2);

    return assocClass;
}

CIMInstance CreateAssocInstance()
{
    CIMInstance assocInstance("Graphic");

    // ATTN-1: It should not be necessary to specify the reference
    // class name when specifying an instance property!

    CIMProperty ref1(
	"ref1", CIMReference("Graphic.color=\"red\""), 0, "Graphic");
    assocInstance.addProperty(ref1);

    CIMProperty ref2(
	"ref2", CIMReference("Graphic.color=\"green\""), 0, "Graphic");
    assocInstance.addProperty(ref2);

    return assocInstance;
}

void test01()
{
    CIMClass graphic = CreateGraphic();
    CIMClass assocClass = CreateAssocClass();
    CIMInstance assocInstance = CreateAssocInstance();
    CIMReference instanceName = assocInstance.getInstanceName(assocClass);

    PEGASUS_OUT(instanceName);
}

int main()
{
    try
    {
	test01();
    }
    catch (Exception& e)
    {
	cout << "Exception: " << e.getMessage() << endl;
	exit(1);
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
