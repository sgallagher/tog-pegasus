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

#ifndef Pegasus_CQLExpressionRep_h
#define Pegasus_CQLExpressionRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/CQL/CQLValue.h>
#include <Pegasus/CQL/Linkage.h>
#include <Pegasus/CQL/CQLTerm.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_CQL_LINKAGE CQLFactory;
class PEGASUS_CQL_LINKAGE QueryContext;

/**
  The CQLExpression class encapsulates a generic CQL expression to make it
easier to break into components and process the expression.  

   A CQL expression is made up of terms and operators. 
   For example,  'A + B' is a CQLExpression, where 'A' and 'B' are terms, and
'+' is an operator.

   There must be exactly one more term than there are operators.
  */
//##ModelId=40FC03230150
class PEGASUS_CQL_LINKAGE CQLExpressionRep
{
  public:
   CQLExpressionRep(){}
    /** constructor takes one CQLTerm object.
      */
    CQLExpressionRep(CQLTerm& theTerm);
   CQLExpressionRep(const CQLExpressionRep* rep);
    /**  the getValue method evaluates the expression and returns the value.
          Any property that need to be resolved into a value is taken from the
    CIMInstance.
      */
    ~CQLExpressionRep();

    CQLValue resolveValue(CIMInstance CI, QueryContext& QueryCtx);

    /** The appendOperation is used by Bison.
          It is invoked 0 or more times for the CQLExpression, and
          when invoked will always pass in an integer that is the Term operation
          type and a CQLTerm object.
      */
    void appendOperation(TermOpType theTermOpType, CQLTerm& theTerm);

   String toString();

   Boolean isSimple();

   Boolean isSimpleValue();

   Array<CQLTerm> getTerms();

   Array<TermOpType> getOperators();

   void applyContext(QueryContext& inContext);

   Boolean operator==(const CQLExpressionRep& rep);

   Boolean operator!=(const CQLExpressionRep& rep);

   friend class CQLFactory;

  private:
   

    /**  The _TermOperators member variable is an 
           array of operators that are valid to operate on Terms in a CQL
    expression. 
           Valid operators include concatentation, plus and minus.
    
           The array is ordered according to the operation from left to right.
      */
    
    Array<TermOpType> _TermOperators;

    /**  The _CQLTerms member variable is an 
           array of operands that are valid in a CQL expression. 
    
           The array is ordered according to the operation from left to right.
      */
    Array<CQLTerm> _CQLTerms;

    Boolean _isSimple;
};

PEGASUS_NAMESPACE_END


#endif /* CQLEXPRESSION_H_HEADER_INCLUDED_BEE5929F */
