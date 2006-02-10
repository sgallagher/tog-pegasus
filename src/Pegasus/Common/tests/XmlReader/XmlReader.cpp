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
// Author: Yi Zhou (yi.zhou@hp.com)
//
// Modified By: David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include <Pegasus/Common/PegasusAssert.h>
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/CIMInstance.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static const char * verbose;

static void testGetErrorElement()
{
    CIMException cimException;
    Buffer text;
    FileSystem::loadFileToMemory(text, "./getErrorElement.xml");
    text.append('\0');

    CIMStatusCode errorCode = CIM_ERR_NO_SUCH_PROPERTY;
    String errorDescription = " The specified Property does not exist.";

    XmlParser parser((char*)text.getData());

    XmlReader::getErrorElement(parser, cimException);

    if (((unsigned)cimException.getCode() != (unsigned)errorCode) ||
        (cimException.getMessage() == errorDescription)
       )
    {
        throw cimException;
    }
}

// Tests PROPERTY as an embedded object.
static void testGetInstanceElement(const char* testDataFile)
{
    //--------------------------------------------------------------------------
    // Read in instance
    //--------------------------------------------------------------------------

    CIMInstance cimInstance;
    Buffer text;
    FileSystem::loadFileToMemory(text, testDataFile);
    text.append('\0');

    XmlParser parser((char*)text.getData());

    XmlReader::getInstanceElement(parser, cimInstance);
    PEGASUS_TEST_ASSERT(cimInstance.getClassName() == CIMName("CIM_InstCreation"));

    Uint32 idx;
    CIMProperty cimProperty;
    CIMValue cimValue;
    CIMType cimType;
    PEGASUS_TEST_ASSERT(cimInstance.getPropertyCount() == 3);

    idx = cimInstance.findProperty(CIMName("IndicationIdentifier"));
    PEGASUS_TEST_ASSERT(idx != PEG_NOT_FOUND);
    cimProperty = cimInstance.getProperty(idx);
    cimValue = cimProperty.getValue();
    cimType = cimProperty.getType();
    PEGASUS_TEST_ASSERT(strcmp(cimTypeToString(cimType), "string") == 0);
    String myString;
    cimValue.get(myString);
    PEGASUS_TEST_ASSERT(strcmp(myString.getCString(), "0") == 0);

    idx = cimInstance.findProperty(CIMName("IndicationTime"));
    PEGASUS_TEST_ASSERT(idx != PEG_NOT_FOUND);
    cimProperty = cimInstance.getProperty(idx);
    cimValue = cimProperty.getValue();
    cimType = cimProperty.getType();
    PEGASUS_TEST_ASSERT(strcmp(cimTypeToString(cimType), "datetime") == 0);
    CIMDateTime myDateTime;
    cimValue.get(myDateTime);
    PEGASUS_TEST_ASSERT(myDateTime.equal(CIMDateTime("20050227225624.524000-300")));

    idx = cimInstance.findProperty(CIMName("SourceInstance"));
    PEGASUS_TEST_ASSERT(idx != PEG_NOT_FOUND);
    cimProperty = cimInstance.getProperty(idx);
    cimValue = cimProperty.getValue();
    cimType = cimProperty.getType();
    PEGASUS_TEST_ASSERT(strcmp(cimTypeToString(cimType), "object") == 0);
    CIMObject cimObject;
    cimValue.get(cimObject);
    PEGASUS_TEST_ASSERT(cimObject.getClassName() == CIMName("Sample_LifecycleIndicationProviderClass"));
    PEGASUS_TEST_ASSERT(cimObject.getPropertyCount() == 2);

    idx = cimObject.findProperty(CIMName("uniqueId"));
    PEGASUS_TEST_ASSERT(idx != PEG_NOT_FOUND);
    cimProperty = cimObject.getProperty(idx);
    cimValue = cimProperty.getValue();
    cimType = cimProperty.getType();
    PEGASUS_TEST_ASSERT(strcmp(cimTypeToString(cimType), "uint32") == 0);
    Uint32 myUint32;
    cimValue.get(myUint32);
    PEGASUS_TEST_ASSERT(myUint32 == 1);

    idx = cimObject.findProperty(CIMName("lastOp"));
    PEGASUS_TEST_ASSERT(idx != PEG_NOT_FOUND);
    cimProperty = cimObject.getProperty(idx);
    cimValue = cimProperty.getValue();
    cimType = cimProperty.getType();
    PEGASUS_TEST_ASSERT(strcmp(cimTypeToString(cimType), "string") == 0);
    cimValue.get(myString);
    PEGASUS_TEST_ASSERT(strcmp(myString.getCString(), "createInstance") == 0);
}

// Tests PROPERTY.ARRAY as an embedded object with array type.
static void testGetInstanceElement2(const char* testDataFile)
{
    CIMInstance cimInstance;
    Buffer text;
    FileSystem::loadFileToMemory(text, testDataFile);
    text.append('\0');

    XmlParser parser((char*)text.getData());

    XmlReader::getInstanceElement(parser, cimInstance);
    PEGASUS_TEST_ASSERT(cimInstance.getClassName() == CIMName("CIM_InstCreation"));

    Uint32 idx;
    CIMProperty cimProperty;
    CIMValue cimValue;
    CIMType cimType;
    PEGASUS_TEST_ASSERT(cimInstance.getPropertyCount() == 3);

    idx = cimInstance.findProperty(CIMName("IndicationIdentifier"));
    PEGASUS_TEST_ASSERT(idx != PEG_NOT_FOUND);
    cimProperty = cimInstance.getProperty(idx);
    cimValue = cimProperty.getValue();
    cimType = cimProperty.getType();
    PEGASUS_TEST_ASSERT(strcmp(cimTypeToString(cimType), "string") == 0);
    String myString;
    cimValue.get(myString);
    PEGASUS_TEST_ASSERT(strcmp(myString.getCString(), "0") == 0);

    idx = cimInstance.findProperty(CIMName("IndicationTime"));
    PEGASUS_TEST_ASSERT(idx != PEG_NOT_FOUND);
    cimProperty = cimInstance.getProperty(idx);
    cimValue = cimProperty.getValue();
    cimType = cimProperty.getType();
    PEGASUS_TEST_ASSERT(strcmp(cimTypeToString(cimType), "datetime") == 0);
    CIMDateTime myDateTime;
    cimValue.get(myDateTime);
    PEGASUS_TEST_ASSERT(myDateTime.equal(CIMDateTime("20050227225624.524000-300")));

    idx = cimInstance.findProperty(CIMName("SourceInstance"));
    PEGASUS_TEST_ASSERT(idx != PEG_NOT_FOUND);
    cimProperty = cimInstance.getProperty(idx);
    cimValue = cimProperty.getValue();
    cimType = cimProperty.getType();
    PEGASUS_TEST_ASSERT(strcmp(cimTypeToString(cimType), "object") == 0);
    Array<CIMObject> cimObject;
    cimValue.get(cimObject);
    PEGASUS_TEST_ASSERT(cimObject.size() == 2);
    for (idx = 0; idx < cimObject.size(); idx++)
    {
        CIMInstance cimInstanceElement(cimObject[idx]);
        PEGASUS_TEST_ASSERT(cimInstanceElement.getPropertyCount() == 2);
        Uint32 propIdx = cimInstanceElement.findProperty(CIMName("uniqueId"));
        if (propIdx != PEG_NOT_FOUND)
        {
            CIMProperty nestedProperty = cimInstanceElement.getProperty(propIdx);
            cimValue = nestedProperty.getValue();
            Uint32 uniqueId;
            cimValue.get(uniqueId);
            propIdx = cimInstanceElement.findProperty(CIMName("lastOp"));
            nestedProperty = cimInstanceElement.getProperty(propIdx);
            cimValue = nestedProperty.getValue();
            String checkStringValue;
            cimValue.get(checkStringValue);
            if (uniqueId == 1)
                PEGASUS_TEST_ASSERT(strcmp(checkStringValue.getCString(), "createInstance") == 0);
            else if (uniqueId == 2)
                PEGASUS_TEST_ASSERT(strcmp(checkStringValue.getCString(), "deleteInstance") == 0);
            else
                PEGASUS_TEST_ASSERT(false);
        }
    }
}

int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");

    try
    {
        testGetErrorElement();

        //
        if (verbose)
        {
            cout << "Testing EmbeddedObject VALUE Property with XML entity references." << endl;
        }
        testGetInstanceElement("./getInstanceElement.xml");

        //
        if (verbose)
        {
            cout << "Testing EmbeddedObject VALUE.ARRAY Property with XML entity references." << endl;
        }
        testGetInstanceElement2("./getInstanceElement2.xml");

        //
        if (verbose)
        {
            cout << "Testing EmbeddedObject VALUE Property with <![CDATA[...]]> escaping." << endl;
        }
        testGetInstanceElement("./getInstanceElementCDATA.xml");

        //
        if (verbose)
        {
            cout << "Testing EmbeddedObject VALUE.ARRAY Property with <![CDATA[...]]> escaping." << endl;
        }
        testGetInstanceElement2("./getInstanceElementCDATA2.xml");
    }
    catch(Exception& e)
    {
        cerr << "Error: " << e.getMessage() << endl;
        exit(1);
    }

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
