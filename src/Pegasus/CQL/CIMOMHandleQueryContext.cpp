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

#include "CIMOMHandleQueryContext.h"

PEGASUS_NAMESPACE_BEGIN

CIMOMHandleQueryContext::CIMOMHandleQueryContext(CIMNamespaceName& inNS, CIMOMHandle& handle)
  :QueryContext(inNS),
   _CH(handle)
{
}

CIMOMHandleQueryContext::CIMOMHandleQueryContext(const CIMOMHandleQueryContext& handle)
  :QueryContext(handle),
   _CH(handle._CH)
{
}

CIMOMHandleQueryContext::~CIMOMHandleQueryContext()
{
}

CIMOMHandleQueryContext& CIMOMHandleQueryContext::operator=(const CIMOMHandleQueryContext& rhs)
{
  if (this == &rhs)
    return *this;

  QueryContext::operator=(rhs);

  _CH = rhs._CH;

  return *this;
}

CIMClass CIMOMHandleQueryContext::getClass(const CIMName& inClassName) const
{
  /* Hardcoded defaults */
  Boolean localOnly = false;
  Boolean includeQualifiers = true;
  Boolean includeClassOrigin = false;
  CIMOMHandle tmp = _CH;

  // ATTN - constructing OperationContext from scratch may be a problem
  // once security is added to that object
  CIMClass _class = tmp.getClass(OperationContext(),
				 getNamespace(),
				 inClassName,
				 localOnly,
				 includeQualifiers,
				 includeClassOrigin,
				 CIMPropertyList());
  return _class;
}

Array<CIMName> CIMOMHandleQueryContext::enumerateClassNames(const CIMName& inClassName)
{
  // ATTN - constructing OperationContext from scratch may be a problem
  // once security is added to that object
  return _CH.enumerateClassNames(OperationContext(),
				  getNamespace(),
				  inClassName,
				  true);          // deepInheritance
}

QueryContext* CIMOMHandleQueryContext::clone()
{
  return new CIMOMHandleQueryContext(*this);
}

PEGASUS_NAMESPACE_END

