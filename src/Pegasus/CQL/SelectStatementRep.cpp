//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L. P.;
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

#include "SelectStatement.h"
#include "SelectStatementRep.h"

PEGASUS_NAMESPACE_BEGIN

SelectStatementRep::SelectStatementRep()
{
}

SelectStatementRep::SelectStatementRep(const SelectStatementRep& ssr)
  :_qlang(ssr._qlang),
   _query(ssr._query),
   _ctx(ssr._ctx)
{
}

SelectStatementRep::SelectStatementRep(String& inQlang, String& inQuery, QueryContext* inCtx)
  :_qlang(inQlang),
   _query(inQuery),
   _ctx(inCtx)
{
}

SelectStatementRep::~SelectStatementRep()
{
}

SelectStatementRep& SelectStatementRep::operator=(const SelectStatementRep& rhs)
{
  if (this == &rhs)
    return *this;

  _qlang = rhs._qlang;
  _query = rhs._query;
  _ctx = rhs._ctx;

  return *this;
}

String SelectStatementRep::getQueryLanguage() const
{
   return _qlang;
}

String SelectStatementRep::getQuery() const
{
   return _query;
}



PEGASUS_NAMESPACE_END
