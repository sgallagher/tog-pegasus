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
#include <Pegasus/Repository/RepositoryQueryContext.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMObjectPath.h>

#define PEGASUS_SINT64_MIN (PEGASUS_SINT64_LITERAL(0x8000000000000000))
#define PEGASUS_UINT64_MAX PEGASUS_UINT64_LITERAL(0xFFFFFFFFFFFFFFFF)

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;
FILE *CQL_in;
int CQL_parse();

PEGASUS_NAMESPACE_BEGIN
CQLParserState* globalParserState = 0;
PEGASUS_NAMESPACE_END


Boolean cqlcli_verbose = false;

void hackInstances(Array<CIMInstance>& instances)
{
  for (Uint32 i=0; i < instances.size(); i++)
  {
    CIMInstance inst = instances[i];
    // Only hack it if it is an instance of CQL_TestPropertyTypes
    if (inst.getClassName() ==  "CQL_TestPropertyTypes")
    {
      // The properties which the mof compiler messes up will be removed and added manually.
      // Start with Instance #1
      Uint64 instID;
      inst.getProperty(inst.findProperty("InstanceID")).getValue().get(instID);
      if (instID == 1)
      {
        // PropertyReal32 = -32.0
        inst.removeProperty(inst.findProperty("PropertyReal32"));
        Real32 real32Val = -32.0;        
        inst.addProperty(CIMProperty("PropertyReal32", CIMValue(real32Val)));
      }
      // Then do Instance #2
      else if (instID == 2)
      {
       // PropertySint64 = -9223372036854775808
        inst.removeProperty(inst.findProperty("PropertySint64"));
      //  Sint64 sint64Val = -9223372036854775808;
        Sint64 sint64Val = PEGASUS_SINT64_MIN;      
        inst.addProperty(CIMProperty("PropertySint64", CIMValue(sint64Val)));
      }
    }
  }
}


void printProperty(CIMProperty& prop, Uint32 propNum, String& prefix)
{
  // Recursive function to handle embedded object trees

  cout << prefix << "Prop #" << propNum << " Name = " << prop.getName().getString();

  CIMValue val = prop.getValue();

  if (val.getType() != CIMTYPE_OBJECT)
  {
    // Not embedded object
    if (val.isNull())
    {
      cout << ", Value = NULL" << endl;
    }
    else
    {
      cout << ", Value = " << val.toString() << endl;
    }
  }
  else
  {
    // Embedded object, or array of objects
    Array<CIMObject> embObjs;
    if (val.isArray())
    {
      val.get(embObjs);
    }
    else
    {
      CIMObject tmpObj;
      val.get(tmpObj);
      embObjs.append(tmpObj);
    }

    for (Uint32 j = 0; j < embObjs.size(); j++)
    {
      CIMObject embObj = embObjs[j];
      if (embObj.isClass())
      {
        // Embedded class
        CIMClass embCls(embObj);
        cout << ", Value = class of " << embCls.getClassName().getString() << endl;
      }
      else
      {
        // Embedded instance, need to recurse on each property
        CIMInstance embInst(embObj);

        String newPrefix = prefix;
        newPrefix.append(prefix);

        cout << endl << newPrefix << "Instance of class " << embInst.getClassName().getString() << endl;

        Uint32 cnt = embInst.getPropertyCount(); 
        if (cnt == 0)
        {
          cout << newPrefix << "No properties left after projection" << endl;
        }

        if (cnt > 10 && !cqlcli_verbose)
        {
          cout << newPrefix << "Instance has " << cnt << " properties" << endl;
        }
        else
        {
          for (Uint32 n = 0; n < cnt; n++)
          {
            CIMProperty prop = embInst.getProperty(n);
            printProperty(prop, n, newPrefix);
          }
        }
      }
    }
  }
}

Boolean _applyProjection(Array<CQLSelectStatement>& _statements, 
                         Array<CIMInstance>& _instances,
                         String testOption)
{
  if(testOption == String::EMPTY || testOption == "2")
  {
    cout << "========Apply Projection Results========" << endl;

    for(Uint32 i = 0; i < _statements.size(); i++)
    {
      cout << "======================================" << i << endl;
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

          String prefix("-----");

          if (cnt > 10 && !cqlcli_verbose)
          {
            cout << "-----Instance has " << cnt << " properties" << endl;
          }
          else
          {
            for (Uint32 n = 0; n < cnt; n++)
            {
              CIMProperty prop = projInst.getProperty(n);
              printProperty(prop, n, prefix);
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
      cout << "======================================" << i << endl;
      cout << _statements[i].toString() << endl;

      try
      {
        _statements[i].validate();
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

    CIMPropertyList propList;

    for(Uint32 i = 0; i < _statements.size(); i++)
    {
      cout << "======================================" << i << endl;
      cout << _statements[i].toString() << endl;

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

      try
      {     
        cout << "SELECT Chained Identifiers" << endl;
        Array<CQLChainedIdentifier> selIds = _statements[i].getSelectChainedIdentifiers();
        for (Uint32 k = 0; k < selIds.size(); k++)
        {
          cout << "-----" << selIds[k].toString() << endl;
        }
      }
      catch(Exception& e){ cout << "-----" << e.getMessage() << endl;}
      catch(...){ cout << "Unknown Exception" << endl;}

      try
      {     
        cout << "WHERE Chained Identifiers" << endl;
        Array<CQLChainedIdentifier> whereIds = _statements[i].getWhereChainedIdentifiers();
        if (whereIds.size() == 0)
        {
          cout << "-----none" << endl; 
        }
        for (Uint32 k = 0; k < whereIds.size(); k++)
        {
          cout << "-----" << whereIds[k].toString() << endl;
        }
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

      // Build a list of unique class names from the instances
      Array<CIMName> classNames;
      for(Uint32 j = 0; j < _instances.size(); j++)
      {
        Boolean found = false;
        for(Uint32 k = 0; k < classNames.size(); k++)
        {
          if (_instances[j].getClassName() == classNames[k])
          {
            found = true;
          }
        }

        if (!found)
        {
          classNames.append(_instances[j].getClassName());
        }
      }

      for(Uint32 j = 0; j < classNames.size(); j++)
      {
        CIMName className = classNames[j];
        CIMObjectPath classPath (String::EMPTY,
                                 ns,
                                 className);

        try
        {
          cout << "Property List for " << className.getString() << endl;
          propList.clear();
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
      }
    }                         
  }
	
  return true;
}

Boolean _evaluate(Array<CQLSelectStatement>& _statements,
                  Array<CIMInstance>& _instances,
                  String testOption)
{
  // Not liking how the mof compiler is working with CQL_TestPropertyTypes, so I am going to hack the instances so that they have the values I need for the function tests.
  hackInstances(_instances);
  
  if(testOption == String::EMPTY || testOption == "1")
  {       
      cout << "=========Evaluate Query==============" << endl;                             
    for(Uint32 i = 0; i < _statements.size(); i++)
    {
      cout << "=========     " << i << "     =========" << endl;
      cout << "-----Query: " << _statements[i].toString() << endl << endl;;

      for(Uint32 j = 0; j < _instances.size(); j++)
      {
        try
        {
          cout << "-----Instance: " << _instances[j].getPath().toString() << endl;
          Boolean result = _statements[i].evaluate(_instances[j]);

	  if(cqlcli_verbose)
	    {
	      cout << "Inst # " << j << ": " <<  _statements[i].toString() << " = ";
	    }
          if(result) printf("TRUE\n");
          else printf("FALSE\n");
        }
        catch(Exception e){ cout << "ERROR!" << endl << e.getMessage() << endl << endl;}
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
      cout << "======================================" << i << endl;

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
  Array<CIMObject> testElemArray;
  Boolean foundTestElem = false;

  CIMInstance testCS;
  Boolean foundCS = false;

  Array<CIMObject> testPropTypesArray;

  for (Uint32 i = 0; i < instances.size(); i++)
  {
    // Find the CQL_TestElement with InstanceId = 0
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
        // Found the CQL_TestElement with InstanceId = 0.
        // Change to InstanceId to 10, and save it.
        foundTestElem = true;
        instances[i].removeProperty(index);
        instances[i].addProperty(CIMProperty(instIdName, CIMValue((Uint64)10)));
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

      // Remove the PrimaryOwnerName property so that apply projection
      // using the wildcard will fail
      Uint32 tmpIdx = testCS.findProperty("PrimaryOwnerName");
      if (tmpIdx != PEG_NOT_FOUND)
      {
        testCS.removeProperty(tmpIdx);
      }
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
  testElem.addProperty(CIMProperty(instIdName, CIMValue((Uint64)11)));
  testElemArray.append(testElem.clone());

  index = testElem.findProperty(instIdName);
  testElem.removeProperty(index);
  testElem.addProperty(CIMProperty(instIdName, CIMValue((Uint64)12)));
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
  //             the array elements have InstanceID properties
  //             set to 10, 11, 12
  //         property TE has instance of CQL_TestElement
  //             property InstanceID has value 10
  //         property CS has instance of CIM_ComputerSystem
  //             note that the PrimaryOwnerName property is removed
  //         property SomeClass has class of CIM_Process
  //         property SomeString has a string
  //   
  //   property EmbObjSub has
  //      instance of CQL_EmbeddedTypePropertyTypes
  //         property InstanceID has value 1001
  //         property TEArray has array of instance of CQL_TestElement
  //             the array elements have InstanceID properties
  //             set to 10, 11, 12
  //         property TE has instance of CQL_TestElement
  //             property InstanceID has value 10
  //         property CS has instance of CIM_ComputerSystem
  //             note that the PrimaryOwnerName property is removed
  //         property TPTArray has array of instance of CQL_TestPropertyTypes
  //             the array is built from the instances compiled in the repository
  //         property TPT has instance of CQL_TestPropertyTypes
  //             this instance is the first instance found in the repository
  //         property SomeClass has class of CIM_Process
  //         property SomeString has a string
  //         property SomeUint8 has a uint8
  //
  CIMInstance embTE("CQL_EmbeddedTestElement");
  embTE.addProperty(CIMProperty("InstanceID", CIMValue((Uint64)1000)));
  embTE.addProperty(CIMProperty("TEArray", CIMValue(testElemArray)));
  CIMObject _obj1 = testElemArray[0];
  CIMValue testElemVal(_obj1);
  embTE.addProperty(CIMProperty("TE", testElemVal));
  CIMValue csVal(testCS);
  embTE.addProperty(CIMProperty("CS", csVal));
  embTE.addProperty(CIMProperty("SomeClass", CIMValue(someClass)));
  embTE.addProperty(CIMProperty("SomeString", CIMValue(String("Huh?"))));

  CIMInstance embTPT("CQL_EmbeddedTestPropertyTypes");
  embTPT.addProperty(CIMProperty("InstanceID", CIMValue((Uint64)1001)));
  embTPT.addProperty(CIMProperty("TEArray", CIMValue(testElemArray)));
  embTPT.addProperty(CIMProperty("TE", testElemVal));
  embTPT.addProperty(CIMProperty("CS", csVal));
  embTPT.addProperty(CIMProperty("TPTArray", CIMValue(testPropTypesArray)));
  embTPT.addProperty(CIMProperty("TPT", CIMValue(testPropTypesArray[0])));
  embTPT.addProperty(CIMProperty("SomeClass", CIMValue(someClass)));
  embTPT.addProperty(CIMProperty("SomeString", CIMValue(String("What?"))));
  embTPT.addProperty(CIMProperty("SomeUint8", CIMValue((Uint8)3)));

  CIMInstance embSub("CQL_EmbeddedSubClass");
  embSub.addProperty(CIMProperty("InstanceID", CIMValue((Uint64)100)));
  embSub.addProperty(CIMProperty("EmbObjBase", CIMValue(embTE)));  
  embSub.addProperty(CIMProperty("EmbObjSub", CIMValue(embTPT)));  

  instances.clear();
  instances.append(embSub);
}


Boolean populateInstances(Array<CIMInstance>& _instances, String& className, CIMNamespaceName& _ns, CIMRepository* _rep)
{
  String embSubName("CQL_EmbeddedSubClass");
  String embBaseName("CQL_EmbeddedBase");

  // IF the class is CIM_RunningOS, then we will setup some references to CIM_ComputerSystem
  if (className == "CIM_RunningOS")
  {
    Array<CIMInstance> cSystems;
    const CIMName CSClass(String("CIM_ComputerSystem"));

    try
    {
      // Deep inh = false to only get the CIM_ComputerSystem
      cSystems.appendArray(_rep->enumerateInstances( _ns, CSClass, false));
    }
    catch(Exception& e)
    {
      cout << endl << endl << "Exception: Invalid namespace/class: " << e.getMessage() << endl << endl;
      return false;
    }

    // For every computer system instance, make a runningOS that has a reference to it.  The RunningOS will be the instance that is stored.
    for (Uint32 i=0; i < cSystems.size(); i++)
    {
      CIMInstance runOS("CIM_RunningOS");
      runOS.addProperty(CIMProperty("Dependent", CIMValue(cSystems[i].getPath())));
      
      _instances.append(runOS);
    }
    return true;
  }
  
  if(className != String::EMPTY && className != embSubName && className != embBaseName)
  {
    // If the classname was specified, and was not an embedded object class, then
    // load its instances from the repository.
    try
    {
      const CIMName _testclass(className);
      _instances = _rep->enumerateInstances( _ns, _testclass, true );  // deep inh true
    }
    catch(Exception& e){
      cout << endl << endl << "Exception: Invalid namespace/class: " << e.getMessage() << endl << endl;
      return false;
    }
  }
  else
  { 
    // load all the non-embedded instances we support
    cout << endl << "Using default class names to test queries. " << endl << endl;
    const CIMName _testclass1(String("CQL_TestElement"));
    const CIMName _testclass2(String("CIM_ComputerSystem"));
    try
    {
      // Deep inh = true for CQL_TestElement to also get CQL_TestPropertyTypes
      // and CQL_TestPropertyTypesMissing
      _instances = _rep->enumerateInstances( _ns, _testclass1, true ); // deep inh true

      // Deep inh = false to only get the CIM_ComputerSystem
      _instances.appendArray(_rep->enumerateInstances( _ns, _testclass2, false )); // deep inh false
    }
    catch(Exception& e)
    {
      cout << endl << endl << "Exception: Invalid namespace/class: " << e.getMessage() << endl << endl;
      return false;
    }

    if (className == embSubName || className == embBaseName)
    {
      // If the embedded object classname was specified, then build its instances.
      // Note: this will remove the other instances from the array.
      buildEmbeddedObjects(_ns,
                           _instances,
                           _rep);
    }
  }
  return true;
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
	cout << " -nameSpace namespace (Example: root/SampleProvider)" << endl;
	cout << " -verbose" << endl << endl;
}

int main(int argc, char ** argv)
{
  // process options
  if(argc == 1 || (argc > 1 && strcmp(argv[1],"-h") == 0) ){
    help(argv[0]);
    exit(0);
  }

  // Since the output of this program will be compared with
  // a master output file, turn off ICU message loading.
  MessageLoader::_useDefaultMsg = true;

  String testOption;
  String className = String::EMPTY;
  String nameSpace;

  for(int i = 0; i < argc; i++){
    if((strcmp(argv[i],"-test") == 0) && (i+1 < argc))
      testOption = argv[i+1];
    if((strcmp(argv[i],"-className") == 0) && (i+1 < argc))
      className = argv[i+1];
    if((strcmp(argv[i],"-nameSpace") == 0) && (i+1 < argc))
      nameSpace = argv[i+1];
    if((strcmp(argv[i],"-verbose") == 0) && (i+1 < argc))
      cqlcli_verbose = true;
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
  String query("dummy statement");
  CQLSelectStatement _ss(lang,query,_ctx);
  if (_ss.getQuery() != query || _ss.getQueryLanguage() != lang)
  {
    cout << "ERROR: unable to get query or query language from select statement" << endl;
    return 1;
  }


  char text[1024];
  char* _text;

  // setup Test Instances
  Array<CIMInstance> _instances;

  if (!populateInstances(_instances, className, _ns, _rep))
    return 1;

  // demo setup
  if(argc == 3 && strcmp(argv[2],"Demo") == 0){
    cout << "Running Demo..." << endl;
    _instances.clear();
    const CIMName _testclassDEMO(String("CIM_Process"));
    _instances.appendArray(_rep->enumerateInstances( _ns, _testclassDEMO ));
    _instances.remove(6,6);
  }

  for(Uint32 i = 0; i < _instances.size(); i++){
    CIMObjectPath op = _instances[i].getPath();
    op.setHost("somesystem.somecountry.somecompany.com");
    _instances[i].setPath(op);
  }	
  // setup input stream
  if(argc >= 2){
    ifstream queryInputSource(argv[1]);
    if(!queryInputSource){
      cout << "Cannot open input file.\n" << endl;
      return 1;
    }
    int statementsInError = 0;
    while(!queryInputSource.eof()){
      queryInputSource.getline(text, 1024);
      char* _ptr = text;
      _text = strcat(_ptr,"\n");	
      // check for comments and ignore
      // a comment starts with a # as the first non whitespace character on the line
      char _comment = '#';
      int i = 0;
      while(text[i] == ' ' || text[i] == '\t') i++; // ignore whitespace
      if(text[i] != _comment)
        if(!(strlen(_text) < 2)){
          try {
            CQLParser::parse(text,_ss);
            _statements.append(_ss);
          } // end-try
          catch(Exception& e){
            cout << "Caught Exception: " << e.getMessage()  << endl;
            cout << "Statement with error = " << text << endl;
            _ss.clear();
            statementsInError++;
          } // end-catch
        } // end-if
    } // end-while
    queryInputSource.close();
    if (statementsInError)
    {
      cout << "There were " << statementsInError << " statements that did NOT parse." << endl;
      // return 1;
    }
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

