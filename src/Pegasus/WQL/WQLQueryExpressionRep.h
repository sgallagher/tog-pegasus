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
// Author: Adrian Schuur (schuur@de.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_WQLQueryExpressionRep_h
#define Pegasus_WQLQueryExpressionRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMPropertyList.h>

#include <Pegasus/Common/QueryExpressionRep.h>
#include "WQLSelectStatement.h"

PEGASUS_NAMESPACE_BEGIN

class WQLSelectStatement;

class PEGASUS_WQL_LINKAGE WQLQueryExpressionRep : public QueryExpressionRep {
  friend class QueryExpression;
  public:
    WQLQueryExpressionRep(const String & queryLanguage)
      : QueryExpressionRep(queryLanguage), _stmt(NULL) {}
    WQLQueryExpressionRep(const String & queryLanguage, WQLSelectStatement *s)
      : QueryExpressionRep(queryLanguage), _stmt(s){}
    WQLQueryExpressionRep(const String & queryLanguage, String & query)
      : QueryExpressionRep(queryLanguage,query), _stmt(NULL) {}

    virtual ~WQLQueryExpressionRep();

    virtual const CIMPropertyList getPropertyList() const;
    virtual Boolean evaluate(const CIMInstance &) const;
    virtual void applyProjection(CIMInstance &, Boolean allowMissing);

    void _parse();
    WQLSelectStatement *_stmt;
};

PEGASUS_NAMESPACE_END

#endif
