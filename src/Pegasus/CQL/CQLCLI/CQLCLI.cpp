
#include <Pegasus/Common/OptionManager.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/FileSystem.h>
#include <stdio.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/CQL/CQLParserState.h>
#include <Pegasus/CQL/CQLSelectStatement.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;
FILE *CQL_in;
int CQL_parse();

PEGASUS_NAMESPACE_BEGIN
CQLParserState* globalParserState = 0;
PEGASUS_NAMESPACE_END

int main(int argc, char ** argv)
{
	// init parser state
	const char* text = "blah";
	globalParserState = new CQLParserState;
        globalParserState->error = false;
        globalParserState->text = text;
        globalParserState->textSize = strlen(text) + 1;
        globalParserState->offset = 0;
        globalParserState->statement = new CQLSelectStatement();

	Boolean readFile = false;
	if(argc > 1){
		FILE *file;
                file = fopen(argv[1],"r");
                if(!file){
                        printf("could not open %s\n",argv[1]);
                        exit(1);
                }
                CQL_in = file;
		printf("Opened %s for reading...\n",argv[1]);
		readFile = true;
	}
        printf("Starting parser...\n");
	while(1){
		CQL_parse();
	}
        	
    	return 0;                                                                                                              
}

