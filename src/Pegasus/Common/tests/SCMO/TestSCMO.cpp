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

#include "TestSCMO.h"
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/FileSystem.h>


PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static Boolean verbose;

void test1()
{
    CIMClass myClass;
    Buffer text;
    FileSystem::loadFileToMemory(text, "./TestSCMO.xml");

    try
    {

        XmlParser theParser((char*)text.getData());
        XmlReader::getObject(theParser,myClass);
    }
    catch (CIMException& e)
    {
        printf("\nCIMException:\n\n %s\n\n",
               (const char*)e.getMessage().getCString());
        exit(-1);
    }

    catch (Exception& e)
    {
        printf("\nException:\n\n %s\n\n",
               (const char*)e.getMessage().getCString());
        exit(-1);
    }

    
    SCMOClass theSCMOClass(myClass);
    
    SCMODump dump;
    
    dump.dumpSCMOClass(theSCMOClass);
    
    SCMOInstance myInstance(theSCMOClass);
    
    SCMOInstance cloneInstance = myInstance.clone();

}


void test2()
{
    CIMClass myClass;
    Buffer text;

    SCMO_RC rc;

    FileSystem::loadFileToMemory(text, "./TestSCMO2.xml");

    try
    {

        XmlParser theParser((char*)text.getData());
        XmlReader::getObject(theParser,myClass);
    }
    catch (CIMException& e)
    {
        printf("\nCIMException:\n\n %s\n\n",
               (const char*)e.getMessage().getCString());
        exit(-1);
    }

    catch (Exception& e)
    {
        printf("\nException:\n\n %s\n\n",
               (const char*)e.getMessage().getCString());
        exit(-1);
    }


    SCMODump dump;
    SCMOClass theSCMOClass(myClass);

    // dump.dumpSCMOClassQualifiers(theSCMOClass);

    SCMOInstance myInstance(theSCMOClass);

    // definition of return values.
    const void* voidReturn;
    CIMType typeReturn;
    Boolean isArrayReturn;
    Uint32 sizeReturn;

    
    Boolean boolValue=true;
    CIMDateTime dateTimeValue(CIMDateTime::getCurrentDateTime());
    


    /**
     * Negative test cases for setting a propertty
     */

    rc = myInstance.setPropertyWithOrigin(
        "NotAProperty",
        CIMTYPE_BOOLEAN,
        &boolValue);

    PEGASUS_TEST_ASSERT(rc==SCMO_NOT_FOUND);

    rc = myInstance.setPropertyWithOrigin(
        "DateTimeProperty",
        CIMTYPE_BOOLEAN,
        &boolValue);

    PEGASUS_TEST_ASSERT(rc==SCMO_WRONG_TYPE);

    rc = myInstance.setPropertyWithOrigin(
        "DateTimeProperty",
        CIMTYPE_DATETIME,
        &dateTimeValue,
        true,10);

    PEGASUS_TEST_ASSERT(rc==SCMO_NOT_AN_ARRAY);

    rc = myInstance.setPropertyWithOrigin(
        "DateTimePropertyArray",
        CIMTYPE_DATETIME,
        &dateTimeValue);

    PEGASUS_TEST_ASSERT(rc==SCMO_IS_AN_ARRAY);

    Uint32 uint32value = 42;
    
    rc = myInstance.setPropertyWithOrigin(
        "Uint32Property",
        CIMTYPE_UINT32,
        &uint32value);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    rc = myInstance.getProperty(
        "Uint32Property",
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    PEGASUS_TEST_ASSERT(uint32value == *((Uint32*)voidReturn));

}



int main (int argc, char *argv[])
{

    verbose = getenv("PEGASUS_TEST_VERBOSE") ? true : false;

    test2();

    cout << argv[0] << " +++++ passed all tests" << endl;
    return 0;

}
