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
#include <unistd.h>
#include <sys/types.h>
#include <cassert>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/MofWriter.h>
#include <Pegasus/Common/OptionManager.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const CIMNamespaceName NAMESPACE = CIMNamespaceName ("root/SampleProvider");
const CIMNamespaceName NAMESPACE1 = CIMNamespaceName ("root/PG_InterOp");
const String CLASSONE = "TimeOne";
const String CLASSTWO = "TimeTwo";

static void EnumerateClassesTiming(CIMClient client, String BaseClassName)
{
    try
    {
        Boolean deepInheritance = true;
        CIMName className;
        Array<CIMName> classNames = client.enumerateClassNames(
                     NAMESPACE, CIMName(BaseClassName), false);
        cout << classNames.size() << " ClassNames" << endl;
        for (Uint32 i = 0, n = classNames.size(); i < n; i++)
            cout << classNames[i] << endl;
    }
    catch(Exception& e)
    {
        cout << "Error Classes Enumeration:" << endl;
		cout << e.getMessage() << endl;
    }
}

static void EnumerateInstancesTiming(CIMClient client, String ClassName)
{
    Boolean localOnly = false;
    Boolean deepInheritance = false;
    Boolean includeQualifiers = false;
    Boolean includeClassOrigin = false;

    CIMClass c1=client.getClass(NAMESPACE,CIMName(ClassName), false);
    cout << "Class = " << c1.getClassName() << endl;
    
    cout << "Fetching Instances for" << c1.getClassName() << endl;

    Array<CIMInstance> instanceNames;
    instanceNames = client.enumerateInstances
                        (NAMESPACE,
                         ClassName,
                         deepInheritance,
                         localOnly,
                         includeQualifiers,
                         includeClassOrigin);

    cout << "instanceNames.size() = " << instanceNames.size() << endl;
    for (Uint32 i = 0; i < instanceNames.size(); i++)
    {
        CIMInstance inst1 = instanceNames[i];
        XmlWriter::printInstanceElement(inst1, cout);
    }
}

int main(int argc, char** argv)
{
    pid_t pid;    
    try
    {
        CIMClient client;

        client.connect("localhost", 5988, "", "");

        if ((pid = fork()) < 0)
        {
                cout << "Fork Error\n" << endl;
                exit(0);
        }
        else if (pid == 0) 
        {   // child
            sleep(10);
            EnumerateInstancesTiming(client, CLASSONE);
            _exit(0);
        }
		// parent
        sleep(10);
        EnumerateInstancesTiming(client, CLASSTWO);
       	sleep(5); 
        EnumerateClassesTiming(client, "TimingSampleClass");
    }
    catch(Exception& e)
    {
        PEGASUS_STD(cerr) << "Error: " << e.getMessage() << 
            PEGASUS_STD(endl);
        exit(1);
    }
    return 0;
}
