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
// Author: Adrian Schuur (schuur@de.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_QueryExpression_h
#define Pegasus_QueryExpression_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMPropertyList.h>

PEGASUS_NAMESPACE_BEGIN

/** This class gives access to the particulars of a query expression.*/

class QueryExpressionRep;

class PEGASUS_COMMON_LINKAGE QueryExpression {
    friend class DefaultProviderManager;
   protected:
   /**
    Constructor.
    At the moment the constructor only accepts WQL style Select statements.
    The constructor is normally called by the Provider Manager.

    @param language contains the query language type, "WQL" is supported.
    @param query contains a WQL style select statement
  */
    QueryExpression(QueryExpressionRep*);

    /**
    Destructor.
    The destructor normally only called by the Provider Manager.
    */
    ~QueryExpression();

   public:
   /**
    \Label{getQueryLanguage}
    Returns the query language identifier.
    @return A String instance containing the query language identifier.
    */
    String getQueryLanguage() const;

    /**
    \Label{getQuery}
    Returns the query select statement.
    @return A String instance containing the query select statement.
    */
    String getQuery() const;

    /**
    \Label{evaluate}
    Returns true if the instance passes where clause evaluation successfully.

    @param inst the instance to be evaluated.
    @return A Boolean indicating result of evaluation.
    */
    Boolean evaluate(const CIMInstance& inst) const;

    /**
    \Label{getPropertyList}
    Returns the select clause projection specification as CIMPropertyList
    A NULL CIMPropertyList is returned when all properties are to be returned.
    @return A CIMPropertylist instance containing the select clause projection specification.
    */
    const CIMPropertyList getPropertyList() const;

  private:
    QueryExpressionRep *_rep;
};

PEGASUS_NAMESPACE_END

#endif
