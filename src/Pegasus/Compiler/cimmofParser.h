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
#include "memobjs.h"
#include <string>

extern int cimmof_parse(); // the yacc parser entry point

using namespace std;

class cimmofRepository;

// This class extends class parser (see parser.h)
class PEGASUS_COMPILER_LINKAGE cimmofParser : public parser {
 private:
  // This is meant to be a singleton, so we hide the constructor
  // and the destrucot
  static cimmofParser *_instance;
  cimmofParser();
  ~cimmofParser();

  const mofCompilerOptions *_cmdline;
  string _includefile;  // temp storage for included file to be entered
  cimmofRepository *_repository; // the repository object to use
  String _defaultNamespacePath;  // The path we'll use if none is given
  String _currentNamespacePath;  // a namespace set from a #pragma
 public:
  // Provide a way for the singleton to be constructed, or a
  // pointer to be returned:
  static cimmofParser *Instance();
  // Since this class became s singleton, we need to allow someone
  // to provide us the compiler options information
  void setCompilerOptions(const mofCompilerOptions *co);
  const mofCompilerOptions *getCompilerOptions() const;
  // if the command line options say we need one, establish a repository.
  // The repository path is part of the compiler command line object
  bool setRepository(void);
  // Set a default root namespace path for the repository
  void setDefaultNamespacePath(const String &path);
  void setCurrentNamespacePath(const String &path); // current override
  // If someone wants a pointer to the Repository object, return it
  const cimmofRepository *getRepository() const;
  const String &getDefaultNamespacePath() const;
  const String &getCurrentNamespacePath() const;
  const String &getNamespacePath() const;
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

  // Do various representation transformations
  //    Octal character input to decimal character output
  char *oct_to_dec(const String &octrep) const;
  //    Hex character input to decimal character output
  char *hex_to_dec(const String &hexrep) const;
  //    Binary character input to decimal character output
  char *binary_to_dec(const String &binrep) const;

  void processPragma(const String &pragmaName, const String &pragmaString);

  int addClass(CIMClass *classdecl);
  CIMClass *newClassDecl(const String &name, const String &superclass);

  int addInstance(CIMInstance *instance);
  CIMQualifierDecl *newQualifierDecl(const String &name, const CIMValue *value,
				  Uint32 scope, Uint32 flavor);
  int addQualifier(CIMQualifierDecl *qualifier);

  CIMQualifier *newQualifier(const String &name, const CIMValue &val, Uint32 flav);
  CIMProperty *newProperty(const String &name, const CIMValue &val);
  int applyProperty(CIMClass &c, CIMProperty &p);
  CIMMethod   *newMethod(const String &name, const CIMType type);
  int applyMethod(CIMClass &c, CIMMethod &m);
  CIMParameter *newParameter(const String &name, const CIMType type);
  int applyParameter(CIMMethod &method, CIMParameter &parm);

  CIMValue *QualifierValue(const String &qualifierName, const String &valstr); 

  void addClassAlias(const String &alias, const CIMClass *cd, 
		bool isInstance);
  void addInstanceAlias(const String &alias, const CIMInstance *cd, 
		bool isInstance);
};

// Exceptions

class PEGASUS_COMPILER_LINKAGE ParseError : public Exception {
 public:
  static const char MSG[];
  ParseError() : Exception(MSG) {}
};

#endif
  


