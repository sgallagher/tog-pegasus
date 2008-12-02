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
#include <Pegasus/Common/CIMError.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static Boolean verbose;


// Tests getting error Element and determing if data matches the predefined
// error elemement xml.
static void testGetErrorElement()
{
    CIMException cimException;
    Buffer text;
    FileSystem::loadFileToMemory(text, "./getErrorElement.xml");

    CIMStatusCode errorCode = CIM_ERR_NO_SUCH_PROPERTY;
    String errorDescription = " The specified Property does not exist.";
    XmlParser parser((char*)text.getData());
    XmlReader::getErrorElement(parser, cimException);

    if (((unsigned)cimException.getCode() != (unsigned)errorCode) ||
        (cimException.getMessage() == errorDescription) )
    {
        throw cimException;
    }
    PEGASUS_TEST_ASSERT(cimException.getErrorCount() == 1);

    CIMInstance errorInstance = cimException.getError(0).clone();

    // Convert it back to a CIM_Error for analysis
    CIMError err1;
    err1.setInstance(errorInstance);

    // Test for correct property data.
    CIMError::CIMStatusCodeEnum statusCodeRtn;
    PEGASUS_TEST_ASSERT(err1.getCIMStatusCode(statusCodeRtn));
    PEGASUS_TEST_ASSERT(
        statusCodeRtn == CIMError::CIM_STATUS_CODE_CIM_ERR_INVALID_PARAMETER);

    String OwningEntity;
    PEGASUS_TEST_ASSERT(err1.getOwningEntity(OwningEntity));
    PEGASUS_TEST_ASSERT(OwningEntity == "OpenPegasus");

    String messageIDrtn;
    PEGASUS_TEST_ASSERT(err1.getMessageID(messageIDrtn));
    PEGASUS_TEST_ASSERT(messageIDrtn == "2206");
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

    XmlParser parser((char*)text.getData());

    XmlReader::getInstanceElement(parser, cimInstance);
    PEGASUS_TEST_ASSERT(
        cimInstance.getClassName() == CIMName("CIM_InstCreation"));

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
    PEGASUS_TEST_ASSERT(myDateTime.equal(
        CIMDateTime("20050227225624.524000-300")));

    idx = cimInstance.findProperty(CIMName("SourceInstance"));
    PEGASUS_TEST_ASSERT(idx != PEG_NOT_FOUND);
    cimProperty = cimInstance.getProperty(idx);
    cimValue = cimProperty.getValue();
    cimType = cimProperty.getType();
    PEGASUS_TEST_ASSERT(strcmp(cimTypeToString(cimType), "object") == 0);
    CIMObject cimObject;
    cimValue.get(cimObject);
    PEGASUS_TEST_ASSERT(
        cimObject.getClassName() == 
            CIMName("Sample_LifecycleIndicationProviderClass"));
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

    XmlParser parser((char*)text.getData());

    XmlReader::getInstanceElement(parser, cimInstance);
    PEGASUS_TEST_ASSERT(cimInstance.getClassName() == 
                        CIMName("CIM_InstCreation"));

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
    PEGASUS_TEST_ASSERT(myDateTime.equal(
        CIMDateTime("20050227225624.524000-300")));

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
            CIMProperty nestedProperty = 
                cimInstanceElement.getProperty(propIdx);
            cimValue = nestedProperty.getValue();
            Uint32 uniqueId;
            cimValue.get(uniqueId);
            propIdx = cimInstanceElement.findProperty(CIMName("lastOp"));
            nestedProperty = cimInstanceElement.getProperty(propIdx);
            cimValue = nestedProperty.getValue();
            String checkStringValue;
            cimValue.get(checkStringValue);
            if (uniqueId == 1)
            {
                PEGASUS_TEST_ASSERT(strcmp(
                    checkStringValue.getCString(), "createInstance") == 0);
            }
            else if (uniqueId == 2)
            {
                PEGASUS_TEST_ASSERT(strcmp(
                    checkStringValue.getCString(), "deleteInstance") == 0);
            }
            else
            {
                PEGASUS_TEST_ASSERT(false);
            }
        }
    }
}

void testVersionFunctions()
{
    // Test DTDVersion function for legal input
    PEGASUS_TEST_ASSERT (XmlReader::isSupportedDTDVersion("2.0"));
    PEGASUS_TEST_ASSERT (XmlReader::isSupportedDTDVersion("2.1"));
    PEGASUS_TEST_ASSERT (XmlReader::isSupportedDTDVersion("2.9"));
    PEGASUS_TEST_ASSERT (XmlReader::isSupportedDTDVersion("2.99"));
    PEGASUS_TEST_ASSERT (!XmlReader::isSupportedDTDVersion("2.A"));
    PEGASUS_TEST_ASSERT (!XmlReader::isSupportedDTDVersion("2."));
    PEGASUS_TEST_ASSERT (!XmlReader::isSupportedDTDVersion("2"));
    PEGASUS_TEST_ASSERT (!XmlReader::isSupportedDTDVersion("1.1"));
    // Test protocolVersion Function
    PEGASUS_TEST_ASSERT (XmlReader::isSupportedProtocolVersion(String("1.0")));
    PEGASUS_TEST_ASSERT (XmlReader::isSupportedProtocolVersion(String("1.1")));
    PEGASUS_TEST_ASSERT (XmlReader::isSupportedProtocolVersion(String("1.9")));
    PEGASUS_TEST_ASSERT (XmlReader::isSupportedProtocolVersion(String("1.99")));
    PEGASUS_TEST_ASSERT (!XmlReader::isSupportedProtocolVersion(String("1.A")));
    PEGASUS_TEST_ASSERT (!XmlReader::isSupportedProtocolVersion(String("1.")));
    PEGASUS_TEST_ASSERT (!XmlReader::isSupportedProtocolVersion(String("2.0")));

    // Test cimVersion

    PEGASUS_TEST_ASSERT (XmlReader::isSupportedCIMVersion("2.0"));
    PEGASUS_TEST_ASSERT (XmlReader::isSupportedCIMVersion("2.1"));
    PEGASUS_TEST_ASSERT (XmlReader::isSupportedCIMVersion("2.9"));
    PEGASUS_TEST_ASSERT (XmlReader::isSupportedCIMVersion("2.99"));
    PEGASUS_TEST_ASSERT (!XmlReader::isSupportedCIMVersion("2.A"));
    PEGASUS_TEST_ASSERT (!XmlReader::isSupportedCIMVersion("2."));
    PEGASUS_TEST_ASSERT (!XmlReader::isSupportedCIMVersion("2"));
    PEGASUS_TEST_ASSERT (!XmlReader::isSupportedCIMVersion("1.1"));
}

int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE") ? true : false;

    try
    {
        if (verbose)
            cout << "Testing GetErrorElement." << endl;
        testGetErrorElement();

        //
        if (verbose)
            cout <<
            "Testing EmbeddedObject VALUE Property with XML entity references."
            << endl;
        testGetInstanceElement("./getInstanceElement.xml");

        //
        if (verbose)
            cout <<
            "Testing EmbeddedObject VALUE.ARRAY Property with XML entity"
            " references."
            << endl;
        testGetInstanceElement2("./getInstanceElement2.xml");

        //
        if (verbose)
            cout <<
            "Testing EmbeddedObject VALUE Property with <![CDATA[...]]>"
            " escaping."
            << endl;
        testGetInstanceElement("./getInstanceElementCDATA.xml");

        //
        if (verbose)
            cout <<
            "Testing EmbeddedObject VALUE.ARRAY Property with <![CDATA[...]]>"
            " escaping."
            << endl;
        testGetInstanceElement2("./getInstanceElementCDATA2.xml");

        // Test the three version functions
        if (verbose)
            cout << "Testing VersionFunctions" << endl;
        testVersionFunctions();
    }
    catch(Exception& e)
    {
        cerr << "Error: " << e.getMessage() << endl;
        exit(1);
    }

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
