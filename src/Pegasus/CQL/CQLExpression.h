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

#ifndef Pegasus_CQLExpression_h
#define Pegasus_CQLExpression_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/CQL/CQLValue.h>
#include <Pegasus/CQL/Linkage.h>
#include <Pegasus/CQL/CQLTerm.h>

#ifdef PEGASUS_USE_EXPERIMENTAL_INTERFACES

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_CQL_LINKAGE CQLFactory;
class PEGASUS_CQL_LINKAGE CQLExpressionRep;
class PEGASUS_CQL_LINKAGE QueryContext;

/**  The enum is private, but the definition is public.
      */
    enum TermOpType {plus,minus};

#ifndef PEGASUS_ARRAY_T
#define PEGASUS_ARRAY_T TermOpType
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T
#endif

/**
  The CQLExpression class encapsulates a generic CQL expression to make it
easier to break into components and process the expression.  

   A CQL expression is made up of terms and operators. 
   For example,  'A + B' is a CQLExpression, where 'A' and 'B' are terms, and
'+' is an operator.

   There must be exactly one more term than there are operators.
  */

class PEGASUS_CQL_LINKAGE CQLExpression
{
  public:
   CQLExpression();
    /** constructor takes one CQLTerm object.
      */
    CQLExpression(const CQLTerm& theTerm);
   CQLExpression(const CQLExpression& inExpress);
    /**  the getValue method evaluates the expression and returns the value.
          Any property that need to be resolved into a value is taken from the
    CIMInstance.
      */
    ~CQLExpression();

    CQLValue resolveValue(const CIMInstance& CI, const QueryContext& QueryCtx);

    /** The appendOperation is used by Bison.
          It is invoked 0 or more times for the CQLExpression, and
          when invoked will always pass in an integer that is the Term operation
          type and a CQLTerm object.
      */
    void appendOperation(const TermOpType theTermOpType, const CQLTerm& theTerm);

   String toString()const;

   Boolean isSimple()const;

   Boolean isSimpleValue()const;

   Array<CQLTerm> getTerms()const;

   Array<TermOpType> getOperators()const;

   void applyContext(QueryContext& inContext, 
                     CQLChainedIdentifier inCid = CQLChainedIdentifier());

   CQLExpression& operator=(const CQLExpression& rhs);

   Boolean operator==(const CQLExpression& expr)const;

   Boolean operator!=(const CQLExpression& expr)const;

   friend class CQLFactory;

  private:

	CQLExpressionRep *_rep;   
};

#ifndef PEGASUS_ARRAY_T
#define PEGASUS_ARRAY_T CQLExpression
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T
#endif

PEGASUS_NAMESPACE_END

#endif
#endif 
