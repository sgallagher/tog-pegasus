#include <cassert>
#include <Pegasus/Common/String.h>
#include <Pegasus/Compiler/cimmofParser.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

void TestAssociations(CIMRepository& r)
{
    String nameSpace = "root";
    {
	CIMReference instanceName = "X.key=\"John Smith\"";

	Array<CIMReference> names = r.associatorNames(
	    nameSpace,
	    instanceName,
	    "A",
	    "Y",
	    "left",
	    "right");

	assert(names.size() == 1);
	Boolean cond = names[0] == CIMReference("Y.key=\"John Jones\"");
	assert(cond);
    }

    {
	CIMReference instanceName = "X.key=\"John Smith\"";

	Array<CIMInstance> result = r.associators(
	    nameSpace,
	    instanceName,
	    "a",
	    "y",
	    "LEFT",
	    "RIGHT");

	assert(result.size() == 1);

	CIMClass tmpClass = r.getClass(nameSpace, result[0].getClassName());
	CIMReference tmpInstanceName = result[0].getInstanceName(tmpClass);

	Boolean t = tmpInstanceName == CIMReference("Y.key=\"John Jones\"");
	assert(t);
	// result[0].print();
    }

    {
	CIMReference instanceName = "X.key=\"John Smith\"";

	Array<CIMReference> result = r.referenceNames(
	    nameSpace,
	    instanceName,
	    "A",
	    "left");

	assert(result.size() == 1);

	CIMReference tmp = "A."
	    "left=\"x.key=\\\"John Smith\\\"\","
	    "right=\"y.key=\\\"John Jones\\\"\"";
	
	Boolean cond = (result[0] == tmp);
	assert(cond);
    }

    {
	CIMReference instanceName = "X.key=\"John Smith\"";

	Array<CIMInstance> result = r.references(
	    nameSpace,
	    instanceName,
	    "A",
	    "left");

	assert(result.size() == 1);

	CIMClass tmpClass = r.getClass(nameSpace, result[0].getClassName());
	CIMReference tmpInstanceName = result[0].getInstanceName(tmpClass);

	CIMReference tmp = "A."
	    "left=\"x.key=\\\"John Smith\\\"\","
	    "right=\"y.key=\\\"John Jones\\\"\"";
	
	Boolean cond = (tmpInstanceName == tmp);
	assert(cond);
    }
}

int main()
{
    try
    {
	CIMRepository r("./repository");

	TestAssociations(r);
    }
    catch (Exception& e)
    {
	cerr << e.getMessage() << endl;
	exit(1);
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
