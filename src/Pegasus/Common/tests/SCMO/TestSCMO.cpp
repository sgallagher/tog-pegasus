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

#define VCOUT if (verbose) cout

static Boolean verbose;

const String MASTERQUALIFIER ("/src/Pegasus/Common/tests/SCMO/masterQualifier");
const String MASTERCLASS ("/src/Pegasus/Common/tests/SCMO/masterClass");
const String TESTSCMOXML("/src/Pegasus/Common/tests/SCMO/TestSCMO.xml");
const String TESTSCMO2XML("/src/Pegasus/Common/tests/SCMO/TestSCMO2.xml");

void CIMClassToSCMOClass()
{
    CIMClass theCIMClass;
    Buffer text;
    VCOUT << endl << "CIMClass to SCMOClass..." << endl;


    VCOUT << "Loading CIM SCMO_TESTClass" << endl;

    String TestSCMOXML (getenv("PEGASUS_ROOT"));
    TestSCMOXML.append(TESTSCMOXML);

    FileSystem::loadFileToMemory(text,(const char*)TestSCMOXML.getCString());

    XmlParser theParser((char*)text.getData());
    XmlReader::getObject(theParser,theCIMClass);


    SCMOClass theSCMOClass(theCIMClass);

    SCMODump dump("TestSCMOClass.log");

    dump.dumpSCMOClass(theSCMOClass);

    String masterFile (getenv("PEGASUS_ROOT"));
    masterFile.append(MASTERCLASS);

    PEGASUS_TEST_ASSERT(dump.compareFile(masterFile));

    dump.deleteFile();

    VCOUT << "Creaing SCMO instance out of SCMOClass." << endl;

    SCMOInstance theSCMOInstance(theSCMOClass);

    dump.closeFile();

    char* tmp = "TestSCMO Class";
    theSCMOInstance.setPropertyWithOrigin(
        "CreationClassName",
        CIMTYPE_STRING,
        tmp);

    Boolean isCought = false;

    try
    {
        theSCMOInstance.buildKeyBindingsFromProperties();
    }
    catch(NoSuchProperty& e)
    {
         isCought = true;
    }
    if (!isCought)
    {
        cout << endl << "'NoSuchProperty' exception "
                "in buildKeyBindingsFromProperties()!" << endl;
        dump.dumpSCMOInstanceKeyBindings(theSCMOInstance);
        exit(-1);
    }

    char* tmp2 ="This is the Name";
    theSCMOInstance.setPropertyWithOrigin(
        "Name",
        CIMTYPE_STRING,
        tmp2);

    theSCMOInstance.buildKeyBindingsFromProperties();

    SCMOInstance cloneInstance = theSCMOInstance.clone();

    VCOUT << endl << "Test 1: Done." << endl;

}


void loadSCMO_TESTClass2(CIMClass& CIM_TESTClass2)
{
    Buffer text;

    SCMO_RC rc;

    VCOUT << endl << "Loading CIM SCMO_TESTClass2" << endl;

    String TestSCMO2XML (getenv("PEGASUS_ROOT"));
    TestSCMO2XML.append(TESTSCMO2XML);

    FileSystem::loadFileToMemory(text,(const char*)TestSCMO2XML.getCString());

    XmlParser theParser((char*)text.getData());
    XmlReader::getObject(theParser,CIM_TESTClass2);

    VCOUT << endl << "Done." << endl;

    return;
}

void SCMOClassQualifierTest(SCMOClass& SCMO_TESTClass2)
{
    SCMO_RC rc;
    VCOUT << endl << "SCMOClass qualifer test ..." << endl;

    String masterFile (getenv("PEGASUS_ROOT"));
    masterFile.append(MASTERQUALIFIER);

    SCMODump dump("TestSCMOClassQualifier.log");

    dump.dumpSCMOClassQualifiers(SCMO_TESTClass2);

    PEGASUS_TEST_ASSERT(dump.compareFile(masterFile));

    dump.deleteFile();

    VCOUT << "Done." << endl;
}

void SCMOInstancePropertyTest(SCMOInstance& SCMO_TESTClass2_Inst)
{
    SCMO_RC rc;

    // definition of return values.
    const void* voidReturn;
    CIMType typeReturn;
    Boolean isArrayReturn;
    Uint32 sizeReturn;


    Boolean boolValue=true;

    /**
     * Negative test cases for setting a propertty
     */

    VCOUT << endl << 
        "SCMOInstance Negative test cases for setting a property ..." 
        << endl << endl;

    VCOUT << "Invalid property name." << endl;

    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "NotAProperty",
        CIMTYPE_BOOLEAN,
        &boolValue);

    PEGASUS_TEST_ASSERT(rc==SCMO_NOT_FOUND);

    VCOUT << "Property type is differente." << endl;
    
    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "DateTimeProperty",
        CIMTYPE_BOOLEAN,
        &boolValue);

    PEGASUS_TEST_ASSERT(rc==SCMO_WRONG_TYPE);

    VCOUT << "Property is not an array." << endl;
    
    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "BooleanProperty",
        CIMTYPE_BOOLEAN,
        &boolValue,
        true,10);

    PEGASUS_TEST_ASSERT(rc==SCMO_NOT_AN_ARRAY);

    VCOUT << "Value is not an array." << endl;

    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "BooleanPropertyArray",
        CIMTYPE_BOOLEAN,
        &boolValue);

    PEGASUS_TEST_ASSERT(rc==SCMO_IS_AN_ARRAY);

    VCOUT << "Empty Array." << endl;
    
    Uint32 *uint32ArrayValue;

    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "Uint32PropertyArray",
        CIMTYPE_UINT32,
        &uint32ArrayValue,
        true,0);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    rc = SCMO_TESTClass2_Inst.getProperty(
        "Uint32PropertyArray",
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);
    PEGASUS_TEST_ASSERT(sizeReturn==0);
    PEGASUS_TEST_ASSERT(isArrayReturn);
    PEGASUS_TEST_ASSERT(voidReturn==NULL);

    VCOUT << "Get default value of the class." << endl;
    rc = SCMO_TESTClass2_Inst.getProperty(
        "BooleanProperty",
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);
    // The default value of this instance is TRUE
    PEGASUS_TEST_ASSERT(*((Boolean*)voidReturn));

 
    VCOUT << endl << "Done." << endl << endl;

    VCOUT << "SCMOInstance setting and reading properties ..." << endl;

    /**
     * Test Char16
     */

    VCOUT << endl << "Test Char16" << endl;

    Char16 char16value = 0x3F4A;

    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "Char16Property",
        CIMTYPE_CHAR16,
        &char16value);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    rc = SCMO_TESTClass2_Inst.getProperty(
        "Char16Property",
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);
    PEGASUS_TEST_ASSERT(sizeReturn==0);
    PEGASUS_TEST_ASSERT(!isArrayReturn);

    PEGASUS_TEST_ASSERT(char16value == *((Char16*)voidReturn));

    Char16 char16ArrayValue[]={1024,2048,4096};

    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "Char16PropertyArray",
        CIMTYPE_CHAR16,
        &char16ArrayValue,
        true,3);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    rc = SCMO_TESTClass2_Inst.getProperty(
        "Char16PropertyArray",
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);

    PEGASUS_TEST_ASSERT(sizeReturn==3);
    PEGASUS_TEST_ASSERT(isArrayReturn);
    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    PEGASUS_TEST_ASSERT(char16ArrayValue[0] == ((Char16*)voidReturn)[0]);
    PEGASUS_TEST_ASSERT(char16ArrayValue[1] == ((Char16*)voidReturn)[1]);
    PEGASUS_TEST_ASSERT(char16ArrayValue[2] == ((Char16*)voidReturn)[2]);

    /**
     * Test Uint8
     */
    VCOUT << "Test Uint8" << endl;

    Uint8 uint8value = 0x77;

    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "Uint8Property",
        CIMTYPE_UINT8,
        &uint8value);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    rc = SCMO_TESTClass2_Inst.getProperty(
        "Uint8Property",
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);
    PEGASUS_TEST_ASSERT(sizeReturn==0);
    PEGASUS_TEST_ASSERT(!isArrayReturn);

    PEGASUS_TEST_ASSERT(uint8value == *((Uint8*)voidReturn));

    Uint8 uint8ArrayValue[]={42,155,192};

    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "Uint8PropertyArray",
        CIMTYPE_UINT8,
        &uint8ArrayValue,
        true,3);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    rc = SCMO_TESTClass2_Inst.getProperty(
        "Uint8PropertyArray",
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);

    PEGASUS_TEST_ASSERT(sizeReturn==3);
    PEGASUS_TEST_ASSERT(isArrayReturn);
    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    PEGASUS_TEST_ASSERT(uint8ArrayValue[0] == ((Uint8*)voidReturn)[0]);
    PEGASUS_TEST_ASSERT(uint8ArrayValue[1] == ((Uint8*)voidReturn)[1]);
    PEGASUS_TEST_ASSERT(uint8ArrayValue[2] == ((Uint8*)voidReturn)[2]);

    /**
     * Test Uint16
     */

    VCOUT << "Test Uint16" << endl;

    Uint16 uint16value = 0xF77F;

    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "Uint16Property",
        CIMTYPE_UINT16,
        &uint16value);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    rc = SCMO_TESTClass2_Inst.getProperty(
        "Uint16Property",
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);
    PEGASUS_TEST_ASSERT(sizeReturn==0);
    PEGASUS_TEST_ASSERT(!isArrayReturn);

    PEGASUS_TEST_ASSERT(uint16value == *((Uint16*)voidReturn));

    Uint16 uint16ArrayValue[]={218,2673,172};

    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "Uint16PropertyArray",
        CIMTYPE_UINT16,
        &uint16ArrayValue,
        true,3);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    rc = SCMO_TESTClass2_Inst.getProperty(
        "Uint16PropertyArray",
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);
    PEGASUS_TEST_ASSERT(sizeReturn==3);
    PEGASUS_TEST_ASSERT(isArrayReturn);

    PEGASUS_TEST_ASSERT(uint16ArrayValue[0] == ((Uint16*)voidReturn)[0]);
    PEGASUS_TEST_ASSERT(uint16ArrayValue[1] == ((Uint16*)voidReturn)[1]);
    PEGASUS_TEST_ASSERT(uint16ArrayValue[2] == ((Uint16*)voidReturn)[2]);



    /**
     * Test Uint32
     */

    VCOUT << "Test Uint32" << endl;

    Uint32 uint32value = 0xF7F7F7F7;

    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "Uint32Property",
        CIMTYPE_UINT32,
        &uint32value);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    rc = SCMO_TESTClass2_Inst.getProperty(
        "Uint32Property",
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);
    PEGASUS_TEST_ASSERT(sizeReturn==0);
    PEGASUS_TEST_ASSERT(!isArrayReturn);

    PEGASUS_TEST_ASSERT(uint32value == *((Uint32*)voidReturn));

    Uint32 uint32ArrayValue2[]={42,289,192};

    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "Uint32PropertyArray",
        CIMTYPE_UINT32,
        &uint32ArrayValue2,
        true,3);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    rc = SCMO_TESTClass2_Inst.getProperty(
        "Uint32PropertyArray",
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);

    PEGASUS_TEST_ASSERT(sizeReturn==3);
    PEGASUS_TEST_ASSERT(isArrayReturn);
    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    PEGASUS_TEST_ASSERT(uint32ArrayValue2[0] == ((Uint32*)voidReturn)[0]);
    PEGASUS_TEST_ASSERT(uint32ArrayValue2[1] == ((Uint32*)voidReturn)[1]);
    PEGASUS_TEST_ASSERT(uint32ArrayValue2[2] == ((Uint32*)voidReturn)[2]);


    /**
     * Test Uint64
     */

    VCOUT << "Test Uint64" << endl;

    Uint64 uint64value = PEGASUS_UINT64_LITERAL(0xA0A0B0B0C0C0D0D0);

    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "Uint64Property",
        CIMTYPE_UINT64,
        &uint64value);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    rc = SCMO_TESTClass2_Inst.getProperty(
        "Uint64Property",
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);
    PEGASUS_TEST_ASSERT(sizeReturn==0);
    PEGASUS_TEST_ASSERT(!isArrayReturn);

    PEGASUS_TEST_ASSERT(uint64value == *((Uint64*)voidReturn));

    Uint64 uint64ArrayValue[]={
        394,
        483734,
        PEGASUS_UINT64_LITERAL(0x1234567890ABCDEF),
        23903483
        };

    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "Uint64PropertyArray",
        CIMTYPE_UINT64,
        &uint64ArrayValue,
        true,4);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    rc = SCMO_TESTClass2_Inst.getProperty(
        "Uint64PropertyArray",
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);

    PEGASUS_TEST_ASSERT(sizeReturn==4);
    PEGASUS_TEST_ASSERT(isArrayReturn);
    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    PEGASUS_TEST_ASSERT(uint64ArrayValue[0] == ((Uint64*)voidReturn)[0]);
    PEGASUS_TEST_ASSERT(uint64ArrayValue[1] == ((Uint64*)voidReturn)[1]);
    PEGASUS_TEST_ASSERT(uint64ArrayValue[2] == ((Uint64*)voidReturn)[2]);
    PEGASUS_TEST_ASSERT(uint64ArrayValue[3] == ((Uint64*)voidReturn)[3]);



    /**
     * Test Sint8
     */

    VCOUT << "Test Sint8" << endl;

    Sint8 sint8value = 0xF3;

    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "Sint8Property",
        CIMTYPE_SINT8,
        &sint8value);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    rc = SCMO_TESTClass2_Inst.getProperty(
        "Sint8Property",
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);
    PEGASUS_TEST_ASSERT(sizeReturn==0);
    PEGASUS_TEST_ASSERT(!isArrayReturn);

    PEGASUS_TEST_ASSERT(sint8value == *((Sint8*)voidReturn));

    Sint8 sint8ArrayValue[]={-2,94,-123};

    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "Sint8PropertyArray",
        CIMTYPE_SINT8,
        &sint8ArrayValue,
        true,3);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    rc = SCMO_TESTClass2_Inst.getProperty(
        "Sint8PropertyArray",
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);

    PEGASUS_TEST_ASSERT(sizeReturn==3);
    PEGASUS_TEST_ASSERT(isArrayReturn);
    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    PEGASUS_TEST_ASSERT(sint8ArrayValue[0] == ((Sint8*)voidReturn)[0]);
    PEGASUS_TEST_ASSERT(sint8ArrayValue[1] == ((Sint8*)voidReturn)[1]);
    PEGASUS_TEST_ASSERT(sint8ArrayValue[2] == ((Sint8*)voidReturn)[2]);

    /**
     * Test Sint16
     */

    VCOUT << "Test Sint16" << endl;

    Sint16 sint16value = 0xF24B;

    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "Sint16Property",
        CIMTYPE_SINT16,
        &sint16value);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    rc = SCMO_TESTClass2_Inst.getProperty(
        "Sint16Property",
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);
    PEGASUS_TEST_ASSERT(sizeReturn==0);
    PEGASUS_TEST_ASSERT(!isArrayReturn);

    PEGASUS_TEST_ASSERT(sint16value == *((Sint16*)voidReturn));

    Sint16 sint16ArrayValue[]={
        -8,
        23872,
        334,
        0xF00F};

    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "Sint16PropertyArray",
        CIMTYPE_SINT16,
        &sint16ArrayValue,
        true,4);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    rc = SCMO_TESTClass2_Inst.getProperty(
        "Sint16PropertyArray",
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);
    PEGASUS_TEST_ASSERT(sizeReturn==4);
    PEGASUS_TEST_ASSERT(isArrayReturn);

    PEGASUS_TEST_ASSERT(sint16ArrayValue[0] == ((Sint16*)voidReturn)[0]);
    PEGASUS_TEST_ASSERT(sint16ArrayValue[1] == ((Sint16*)voidReturn)[1]);
    PEGASUS_TEST_ASSERT(sint16ArrayValue[2] == ((Sint16*)voidReturn)[2]);
    PEGASUS_TEST_ASSERT(sint16ArrayValue[3] == ((Sint16*)voidReturn)[3]);


    /**
     * Test Sint32
     */

    VCOUT << "Test Sint32" << endl;

    Sint32 sint32value = 0xF0783C;

    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "Sint32Property",
        CIMTYPE_SINT32,
        &sint32value);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    rc = SCMO_TESTClass2_Inst.getProperty(
        "Sint32Property",
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);
    PEGASUS_TEST_ASSERT(sizeReturn==0);
    PEGASUS_TEST_ASSERT(!isArrayReturn);

    PEGASUS_TEST_ASSERT(sint32value == *((Sint32*)voidReturn));

    Sint32 sint32ArrayValue[]={42,-28937332,19248372};

    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "Sint32PropertyArray",
        CIMTYPE_SINT32,
        &sint32ArrayValue,
        true,3);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    rc = SCMO_TESTClass2_Inst.getProperty(
        "Sint32PropertyArray",
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);
    PEGASUS_TEST_ASSERT(sizeReturn==3);
    PEGASUS_TEST_ASSERT(isArrayReturn);

    PEGASUS_TEST_ASSERT(sint32ArrayValue[0] == ((Sint32*)voidReturn)[0]);
    PEGASUS_TEST_ASSERT(sint32ArrayValue[1] == ((Sint32*)voidReturn)[1]);
    PEGASUS_TEST_ASSERT(sint32ArrayValue[2] == ((Sint32*)voidReturn)[2]);

    /**
     * Test Sint64
     */

    VCOUT << "Test Sint64" << endl;

    Sint64 sint64value = (Sint64)-1;

    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "Sint64Property",
        CIMTYPE_SINT64,
        &sint64value);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    rc = SCMO_TESTClass2_Inst.getProperty(
        "Sint64Property",
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);
    PEGASUS_TEST_ASSERT(sizeReturn==0);
    PEGASUS_TEST_ASSERT(!isArrayReturn);

    PEGASUS_TEST_ASSERT(sint64value == *((Sint64*)voidReturn));

    Sint64 sint64ArrayValue[]={394,-483734324,232349034};

    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "Sint64PropertyArray",
        CIMTYPE_SINT64,
        &sint64ArrayValue,
        true,3);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    rc = SCMO_TESTClass2_Inst.getProperty(
        "Sint64PropertyArray",
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);
    PEGASUS_TEST_ASSERT(sizeReturn==3);
    PEGASUS_TEST_ASSERT(isArrayReturn);

    PEGASUS_TEST_ASSERT(sint64ArrayValue[0] == ((Sint64*)voidReturn)[0]);
    PEGASUS_TEST_ASSERT(sint64ArrayValue[1] == ((Sint64*)voidReturn)[1]);
    PEGASUS_TEST_ASSERT(sint64ArrayValue[2] == ((Sint64*)voidReturn)[2]);

    /**
     * Test Real32
     */

    VCOUT << "Test Real32" << endl;

    Real32 real32value = 2.4271e-4;

    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "Real32Property",
        CIMTYPE_REAL32,
        &real32value);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    rc = SCMO_TESTClass2_Inst.getProperty(
        "Real32Property",
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);
    PEGASUS_TEST_ASSERT(sizeReturn==0);
    PEGASUS_TEST_ASSERT(!isArrayReturn);

    PEGASUS_TEST_ASSERT(real32value == *((Real32*)voidReturn));

    Real32 real32ArrayValue[]={3.94e30,-4.83734324e-35,2.323490e34};

    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "Real32PropertyArray",
        CIMTYPE_REAL32,
        &real32ArrayValue,
        true,3);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    rc = SCMO_TESTClass2_Inst.getProperty(
        "Real32PropertyArray",
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);
    PEGASUS_TEST_ASSERT(sizeReturn==3);
    PEGASUS_TEST_ASSERT(isArrayReturn);

    PEGASUS_TEST_ASSERT(real32ArrayValue[0] == ((Real32*)voidReturn)[0]);
    PEGASUS_TEST_ASSERT(real32ArrayValue[1] == ((Real32*)voidReturn)[1]);
    PEGASUS_TEST_ASSERT(real32ArrayValue[2] == ((Real32*)voidReturn)[2]);

    /**
     * Test Real64
     */

    VCOUT << "Test Real64" << endl;

    Real64 real64value = Real64(2.4271e-40);

    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "Real64Property",
        CIMTYPE_REAL64,
        &real64value);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    rc = SCMO_TESTClass2_Inst.getProperty(
        "Real64Property",
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);
    PEGASUS_TEST_ASSERT(sizeReturn==0);
    PEGASUS_TEST_ASSERT(!isArrayReturn);

    PEGASUS_TEST_ASSERT(real64value == *((Real64*)voidReturn));

    Real64 real64ArrayValue[]={
        Real64(3.94e38),
        Real64(-4.83734644e-35),
        Real64(2.643490e34)};

    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "Real64PropertyArray",
        CIMTYPE_REAL64,
        &real64ArrayValue,
        true,3);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    rc = SCMO_TESTClass2_Inst.getProperty(
        "Real64PropertyArray",
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);
    PEGASUS_TEST_ASSERT(sizeReturn==3);
    PEGASUS_TEST_ASSERT(isArrayReturn);

    PEGASUS_TEST_ASSERT(real64ArrayValue[0] == ((Real64*)voidReturn)[0]);
    PEGASUS_TEST_ASSERT(real64ArrayValue[1] == ((Real64*)voidReturn)[1]);
    PEGASUS_TEST_ASSERT(real64ArrayValue[2] == ((Real64*)voidReturn)[2]);

    /**
     * Test Boolean
     */

    VCOUT << "Test Boolean" << endl;

    boolValue=true;

    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "BooleanProperty",
        CIMTYPE_BOOLEAN,
        &boolValue);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    rc = SCMO_TESTClass2_Inst.getProperty(
        "BooleanProperty",
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);
    PEGASUS_TEST_ASSERT(sizeReturn==0);
    PEGASUS_TEST_ASSERT(!isArrayReturn);

    PEGASUS_TEST_ASSERT(*((Boolean*)voidReturn));

    Boolean boolArrayValue[]={true,false,true};

    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "BooleanPropertyArray",
        CIMTYPE_BOOLEAN,
        &boolArrayValue,
        true,3);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    rc = SCMO_TESTClass2_Inst.getProperty(
        "BooleanPropertyArray",
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);
    PEGASUS_TEST_ASSERT(sizeReturn==3);
    PEGASUS_TEST_ASSERT(isArrayReturn);

    PEGASUS_TEST_ASSERT(((Boolean*)voidReturn)[0]);
    PEGASUS_TEST_ASSERT(!((Boolean*)voidReturn)[1]);
    PEGASUS_TEST_ASSERT(((Boolean*)voidReturn)[2]);

    /**
     * Test DateTime
     */

    VCOUT << "Test DateTime" << endl;

    CIMDateTimeRep dateTimeValue = {PEGASUS_UINT64_LITERAL(17236362),0,':',0};

    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "DateTimeProperty",
        CIMTYPE_DATETIME,
        &dateTimeValue);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    rc = SCMO_TESTClass2_Inst.getProperty(
        "DateTimeProperty",
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);
    PEGASUS_TEST_ASSERT(sizeReturn==0);
    PEGASUS_TEST_ASSERT(!isArrayReturn);

    PEGASUS_TEST_ASSERT(
        memcmp(&dateTimeValue,voidReturn,sizeof(CIMDateTimeRep))== 0);

    CIMDateTimeRep dateTimeArrayValue[]=
        {{Uint64(988243387),0,':',0},
         {Uint64(827383727),0,':',0},
         {Uint64(932933892),0,':',0}};

    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "DateTimePropertyArray",
        CIMTYPE_DATETIME,
        &dateTimeArrayValue,
        true,3);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    rc = SCMO_TESTClass2_Inst.getProperty(
        "DateTimePropertyArray",
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);
    PEGASUS_TEST_ASSERT(sizeReturn==3);
    PEGASUS_TEST_ASSERT(isArrayReturn);


    PEGASUS_TEST_ASSERT(
        memcmp(
            &(dateTimeArrayValue[0]),
            &(((CIMDateTimeRep*)voidReturn)[0]),
            sizeof(CIMDateTimeRep))== 0);
    PEGASUS_TEST_ASSERT(
        memcmp(
            &(dateTimeArrayValue[1]),
            &(((CIMDateTimeRep*)voidReturn)[1]),
            sizeof(CIMDateTimeRep))== 0);
    PEGASUS_TEST_ASSERT(
        memcmp(
            &(dateTimeArrayValue[2]),
            &(((CIMDateTimeRep*)voidReturn)[2]),
            sizeof(CIMDateTimeRep))== 0);

    /**
     * Test string
     */

    VCOUT << "Test String" << endl;

    char* stringValue = "This is a single String!";

    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "StringProperty",
        CIMTYPE_STRING,
        stringValue);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    rc = SCMO_TESTClass2_Inst.getProperty(
        "StringProperty",
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);
    PEGASUS_TEST_ASSERT(sizeReturn==0);
    PEGASUS_TEST_ASSERT(!isArrayReturn);

    PEGASUS_TEST_ASSERT(strcmp(stringValue,(const char*)voidReturn) == 0);


    char* stringArrayValue[]=
        {"The Array String Number one.",
         "The Array String Number two.",
         "The Array String Number three."};

    rc = SCMO_TESTClass2_Inst.setPropertyWithOrigin(
        "StringPropertyArray",
        CIMTYPE_STRING,
        stringArrayValue,
        true,3);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    rc = SCMO_TESTClass2_Inst.getProperty(
        "StringPropertyArray",
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);
    PEGASUS_TEST_ASSERT(sizeReturn==3);
    PEGASUS_TEST_ASSERT(isArrayReturn);

    PEGASUS_TEST_ASSERT(
        strcmp(stringArrayValue[0],((const char**)voidReturn)[0]) == 0);
    PEGASUS_TEST_ASSERT(
        strcmp(stringArrayValue[1],((const char**)voidReturn)[1]) == 0);
    PEGASUS_TEST_ASSERT(
        strcmp(stringArrayValue[2],((const char**)voidReturn)[2]) == 0);
    // do not forget !!!
    free((void*)voidReturn);

    VCOUT << endl << "Done." << endl << endl;

}

void SCMOInstanceKeyBindingsTest(SCMOInstance& SCMO_TESTClass2_Inst)
{
    SCMO_RC rc;

    CIMKeyBinding::Type returnKeyBindType;
    const char * returnKeyBindValue;
    Uint32 noKeyBind;
    const char * returnName;


    /** 
     * Test Key bindings
     */

    VCOUT << "Key Bindings Tests." << endl << endl;

    VCOUT << "Wrong key binding name." << endl;
    // BooleanProperty is not a key !
    rc = SCMO_TESTClass2_Inst.setKeyBinding(
        "BooleanProperty",
        CIMKeyBinding::BOOLEAN,
        "TRUE");

    PEGASUS_TEST_ASSERT(rc==SCMO_NOT_FOUND);

    VCOUT << "Wrong key binding type." << endl;
    // Real32Property is a key property 
    rc = SCMO_TESTClass2_Inst.setKeyBinding(
        "Real32Property",
        CIMKeyBinding::BOOLEAN,
        "3.9399998628365712e+30");

    PEGASUS_TEST_ASSERT(rc==SCMO_TYPE_MISSMATCH);

    VCOUT << "Key binding not set." << endl;

    rc = SCMO_TESTClass2_Inst.getKeyBinding(
        "Real32Property",
        returnKeyBindType,
        &returnKeyBindValue);

    PEGASUS_TEST_ASSERT(rc==SCMO_NULL_VALUE);
    PEGASUS_TEST_ASSERT(returnKeyBindType==CIMKeyBinding::NUMERIC);
    PEGASUS_TEST_ASSERT(returnKeyBindValue==NULL);

    // set key bindings

    rc = SCMO_TESTClass2_Inst.setKeyBinding(
        "StringProperty",
        CIMKeyBinding::STRING,
        "This is the String key binding.");

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);


    rc = SCMO_TESTClass2_Inst.setKeyBinding(
        "Real32Property",
        CIMKeyBinding::NUMERIC,
        "3.9399998628365712e+30");

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    rc = SCMO_TESTClass2_Inst.setKeyBinding(
        "Uint64Property",
        CIMKeyBinding::NUMERIC,
        "48349872897287");

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    VCOUT << "Get Key binding by index." << endl;

    noKeyBind = SCMO_TESTClass2_Inst.getKeyBindingCount();

    PEGASUS_TEST_ASSERT(noKeyBind==3);

    VCOUT << "Test index boundaries." << endl;

    rc = SCMO_TESTClass2_Inst.getKeyBindingAt(
        noKeyBind+1,
        &returnName,
        returnKeyBindType,
        &returnKeyBindValue);

    PEGASUS_TEST_ASSERT(rc==SCMO_INDEX_OUT_OF_BOUND);
    PEGASUS_TEST_ASSERT(returnKeyBindValue==NULL);

    VCOUT << "Iterate for index 0 to " << noKeyBind-1 << "." << endl;
    for (Uint32 i = 0; i < noKeyBind; i++)              
    {
        rc = SCMO_TESTClass2_Inst.getKeyBindingAt(
            i,
            &returnName,
            returnKeyBindType,
            &returnKeyBindValue);

        PEGASUS_TEST_ASSERT(rc==SCMO_OK);
    }

    VCOUT << endl << "Done." << endl;
}

void SCMOInstancePropertyFilterTest(SCMOInstance& SCMO_TESTClass2_Inst)
{
    SCMO_RC rc;

    // definition of return values.
    const void* voidReturn;
    CIMType typeReturn;
    Boolean isArrayReturn;
    Uint32 sizeReturn;
    const char * nameReturn;

    /**
     * Test property filter
     */

    VCOUT << endl << "SCMOInstance property filter tests..." << endl;
    VCOUT << endl << "Set regular property filter." << endl;
    const char* propertyFilter[] =
    {
        "Uint16Property",
        "StringPropertyArray",
        "Sint16PropertyArray",
        0
    };

    SCMO_TESTClass2_Inst.setPropertyFilter(propertyFilter);

    // 6 properties = 3 of filter + 3 key properies

    PEGASUS_TEST_ASSERT(SCMO_TESTClass2_Inst.getPropertyCount()==6);    

    for (Uint32 i = 0; i < SCMO_TESTClass2_Inst.getPropertyCount();i++)
    {
        rc = SCMO_TESTClass2_Inst.getPropertyAt(
            i,
            &nameReturn,
            typeReturn,
            &voidReturn,
            isArrayReturn,
            sizeReturn);

        if (isArrayReturn && 
            typeReturn == CIMTYPE_STRING 
            && sizeReturn > 0)
        {
                // do not forget !!!
                free((void*)voidReturn);
        }

        PEGASUS_TEST_ASSERT(rc==SCMO_OK);
    }

    VCOUT << "Check indexing." << endl;
    // the indey is just from 0 to 5. 6 is invalid !
    rc = SCMO_TESTClass2_Inst.getPropertyAt(
        SCMO_TESTClass2_Inst.getPropertyCount(),
        &nameReturn,
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);


    PEGASUS_TEST_ASSERT(rc==SCMO_INDEX_OUT_OF_BOUND);

    Uint32 nodeIndex;
    // The property Uint32Property is not part of the filter 
    // and not a key property!
    rc = SCMO_TESTClass2_Inst.getPropertyNodeIndex("Uint32Property",nodeIndex);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    Uint32 uint32value = 0x123456;

    rc = SCMO_TESTClass2_Inst.setPropertyWithNodeIndex(
        nodeIndex,
        CIMTYPE_UINT32,
        &uint32value,
        false,0);

    PEGASUS_TEST_ASSERT(rc==SCMO_OK);

    rc = SCMO_TESTClass2_Inst.getPropertyNodeIndex(NULL,nodeIndex);

    PEGASUS_TEST_ASSERT(rc==SCMO_INVALID_PARAMETER);

    VCOUT << "Reset filter." << endl;
    // reset filter
    SCMO_TESTClass2_Inst.setPropertyFilter(NULL);

    PEGASUS_TEST_ASSERT(SCMO_TESTClass2_Inst.getPropertyCount()==28);

    rc = SCMO_TESTClass2_Inst.getProperty(
        "Uint32Property",
        typeReturn,
        &voidReturn,
        isArrayReturn,
        sizeReturn);

    // The property should not have changed! The setPropertyWithNodeIndex()
    // must not have changed the value.
    PEGASUS_TEST_ASSERT(uint32value != *((Uint32*)voidReturn));

    const char* noPropertyFiler[] = { 0 };

    VCOUT << "Empty filter." << endl;
    // no properties in the filter
    SCMO_TESTClass2_Inst.setPropertyFilter(noPropertyFiler);

    // you can not filter out key properties !
    PEGASUS_TEST_ASSERT(SCMO_TESTClass2_Inst.getPropertyCount()==3);

    VCOUT << endl << "Done." << endl;
    // dump.dumpSCMOInstance(SCMO_TESTClass2_Inst);


}



int main (int argc, char *argv[])
{

    CIMClass CIM_TESTClass2;

    verbose = getenv("PEGASUS_TEST_VERBOSE") ? true : false;

    try
    {

        CIMClassToSCMOClass();
/*
        loadSCMO_TESTClass2(CIM_TESTClass2);

        SCMOClass SCMO_TESTClass2(CIM_TESTClass2);

        SCMOClassQualifierTest( SCMO_TESTClass2 );

        SCMOInstance SCMO_TESTClass2_Inst(SCMO_TESTClass2);    

        SCMOInstancePropertyTest(SCMO_TESTClass2_Inst);

        SCMOInstanceKeyBindingsTest(SCMO_TESTClass2_Inst);

        SCMOInstancePropertyFilterTest(SCMO_TESTClass2_Inst);
        */
    }     
    catch (CIMException& e)
    {
        cout << endl << "CIMException: " ;
        cout << e.getMessage() << endl << endl ;
        exit(-1);
    }

    catch (Exception& e)
    {
        cout << endl << "Exception: " ;
        cout << e.getMessage() << endl << endl ;
        exit(-1);
    }
    catch (...)
    {
        cout << endl << "Unkown excetption!" << endl << endl;
        exit(-1);
    }     

    cout << endl << argv[0] << " +++++ passed all tests" << endl;
    return 0;

}
