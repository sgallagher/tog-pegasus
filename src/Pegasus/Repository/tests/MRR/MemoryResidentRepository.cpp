#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Repository/MRR.h>
#include "root_cimv2_namespace.h"

PEGASUS_USING_PEGASUS;

int main()
{
    MRRAddNameSpace(&root_cimv2_namespace);

    CIMRepository cr("/", CIMRepository::MODE_DEFAULT);

    Array<CIMName> names;
    names.append("CreationClassName");
    CIMPropertyList pl(names);

    for (Uint32 i = 0; i < 1000; i++)
    {
        try
        {
            CIMClass cc = cr.getClass(
                "root/cimv2", 
                "CIM_ComputerSystem",
                false,
                true,
                true,
                pl);
        }
        catch (...)
        {
            fprintf(stderr, "ERROR\n");
            exit(1);
        }
    }

    printf("+++++ passed all tests\n");

    return 0;
}
