#include <Pegasus/Common/String.h>
#include <Pegasus/Compiler/cimmofParser.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

int main()
{
    try
    {
PEGASUS_TRACE;
	cimmofParser* parser = cimmofParser::Instance();
PEGASUS_TRACE;
	parser->setRepository();
PEGASUS_TRACE;
	parser->setDefaultNamespacePath(".");
PEGASUS_TRACE;
	parser->setInputBufferFromName("x.mof");
PEGASUS_TRACE;
	parser->parse();
PEGASUS_TRACE;
    }
    catch (Exception& e)
    {
	cerr << e.getMessage() << endl;
	exit(1);
    }

    cout << "+++++ passed all tests" << endl;
    return 0;
}
