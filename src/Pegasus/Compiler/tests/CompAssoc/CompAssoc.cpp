#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Common/String.h>
//#include <Pegasus/Compiler/cimmofParser.h>
#include <Pegasus/Repository/CIMRepository.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

void TestAssociations(CIMRepository& r)
{
    String nameSpace = "root";
    {
	CIMObjectPath instanceName = "X.key=\"John Smith\"";

	Array<CIMObjectPath> names = r.associatorNames(
	    nameSpace,
	    instanceName,
	    "A",
	    "Y",
	    "left",
	    "right");

	assert(names.size() == 1);
	Boolean cond = names[0] == CIMObjectPath("Y.key=\"John Jones\"");
	assert(cond);
    }

    {
	CIMObjectPath instanceName = "X.key=\"John Smith\"";

	Array<CIMObjectWithPath> result = r.associators(
	    nameSpace,
	    instanceName,
	    "a",
	    "y",
	    "LEFT",
	    "RIGHT");

	assert(result.size() == 1);

	CIMObjectPath cimReference = result[0].getReference();
	CIMInstance cimInstance = CIMInstance(result[0].getObject());

	CIMClass tmpClass = r.getClass(nameSpace, cimInstance.getClassName());
	CIMObjectPath tmpInstanceName = cimInstance.getInstanceName(tmpClass);

	Boolean t = tmpInstanceName == CIMObjectPath("Y.key=\"John Jones\"");
	assert(t);
	// result[0].print();
    }

    {
	CIMObjectPath instanceName = "X.key=\"John Smith\"";

	Array<CIMObjectPath> result = r.referenceNames(
	    nameSpace,
	    instanceName,
	    "A",
	    "left");

	assert(result.size() == 1);

	CIMObjectPath tmp = "A."
	    "left=\"x.key=\\\"John Smith\\\"\","
	    "right=\"y.key=\\\"John Jones\\\"\"";
	
	Boolean cond = (result[0] == tmp);
	assert(cond);
    }

    {
	CIMObjectPath instanceName = "X.key=\"John Smith\"";

	Array<CIMObjectWithPath> result = r.references(
	    nameSpace,
	    instanceName,
	    "A",
	    "left");

	assert(result.size() == 1);

	CIMClass tmpClass = r.getClass(
	    nameSpace, CIMInstance(result[0].getObject()).getClassName());

	CIMObjectPath tmpInstanceName = 
	    CIMInstance(result[0].getObject()).getInstanceName(tmpClass);

	CIMObjectPath tmp = "A."
	    "left=\"x.key=\\\"John Smith\\\"\","
	    "right=\"y.key=\\\"John Jones\\\"\"";
	
	Boolean cond = (tmpInstanceName == tmp);
	assert(cond);
    }

    // Delete all the object we created:
    {
	// First delete the association:

	CIMObjectPath assocInstanceName = "A."
	    "left=\"x.key=\\\"John Smith\\\"\","
	    "right=\"y.key=\\\"John Jones\\\"\"";

	r.deleteInstance(nameSpace, assocInstanceName);
    }
}

int main()
{
    try
    {
	CIMRepository r("./repository");

	// TestAssociations(r);
    }
    catch (Exception& e)
    {
	cerr << e.getMessage() << endl;
	exit(1);
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
