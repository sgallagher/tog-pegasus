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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Gerarda Marquez (gmarquez@us.ibm.com)
//              -- PEP 43 changes
//              Terry Martin, Hewlett-Packard Company (terry.martin@hp.com)
//
//%////////////////////////////////////////////////////////////////////////////

#ifndef PegasusCompiler_Compiler_Decl_Context_h
#define PegasusCompiler_Compiler_Decl_Context_h

///////////////////////////////////////////////////////////////////////////////
//
// compilerDeclContext
//
//	This context is used by the resolve() methods to lookup dependent
//	objects during resolution.  This subclass takes into account
//      environments where some or all of the objects which need
//      to exist are not in the repository, but rather have been
//      explicitly provided.
//
//////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Repository/RepositoryDeclContext.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Compiler/compilerCommonDefs.h>
#include <Pegasus/Compiler/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMPILER_LINKAGE compilerDeclContext :
public RepositoryDeclContext {
 public:
  compilerDeclContext(CIMRepository *repository,
			      compilerCommonDefs::operationType ot);

  virtual ~compilerDeclContext();

  virtual
    CIMQualifierDecl lookupQualifierDecl (const CIMNamespaceName &nameSpace,
					  const CIMName &qualifierName) const;
  virtual
    CIMClass         lookupClass        (const CIMNamespaceName &nameSpace,
					 const CIMName &className) const;

  void               addQualifierDecl   (const CIMNamespaceName &nameSpace,
					 const CIMQualifierDecl& x);

  void               addClass           (const CIMNamespaceName &nameSpace,
					 CIMClass &x);

  void               addInstance        (const CIMNamespaceName &nameSpace,
					 CIMInstance &x);

  void               modifyClass           (const CIMNamespaceName &nameSpace,
					 CIMClass &x);

 private:
  CIMRepository *_cimRepository;
  compilerCommonDefs::operationType _ot;
  Array<CIMClass>         _classes;
  Array<CIMQualifierDecl> _qualifiers;
  Array<CIMInstance>      _instances;
  const CIMClass *_findClassInMemory(const CIMName &classname) const;
  const CIMQualifierDecl *_findQualifierInMemory(const CIMName &name) const;
};

PEGASUS_NAMESPACE_END

#endif // PegasusCompiler_Compiler_Decl_Context_h

