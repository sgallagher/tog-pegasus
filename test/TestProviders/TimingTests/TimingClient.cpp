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
