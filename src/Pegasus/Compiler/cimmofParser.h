//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Author: Bob Blair (bblair@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


//
//
// This header describes the cimmofParser class.  
// It is a singleton, and can only be accessed via the pointer
// returned by its static Intance() method.
// //
// The instance of this
// class hold enough state information that there should be no need for
// the underlying YACC parser to be written reentrant.
//
// The YACCer (and LExer) communicate with the instance of this class
// via the ointer returned by the Instance() method.
//
// This specialization contains a reference to the containing program's
// mofComplerCmdLine object, which holds the command line arguments
// including the list of directories to search to find included mof files
//

#ifndef _CIMMOFPARSER_H_
#define _CIMMOFPARSER_H_


#include "parser.h"
#include "mofCompilerOptions.h"
#include "cimmofRepository.h"
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Compiler/compilerCommonDefs.h>
#include "memobjs.h"
#include "objname.h"

extern int cimmof_parse(); // the yacc parser entry point

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

class cimmofRepository;

// This class extends class parser (see parser.h)
class PEGASUS_COMPILER_LINKAGE cimmofParser : public parser {
 private:
  // This is meant to be a singleton, so we hide the constructor
  // and the destructor
  static cimmofParser *_instance;
  cimmofParser();
  ~cimmofParser();
  void elog(const String &msg) const; // handle logging of warnings
  void wlog(const String &msg) const; // handle logging of warnings
  void trace(const String &head, const String &tail) const;
  //either throw us out or retry depending on user preference
  void maybeThrowParseError(const String &msg) const;
  void maybeThrowLexerError(const String &msg) const;

  // Here are the members added by this specialization
  const mofCompilerOptions *_cmdline;
  String _includefile;  // temp storage for included file to be entered
  cimmofRepository *_repository; // the repository object to use
  String _defaultNamespacePath;  // The path we'll use if none is given
  String _currentNamespacePath;  // a namespace set from a #pragma
  compilerCommonDefs::operationType _ot;
 public:
  // Provide a way for the singleton to be constructed, or a
  // pointer to be returned:
  static cimmofParser *Instance();

  //------------------------------------------------------------------
  // Methods for manipulating the members added in this specialization
  //------------------------------------------------------------------
  // compiler options.  This may be set from command line data,
  // or by an embedding application
  void setCompilerOptions(const mofCompilerOptions *co);
  const mofCompilerOptions *getCompilerOptions() const;
  // for all, or nearly all, operations, a repository object is needed
  Boolean setRepository(void);
  const cimmofRepository *getRepository() const;
  // Whether you need a repository or not depends on the operationsType
  void setOperationType(compilerCommonDefs::operationType);
  compilerCommonDefs::operationType getOperationType() const;
  // Set a default root namespace path to pass to  the repository
  void setDefaultNamespacePath(const String &path); // default value
  void setCurrentNamespacePath(const String &path); // current override
  const String &getDefaultNamespacePath() const;
  const String &getCurrentNamespacePath() const;
  // Get the effective namespace path -- the override, if there is one.
  const String &getNamespacePath() const;
  //------------------------------------------------------------------
  // Methods that implement or override base class methods
  //------------------------------------------------------------------
  // establish an input buffer given an input file stream
  int setInputBuffer(const FILE *f);
  // establish an input buffer given an existing context (YY_BUFFERSTATE)
  int setInputBuffer(void *buffstate);
  // Dig into an include file given its name
  int enterInlineInclude(const String &filename);
  // Dig into an include file given an input file stream
  int enterInlineInclude(const FILE *f);
  // Handle end-of-file 
  int wrapCurrentBuffer();
  // Parse an input file
  int parse();
  // Log a parser error
  void log_parse_error(char *token, char *errmsg) const;

  //------------------------------------------------------------------
  // Do various representation transformations.
  // These are in this class simply because there wasn't another
  // conventient place for them.  They could just as well be static
  // methods of some convenience class.
  //------------------------------------------------------------------
  //    Octal character input to decimal character output
  char *oct_to_dec(const String &octrep) const;
  //    Hex character input to decimal character output
  char *hex_to_dec(const String &hexrep) const;
  //    Binary character input to decimal character output
  char *binary_to_dec(const String &binrep) const;

  //------------------------------------------------------------------
  // Handle the processing of CIM-specific constructs
  //------------------------------------------------------------------
  // This is called after a completed #pragma production is formed
  void processPragma(const String &pragmaName, const String &pragmaString);
  // This is called when a completed class declaration production is formed
  int addClass(CIMClass *classdecl);
  // This is called when a new class declaration heading is discovered
  CIMClass *newClassDecl(const String &name, const String &superclass);
  // Called when a completed instanace declaration production is formed
  int addInstance(CIMInstance *instance);
  // Called when a new qualifier declaration heading is discovered
  CIMQualifierDecl *newQualifierDecl(const String &name, const CIMValue *value,
				  Uint32 scope, Uint32 flavor);
  // Called when a completed qualifier declaration production is formed
  int addQualifier(CIMQualifierDecl *qualifier);
  // Called when a new qualifier declaration heading is discovered
  CIMQualifier *newQualifier(const String &name, const CIMValue &val,
			     Uint32 flav);
  // Called when a new instance declaration heading is discovered
  CIMInstance *newInstance(const String &name);
  // Called when a new property is discovered
  CIMProperty *newProperty(const String &name, const CIMValue &val,
			   const String &referencedObj = String::EMPTY) const;
  // Called when a property production inside a class is complete
  int applyProperty(CIMClass &c, CIMProperty &p);
  // Called when a property production inside an instance is complete
  int applyProperty(CIMInstance &instance, CIMProperty &p);
  // Called when a new method is discovered
  CIMMethod   *newMethod(const String &name, const CIMType type);
  // Called when a method production inside a class is complete
  int applyMethod(CIMClass &c, CIMMethod &m);
  // Called when a method parameter is discovered
  CIMParameter *newParameter(const String &name, const CIMType type,
			     Boolean isArray=false, Uint32 array=0, 
			     const String &objName=String::EMPTY);
  // Called when a method parameter production is complete
  int applyParameter(CIMMethod &method, CIMParameter &parm);
  // Called when a qualifier value production is complete
  CIMValue *QualifierValue(const String &qualifierName, const String &valstr);
  // Called to retrieve the value object for an existing parameter
  CIMProperty *PropertyFromInstance(CIMInstance &instance,
				    const String &propertyName) const;
  CIMValue *ValueFromProperty(const CIMProperty &prop) const;
  CIMValue *PropertyValueFromInstance(CIMInstance &instance, 
				      const String &propertyName) const; 
  // Called when a class alias is found
  void addClassAlias(const String &alias, const CIMClass *cd, 
		Boolean isInstance);
  // Called when an instance alias is found
  void addInstanceAlias(const String &alias, const CIMInstance *cd, 
		Boolean isInstance);
  // Called when a reference declaration is found
  CIMReference *newReference(const objectName &oname);
  // Make a clone of a property object, inserting a new value object
  CIMProperty *copyPropertyWithNewValue(const CIMProperty &p,
					const CIMValue &v) const;
};

// Exceptions

class PEGASUS_COMPILER_LINKAGE LexerError : public Exception {
 public:
  static const char MSG[];
  LexerError(const String &lexerr) : Exception(MSG + lexerr) {}
};

#endif
  


