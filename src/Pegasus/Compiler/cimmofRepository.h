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
// interface definition for the cimmofRepository class, a specialiazation
// of the Pegasus CIMRepository class with error handling.
//

#ifndef _CIMMOFREPOSITORY_H_
#define _CIMMOFREPOSITORY_H_


#include <Pegasus/Common/Config.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Common/String.h>
//#include <Pegasus/Compiler/cimmofParser.h>
#include <Pegasus/Compiler/compilerDeclContext.h>

PEGASUS_NAMESPACE_BEGIN


class PEGASUS_COMPILER_LINKAGE cimmofRepository  {
 public:
  cimmofRepository(const String &path, compilerCommonDefs::operationType ot);
  virtual ~cimmofRepository();

  // Add some methods for use at the compiler level
  virtual int addClass(const String &nameSpace, CIMClass *classdecl);
  virtual int addInstance(const String &nameSpace, CIMInstance *instance);
  virtual int addQualifier(const String &nameSpace,
			   CIMQualifierDecl *qualifier);

  virtual CIMQualifierDecl getQualifierDecl(const String &nameSpace,
					    const String &name);
  virtual CIMClass getClass(const String &nameSpace, const String &classname);
  virtual void createNameSpace(const String &nameSpaceName);
 private:
  CIMRepository *_cimrepository;
  compilerDeclContext *_context;
  compilerCommonDefs::operationType _ot;
};

PEGASUS_NAMESPACE_END

#endif
