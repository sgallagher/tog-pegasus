//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//END_LICENSE
//BEGIN_HISTORY
//
// Author: Bob Blair (bblair@bmc.com)
//
// $Log: cimmofParser.cpp,v $
// Revision 1.3  2001/02/18 19:02:17  mike
// Fixed CIM debacle
//
// Revision 1.2  2001/02/18 02:45:49  mike
// Added bison.simple to distribution so that there
// would be no dependency on an external directory.
//
// Revision 1.1  2001/02/16 23:59:09  bob
// Initial checkin
//
//
//END_HISTORY
//
// implementation of valueFactory 
//
//
//
// Implementation of methods of cimmofParser class
//
//
#include "cimmofParser.h"
#include <cstring>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMScope.h>
#include <iostream>
#include "valueFactory.h"

//
// These routines are in the lexer.  They are there because
// there is no need for class cimmofParser to know the details
// of YY_BUFFER_STATE and its associated methods.
//
extern int get_yy_buf_size_wrapper();
extern void *get_cimmof__current_buffer_wrapper();
extern int switch_to_buffer_wrapper(void *buffstate);
extern void *create_cimmof_buffer_wrapper(const FILE *f, int size);

const char ParseError::MSG[] = "Cimmof Parse Error";

cimmofParser *cimmofParser::_instance = 0;

cimmofParser::cimmofParser(): 
  parser(),  _cmdline(0), _repository(0) {
}

cimmofParser::~cimmofParser() {
  if (_repository)
    delete _repository;
}

cimmofParser *
cimmofParser::Instance() {
  if (!_instance) {
    _instance = new cimmofParser();
  }
  return _instance;
}

int
cimmofParser::setInputBuffer(const FILE *f) {
  void *buf = create_cimmof_buffer_wrapper(f, get_buffer_size());
  if (buf)
    return setInputBuffer(buf);
  else
    return -1;
}

int
cimmofParser::setInputBuffer(void *buffstate)
{
  return switch_to_buffer_wrapper(buffstate);
};

int
cimmofParser::enterInlineInclude(const String &filename) {
  int ret = 1;
  FILE *f = 0;
  char *fqname = (char *)filename.allocateCString(0, true);

  //cout << "In enterInlineInclude(" << filename << ")" << endl;

  f = fopen(fqname, "r");
  if (!f) {
    if (_cmdline) {
      const vector<string> &include_paths = _cmdline->get_include_paths();
      for (unsigned int i = 0; i < include_paths.size(); i++) {
	string s = include_paths[i] + "/" + fqname;
	if ( (f = fopen(s.c_str(), "r")) ) {
	  _includefile = s;
	  break;
	}	 
      } 
    } else {  // incorrect call:  cmdline should have been set
      return ret;
    }
  }
  if (f) {
     ret = enterInlineInclude((const FILE *)f);
  } else {
    // FIXME:  need to throw an exception 
    cerr << "Could not open include file " << fqname << endl;
  }
  return ret;
}
    
int
cimmofParser::enterInlineInclude(const FILE *f) {
  if (f) {
    set_buffer_size(get_yy_buf_size_wrapper());
    void *buf = get_cimmof__current_buffer_wrapper();
    bufstate *bs = new bufstate;
    bs->buffer_state = buf;
    bs->filename = get_current_filename();
    bs->lineno = get_lineno();
    push_statebuff(bs);
    set_current_filename(_includefile);
    set_lineno(0);
    return setInputBuffer(f);
  }
  return 1;
}

//---------------------------------------------------------------------
// allow someone to set our compiler options object reference
//---------------------------------------------------------------------
void
cimmofParser::setCompilerOptions(const mofCompilerOptions *co) {
  _cmdline = co;
  const string path = co->get_namespacePath();
  setDefaultNamespacePath(path.c_str());
}

const mofCompilerOptions *
cimmofParser::getCompilerOptions() const {
  return _cmdline;
}

//---------------------------------------------------------------------
// Set the repository we will be using.  The path should be in
// the command line
//---------------------------------------------------------------------
bool
cimmofParser::setRepository(void) {
  const String &s = getDefaultNamespacePath();
  if (_cmdline) {
    String rep = _cmdline->get_repository_name();
    if (rep != "") {
      try {
      _repository = new cimmofRepository(rep);
      } catch(CannotCreateDirectory &e) {
	cerr << "Creating CIMRepository: " << e.getMessage() << endl;
	return false;
      } catch(CimException &e) {
	cerr << "Creating CIMRepository: " << e.codeToString(e.getCode()) << 
	  "(" << e.getMessage() << endl;
	return false;
      } 
      try {
        _repository->createNameSpace(s);
      } catch(AlreadyExists &) {
	// OK, that's what we expect
      } catch(IllegalName &) {
        cerr << "Creating CIMRepository namespace: " << 
	cerr << "CIMRepository reported " << s << " is not a legal CIM name"
	     << endl;
	return false;
      } catch(Exception &e) {
	cerr << "Creating namespace: " << e.getMessage() << endl;
	return false;
      }
    } else {
      //FIXME:  Bad repository name
      cerr << "Internal Error: Bad CIMRepository CIMName" << endl;
    }
  } else {
    // FIXME:  Invalid call -- somebody has to set the cmdline
    cerr << "Internal Error: No compiler options object" << endl;
  }
  return (_repository ? true : false);
}

//------------------------------------------------------------------
// Set up the default namespace path in the repository
//------------------------------------------------------------------
void 
cimmofParser::setDefaultNamespacePath(const String &path) {
  if (_defaultNamespacePath == "")  // it can only be set once
    _defaultNamespacePath = path;
}

void
cimmofParser::setCurrentNamespacePath(const String &path) {
  _currentNamespacePath = path;
}

//------------------------------------------------------------------
// Return a pointer to our CIMRepository object
//------------------------------------------------------------------
const cimmofRepository *
cimmofParser::getRepository() const {
  return _repository;
}

//------------------------------------------------------------------
// Return the namespace path members
//------------------------------------------------------------------
const String &
cimmofParser::getDefaultNamespacePath() const {
  return _defaultNamespacePath;
}

const String &
cimmofParser::getCurrentNamespacePath() const {
  return _currentNamespacePath;
}

const String &
cimmofParser::getNamespacePath() const {
  if (_currentNamespacePath == "") {
    return _defaultNamespacePath;
  }
  return _currentNamespacePath;
}

// -----------------------------------------------------------------
// Convert a string representing an octal integer to a string        
// representing the corresponding decimal integer                    
// FIXME:  Need to support 64-bit integers
// FIXME:  Needs to support non-ascii strings
// ------------------------------------------------------------------
char *
cimmofParser::oct_to_dec(const String &octrep) const {
  unsigned long oval = 0;
  char buf[40];  // can't overrrun on an itoa of a long
  for (int i = octrep.getLength() - 1; i >= 0; --i) {
     oval *= 8;
     switch(octrep[1]) {
     case '1': oval += 1; break;
     case '2': oval += 2; break;
     case '3': oval += 3; break;
     case '4': oval += 4; break;
     case '5': oval += 5; break;
     case '6': oval += 6; break;
     case '7': oval += 7; break; 
     case '8': oval += 8; break;
     }
  }
  sprintf(buf, "%ld", oval);
  return strdup(buf);
}

// ----------------------------------------------------------------- 
// Convert a string representing a hexadecimal integer to a string   
// representing the corresponding decimal integer                    
// FIXME:  Need to support 64-bit integers
// FIXME:  Needs to support non-ascii strings
// ------------------------------------------------------------------
char *
cimmofParser::hex_to_dec(const String &hexrep) const {
  unsigned long hval = 0;
  char buf[40];  // can't overrrun on an itoa of a long
  for (int i = hexrep.getLength() - 1; i >= 2; --i) {
     hval *= 16;
     switch(hexrep[i]) {
     case '1': hval += 1; break;
     case '2': hval += 2; break;
     case '3': hval += 3; break;
     case '4': hval += 4; break;
     case '5': hval += 5; break;
     case '6': hval += 6; break;
     case '7': hval += 7; break; 
     case '8': hval += 8; break;
     case '9': hval += 9; break;
     case 'a': case 'A':
       hval += 10;
       break;
     case 'b': case 'B':
       hval += 11;
       break;
     case 'c': case 'C':
       hval += 12;
       break;
     case 'd': case 'D':
       hval += 13;
       break;
     case 'e': case 'E':
       hval += 14;
       break;
     case 'f': case 'F':
       hval += 15;
       break;
     }
  }
  sprintf(buf, "%ld", hval);
  //  ltoa(hval, buf, 10);
  return strdup(buf);
}

// ----------------------------------------------------------------- 
// Convert a string representing a binary integer to a string        
// representing the corresponding decimal integer                    
// FIXME:  Need to support 64-bit integers
// ------------------------------------------------------------------
char *
cimmofParser::binary_to_dec(const String &binrep) const {
  unsigned long bval = 0;
  char buf[40];  // can't overrrun on an itoa of a long
  for (int i = binrep.getLength() - 1; i >= 0; --i) {
     bval *= 2;
     bval += binrep[i] == '1' ? 1 : 0;
  }
  sprintf(buf, "%ld", bval);
  return strdup(buf);
}

void
cimmofParser::processPragma(const String &name, const String &value) {
  // The valid names are:
  // instancelocale
  // locale
  // namespace
  // nonlocal
  // nonlocaltype
  // source
  // sourcetype

  // FIXME:  We have to be able to do something about the namespaces
  // at least.
}

int 
cimmofParser::addClass(CIMClass *classdecl)
  // FIXME
{
  int ret = 0;
  if ( _cmdline->trace() ) {
    cout << "<trace> ADD CLASS : ";
    classdecl->print();
  }
  if (_cmdline->syntax_only()) {
    return ret; 
  }
  try {
    ret = _repository->addClass(classdecl);
  } catch(CimException &e) {
    cerr << "Adding Class " << classdecl->getClassName() << ": "
	 << e.getMessage() << endl;
    if (e.getCode() != CimException::ALREADY_EXISTS)
      throw ParseError();
  } 
// FIXME

  if (_cmdline->trace()) {
    cout << "\tOK." << endl;
  }
  return ret;
}

CIMClass *
cimmofParser::newClassDecl(const String &name, const String &superclassname)
{
  CIMClass *c = 0;
  //cout << "DEBUG: newClassDecl: (" << name << ")(" << superclassname
  //     << ")" <<endl;
  try {
    c = new CIMClass(name, superclassname);
  } catch(IllegalName) {
    cerr << "Parsing error in Class declaration for " << name << 
      ":  Illegal CIMName" << endl;
    throw ParseError();
  } catch(CimException &e) {
    cerr << "Creating Class " << name << ": " << e.getMessage() << endl;
    throw ParseError();
  } 

  return c;
}

CIMProperty *
cimmofParser::newProperty(const String &name, const CIMValue &val)
{
  CIMProperty *p = 0; 
  // cout << "DEBUG: newProperty: (" << name << ")(" << val.toString()
  //     << ")" << endl;
  try {
    p = new CIMProperty(name, val);
  } catch(IllegalName) {
    cerr << "Parsing error in CIMProperty declaration for " << name << 
      ":  Illegal CIMName" << endl;
    throw ParseError();
  } catch(CimException &e) {
    cerr << "Creating New CIMProperty " << name << ": " 
	 << e.getMessage() << endl;
    throw ParseError();
  } 

  return p;
}

int
cimmofParser::applyProperty(CIMClass &c, CIMProperty &p)
{
  try {
    c.addProperty(p);
  } catch(UnitializedHandle) {
    cerr << "Internal Error: unitialized parameter in class " <<
         c.getClassName() << endl;
    throw ParseError();
  } catch(AlreadyExists) {
    cerr << "Warning: property " << p.getName() << " in class " <<
         c.getClassName() << " already exists." << endl;
  } catch(AddedReferenceToClass) {
    cerr <<"Tried to add reference property " << p.getName() <<
      " to non-aggregate class " << c.getClassName() << endl;
    throw ParseError();
  } catch(CimException &e) {
    cerr << "Applying CIMProperty " << p.getName() << " to class " 
	 << c.getClassName() << ": " << e.getMessage() << endl;
    throw ParseError();
  } 

  return 0;
}

int 
cimmofParser::addInstance(CIMInstance *instance)
  // FIXME
{ 
  int ret = 0;
  if (_cmdline->trace()) {
    cout << "<trace>ADD INSTANCE : ";
    instance->print();
  }
  if (_cmdline->syntax_only()) {
    return ret; 
  }
  ret = _repository->addInstance(instance);
  if (_cmdline->trace()) {
    cout << "\tOK." << endl;
  }
  return ret;
}

CIMQualifierDecl *
cimmofParser::newQualifierDecl(const String &name, const CIMValue *value,
			       Uint32 scope, Uint32 flavor) {
  CIMQualifierDecl *q = 0;
  //cout << "DEBUG: newQualifierDecl: (" << name << ")(" << value->toString() <<
  //")(" << scope << ")(" << flavor << ")" << endl;
  try {
    q = new CIMQualifierDecl(name, *value, scope, flavor);
  } catch(IllegalName) {
    cerr << "Parsing Error in CIMQualifier Declaration for " << name 
	 << ": " << endl << "\tIllegal CIM CIMName" << endl;
    throw ParseError();
  } catch(CimException &e) {
    cerr << "Creating New CIMQualifierDecl " << name << ": " 
	 << e.getMessage() << endl;
    throw ParseError();
  } 

  return q;
}

CIMQualifier *
cimmofParser::newQualifier(const String &name, const CIMValue &value,
			   Uint32 flavor)
{
  CIMQualifier *q = 0;
  //cout << "DEBUG: new CIMQualifier: (" << name << ")(" << value.toString() <<
  //  ")(" << flavor << ")" << endl;
  try {
    q = new CIMQualifier(name, value, flavor);
  } catch(IllegalName) {
    cerr << "Parsing Errror in CIMQualifier Declaration for " << name
	 << ": " << endl << "\tIllegal CIM CIMName" << endl;
    throw ParseError();
  } catch(CimException &e) {
    cerr << "Creating New CIMQualifier " << name << ": " 
	 << e.getMessage() << endl;
    throw ParseError();
  } 

  //cout << "\tOK" << endl;
  return q;
}

int 
cimmofParser::addQualifier(CIMQualifierDecl *qualifier)
  // FIXME
{
  int ret  = 0;
  if (_cmdline->trace()) {
    cout << "[trace]ADD QUALIFIER : ";
    qualifier->print();
  }
  if (_cmdline->syntax_only()) {
    return ret; 
  }
  try {
    ret = _repository->addQualifier(qualifier);
  } catch(Exception e) {
    cerr << "Error trying to add the qualifier: "; 
    qualifier->print();
    cerr << "Parsing error was: " << e.getMessage() << endl;
    return ret;
  }
  if (_cmdline->trace()) {
    cout << "\tOK." << endl;
  }
  return ret;
}

CIMMethod *
cimmofParser::newMethod(const String &name, const CIMType type)
{ 
  CIMMethod *m;
  //cout << "DEBUG: New CIMMethod(" << name << ")(" << type << ")" << endl;
  try {
    m = new CIMMethod(name, type);
  } catch(IllegalName) {
    cerr << "Parsing Errror in CIMQualifier Declaration for " << name
	 << ": " << endl << "\tIllegal CIM CIMName" << endl;
    throw ParseError();
  } catch(CimException &e) {
    cerr << "Creating new CIMMethod " << name << ": " 
	 << e.getMessage() << endl;
    throw ParseError();
  } 

  //cout << "\tOK" << endl;
  return m;
}

int
cimmofParser::applyMethod(CIMClass &c, CIMMethod &m) {
  try {
    c.addMethod(m);
  } catch(UnitializedHandle e) {
    cerr << "Internal Error: unitialized parameter in class " 
	 << c.getClassName() << endl;
    throw ParseError();
  } catch(AlreadyExists e) {
    cerr << "Warning: CIMMethod " << m.getName() << " already exists for class "
	 << c.getClassName() << endl;
  } catch(CimException &e) {
    cerr << "Applying CIMMethod " << m.getName() << " to " << "class "
	 << c.getClassName() << ": " << e.getMessage() << endl;
    throw ParseError();
  } 

  return 0;
}

CIMParameter *
cimmofParser::newParameter(const String &name, const CIMType type)
{
  CIMParameter *p = 0;
  try {
    p = new CIMParameter(name, type);
  } catch(IllegalName) {
    cerr << "Parsing Errror in CIMQualifier Declaration for " << name
	 << ": " << endl << "\tIllegal CIM CIMName" << endl;
    throw ParseError();
  } catch(CimException &e) {
    cerr << "Creating new MethodParameter " << name << ": " 
	 << e.getMessage() << endl;
    throw ParseError();
  } 
  return p;
}

int
cimmofParser::applyParameter(CIMMethod &m, CIMParameter &p) {
  try {
    m.addParameter(p);
  } catch(CimException &e) {
    cerr << "Applying parameter " << p.getName() << " to method " 
	 << m.getName() <<  ": "  << e.getMessage() << endl;
    throw ParseError();
  } 
  return 0;
}



CIMValue *
cimmofParser::QualifierValue(const String &qualifierName, const String &valstr)
{
  CIMQualifierDecl q = _repository->getQualifierDecl(qualifierName);
  CIMValue v = q.getValue();
  Uint32 asize = v.getArraySize();
  //cout << "Found existing declaration for qualifier " << qualifierName << endl;
  //cout << "      type is " << v.getType() << ", isArray=" << v.isArray() <<
  //     endl;
  //cout << "      value to set is " << valstr << endl;
  if (valstr == String::EMPTY) {
    if (v.getType() == CIMType::BOOLEAN) {
      Boolean b;
      v.get(b);
      v.set(!b);
    }
    return new CIMValue(v);
  } else {
    return valueFactory::createValue(v.getType(),
				     v.isArray() ? (int)asize : -1, 
				     &valstr);
  }
}


void
cimmofParser::addClassAlias(const String &alias, const CIMClass *cd,
		       bool isInstance)
{
  // FIXME:  As soon as we figure out what Aliases are for, do something
}

void
cimmofParser::addInstanceAlias(const String &alias, const CIMInstance *cd,
		       bool isInstance)
{
  // FIXME:  As soon as we figure out what Aliases are for, do something
}

int
cimmofParser::wrapCurrentBuffer()
{ return wrap(); }

int
cimmofParser::parse()
{ return cimmof_parse(); }

