#include <cassert>
#include <Pegasus/Common/String.h>
#include <Pegasus/Compiler/cimmofParser.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

void TestAssociators(CIMRepository& r)
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
}

int main()
{
    try
    {
	CIMRepository r("./repository");

	TestAssociators(r);
    }
    catch (Exception& e)
    {
	cerr << e.getMessage() << endl;
	exit(1);
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
