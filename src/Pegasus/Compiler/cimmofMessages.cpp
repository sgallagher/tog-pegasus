//
// implementation of cimmofMessages class

#include "cimmofMessages.h"
#include <cstdio>
#include <iostream> // for debug only

PEGASUS_NAMESPACE_BEGIN
using namespace std;

const cimmofMessages::arglist cimmofMessages::EMPTYLIST(0);

static String _cimmofMessages[] = {
  "OK", //SUCCESS
  "%1:%2: %3 before '%4'", //PARSER_SYNTAX_ERROR
  "Error applying parameter %1 to method %2: %3", //APPLY_PARAMETER_ERROR
  "Error creating new CIMParameter object %1: %2", //NEW_PARAMETER_ERROR
                                                 //UNINTIALIZED_PARAMETER_ERROR
  "Internal Error: Uninitialized parameter handle %1 in class %2",
                                                //METHOD_ALREADY_EXISTS_WARNING
  "Warning: CIMMethod %1 already exists for CIMClass %2",
  "Error applying method %1 to CIMClass %2: %3:", //APPLY_METHOD_ERROR
  "Error creating new CIMMethod %1: %2", //NEW_METHOD_ERROR
  "ADD QUALIFIER: ", //ADD_QUALIFIER
  "[Trace]", //TRACE
  "Error adding qualifier declaration %1: %2", //ADD_QUALIFIER_DECL_ERROR
  "\tOK", //TAB_OK
  "Error creating new Qualifier %1: %2", //NEW_QUALIFIER_ERROR
  "Error adding new Qualifier %1: %2", //ADD_QUALIFIER_ERROR
                                              //NEW_QUALIFIER_DECLARATION_ERROR
  "Error creating new Qualifier Declaration %1: %2",
  "ADD INSTANCE: ",  //ADD_INSTANCE
                                                 //UNINITIALIZED_PROPERTY_ERROR
  "Internal Error: Uninitialized parameter %2 in class %1",
                                              //PROPERTY_ALREADY_EXISTS_WARNING
  "Warning:  Property %2 already exists in class %1",
  "Error applying property %2 to class %1: %3", //APPLYING_PROPERTY_ERROR
  "Error creating new Property %1: %2", //NEW_PROPERTY_ERROR
  "Error creating new Class Declaration %1: %2", //NEW_CLASS_ERROR
  "ADD CLASS: ", //ADD_CLASS
  "Warning:  Class %1 already exists in the repository", //CLASS_EXISTS_WARNING
  "Error adding class %1 to the repository: %2", //ADD_CLASS_ERROR
                                            //SETREPOSITORY_NO_COMPILER_OPTIONS
  "Internal Error: Compiler options not set before setting repository",
                                                     //SETREPOSITORY_BLANK_NAME
  "Internal Error: No repository name was specified to setRepository",
                                                       //NAMESPACE_CREATE_ERROR
  "Error trying to create repository name space %1: %2",
  "Error trying to create Repository in path %1: %2", //REPOSITORY_CREATE_ERROR
  "Error trying to create a reference to object %1: %2", //NEW_REFERENCE_ERROR
                                              //FIND_CLASS_OF_INSTANCE_ERROR
  "Error looking for class of current instance while looking up property %1: %2",
                                              // FIND_PROPERTY_VALUE_ERROR
  "Error looking up value of property %3 in class %2 (namespace %1): %4",
  "Error cloning property %1: %2",            // CLONING_PROPERTY_ERROR
                                           // APPLY_INSTANCE_PROPERTY_ERROR
  "Error applying property %1 to an instance of class %2: %3",
	                                  // GET_INSTANCE_PROPERTY_ERROR
  "Error getting property %1 from an instance: %2", 
  "Error getting property %1 from class %2: %4", //GET_CLASS_PROPERTY_ERROR
  "Error getting value of property %1: %2", //GET_PROPERTY_VALUE_ERROR
  "Error creating new instance of class %1: %2", // NEW_INSTANCE_ERROR
                                          // INSTANCE_PROPERTY_EXISTS_WARNING
  "Warning: property %2 already exists for this instance of class %1",
	                                  // INSTANCE_EXISTS_WARNING
  "Warning: the instance already exists in the repository.\nIn this"
  " implementation, that means it cannot be changed.",
  "Error adding an instance to the repository: %1", // ADD_INSTANCE_ERROR
  ""
};

const String &
cimmofMessages::msgCodeToString(MsgCode code)
{
  return _cimmofMessages[(unsigned int)code];
}

static int
find(const String &haystack, const String &needle)
{
  unsigned int size = haystack.getLength();
  unsigned int npos = 0;
  unsigned int nsize = needle.getLength();
  for (unsigned int i = 0; i < size; i++) {
    if (haystack[i] == '\\')
      i++;
    if (haystack[i] == needle[npos]) {
      npos++;
      if (npos >= nsize)
	return ( (int)(i - nsize + 1) );
    } else {
      npos = 0;
    }
  }
  return -1;
}

static void 
replace(String &s, unsigned int pos, unsigned int len, const String &r)
{
  String s1 = s.subString(0, pos) + r + s.subString(pos  + len);
  s = s1;
}

void
cimmofMessages::getMessage(String &out, MsgCode code, const arglist &args)
{
  String s = msgCodeToString(code);
  out = s;
  int pos;
  for (unsigned int i = 0; i < args.size(); i++) {
    int state = 0;
    char buf[40];
    sprintf(buf, "%d", i + 1);
    String srchstr = "%";
    srchstr.append(buf);
    if ( (pos = find(out, srchstr)) != -1 ) {
      replace(out, pos, srchstr.getLength(), args[i]);
    }
  }
}

PEGASUS_NAMESPACE_END
