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
/*
   const CIMName _cimName(String("CIM_OperatingSystem"));
   CIMInstance _i1(_cimName);
   CIMProperty _p1(CIMName("Description"),CIMValue(String("Bert Rules")));
   CIMProperty _p2(CIMName("EnabledState"),CIMValue(Uint16(2)));
   _i1.addProperty(_p1);
   _i1.addProperty(_p2);
   instances.append(_i1);
*/
   // #2

   const CIMName _cimName(String("CQL_TestPropertyTypes"));
   CIMInstance _i1(_cimName);

   CIMProperty _p1(CIMName("PropertyString"),CIMValue(String("Bert Rules")));
   CIMProperty _p2(CIMName("PropertyUint8"),CIMValue(Uint8(8)));
   CIMProperty _p3(CIMName("PropertyUint16"),CIMValue(Uint16(8)));
   CIMProperty _p4(CIMName("PropertyUint32"),CIMValue(Uint32(32)));
   CIMProperty _p5(CIMName("PropertyUint64"),CIMValue(Uint64(18446744073709551615))); // = (2^64)-1
   CIMProperty _p6(CIMName("PropertySint8"),CIMValue(Sint8(16)));
   CIMProperty _p7(CIMName("PropertySint16"),CIMValue(Sint16(16)));
   CIMProperty _p8(CIMName("PropertySint32"),CIMValue(Sint32(32)));
   CIMProperty _p9(CIMName("PropertySint64"),CIMValue(Sint64(-9223372036854775807))); // = (2^63)-1
   CIMProperty _p10(CIMName("PropertyBoolean"),CIMValue(Boolean(true)));
   CIMProperty _p11(CIMName("PropertyReal32"),CIMValue(Real32(32.323232)));
   CIMProperty _p12(CIMName("PropertyReal64"),CIMValue(Real64(64.6464)));

   String _date("20040811105625.000000-360");
   CIMDateTime date(_date);
   CIMProperty _p13(CIMName("PropertyDatetime"),CIMValue(CIMDateTime(_date)));

   CIMProperty _p14(CIMName("PropertyChar16"),CIMValue(Char16('A')));

   _i1.addProperty(_p1);
   _i1.addProperty(_p2);
   _i1.addProperty(_p3);
   _i1.addProperty(_p4);
   _i1.addProperty(_p5);
   _i1.addProperty(_p6);
   _i1.addProperty(_p7);
   _i1.addProperty(_p8);
   _i1.addProperty(_p9);
   _i1.addProperty(_p10);
   _i1.addProperty(_p11);
   _i1.addProperty(_p12);
   _i1.addProperty(_p13);
   _i1.addProperty(_p14);
   instances.append(_i1);

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
        CIMNamespaceName _ns("root/SampleProvider");
        CIMRepository* _rep = new CIMRepository(repositoryDir);
        RepositoryQueryContext _ctx(_ns, _rep);
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
		   try{
                        result = _statements[i].evaluate(_instances[0]);
			cout << _statements[i].toString() << " = ";
                        if(result) printf("TRUE\n");
                        else printf("FALSE\n");
		   }catch(Exception e){ cout << e.getMessage() << endl; }
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

