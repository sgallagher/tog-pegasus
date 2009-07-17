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

#include <Pegasus/Common/SCMO.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/FileSystem.h>


PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static Boolean verbose;


void test1()
{
    CIMName theClassName("TheLongLongLongLongLongClassName");
    CIMName theSuperClassName("TheSuperClass");
    CIMNamespaceName theNameSpace("root/cimv2");
    
    CIMObjectPath theObjectPath;
    theObjectPath.setClassName(theClassName);
    theObjectPath.setNameSpace(theNameSpace);

    CIMClass myClass(theClassName,theSuperClassName);
    myClass.setPath(theObjectPath);

    SCMOClass theSCMOClass(myClass);

}

void test2(char* filename)
{
    CIMClass myClass;
    Buffer text;
    FileSystem::loadFileToMemory(text, filename);

    
    XmlParser theParser((char*)text.getData());
    XmlReader::getObject(theParser,myClass);
    
    SCMOClass theSCMOClass(myClass);
    
    SCMOInstance myInstance(theSCMOClass);

    SCMOInstance cloneInstance = myInstance.clone();
    
}

int main (int argc, char *argv[])
{

    verbose = getenv("PEGASUS_TEST_VERBOSE") ? true : false;

    test2("./TestSCMO.xml");

    cout << argv[0] << " +++++ passed all tests" << endl;
    return 0;
}
