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

Boolean _applyProjection(Array<CQLSelectStatement>& _statements, Array<CIMInstance>& _instances){
	/*
                        CIMInstance projInst = _instances[0].clone();
                        _statements[i].applyProjection(projInst);
                        for (Uint32 n = 0; n < projInst.getPropertyCount(); n++)
                          {
                            CIMProperty prop = projInst.getProperty(n);
                            CIMValue val = prop.getValue();
                            cout << "Prop #" << n << " name = " << prop.getName().getString();
                            cout << " Value = " << val.toString() << endl;
                          }
        */
	return true;
}

Boolean _validateProperties(Array<CQLSelectStatement>& _statements, Array<CIMInstance>& _instances){
        return true;                                                                                        
}

Boolean _getPropertyList(Array<CQLSelectStatement>& _statements, Array<CIMInstance>& _instances){
        /*
                        CIMObjectPath cname(String::EMPTY,
                                            "root/cimv2",
                                            "CIM_Indication");
                        CIMPropertyList list = _statements[i].getPropertyList(cname);
                        Array<CIMName> names = list.getPropertyNameArray();
                        for (Uint32 n = 0; n < names.size(); n++)
                          {
                            cout << "Required: " << names[i].getString() << endl;
                          }
        */
	return true;
}

Boolean _evaluate(Array<CQLSelectStatement>& _statements, Array<CIMInstance>& _instances){
	for(Uint32 i = 0; i < _statements.size(); i++){
        	printf("Evaluating query %d...\n",i);
           	for(Uint32 j = 0; j < _instances.size(); j++){
		  try
		    {
        		Boolean result = _statements[i].evaluate(_instances[j]);
			cout << _statements[i].toString() << " = ";
			if(result) printf("TRUE\n");
                	else printf("FALSE\n");
		    }
		  catch(Exception e){ cout << e.getMessage() << endl;}
		  catch(...){ cout << "Unknown Exception" << endl;}
	   	}
	}                                                                            
	return true;
}

int main(int argc, char ** argv)
{
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

	// setup Test Instances
        const CIMName _testclass(String("CQL_TestPropertyTypes"));
	const CIMName _testclass1(String("CIM_ComputerSystem"));
        Array<CIMInstance> _instances = _rep->enumerateInstances( _ns, _testclass );
	_instances.appendArray(_rep->enumerateInstances( _ns, _testclass1 ));

	// setup input stream
	if(argc == 2){
		ifstream queryInputSource(argv[1]);
		if(!queryInputSource){
			cout << "Cannot open input file.\n" << endl;
			return 1;
		}
		while(!queryInputSource.eof()){
			queryInputSource.getline(text, 255);
			char* _ptr = text;
			_text = strcat(_ptr,"\n");	
			// check for comments and ignore
			// a comment starts with a # as the first non whitespace character on the line
			char _comment = '#';
			int i = 0;
			while(text[i] == ' ' || text[i] == '\t') i++; // ignore whitespace
			if(text[i] != _comment){
				if(!(strlen(_text) < 2)){
					CQLParser::parse(text,_ss);
					_statements.append(_ss);
				}
			}
		}
		queryInputSource.close();
		try{
			_applyProjection(_statements,_instances);
			_validateProperties(_statements,_instances);
			_getPropertyList(_statements,_instances);
			_evaluate(_statements,_instances);
		}
		catch(Exception e){ 
			cout << e.getMessage() << endl; 
		}
		catch(...){
			cout << "CAUGHT ... BADNESS HAPPENED!!!" << endl;
		}
	}else{
		cout << "Invalid number of arguments.\n" << endl;
	}

    	return 0;                                                                                                              
}

