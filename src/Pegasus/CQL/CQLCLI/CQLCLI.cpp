//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
//
// Authors: David Rosckes (rosckes@us.ibm.com)
//          Bert Rivero (hurivero@us.ibm.com)
//          Chuck Carmack (carmack@us.ibm.com)
//          Brian Lucier (lucier@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/FileSystem.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/CQL/CQLParser.h>
#include <Pegasus/CQL/CQLParserState.h>
#include <Pegasus/CQL/CQLSelectStatement.h>
#include <Pegasus/CQL/RepositoryQueryContext.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMObjectPath.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;
FILE *CQL_in;
int CQL_parse();

PEGASUS_NAMESPACE_BEGIN
CQLParserState* globalParserState = 0;
PEGASUS_NAMESPACE_END

void buildInstances(Array<CIMInstance>& instances){

   // #1
   const CIMName _cimName(String("CIM_OperatingSystem"));
   CIMInstance _i1(_cimName);
   CIMProperty _p1(CIMName("Description"),CIMValue(String("Bert Rules")));
   CIMProperty _p2(CIMName("EnabledState"),CIMValue(Uint16(2)));
   _i1.addProperty(_p1);
   _i1.addProperty(_p2);
   instances.append(_i1);

   // #2
}

int main(int argc, char ** argv)
{
	// setup Instances
	Array<CIMInstance> _instances;
	buildInstances(_instances);

	Array<CQLSelectStatement> _statements;

	// init parser state
	const char* env = getenv("PEGASUS_HOME");
        String repositoryDir(env);
        repositoryDir.append("/repository");
        CIMNamespaceName _ns("root/cimv2");
        CIMRepository* _rep = new CIMRepository(repositoryDir);
        RepositoryQueryContext* _ctx = new RepositoryQueryContext(_ns, _rep);
	String lang("CIM:CQL");
        String query("");
	CQLSelectStatement _ss(lang,query,_ctx);
	char text[255];
	char* _text;

	// setup input stream
	if(argc > 1){
		ifstream queryInputSource(argv[1]);
		if(!queryInputSource){
			cout << "Cannot open input file.\n" << endl;
			return 1;
		}
		while(!queryInputSource.eof()){
			queryInputSource.getline(text, 255);
			char* _ptr = text;
			_text = strcat(_ptr,"\n");	
			cout << "text = " << _text;
			if(!(strlen(_text) < 2)){
				CQLParser::parse(text,_ss);
				_statements.append(_ss);
			}
			
		}
		queryInputSource.close();
		Boolean result = false;
		for(Uint32 i=0; i < _statements.size(); i++){
			printf("Evaluating query %d...\n",i);
                        result = _statements[i].evaluate(_instances[0]);
			cout << _statements[i].toString() << " = ";
                        if(result) printf("TRUE\n");
                        else printf("FALSE\n");
		}
	}else{
		// manually setup parser state
		String lang("CQL");
	        String query("SELECTSTATEMENT");
        	globalParserState = new CQLParserState;
        	globalParserState->error = false;
        	globalParserState->text = text;
        	globalParserState->textSize = strlen(text) + 1;
        	globalParserState->offset = 0;
        	globalParserState->statement = new CQLSelectStatement(lang,query,_ctx);

		printf("Starting CQL lexer/parser...\n");
		int i = 0;
		while(1){
         	       globalParserState->statement->clear();
                	CQL_parse();
                                                                                                                  
                	String s = globalParserState->statement->toString();
               		printf("%s\n",(const char*)(s.getCString()));
                	//if(argc == 2){
                        	printf("Evaluating...\n");
                        	Boolean result = globalParserState->statement->evaluate(_instances[i]);
                        	if(result) printf("Result = true\n");
				else printf("Result = false\n");
                	//}
                	i++;  // current query expression*/
        	}
	}

    	return 0;                                                                                                              
}

