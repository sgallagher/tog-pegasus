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
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Gerarda Marquez (gmarquez@us.ibm.com)
//              -- PEP 43 changes
//
//%/////////////////////////////////////////////////////////////////////////////


//
// implementation of  cimmofRepository
//
//
//
// This class acts as a buffer between the compiler and the repository
// interface.  The main thing it does is registers a non-standard
// DeclContext so we can do local checking of context for new objects
//

#include "cimmofRepository.h"

PEGASUS_USING_PEGASUS;

cimmofRepository::cimmofRepository(const String &path, const CIMRepository_Mode Mode,  compilerCommonDefs::operationType ot) :
  _cimrepository(0), _context(0), _ot(ot)
{
  // don't catch the exceptions that might be thrown.  They should go up.
  if (_ot != compilerCommonDefs::IGNORE_REPOSITORY) {
    _cimrepository = new CIMRepository(path, Mode );
  }
  _context = new compilerDeclContext(_cimrepository, _ot);
  if (_cimrepository)
    _cimrepository->setDeclContext(_context);
}

cimmofRepository::~cimmofRepository() {
  if (_cimrepository)
    delete(_cimrepository);
  if (_context)
    delete(_context);
}

int 
cimmofRepository::addClass(const CIMNamespaceName &nameSpace, 
                           CIMClass *classdecl)
{
  // Don't catch errors: pass them up to the requester
  _context->addClass( nameSpace,  *classdecl);
  return 0;
}


int 
cimmofRepository::addInstance(const CIMNamespaceName &nameSpace, 
                              CIMInstance *instance)
{ 
  // Don't catch errors: pass them up to the requester
  _context->addInstance(nameSpace, *instance);
  return 0;
}

int 
cimmofRepository::addQualifier(const CIMNamespaceName &nameSpace,
			       CIMQualifierDecl *qualifier)
{ 
  // Don't catch errors: pass them up to the requester
  _context->addQualifierDecl(nameSpace, *qualifier);
  return 0;
}

CIMQualifierDecl
cimmofRepository::getQualifierDecl(const CIMNamespaceName &nameSpace, 
                                   const CIMName &name)
{
  // Don't catch errors: pass them up to the requester
  return _context->lookupQualifierDecl(nameSpace, name);
}

CIMClass
cimmofRepository::getClass(const CIMNamespaceName &nameSpace, 
                           const CIMName &classname)
{
  // Don't catch errors: pass them up to the requester
  return _context->lookupClass(nameSpace, classname);
}

int 
cimmofRepository::modifyClass(const CIMNamespaceName &nameSpace, 
                           CIMClass *classdecl)
{
  // Don't catch errors: pass them up to the requester
  _context->modifyClass( nameSpace,  *classdecl);
  return 0;
}

void 
cimmofRepository::createNameSpace(const CIMNamespaceName &nameSpaceName)
{
  if (_cimrepository && _ot != compilerCommonDefs::IGNORE_REPOSITORY)
    _cimrepository->createNameSpace(nameSpaceName);
}
