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

Boolean _applyProjection(Array<CQLSelectStatement>& _statements, 
                         Array<CIMInstance>& _instances,
                         String testOption)
{
  if(testOption == String::EMPTY || testOption == "2")
  {
    cout << "========Apply Projection Results========" << endl;

    for(Uint32 i = 0; i < _statements.size(); i++)
    {
      cout << "======================================" << endl;
      cout << _statements[i].toString() << endl;

      for(Uint32 j = 0; j < _instances.size(); j++)
      {
        cout << "Instance of class " << _instances[j].getClassName().getString() << endl;

        try
        {
          CIMInstance projInst = _instances[j].clone();

          // Remove the property "MissingProperty" for the 
          // testcases that depend on the property being missing.
          Uint32 missing = projInst.findProperty("MissingProperty");
          if (missing != PEG_NOT_FOUND)
          {
            projInst.removeProperty(missing);
          }

          _statements[i].applyProjection(projInst);

          Uint32 cnt = projInst.getPropertyCount(); 
          if (cnt == 0)
          {
            cout << "-----No properties left after projection" << endl;
          }

          for (Uint32 n = 0; n < cnt; n++)
          {
            CIMProperty prop = projInst.getProperty(n);
            CIMValue val = prop.getValue();
            cout << "-----Prop #" << n << " Name = " << prop.getName().getString();
            if (val.isNull())
            {
              cout << " Value = NULL" << endl;
            }
            else
            {
              cout << " Value = " << val.toString() << endl;
            }
          }
        }
        catch(Exception& e){ cout << "-----" << e.getMessage() << endl;}
        catch(...){ cout << "Unknown Exception" << endl;}
      }
    }                         
  }

  return true;
}

Boolean _validateProperties(Array<CQLSelectStatement>& _statements, 
                            Array<CIMInstance>& _instances,
                            String testOption)
{
  if(testOption == String::EMPTY || testOption == "4")
  {
    cout << "======Validate Properties Results=======" << endl;

    for(Uint32 i = 0; i < _statements.size(); i++)
    {
      cout << "======================================" << endl;
      cout << _statements[i].toString() << endl;

      try
      {
        _statements[i].validateProperties();
        cout << "----- validate ok" << endl;
      }
      catch(Exception& e){ cout << "-----" << e.getMessage() << endl;}
      catch(...){ cout << "Unknown Exception" << endl;}
    }
  }

  return true;                                                                                        
}

void _printPropertyList(CIMPropertyList& propList)
{
  if (propList.isNull())
  {
    cout << "-----all properties required" << endl;
  }
  else if (propList.size() == 0)
  {
    cout << "-----no properties required" << endl;
  }
  else
  {
    for (Uint32 n = 0; n < propList.size(); n++)
    {
      cout << "-----Required property " << propList[n].getString() << endl;
    }
  }
}

Boolean _getPropertyList(Array<CQLSelectStatement>& _statements, 
                         Array<CIMInstance>& _instances,
                         CIMNamespaceName ns,
                         String testOption)
{
  if(testOption == String::EMPTY || testOption == "3")
  {
    cout << "========Get Property List Results=======" << endl;

    for(Uint32 i = 0; i < _statements.size(); i++)
    {
      cout << "======================================" << endl;
      cout << _statements[i].toString() << endl;

      for(Uint32 j = 0; j < _instances.size(); j++)
      {
        try
        {
          cout << endl << "Get Class Path List" << endl;
          Array<CIMObjectPath> fromPaths = _statements[i].getClassPathList();
          for (Uint32 k = 0; k < fromPaths.size(); k++)
          {
            cout << "-----" << fromPaths[k].toString() << endl;
          }
        }
        catch(Exception& e){ cout << "-----" << e.getMessage() << endl;}
        catch(...){ cout << "Unknown Exception" << endl;}

        CIMName className = _instances[j].getClassName();
        CIMObjectPath classPath (String::EMPTY,
                                 ns,
                                 className);

        try
        {
          cout << "Validate Class for " << className.getString() << endl;
          _statements[i].validateClass(classPath);
          cout << "-----validate class ok" << endl;
        }
        catch(Exception& e){ cout << "-----" << e.getMessage() << endl;}
        catch(...){ cout << "Unknown Exception" << endl;}

        CIMPropertyList propList;

        try
        {
          cout << "Property List for " << className.getString() << endl;
           propList = _statements[i].getPropertyList(classPath);
          _printPropertyList(propList);
        }
        catch(Exception& e){ cout << "-----" << e.getMessage() << endl;}
        catch(...){ cout << "Unknown Exception" << endl;}

        try
        {     
          cout << "SELECT Property List for " << className.getString() << endl;
          propList.clear();
          propList = _statements[i].getSelectPropertyList(classPath);
          _printPropertyList(propList);
        }
        catch(Exception& e){ cout << "-----" << e.getMessage() << endl;}
        catch(...){ cout << "Unknown Exception" << endl;}

        try
        {
          cout << "WHERE Property List for " << className.getString() << endl;
          propList.clear();
          propList = _statements[i].getWherePropertyList(classPath);
          _printPropertyList(propList);
        }
        catch(Exception& e){ cout << "-----" << e.getMessage() << endl;}
        catch(...){ cout << "Unknown Exception" << endl;}

        try
        {
          cout << "Property List for the FROM class " << endl;
          propList.clear();
          propList = _statements[i].getPropertyList();
          _printPropertyList(propList);
        }
        catch(Exception& e){ cout << "-----" << e.getMessage() << endl;}
        catch(...){ cout << "Unknown Exception" << endl;}

        try
        {
          cout << "SELECT Property List for the FROM class " << endl;
          propList.clear();
          propList = _statements[i].getSelectPropertyList();
          _printPropertyList(propList);
        }
        catch(Exception& e){ cout << "-----" << e.getMessage() << endl;}
        catch(...){ cout << "Unknown Exception" << endl;}

        try
        {
          cout << "WHERE Property List for the FROM class " << endl;
          propList.clear();
          propList = _statements[i].getWherePropertyList();
          _printPropertyList(propList);
        }
        catch(Exception& e){ cout << "-----" << e.getMessage() << endl;}
        catch(...){ cout << "Unknown Exception" << endl;}
      }
    }                         
  }
	
  return true;
}

Boolean _evaluate(Array<CQLSelectStatement>& _statements,
                  Array<CIMInstance>& _instances,
                  String testOption)
{
  if(testOption == String::EMPTY || testOption == "1")
  {                                      
    for(Uint32 i = 0; i < _statements.size(); i++)
    {
      printf("\n\nEvaluating query %d :  ",i+1);
      cout << _statements[i].toString() << endl << endl;;

      for(Uint32 j = 0; j < _instances.size(); j++)
      {
        try
        {
          Boolean result = _statements[i].evaluate(_instances[j]);
          cout << _statements[i].toString() << " = ";
          if(result) printf("TRUE\n\n");
          else printf("FALSE\n\n");
        }
        catch(Exception e){ cout << e.getMessage() << endl;}
        catch(...){ cout << "Unknown Exception" << endl;}
      }
    }
  }

  return true;
}

Boolean _normalize(Array<CQLSelectStatement>& _statements, 
                   Array<CIMInstance>& _instances,
                   String testOption)
{
  if(testOption == String::EMPTY || testOption == "5")
  {
    cout << "=========Normalize Results==============" << endl;

    for(Uint32 i = 0; i < _statements.size(); i++)
    {
      cout << "======================================" << endl;

      try
      {
        cout << "-----Statement before normalize" << endl;
        _statements[i].applyContext();
        cout << _statements[i].toString() << endl;

        _statements[i].normalizeToDOC();
        cout << "-----Statement after normalize" << endl;
        cout << _statements[i].toString() << endl;

        cout << "-----Traversing the predicates" << endl;
        CQLPredicate topPred = _statements[i].getPredicate();
        if (topPred.isSimple())
        {
          cout << "-----Top predicate is simple: " << topPred.toString() << endl;
        }
        else
        {
          cout << "-----Top predicate is not simple: " << topPred.toString() << endl;

          Array<CQLPredicate> secondLevelPreds = topPred.getPredicates();
          for (Uint32 n = 0; n < secondLevelPreds.size(); n++)
          {
            if (secondLevelPreds[n].isSimple())
            {
              cout << "-----2nd level predicate is simple: "
                   << secondLevelPreds[n].toString() << endl;
            }
            else
            {
              cout << "-----ERROR - 2nd level predicate is NOT simple!: " 
                   << secondLevelPreds[n].toString() << endl;
            }
          }
        }
      }
      catch(Exception& e){ cout << "-----" << e.getMessage() << endl;}
      catch(...){ cout << "Unknown Exception" << endl;}
    }
  }

  return true;                                                                                        
}

void buildEmbeddedObjects(CIMNamespaceName& ns,
                          Array<CIMInstance>& instances,
                          CIMRepository* rep)
{

  //
  // Sort out the instances of CQL_TestElement, CQL_TestPropertyTypes,
  // CIM_ComputerSystem that will be added as embedded objects to
  // the embedded object test class
  //

  CIMName nameTE("CQL_TestElement");
  CIMName instIdName("InstanceId");
  CIMName nameTPT("CQL_TestPropertyTypes");
  CIMName nameCS("CIM_ComputerSystem");

  CIMInstance testElem;
  Array<CIMInstance> testElemArray;
  Boolean foundTestElem = false;

  CIMInstance testCS;
  Boolean foundCS = false;

  Array<CIMInstance> testPropTypesArray;

  for (Uint32 i = 0; i < instances.size(); i++)
  {
    // Save the CQL_TestElement with InstanceId = 0
    if (instances[i].getClassName() == nameTE)
    {
      Uint32 index = instances[i].findProperty(instIdName);
      if (index == PEG_NOT_FOUND)
      {
        throw Exception("Error building embedded objects.  CQL_TestElement with InstanceId prop not found");
      }
      
      Uint64 instId;
      instances[i].getProperty(index).getValue().get(instId);
      if (instId == 0)
      {
        foundTestElem = true;
        testElem = instances[i].clone();
      }
    }

    // Save the CQL_TestPropertyType instances as we find them
    if (instances[i].getClassName() == nameTPT)
    {
      testPropTypesArray.append(instances[i].clone());
    }

    // Save the CIM_ComputerSystem instance
    if (instances[i].getClassName() == nameCS)
    {
      foundCS = true;
      testCS = instances[i].clone();
    }
  }

  if (!foundTestElem)
  {
    throw Exception("Error building embedded objects.  CQL_TestElement with InstanceId = 0 not found");
  }

  if (!foundCS)
  {
    throw Exception("Error building embedded objects.  CIM_ComputerSystem not found");
  }

  if (testPropTypesArray.size() <= 1)
  {
    throw Exception("Error building embedded objects. Not enough CQL_TestPropertyTypes found");
  }

  //
  // Now build the array of CQL_TestElement (see below for why we are doing this)
  // 

  testElemArray.append(testElem.clone());

  Uint32 index = testElem.findProperty(instIdName);
  testElem.removeProperty(index);
  testElem.addProperty(CIMProperty(instIdName, CIMValue((Uint64)1)));
  testElemArray.append(testElem.clone());

  index = testElem.findProperty(instIdName);
  testElem.removeProperty(index);
  testElem.addProperty(CIMProperty(instIdName, CIMValue((Uint64)2)));
  testElemArray.append(testElem.clone());

  //
  // Get the class that will be added as an embedded object
  //

  CIMClass someClass = rep->getClass(ns,
                                     "CIM_Process",
                                     false,  // local only
                                     true,   // include quals
                                     true);   // include class origin

  // Build the embedded object structure.
  //
  // The structure looks like this:
  //
  // class CQL_EmbeddedSubClass is subclass of CQL_EmbeddedBase
  // class CQL_EmbeddedTestPropertyTypes is subclass of CQL_EmbeddedTestElement
  //
  // instance of CQL_EmbeddedSubClass 
  //
  //   property InstanceID has value 100
  //
  //   property EmbObjBase has
  //      instance of CQL_EmbeddedTestElement
  //         property InstanceID has value 1000
  //         property TEArray has array of instance of CQL_TestElement
  //             each array element has an InstanceID property
  //             that matches the index of the element
  //         property TE has instance of CQL_TestElement
  //             property InstanceID has value 0
  //         property CS has instance of CIM_ComputerSystem
  //         property SomeClass has class of CIM_Process
  //         property SomeString has a string
  //   
  //   property EmbObjSub has
  //      instance of CQL_EmbeddedTypePropertyTypes
  //         property InstanceID has value 1001
  //         property TEArray has array of instance of CQL_TestElement
  //             each array element has an InstanceID property
  //             that matches the index of the element
  //         property TE has instance of CQL_TestElement
  //             property InstanceID has value 0
  //         property CS has instance of CIM_ComputerSystem
  //         property TPTArray has array of instance of CQL_TestPropertyTypes
  //             the array is built from the instances compiled in the repository
  //         property TPT has instance of CQL_TestPropertyTypes
  //             this instance is the first instance found
  //         property SomeClass has class of CIM_Process
  //         property SomeUint8 has a uint8
  //

  // ATTN - uncomment when emb obj are supported

  CIMInstance embTE("CQL_EmbeddedTestElement");
  embTE.addProperty(CIMProperty("InstanceID", CIMValue((Uint64)1000)));
  //  embTE.addProperty(CIMProperty("TEArray", CIMValue(testElemArray)));
  //  embTE.addProperty(CIMProperty("TE", CIMValue(testElemArray[0])));
  //  embTE.addProperty(CIMProperty("CS", CIMValue(testCS)));
  //  embTE.addProperty(CIMProperty("SomeClass", CIMValue(someClass)));
  embTE.addProperty(CIMProperty("SomeString", CIMValue("Huh?")));
  

  CIMInstance embTPT("CQL_EmbeddedTestPropertyTypes");
  embTPT.addProperty(CIMProperty("InstanceID", CIMValue((Uint64)1001)));
  //  embTE.addProperty(CIMProperty("TEArray", CIMValue(testElemArray)));
  //  embTE.addProperty(CIMProperty("TE", CIMValue(testElemArray[0])));
  //  embTE.addProperty(CIMProperty("CS", CIMValue(testCS)));
  //  embTPT.addProperty(CIMProperty("TPTArray", CIMValue(testPropTypesArray)));
  //  embTPT.addProperty(CIMProperty("TPT", CIMValue(testPropTypesArray[0])));
  //  embTPT.addProperty(CIMProperty("SomeClass", CIMValue(someClass)));
  embTPT.addProperty(CIMProperty("SomeUint8", CIMValue((Uint8)3)));

  CIMInstance embSub("CQL_EmbeddedSubClass");
  embSub.addProperty(CIMProperty("InstanceID", CIMValue((Uint64)100)));
  //  embSub.addProperty(CIMProperty("EmbObjBase", CIMValue(embTE)));  
  //  embSub.addProperty(CIMProperty("EmbObjSub", CIMValue(embTPT)));  

  instances.append(embSub);
}

void help(const char* command){
	cout << command << " queryFile [option]" << endl;
	cout << " options:" << endl;
	cout << " -test: ";
	cout << "1 = evaluate" << endl 
             << "        2 = apply projection" << endl 
             << "        3 = get property list" << endl;
	cout << "        4 = validate properties" << endl;
	cout << "        5 = normalize to DOC" << endl;
	cout << " -className class" << endl;
	cout << " -nameSpace namespace (Example: root/SampleProvider)" << endl << endl;
}

int main(int argc, char ** argv)
{
	String testOption = String::EMPTY;
        String className = String::EMPTY;
	String nameSpace = String::EMPTY;

	// process options
	if(argc == 1 || (argc > 1 && strcmp(argv[1],"-h") == 0) ){
               	help(argv[0]);
               	exit(0);
	}

	for(int i = 0; i < argc; i++){
		if((strcmp(argv[i],"-test") == 0) && (i+1 < argc))
                        testOption = argv[i+1];
		if((strcmp(argv[i],"-className") == 0) && (i+1 < argc))
                	className = argv[i+1];
		if((strcmp(argv[i],"-nameSpace") == 0) && (i+1 < argc))
                        nameSpace = argv[i+1];
	}

	Array<CQLSelectStatement> _statements;

	// setup test environment
	const char* env = getenv("PEGASUS_HOME");
        String repositoryDir(env);
        repositoryDir.append("/repository");

	CIMNamespaceName _ns;
	if(nameSpace != String::EMPTY){
		_ns = nameSpace;
	}else{
		cout << "Using root/SampleProvider as default namespace." << endl;
      _ns = String("root/SampleProvider");
	}

   CIMRepository* _rep = new CIMRepository(repositoryDir);
   RepositoryQueryContext _ctx(_ns, _rep);
	String lang("CIM:CQL");
   String query("");
	CQLSelectStatement _ss(lang,query,_ctx);
	char text[255];
	char* _text;

	// setup Test Instances
	Array<CIMInstance> _instances;
	if(className != String::EMPTY){
		try{
			const CIMName _testclass(className);
			_instances = _rep->enumerateInstances( _ns, _testclass, true );  // deep inh true
		}catch(Exception& e){
			cout << endl << endl << "Exception: Invalid namespace/class: " << e.getMessage() << endl << endl;
		}
	}else{ // load default class names
		cout << endl << "Using default class names to test queries. " << endl << endl;
        	const CIMName _testclass(String("CQL_TestPropertyTypes"));
		const CIMName _testclass1(String("CIM_ComputerSystem"));
		try{
        _instances = _rep->enumerateInstances( _ns, _testclass, true ); // deep inh true
        _instances.appendArray(_rep->enumerateInstances( _ns, _testclass1, false )); // deep inh false
		}catch(Exception& e){
			cout << endl << endl << "Exception: Invalid namespace/class: " << e.getMessage() << endl << endl;
      }
	}

   // Add the embedded object instances to the array
   buildEmbeddedObjects(_ns,
                        _instances,
                        _rep);
   
	// demo setup
	if(argc == 3 && strcmp(argv[2],"Demo") == 0){
		cout << "Running Demo..." << endl;
		_instances.clear();
		const CIMName _testclassDEMO(String("CIM_Process"));
		_instances.appendArray(_rep->enumerateInstances( _ns, _testclassDEMO ));
		_instances.remove(6,6);
	}
	
	// setup input stream
	if(argc >= 2){
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
					try{
						CQLParser::parse(text,_ss);
						_statements.append(_ss);
					}catch(Exception& e){
						cout << endl << endl << "Caught Exception: " << e.getMessage() << endl << endl;
                                                cout << "Statement with error = " << text << endl;
						_ss.clear();
					}
				}
			}
		}
		queryInputSource.close();
		try{
			_applyProjection(_statements,_instances, testOption);
			_validateProperties(_statements,_instances, testOption);
			_getPropertyList(_statements,_instances, _ns, testOption);
			_evaluate(_statements,_instances, testOption);
			_normalize(_statements,_instances, testOption);
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

