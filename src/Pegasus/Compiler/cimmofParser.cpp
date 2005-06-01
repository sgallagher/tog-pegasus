//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Modified By:  Karl Schopmeyer (k.schopmeyer@opengroup.org)
//                  1. add comment line to xml output.  aug 2001
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Gerarda Marquez (gmarquez@us.ibm.com)
//              -- PEP 43 changes
//              Seema gupta (gseema@in.ibm.com) Bug 281
//              Terry Martin, Hewlett-Packard Company (terry.martin@hp.com)
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////


//
// implementation of valueFactory
//
//
//
// Implementation of methods of cimmofParser class
//
//
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMScope.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Compiler/compilerCommonDefs.h>
#include "valueFactory.h"
#include "cimmofMessages.h"
#include "cimmofParser.h"
#include <cstring>
#include <iostream>

#define CHAR_PERIOD '.'
#define EXPERIMENTAL "Experimental"
#define VERSION "Version"

PEGASUS_USING_PEGASUS;

PEGASUS_USING_STD;


//
// These routines are in the lexer.  They are there because
// there is no need for class cimmofParser to know the details
// of YY_BUFFER_STATE and its associated methods.
//
extern int get_yy_buf_size_wrapper();
extern void *get_cimmof__current_buffer_wrapper();
extern int switch_to_buffer_wrapper(void *buffstate, Boolean closeCurrent);
extern void *create_cimmof_buffer_wrapper(const FILE *f, int size);

const char LexerError::MSG[] = "";

cimmofParser *cimmofParser::_instance = 0;

cimmofParser::cimmofParser():
  parser(),  _cmdline(0),
  _ot(compilerCommonDefs::USE_REPOSITORY) {
}

cimmofParser::~cimmofParser() {
}

cimmofParser *
cimmofParser::Instance() {
  if (!_instance) {
    _instance = new cimmofParser();
  }
  return _instance;
}


  //------------------------------------------------------------------
  // Methods for manipulating the members added in this specialization
  //------------------------------------------------------------------

//---------------------------------------------------------------------
// allow someone to set/get our compiler options object reference
//---------------------------------------------------------------------
void
cimmofParser::setCompilerOptions(const mofCompilerOptions *co) {
  _cmdline = co;
  const String path = co->get_namespacePath();
  setDefaultNamespacePath(path);
}

const mofCompilerOptions *
cimmofParser::getCompilerOptions() const {
  return _cmdline;
}

//---------------------------------------------------------------------
// Set/get the repository we will be using.  The path should be in
// the command line
//---------------------------------------------------------------------
Boolean
cimmofParser::setRepository(void) {
  String message;
  cimmofMessages::arglist arglist;
  const String &s = getDefaultNamespacePath();
  if (_cmdline) {
    String rep = _cmdline->get_repository();
    if (rep != "") {
      cimmofRepositoryInterface::_repositoryType rt;
      if (_cmdline->is_local())
        rt = cimmofRepositoryInterface::REPOSITORY_INTERFACE_LOCAL;
      else
        rt = cimmofRepositoryInterface::REPOSITORY_INTERFACE_CLIENT;
      try {
        // need to cat repo name to the dir
        String rep_name = _cmdline->get_repository_name();
        String combined = rep + "/";
        combined = combined + rep_name;

        CIMRepository_Mode Mode;
        Mode.flag = CIMRepository_Mode::NONE;

        String mode = _cmdline->get_repository_mode();
        if (String::equalNoCase(mode, "BIN") )
          Mode.flag |= CIMRepository_Mode::BIN;
        _repository.init(rt, combined, Mode,  _ot);
      } catch(Exception &e) {
        arglist.append(rep);
        arglist.append(e.getMessage());
        cimmofMessages::getMessage(message,
            cimmofMessages::REPOSITORY_CREATE_ERROR,
            arglist);
        elog(message);
        return false;
      }
      try {
        _repository.createNameSpace(s);
      }
      catch(CIMException &e) {
        if (e.getCode() == CIM_ERR_ALREADY_EXISTS) {
          // Not a problem.  Happens all the time.
        } else {
            arglist.append(e.getMessage());
            cimmofMessages::getMessage(message,
                cimmofMessages::GENERAL_ERROR,
                arglist);
            elog(message);
            return false;
        }
      }
      catch(Exception &e) {
        arglist.append(e.getMessage());
        cimmofMessages::getMessage(message,
            cimmofMessages::GENERAL_ERROR,
            arglist);
        elog(message);
        return false;
      }
    } else {
      cimmofMessages::getMessage(message,
          cimmofMessages::SETREPOSITORY_BLANK_NAME);
      elog(message);
    }
  } else {
    cimmofMessages::getMessage(message,
        cimmofMessages::SETREPOSITORY_NO_COMPILER_OPTIONS);
    elog(message);
  }
  return (_repository.ok() ? true : false);
}

const cimmofRepositoryInterface *
cimmofParser::getRepository() const {
  return &_repository;
}

//------------------------------------------------------------------
//  Set and get the operationType (defined in compilerCommonDefs)
//  which tells the parser and cimmofRepository objects how, if
//  at all, to use the CIM repository.
//------------------------------------------------------------------
void
cimmofParser::setOperationType(compilerCommonDefs::operationType ot)
{
  _ot = ot;
  if (_ot == compilerCommonDefs::USE_REPOSITORY && !_repository.ok()) {
    // ATTN: P2  throw an exception on bad commonDef. Just goes away now
  }
}

compilerCommonDefs::operationType
cimmofParser::getOperationType() const
{
  return _ot;
}

//------------------------------------------------------------------
// Set up the default and override namespace path in the repository
//------------------------------------------------------------------
void
cimmofParser::setDefaultNamespacePath(const String &path) {
  if (String::equal(_defaultNamespacePath, ""))  // it can only be set once
    _defaultNamespacePath = path;
}

void
cimmofParser::setCurrentNamespacePath(const String &path) {
  _currentNamespacePath = path;
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
  if (String::equal(_currentNamespacePath, "")) {
    return _defaultNamespacePath;
  }
  return _currentNamespacePath;
}

  //------------------------------------------------------------------
  // Methods that implement or override base class methods
  //------------------------------------------------------------------

//-------------------------------------------------------------------
// Methods for setting the parser's input buffer either from a saved
// buffer state or from an open file handle
//-------------------------------------------------------------------
int
cimmofParser::setInputBuffer(const FILE *f, Boolean closeCurrent) {
  void *buf = create_cimmof_buffer_wrapper(f, get_buffer_size());
  if (buf)
    return setInputBuffer(buf, closeCurrent);
  else
    return -1;
}

int
cimmofParser::setInputBuffer(void *buffstate, Boolean closeCurrent)
{
  return switch_to_buffer_wrapper(buffstate, closeCurrent);
};

//--------------------------------------------------------------------
// Handle include files from either the file name or an open handle
//--------------------------------------------------------------------
int
cimmofParser::enterInlineInclude(const String &filename) {
  int ret = 1;
  FILE *f = 0;

  f = fopen(filename.getCString(), "r");
  if (!f) {
    if (_cmdline) {
      const Array<String> &include_paths = _cmdline->get_include_paths();
      for (unsigned int i = 0; i < include_paths.size(); i++) {
        String s = include_paths[i] + "/" + filename;
        if ( (f = fopen(s.getCString(), "r")) ) {
          _includefile = s;
          break;
        }
      }
    } else {  // incorrect call:  cmdline should have been set
      return ret;
    }
  } else {
    _includefile = filename;
  }
  if (f) {
     ret = enterInlineInclude((const FILE *)f);
  } else {
    // ATTN:  need to throw an exception when include file not found. error only
    cerr << "Could not open include file " << filename << endl;
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
    return setInputBuffer(f, false);
  }
  return 1;
}

//--------------------------------------------------------------------
// Handle the parser telling us he's reached end-of-file
//--------------------------------------------------------------------
int
cimmofParser::wrapCurrentBuffer()
{ return wrap(); }

//--------------------------------------------------------------------
// Tell the parser to start on the buffer that's been set
//--------------------------------------------------------------------
int
cimmofParser::parse()
{
    int ret;
    if (_cmdline)
    {
        // ATTN: KS added the following 7 Aug 2001 to put header and trailer
        // lines on xml output from the parser.
        // If xml_output put the XML headers and trailers around the output
      if (_cmdline->xml_output() )
      {
          cout << "<?xml version=\"1.0\"?>" << endl;
          cout << "<!-- Open Group Pegasus CIM Compiler V "
               << PEGASUS_PRODUCT_VERSION << " Built " << __DATE__
               << " -->" << endl;
          cout << "<CIM CIMVERSION=\"2.0\" DTDVERSION=\"2.0\">" << endl;
          cout << "<DECLARATION>" << endl;
          cout << "<DECLGROUP>" << endl;
      }
    }
    ret =  cimmof_parse();
    if (_cmdline)
    {

      if (_cmdline->xml_output() )
      {
          cout << "</DECLGROUP>" << endl;
          cout << "</DECLARATION>" << endl;
          cout << "</CIM>" << endl;
      }
    }


    return ret;
}

//----------------------------------------------------------------------
// Override the default parser error routine to enable I18n
//----------------------------------------------------------------------
void
cimmofParser::log_parse_error(char *token, const char *errmsg) const {
  String message;
  char buf[40];  // itoa can't overflow
  sprintf(buf, "%d", get_lineno());
  cimmofMessages::arglist arglist;
  arglist.append(get_current_filename());
  arglist.append(buf);
  arglist.append(errmsg);
  arglist.append(token);
  cimmofMessages::getMessage(message, cimmofMessages::PARSER_SYNTAX_ERROR,
      arglist);
  elog(message);
  maybeThrowLexerError(message);
}

  //------------------------------------------------------------------
  // Do various representation transformations.
  //------------------------------------------------------------------

// -----------------------------------------------------------------
// Convert a String representing an octal integer to a String
// representing the corresponding decimal integer
// ATTN: P1 BB 2001 Need to support 64-bit integers in String transformation
// ATTN: P1 BB 2001 Needs to support non-ascii strings (UTF-8)
// ------------------------------------------------------------------
char *
cimmofParser::oct_to_dec(const String &octrep) const {
  signed long oval = 0;
  char buf[40];  // can't overrrun on an itoa of a long

//The format of octrep string is [+-]0[0-7]+
//E.g., +0345 (octal) = 228 (decimal)

  for (unsigned int i = 1; i <= octrep.size() - 1; i++) {
     oval *= 8;
     switch(octrep[i]) {
     case '1': oval += 1; break;
     case '2': oval += 2; break;
     case '3': oval += 3; break;
     case '4': oval += 4; break;
     case '5': oval += 5; break;
     case '6': oval += 6; break;
     case '7': oval += 7; break;
     }
  }

  if (octrep[0] == '-')
  {
     oval = -oval;
  }
  sprintf(buf, "%ld", oval);
  return strdup(buf);
}

// -----------------------------------------------------------------
// Convert a String representing a hexadecimal integer to a String
// representing the corresponding decimal integer
// ATTN: 2001 P1 BB Need to support 64-bit integers in string conversion
// ATTN: 2001 P1 BB  Needs to support non-ascii strings in string conversion
// ------------------------------------------------------------------
char *
cimmofParser::hex_to_dec(const String &hexrep) const {
  unsigned long hval = 0;
  char buf[40];  // can't overrrun on an itoa of a long

//The format of hexrep string is 0x[0-9A-Fa-f]+
//E.g., 0x9FFF (hex) = 40959 (decimal)

  for (unsigned int i = 2; i <= hexrep.size() - 1; i++) {
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
// Convert a String representing a binary integer to a String
// representing the corresponding decimal integer
// ATTN: P1 BB 2001 Need to support 64-bit integers in string conversion
// ------------------------------------------------------------------
char *
cimmofParser::binary_to_dec(const String &binrep) const {
  signed long bval = 0;
  char buf[40];  // can't overrrun on an itoa of a long

//The format of binrep string is [+-][01]+[Bb]
//E.g., +01011b = 11

  for (unsigned int i = 1; i <= binrep.size() - 2; i++) {
     bval *= 2;
     bval += (binrep[i] == '1' ? 1 : 0);
  }
  if (binrep[0] == '-')
  {
     bval = -bval;
  }
  sprintf(buf, "%ld", bval);
  return strdup(buf);
}

  //------------------------------------------------------------------
  // Handle the processing of CIM-specific constructs
  //------------------------------------------------------------------

//--------------------------------------------------------------------
// Take the compiler-local action specified by the #pragma (directive).
// Note that the Include #pragma is handled elsewhere.
//--------------------------------------------------------------------
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

  // ATTN: P1 BB 2001 Des not process several Pragmas
  // instancelocale, locale, namespace, nonlocal, nonlocaltype, source, etc.
  // at least.
}

//-------------------------------------------------------------------
// When a class declaration production is complete, try to add it to
// the Repository
//-------------------------------------------------------------------
int
cimmofParser::addClass(CIMClass *classdecl)
{
  int ret = 0;
  String message;
  cimmofMessages::arglist arglist;
  arglist.append(classdecl->getClassName().getString());
  if (_cmdline) {
    if (_cmdline->xml_output() )
    {
      if (classdecl)
      {
        cout << "<VALUE.OBJECT>" << endl;
        XmlWriter::printClassElement(*classdecl, PEGASUS_STD(cout));
        cout << "</VALUE.OBJECT>" << endl;
        cout << endl;
      }
      return ret;
    } else if (_cmdline->trace() ) {
      String header;
      cimmofMessages::getMessage(header, cimmofMessages::ADD_CLASS);
      trace(header,"");
      if (classdecl)
        XmlWriter::printClassElement(*classdecl, _cmdline->traceos());
    }
  }
  if (_cmdline &&
      _cmdline->operationType() != compilerCommonDefs::USE_REPOSITORY) {
    return ret;
  }
  try {
   Boolean classExist;
   cimmofMessages::MsgCode updateMessage;
   // Determine if class can be updated or added. Class is updated or
   // added based on the compiler options specified.
   if (updateClass(*classdecl, updateMessage, classExist))
   {
      if (classExist)
      {
        _repository.modifyClass(getNamespacePath(), *classdecl);
      }
      else
      {
        _repository.addClass(getNamespacePath(), *classdecl);
      }
   }
   else
   {
      if (updateMessage == cimmofMessages::NO_CLASS_UPDATE)
      {
         // This was added to maintain backward compatibility of messages seen by
         // the user.
         cimmofMessages::getMessage(message, cimmofMessages::CLASS_EXISTS_WARNING,
             arglist);
      }
      else
      {
          arglist.append(cimmofMessages::msgCodeToString(updateMessage));
          cimmofMessages::getMessage(message, cimmofMessages::CLASS_NOT_UPDATED,
              arglist);
      }
      wlog(message);
   }
  } catch(AlreadyExistsException&) {
    //ATTN: P1 2001 BB  We should be able to modify the class through the compiler
    // 04/26/2003 GM  See cimmofParser::updateClass() for info on why modify was not
    // done here
    cimmofMessages::getMessage(message, cimmofMessages::CLASS_EXISTS_WARNING,
        arglist);
    wlog(message);
  } catch (CIMException &e) {
    if (e.getCode() == CIM_ERR_ALREADY_EXISTS) {
      // 04/26/2003 GM  See cimmofParser::updateClass() for info on why modify
      // was not done here. This where cimmofl and cimmof fell into prior to
      // changes above
      cimmofMessages::getMessage(message, cimmofMessages::CLASS_EXISTS_WARNING,
          arglist);
      wlog(message);
    } else {
      arglist.append(e.getMessage());
      cimmofMessages::getMessage(message, cimmofMessages::ADD_CLASS_ERROR,
          arglist);
      elog(message);
      maybeThrowParseError(message);
    }
  } catch(Exception &e) {
    arglist.append(e.getMessage());
    cimmofMessages::getMessage(message, cimmofMessages::ADD_CLASS_ERROR,
        arglist);
    elog(message);
    maybeThrowParseError(message);
  }

  if (_cmdline && _cmdline->trace()) {
    String ok;
    cimmofMessages::getMessage(ok, cimmofMessages::TAB_OK);
    trace(ok, "");
  }
  return ret;
}

//---------------------------------------------------------------------
// When a new class declaration is detected, create the CIMClassDecl
// object
//---------------------------------------------------------------------
CIMClass *
cimmofParser::newClassDecl(const CIMName &name, const CIMName &superclassname)
{
  CIMClass *c = 0;
  try {
    c = new CIMClass(name, superclassname);
  } catch(CIMException &e) {
    cimmofMessages::arglist arglist;
    arglist.append(name.getString());
    arglist.append(e.getMessage());
    String message;
    cimmofMessages::getMessage(message, cimmofMessages::NEW_CLASS_ERROR,
        arglist);
    elog(message);
    maybeThrowParseError(message);
  }

  return c;
}

//---------------------------------------------------------------------
// When an instance production is complete, add it to the Repository
//---------------------------------------------------------------------
int
cimmofParser::addInstance(CIMInstance *instance)
{
  cimmofMessages::arglist arglist;
  String message;
  int ret = 0;
  Boolean err_out = false;
  if (_cmdline)
  {
    if (_cmdline->xml_output())
    {
      if (instance)
      {
        cout << "<VALUE.OBJECT>" << endl;
        XmlWriter::printInstanceElement(*instance, PEGASUS_STD(cout));
        cout << "</VALUE.OBJECT>" << endl;
        cout << endl;
      }
      return ret;
    }
    else if (_cmdline->trace())
    {
      String header;
      cimmofMessages::getMessage(header, cimmofMessages::ADD_INSTANCE);
      trace(header, "");
      if (instance)
        XmlWriter::printInstanceElement(*instance, _cmdline->traceos());
    }
  }
  if (_cmdline &&
      _cmdline->operationType() != compilerCommonDefs::USE_REPOSITORY) {
    return ret;
  }
  try {
    _repository.addInstance(getNamespacePath(), *instance);
  } catch (CIMException &e) {
    arglist.append(e.getMessage());
    if (e.getCode() == CIM_ERR_ALREADY_EXISTS) {
      // ATTN: P1 BB 2001  We should be able to modify the instance through the compiler
      cimmofMessages::getMessage(message,
          cimmofMessages::INSTANCE_EXISTS_WARNING,
          arglist);
      wlog(message);
    } else {
      err_out = true;
    }
  } catch (Exception &e) {
    arglist.append(e.getMessage());
    err_out = true;
  }
  if (err_out) {
    cimmofMessages::getMessage(message,
        cimmofMessages::ADD_INSTANCE_ERROR,
        arglist);
    elog(message);
    maybeThrowParseError(message);
  }

  if (_cmdline && _cmdline->trace()) {
    String ok;
    cimmofMessages::getMessage(ok, cimmofMessages::TAB_OK);
    trace(ok, "");
  }
  return ret;
}

//---------------------------------------------------------------------
// When the start of a Qualifier Declaration is found, create the new
// CIMQualifierDecl object
//---------------------------------------------------------------------
CIMQualifierDecl *
cimmofParser::newQualifierDecl(const String &name, const CIMValue *value,
    const CIMScope & scope, const CIMFlavor & flavor) {

  CIMQualifierDecl *q = 0;
  try {
    q = new CIMQualifierDecl(name, *value, scope, flavor);
  } catch(Exception &e) {
    cimmofMessages::arglist arglist;
    arglist.append(name);
    arglist.append(e.getMessage());
    String message;
    cimmofMessages::getMessage(message,
        cimmofMessages::NEW_QUALIFIER_DECLARATION_ERROR,
        arglist);
    elog(message);
    maybeThrowParseError(message);
  }

  return q;
}

//---------------------------------------------------------------------
// When a QualifierDeclaration production is complete, add the qualifier
// to the Repository.
//---------------------------------------------------------------------
int
cimmofParser::addQualifier(CIMQualifierDecl *qualifier)
{
  int ret  = 0;
  cimmofMessages::arglist arglist;
  if (qualifier)
    arglist.append(qualifier->getName().getString());
  String message;
  if (_cmdline) {
    if (_cmdline->xml_output())
    {
      if (qualifier)
      {
        cout << "<VALUE.OBJECT>" << endl;
        XmlWriter::printQualifierDeclElement(*qualifier, PEGASUS_STD(cout));
        cout << "</VALUE.OBJECT>" << endl;
        cout << endl;
      }
      return ret;
    }
    else if (_cmdline->trace())
    {
      String header;
      cimmofMessages::getMessage(header, cimmofMessages::ADD_QUALIFIER);
      trace(header, "");
      if (qualifier)
        XmlWriter::printQualifierDeclElement(*qualifier, _cmdline->traceos());
    }
  }

  if (_cmdline &&
      _cmdline->operationType() != compilerCommonDefs::USE_REPOSITORY) {
    return ret;
  }
  try {
    _repository.addQualifier(getNamespacePath(), *qualifier);
  } catch(CIMException& e) {
    if (e.getCode() == CIM_ERR_NOT_SUPPORTED) {
      // OK, just skip it for now.
      // In a later implementation we will overwrite if the compiler
      // switches say to do so.
    } else {
      arglist.append(e.getMessage());
      cimmofMessages::getMessage(message, cimmofMessages::ADD_QUALIFIER_ERROR,
          arglist);
      elog(message);
      maybeThrowParseError(message);
    }
  } catch(Exception& e) {
    // ATTN:2001 P1 BB  at the time of writing, the Common code does not throw
    // an CIM_ERR_ALREADY_EXISTS CIMException.  It might at any time.
    arglist.append(e.getMessage());
    cimmofMessages::getMessage(message, cimmofMessages::ADD_QUALIFIER_ERROR,
        arglist);
    elog(message);
    maybeThrowParseError(message);
  }
  if (_cmdline && _cmdline->trace()) {
    String ok;
    cimmofMessages::getMessage(ok, cimmofMessages::TAB_OK);
    trace(ok, "");
  }
  return ret;
}

//--------------------------------------------------------------------
// When a qualifier itself (not its declaration) is detected,
// create the CIMQualifier object.
//--------------------------------------------------------------------
CIMQualifier *
cimmofParser::newQualifier(const String &name, const CIMValue &value,
    const CIMFlavor & flavor)
{
  CIMQualifier *q = 0;
  try {
    q = new CIMQualifier(name, value, flavor);
  } catch(Exception &e) {
    cimmofMessages::arglist arglist;
    arglist.append(name);
    arglist.append(e.getMessage());
    String message;
    cimmofMessages::getMessage(message, cimmofMessages::NEW_QUALIFIER_ERROR,
        arglist);
    elog(message);
    maybeThrowParseError(message);
  }

  return q;
}

//----------------------------------------------------------------------
// When a new instance declaration heading is detected, create the
// backing instance object of that class.  We may add it later, or
// use it to modify an existing instance
//----------------------------------------------------------------------
CIMInstance *
cimmofParser::newInstance(const CIMName &className)
{
  CIMInstance *instance = 0;
  try {
    instance = new CIMInstance(className);
  } catch (Exception &e) {
    cimmofMessages::arglist arglist;
    arglist.append(className.getString());
    arglist.append(e.getMessage());
    String message;
    cimmofMessages::getMessage(message, cimmofMessages::NEW_INSTANCE_ERROR,
        arglist);
    elog(message);
    maybeThrowParseError(message);
  }
  return instance;
}

//----------------------------------------------------------------------
// When a property of a class being declared is discovered, creat the
// new CIMProperty object.
//----------------------------------------------------------------------

// KS 8 Mar 2002 - Added is array and arraySize to parameters
CIMProperty *
cimmofParser::newProperty(const CIMName &name, const CIMValue &val,
    const Boolean isArray,
    const Uint32 arraySize,
    const CIMName &referencedObject) const
{
  CIMProperty *p = 0;

  try {
     //ATTNKS: P1 Note that we are not passing isArray
    p = new CIMProperty(name, val, arraySize, referencedObject);
  } catch(Exception &e) {
    cimmofMessages::arglist arglist;
    arglist.append(name.getString());
    arglist.append(e.getMessage());
    String message;
    cimmofMessages::getMessage(message, cimmofMessages::NEW_PROPERTY_ERROR,
        arglist);
    elog(message);
    maybeThrowParseError(message);
  }
  return p;
}

//-----------------------------------------------------------------------
// When a property production is complete, apply it to the
// class being declared.
//-----------------------------------------------------------------------
int
cimmofParser::applyProperty(CIMClass &c, CIMProperty &p)
{
  cimmofMessages::arglist arglist;
  arglist.append(c.getClassName().getString());
  arglist.append(p.getName().getString());
  String message;
  try {
    c.addProperty(p);
  } catch(UninitializedObjectException&) {
    cimmofMessages::getMessage(message,
        cimmofMessages::UNINITIALIZED_PROPERTY_ERROR,
        arglist);
    elog(message);
    maybeThrowParseError(message);
  } catch(AlreadyExistsException&) {
    cimmofMessages::getMessage(message,
        cimmofMessages::PROPERTY_ALREADY_EXISTS_WARNING,
        arglist);
    wlog(message);
  } catch(Exception &e) {
    arglist.append(e.getMessage());
    cimmofMessages::getMessage(message,
        cimmofMessages::APPLYING_PROPERTY_ERROR,
        arglist);
    elog(message);
    maybeThrowParseError(message);
  }

  return 0;
}

//-----------------------------------------------------------------------
// When a property production is complete, apply it to the
// instance being declared.
//-----------------------------------------------------------------------
int
cimmofParser::applyProperty(CIMInstance &i, CIMProperty &p)
{
  cimmofMessages::arglist arglist;
  const CIMName &propertyName = p.getName();
  arglist.append(i.getClassName().getString());
  arglist.append(propertyName.getString());
  String message;
  Boolean err_out = false;
  try {
    Uint32 pos = i.findProperty(propertyName);
    if (pos == (Uint32)-1) {
      i.addProperty(p);   // Add the property
    } else {
      // ATTN: 2001 There doesn't seem to be a way to change a property value
      // yet.
    }
  } catch (CIMException &e) {
    if (e.getCode() == CIM_ERR_ALREADY_EXISTS) {
      cimmofMessages::getMessage(message,
          cimmofMessages::INSTANCE_PROPERTY_EXISTS_WARNING,
          arglist);
      wlog(message);
    } else {
      arglist.append(e.getMessage());
      err_out = true;
    }
  } catch (Exception &e) {
    arglist.append(e.getMessage());
    err_out = true;
  }
  if (err_out) {
    cimmofMessages::getMessage(message,
        cimmofMessages::APPLY_INSTANCE_PROPERTY_ERROR,
        arglist);
    elog(message);
    maybeThrowParseError(message);
  }
  return 0;
}

//----------------------------------------------------------------------
// When a property object's value changes, create a clone of the
// property object with a new value
//----------------------------------------------------------------------
CIMProperty *
cimmofParser::copyPropertyWithNewValue(const CIMProperty &p,
    const CIMValue &v) const
{
  cimmofMessages::arglist arglist;
  String message;
  CIMProperty *newprop = 0;
  arglist.append(p.getName().getString());
  try {
    newprop = new CIMProperty(p);
    newprop->setValue(v);
  } catch (Exception &e) {
    arglist.append(e.getMessage());
    cimmofMessages::getMessage(message,
        cimmofMessages::CLONING_PROPERTY_ERROR,
        arglist);
    elog(message);
    maybeThrowParseError(message);
  }
  return newprop;
}

//----------------------------------------------------------------------
// When a method of a class being declared is discovered, create the
// new CIMMethod object.
//----------------------------------------------------------------------
CIMMethod *
cimmofParser::newMethod(const CIMName &name, const CIMType type)
{
  CIMMethod *m = 0;
  try {
    m = new CIMMethod(name, type);
  } catch(Exception &e) {
    cimmofMessages::arglist arglist;
    String message;
    arglist.append(name.getString());
    arglist.append(e.getMessage());
    cimmofMessages::getMessage(message, cimmofMessages::NEW_METHOD_ERROR,
        arglist);
    elog(message);
    maybeThrowParseError(message);
  }
  return m;
}

//-----------------------------------------------------------------------
// When a method production is complete, apply it to the
// class being declared.
//-----------------------------------------------------------------------
int
cimmofParser::applyMethod(CIMClass &c, CIMMethod &m) {
  cimmofMessages::arglist arglist;
  String message;
  arglist.append(m.getName().getString());
  arglist.append(c.getClassName().getString());
  try {
    c.addMethod(m);
  } catch(UninitializedObjectException&) {
    cimmofMessages::getMessage(message,
        cimmofMessages::UNINITIALIZED_PARAMETER_ERROR,
        arglist);
    elog(message);
    maybeThrowParseError(message);
  } catch(AlreadyExistsException&) {
    cimmofMessages::getMessage(message,
        cimmofMessages::METHOD_ALREADY_EXISTS_WARNING,
        arglist);
    wlog(message);
  } catch(Exception &e) {
    arglist.append(e.getMessage());
    cimmofMessages::getMessage(message,
        cimmofMessages::APPLY_METHOD_ERROR,
        arglist);
    elog(message);
    maybeThrowParseError(message);
  }

  return 0;
}

CIMParameter *
cimmofParser::newParameter(const CIMName &name, const CIMType type,
    Boolean isArray, Uint32 array, const CIMName &objName)
{
  CIMParameter *p = 0;
  try {
    p = new CIMParameter(name, type, isArray, array, objName);
  } catch(Exception &e) {
    cimmofMessages::arglist arglist;
    arglist.append(name.getString());
    arglist.append(e.getMessage());
    String message;
    cimmofMessages::getMessage(message, cimmofMessages::NEW_PARAMETER_ERROR,
        arglist);
    elog(message);
    maybeThrowParseError(message);
  }
  return p;
}

int
cimmofParser::applyParameter(CIMMethod &m, CIMParameter &p) {
  try {
    m.addParameter(p);
  } catch(CIMException &e) {
    cimmofMessages::arglist arglist;
    String message;
    arglist.append(p.getName().getString());
    arglist.append(m.getName().getString());
    arglist.append(e.getMessage());
    cimmofMessages::getMessage(message, cimmofMessages::APPLY_PARAMETER_ERROR,
        arglist);
    elog(message);
    maybeThrowParseError(message);
  }
  return 0;
}



CIMValue *
cimmofParser::QualifierValue(const CIMName &qualifierName,
                             Boolean isNull,
                             const String &valstr)
{
  CIMQualifierDecl q;
  try {
    q = _repository.getQualifierDecl(getNamespacePath(), qualifierName);
  }
  catch (CIMException &e) {
    cimmofMessages::arglist arglist;
    String message;
    arglist.append(qualifierName.getString());
    arglist.append(e.getMessage());
    cimmofMessages::getMessage(message,
        cimmofMessages::GET_QUALIFIER_DECL_ERROR,
        arglist);
    elog(message);
    maybeThrowParseError(message);
  }

  CIMValue v = q.getValue();
  Uint32 asize = v.getArraySize();

  if (isNull && (v.getType() == CIMTYPE_BOOLEAN))
  {
    // From CIM Specification version 2.2 section 4.5.4:
    //   If only the qualifier name is listed for a boolean qualifier,
    //   it is implicitly set to TRUE.
    return new CIMValue(Boolean(true));
  }
  return valueFactory::createValue(v.getType(),
      v.isArray() ? (int)asize : -1,
      isNull,
      &valstr);
}

CIMProperty *
cimmofParser::PropertyFromInstance(CIMInstance &instance,
    const CIMName &propertyName) const
{
  cimmofMessages::arglist arglist;
  arglist.append(propertyName.getString());
  CIMName className;
  String message;
  try {
    Uint32 pos = instance.findProperty(propertyName);
    if (pos != (Uint32)-1) {
      //ATTN: P2 2001 There doesn't seem to be a way to get a copy of an
      // instance's properties (or to change them if you got one)
      CIMProperty oldp = instance.getProperty(pos);
      //CIMProperty *p = new CIMProperty(oldp);
      //return p;
    }
  } catch (Exception &e) {
    arglist.append(e.getMessage());
    cimmofMessages::getMessage(message,
        cimmofMessages::GET_INSTANCE_PROPERTY_ERROR,
        arglist);
    elog(message);
    maybeThrowParseError(message);
  }
  // That didn't work.  Try getting the property from the class decl
  try {
    className = instance.getClassName();
  } catch (Exception &e) {
    arglist.append(className.getString());
    arglist.append(e.getMessage());
    cimmofMessages::getMessage(message,
        cimmofMessages::FIND_CLASS_OF_INSTANCE_ERROR,
        arglist);
    elog(message);
    maybeThrowParseError(message);
  }
  // OK, we got the className.  Use it to find the class object.
  try {
    Array<String> propertyList;
    propertyList.append(propertyName.getString());
    CIMClass c = _repository.getClass(getNamespacePath(), className);
    Uint32 pos = c.findProperty(propertyName);
    if (pos != (Uint32)-1) {
      return new CIMProperty(c.getProperty(pos));
    }
  } catch (Exception &e) {
    arglist.append(className.getString());
    arglist.append(getNamespacePath());
    arglist.append(e.getMessage());
    cimmofMessages::getMessage(message,
        cimmofMessages::GET_CLASS_ERROR,
        arglist);
    elog(message);
    maybeThrowParseError(message);
  }
  const CIMValue defaultValue(false);
  // KS 8 Mar add the values for isArray and arraysize (defaults)
  CIMProperty *p = newProperty(propertyName, defaultValue, false, 0);
  return p;
}

CIMValue *
cimmofParser::ValueFromProperty(const CIMProperty &prop) const
{
  CIMName propname;
  try {
    propname = prop.getName();
    const CIMValue &v = prop.getValue();
    return new CIMValue(v);
  } catch (Exception &e) {
    cimmofMessages::arglist arglist;
    String message;
    arglist.append(propname.getString());
    arglist.append(e.getMessage());
    cimmofMessages::getMessage(message,
        cimmofMessages::GET_PROPERTY_VALUE_ERROR,
        arglist);
    elog(message);
    maybeThrowParseError(message);
  }
  return new CIMValue();
}


CIMValue *
cimmofParser::PropertyValueFromInstance(CIMInstance &instance,
    const CIMName &propertyName)const
{
  CIMProperty *prop = PropertyFromInstance(instance, propertyName);
  CIMValue *value = ValueFromProperty(*prop);
  delete prop;
  return value;
}

CIMObjectPath *
cimmofParser::newReference(const objectName &oname)
{
  String nameSpaceString = oname.handle();
  CIMNamespaceName nameSpace;
  if (nameSpaceString != String::EMPTY)
  {
    nameSpace = nameSpaceString;
  }

  CIMObjectPath *ref = 0;
  try {
    ref = new CIMObjectPath(oname.host(), nameSpace, oname.className(),
        oname.KeyBindings());
  } catch(Exception &e) {
    cimmofMessages::arglist arglist;
    arglist.append(oname.className());
    arglist.append(e.getMessage());
    String message;
    cimmofMessages::getMessage(message, cimmofMessages::NEW_REFERENCE_ERROR,
        arglist);
    elog(message);
    maybeThrowParseError(message);
  }
  return ref;
}

void
cimmofParser::addClassAlias(const String &alias, const CIMClass *cd,
    Boolean isInstance)
{
  // ATTN: P3 BB 2001 ClassAlias - to be deprecated.  Simply warning here
}

void
cimmofParser::addInstanceAlias(const String &alias, const CIMInstance *cd,
    Boolean isInstance)
{
  // ATTN:P3 BB 2001  As soon as we figure out what Aliases are for, do something
}



    //-------------------------------------------------------------------
    //  Class Private methods
    //-------------------------------------------------------------------
//--------------------------------------------------------------------
// Error logging
//--------------------------------------------------------------------
void
cimmofParser::elog(const String &msg) const
{
  if (_cmdline)
    if (!_cmdline->suppress_all_messages() && !_cmdline->suppress_warnings())
      _cmdline->erroros() << msg << endl;
}

//----------------------------------------------------------------------
// Warning logging
//----------------------------------------------------------------------
void
cimmofParser::wlog(const String &msg) const
{
  if (_cmdline)
    if (!_cmdline->suppress_all_messages())
      if (!_cmdline->suppress_warnings())
        _cmdline->warningos() << msg << endl;
}

//-------------------------------------------------------------------
// Tracing
//-------------------------------------------------------------------
void
cimmofParser::trace(const String &head, const String &tail) const
{
  if (_cmdline)
    _cmdline->traceos() << head << " " << tail << endl;
}

//--------------------------------------------------------------------
// Exception Control
//--------------------------------------------------------------------
void
cimmofParser::maybeThrowParseError(const String &msg) const
{
  // unless the continue-processing flag is on
  throw ParseError(msg);
}

void
cimmofParser::maybeThrowLexerError(const String &msg) const
{
  // ATTN: P1 BB 2001-unless we may want to continue (and that's not possible with the
  // lexer written as it is now),
  throw LexerError(msg);
}

//--------------------------------------------------------------------
// Update class
//--------------------------------------------------------------------
Boolean
cimmofParser::updateClass(const CIMClass &classdecl,
                          cimmofMessages::MsgCode &updateMessage,
                          Boolean &classExist)
{
  classExist = true;

  Boolean ret = true;
  Boolean iExperimental = false;
  Boolean rExperimental = false;
  String iVersion;  /* format of version is m.n.u */
  int iM = -1;   /* Major id  */
  int iN = -1;   /* Minor id  */
  int iU = -1;   /* Update id */
  String rVersion;  /* format of version is m.n.u */
  int rM = -1;   /* Major id  */
  int rN = -1;   /* Minor id  */
  int rU = -1;   /* Update id */

  Sint32 idx;
  CIMClass cRep;


  // This function was created to implement PEP #43. It allows updates
  // to the repository.  PEP #43 only allows updates to leaf classes.
  // Superclasses and classes that have subclasses cannot be updated.
  // Please reference PEP #43 for more information.

  // Note: Updating the class was not done when an "class already exists
  // exception" in cimmofParser::addClass() occurs because when it gets
  // to the catch the class has been fully populated (it has inherited
  // all the properties and qualifiers from the superclass).  This means
  // that the Version and Experimental qualifers were propagated to child
  // class.  This made checking whether a version or experimental change
  // would occur difficult. This class, cimmofParser::updateClass(), will
  // get the class, if it exists, from the repository and save off the
  // version and experimental qualifiers so that it can be compared with
  // qualifiers of the class in the mof files.  It it finds the class and
  // the class cannot be updated, cimmofParser::addClass() will be notified
  // that the class exists and send the same message that it has sent
  // in the past.  In cases when the class cannot be modified due to that
  // the allow experimental and allow version options have not be specified
  // then an appropriate message will also be sent to cimmofParser::addClass().

  // Note on Experimental qualifier:
  // With the implementation of PEP #43 Experimental classes cannot be
  // added to the repository unless the -aE option is specified in the
  // cimmof/cimmofl CLIs.

  // Note on Version qualifier:
  // Classes that cause a Major update (the m in m.n.u is changed) will
  // require that the -aV option be specified.  Classes that cause a
  // Down Revision will also require that the -aV option be specified.
  // If the version of the class in the mof file has the same version
  // as the class in the repository the class will not be updated.
  // Classes with the same version are considered the same.
  // The version specified in the Version qualifier will be checked for
  // a valid format.  A valid version format is m.n.u (m is major, n is minor,
  // and u is update).  Examples of valid version are 2, 2.7, 2.7.0

  // Get the class from the repository
  try
  {
    cRep = _repository.getClass(getNamespacePath(), classdecl.getClassName());
  }
  catch (const CIMException &e)
  {
    if (e.getCode() == CIM_ERR_NOT_FOUND)
    {
        classExist = false;
    }
    else
    if (e.getCode() == CIM_ERR_INVALID_CLASS)
    {
        /* Note:  this is where cimmofl and cimmof fall into */
        classExist = false;
    }
    else
    {
        throw;
    }
  }

  // Get the experimental qualifier from the input class
  idx = classdecl.findQualifier(EXPERIMENTAL);
  if (idx >= 0)
  {
      CIMConstQualifier iExp = classdecl.getQualifier(idx);
      CIMValue iExpVal = iExp.getValue();
      iExpVal.get(iExperimental);
  }

  // Get the version qualifier from the input class
  idx = classdecl.findQualifier(VERSION);
  // A version was found for the input class
  if (idx >= 0)
  {
      CIMConstQualifier iVer = classdecl.getQualifier(idx);
      CIMValue iVerVal = iVer.getValue();
      iVerVal.get(iVersion);
  }

  // Get experimental and version qualifiers from the repository class
  if (classExist)
  {
      // Get the experimental qualifier from the repository class
      idx = cRep.findQualifier(EXPERIMENTAL);
      if (idx >= 0)
      {
          CIMQualifier rExp = cRep.getQualifier(idx);
          CIMValue rExpVal = rExp.getValue();
          rExpVal.get(rExperimental);
      }

      // Get the version qualifier from the repository class
      idx = cRep.findQualifier(VERSION);
      if (idx >= 0)
      {
          CIMQualifier rVer = cRep.getQualifier(idx);
          CIMValue rVal = rVer.getValue();
          rVal.get(rVersion);
      }
  }

  // Verify version format specified in the Version qualifier of mof class
  if (!verifyVersion(iVersion, iM, iN, iU))
  {
       updateMessage = cimmofMessages::INVALID_VERSION_FORMAT;
       return false;
  }

  // Verify version format specified in the Version qualifier of repository class
  if (!verifyVersion(rVersion, rM, rN, rU))
  {
       updateMessage = cimmofMessages::INVALID_VERSION_FORMAT;
       return false;
  }

  //
  // The following code was modeled after the algorithm in PEP 43.
  //

  if (!classExist)
  {
      // Will create an experimental class in the repository
      if (iExperimental)
      {
          if (!_cmdline->allow_experimental())
          {
              /* PEP43: ID = 1 */
              //printf("ID=1 (NoAction): Does Not Exist. -aE not set.\n");
              updateMessage = cimmofMessages::NO_EXPERIMENTAL_UPDATE;
              return false;
          }
          else
          {
              /* PEP43: ID = 2 */
              //printf("ID=2 (CreateClass): Does Not Exist. -aE set.\n");
          }
      }
      else
      {
          /* PEP43: ID = 3 */
          //printf("ID=3 (CreateClass): Does Not Exist. Not Experimental.\n");
      }
  }
  else
  {
      if (!_cmdline->update_class())
      {
          /* PEP43: ID = 4 */
          //printf("ID=4 (NoAction): Exists. -uC not set.\n");
          updateMessage = cimmofMessages::NO_CLASS_UPDATE;
          return false;
      }

      // Will create an experimental class in the repository
      if (!rExperimental && iExperimental)   /* FALSE->TRUE */
      {
          if (!_cmdline->allow_experimental())
          {
              /* PEP43: ID = 5 */
              //printf("ID=5 (NoAction): Exists. -aE not set.\n");
              updateMessage = cimmofMessages::NO_EXPERIMENTAL_UPDATE;
              return false;
          }

          // Some examples:
          // Requires minor and update ids in repository and mof to be set with the
          // exception of the major id
          // 2.7.0->NULL (ex. repository has 2.7.0 and mof has no version)
          // 2.7.0->3.x.x or 2.7.0->1.x.x (Major Update)
          // 2.7.0->2.6.x                 (Down Revision of minor id)
          // 2.7.1->2.7.0                 (Down Revision of update id)
          if ((rM >= 0  &&  iM < 0) ||                     /* remove version (Version->NULL) */
              (iM != rM  &&  rM >= 0) ||                   /* Major Update (up or down)      */
              (iN < rN  &&  rN >= 0 && iN >= 0) ||         /* Down Revision of minor id      */
              (iU < rU  &&  rU >= 0 && iU >= 0 && rN==iU)) /* Down Revision of update id     */

          {
              if (!_cmdline->allow_version())
              {
                  /* PEP43: ID = 6 */
                  //printf("ID=6 (No Action): Exists. -aV not set.\n");
                  updateMessage = cimmofMessages::NO_VERSION_UPDATE;
                  return false;
              }
              else
              {
                  /* ID = 7 */
                  //printf("ID=7: (ModifyClass): Exists. -aEV set. (Major Update, Down Revision, Version->NULL)\n");

              }
          }
          else
          {
             // Some examples:
             // NULL->x.x.x (ex. repository has no version and mof has x.x.x)
             // 2.7.0->2.8.x   (minor update of minor id )
             // 2.7.0->2.7.1   (minor update of update id )
             // 2.7.9->2.8.0   (minor update of both minor and update id)
             if ((rM < 0)  ||              /* add a version (NULL->Any) */
                 (iN > rN && rN >= 0) ||   /* minor update of minor id  */
                 (iU > rU && rU >= 0))     /* minor update of update id */
             {
                 /* PEP43: ID = 8 */
                 //printf("ID=8 (ModifyClass): Exists. -aE set. (Minor Update, NULL->Any)\n");

             }
             else
             {
                 // Some examples:
                 // 2.7.0->2.7.0 (ex. repository has 2.7.0 and mof has 2.7.0)
                 // 2    ->2.0.0 or 2.0.0->2   (equates to same version)
                 // 2.7  ->2.7.0 or 2.7.0->2.7 (equates to same version)
                 /* PEP43: ID = 9 --> Same Version */
                 //printf("ID=9 (NoAction): Exists. Same Version.\n");
                 updateMessage = cimmofMessages::SAME_VERSION;
                 return false;

             }
          }
      }
      else
      {
          // Will not create an experimental class in the repository or
          // class in the repository is already experimental
          if ((rExperimental && iExperimental) ||        /* TRUE->TRUE */
              (!iExperimental))                          /* Any->FALSE */
          {
              // See above for examples...ID=6
              if ((rM >= 0  &&  iM < 0) ||                     /* remove version (Version->NULL) */
                  (iM != rM  &&  rM >= 0) ||                   /* Major Update (up or down)      */
                  (iN < rN  &&  rN >= 0 && iN >= 0) ||         /* Down Revision of minor id      */
                  (iU < rU  &&  rU >= 0 && iU >= 0 && rN==iU)) /* Down Revision of update id     */
              {
                  if (!_cmdline->allow_version())
                  {
                      /* PEP43: ID = 10 */
                      //printf("ID=10 (NoAction): Exists. -aV not set.\n");
                      updateMessage = cimmofMessages::NO_VERSION_UPDATE;
                      return false;
                  }
                  else
                  {
                      /* ID = 11 */
                      //printf("ID=11 (ModifyClass): Exists. -aV set. (Major Update, Down Revision, Version->NULL)\n");

                  }
              }
              else
              {
                 // See above for examples...ID=8
                 if ((rM < 0)  ||              /* add a version (NULL->Any) */
                     (iN > rN && rN >= 0) ||   /* minor update of minor id  */
                     (iU > rU && rU >= 0))     /* minor update of update id */
                 {
                     /* PEP43: ID = 12 */
                     //printf("ID=12 (ModifyClass): Exists: (Minor Update, NULL->Any)\n");

                 }
                 else
                 {
                     /* PEP43: ID = 13 --> Same Version */
                     // See above for examples...ID=9
                     //printf("ID=13 (NoAction): Exists. Same Version.\n");
                     updateMessage = cimmofMessages::SAME_VERSION;
                     return false;

                 }
              }
          }
      }
  }

  return ret;
}

Boolean
cimmofParser::verifyVersion(const String &version, int &iM, int &iN, int &iU)
{
  Boolean ret = true;

  int pos  = -1;
  int posM = -1;   /* Major  */
  int posN = -1;   /* Minor  */
  int posU = -1;   /* Update */

  // Parse the input version
  if (version.size())
  {
      // If "V" specified as first character go ahead and ignore
      if ((version[0] >= '0') && (version[0] <= '9'))
      {
          pos = 0;
          posM = version.find(0, CHAR_PERIOD);
      }
      else
      {
          pos = 1;
          if (String::equalNoCase(version.subString(0,1), "V"))
          {
              posM = version.find(1, CHAR_PERIOD);
          }
          else
          {
              // First character is unknown.
              // Examples of invalid version:  ".2.7", ".", "..", "...", "AB"
              return false;
          }
      }

      // Find the second and possible third period
      if (posM >=0)
      {
          posN = version.find(posM+1, CHAR_PERIOD);
      }
      if (posN >= 0)
      {
          posU = version.find(posN+1, CHAR_PERIOD);
      }

      // There should be no additional identifiers after the update identifier
      if (posU >= 0)
      {
          // Examples of invalid version:  "2.7.0.", "2.7.0.1", "2.7.0.a", "2.7.0.1."
          return false;
      }

      // Check for trailing period
      if ((posM >=0 && posM+1 == (int)version.size()) ||
          (posN >=0 && posN+1 == (int)version.size()) ||
          (posU >=0 && posU+1 == (int)version.size()))
      {
          // Examples of invalid version:  "2.", "2.7.", "V.", "9.."
          return false;
      }

      // Major identifier is not specified
      if (((pos > 0) && (posM < 0) && (!version.subString(pos).size())) ||
          ((pos > 0) && (posM >= 0) && (posM <= pos)))
      {
          // Examples of invalid version: "V.2.7", "V"
          return false;
      }

      // Check Major identifier for invalid format
      int endM = posM;
      if (posM < 0)
      {
         endM = version.size();
      }
      for (int i = pos; i < endM; i++)
      {
           if (!((version[i] >= '0') && (version[i] <= '9')))
           {
               // Example of invalid version:  "1B.2D.3F"
               return false;
           }
      }

      // Minor identifier is not specified
      if (posM+1 == posN)
      {
          // Example of invalid version:  "2..9"
          return false;
      }

      // Check Minor identifier for invalid format
      if (posM > 0)
      {
          int endN = posN;
          if (posN < 0)
          {
              endN = version.size();
          }
          for (int i = posM+1; i < endN; i++)
          {
               if (!((version[i] >= '0') && (version[i] <= '9')))
               {
                   // Example of invalid version:  "99.CD", "11.2D.3F"
                   return false;
               }
          }
      }

      // Check Update identifier for invalid format
      if (posN > 0)
      {
          for (int i = posN+1; i < (int)version.size(); i++)
          {
               if (!((version[i] >= '0') && (version[i] <= '9')))
               {
                    // Example of invalid version: "99.88.EF", "11.22.3F"
                    return false;
               }
          }
      }

      // Set major, minor, and update values as integers
      if (posM >= 0)
      {
          iM = atoi((const char*)(version.subString(pos, posM).getCString()));
          if (posN >= 0)
          {
              iN = atoi((const char*)(version.subString(posM+1, posN).getCString()));
              iU = atoi((const char*)(version.subString(posN+1).getCString()));
          }
          else
          {
              iN = atoi((const char*)(version.subString(posM+1).getCString()));
          }
      }
      else
      {
          iM = atoi((const char*)(version.subString(pos).getCString()));
      }
  }

  return ret;
}

