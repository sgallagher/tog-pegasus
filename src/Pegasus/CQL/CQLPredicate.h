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

#ifndef Pegasus_CQLPredicate_h
#define Pegasus_CQLPredicate_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/CQL/CQLSimplePredicate.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/CQL/Linkage.h>


#ifdef PEGASUS_USE_EXPERIMENTAL_INTERFACES

PEGASUS_NAMESPACE_BEGIN

    enum BooleanOpType { AND, OR } ;

/*
#ifndef PEGASUS_ARRAY_T
#define PEGASUS_ARRAY_T BooleanOpType
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T
#endif
*/

class PEGASUS_CQL_LINKAGE CQLPredicateRep;
class PEGASUS_CQL_LINKAGE CQLFactory;
class PEGASUS_QUERYCOMMON_LINKAGE QueryContext;

/** 
   This object is populated by Bison.

   Valid operations for each type of CQLValue are in accordance with the DMTF
CQL Specification.
   
   The CQLPredicate is considered "terminal" if it does not contain any
CQLPredicate objects.
    A 'terminal' predicate can be evaluated to TRUE/FALSE by examining the
CQLExpressions and operator.
    Valid operators are:
            <, >, =, <=, >=, <>, IS NULL, IS NOT NULL, ISA, LIKE

   CQLExpressions: 
      For an expression, CQLExpression::getValue is called and will return a
CQLValue.
      The appropriate operator is then invoked on CQLValue and that operator
function will
      enforce the type restrictions as documented in the DMTF CQL
Specification.
      That operator then determines whether the predicate is TRUE / FALSE.


   CQLPredicates: 
      The CQLPredicate is non-terminal if it contains only CQLPredicate
objects.
      A non-terminal CQLPredicate is evaluated by in turn evaluating the
contained CQLPredicates and
      boolean operator.
     Valid operators are:
              AND, OR

    For the evaluate method on each CQLPredicate. the CQLPredicate is evaluated
to TRUE/FALSE and 
     the result of the evaluation is then applied to the appropriate boolean
operator. 

    The result of the evaluation is and then inverted if the _invert member
variable is set to TRUE
    and then returned to the caller.

   */
class PEGASUS_CQL_LINKAGE CQLPredicate
{
  public:
    /**
        Default constructor

        @param  -  None.
        @return - None.
        @throws - None.
        <I><B>Experimental Interface</B></I><BR>
    */
    CQLPredicate();

    /**
        Constructor. Using this constructor sets isSimple() to true.
                                                                                                                   
        @param  -  inSimplePredicate.
        @param  -  inVerted. Defaults to false.  This is a logical NOT of this predicate
        @return - None.
        @throws - None.
        <I><B>Experimental Interface</B></I><BR>
    */    
    CQLPredicate(const CQLSimplePredicate & inSimplePredicate, Boolean inVerted = false);

    /**
        Constructor. Using this constructor sets isSimple() to false.
                                                                                                                   
        @param  -  inPredicate.
        @param  -  inVerted. Defaults to false.  This is a logical NOT of this predicate
        @return - None.
        @throws - None.
        <I><B>Experimental Interface</B></I><BR>
    */
    CQLPredicate(const CQLPredicate & inPredicate, Boolean inVerted = false);

    /**
        Destructor
                                                                                                                   
        @param  - None.
        @return - None.
        @throws - None.
        <I><B>Experimental Interface</B></I><BR>
    */
   ~CQLPredicate();
    /**  
      CQLExpressions: 
          For an expression, CQLExpression::getValue is called and will return a
    CQLValue.
          The appropriate operator is then invoked on CQLValue and that operator
    function will
          enforce the type restrictions as documented in the DMTF CQL
    Specification.
          That operator then determines whether the predicate is TRUE / FALSE.
    
    
       CQLPredicates: 
          The CQLPredicate is non-terminal if it contains only CQLPredicate
    objects.
          A non-terminal CQLPredicate is evaluated by in turn evaluating the
    contained CQLPredicates and
          boolean operator.
         Valid operators are:
                  AND, OR
    
         For the evaluate method on each CQLPredicate. the CQLPredicate is
    evaluated to TRUE/FALSE and 
         the result of the evaluation is then applied to the appropriate boolean
    operator. 
    
        The result of the evaluation is and then inverted if the _invert member
    variable is set to TRUE
        and then returned to the caller.

   @param  - CI. Instance to evaluate query against.
   @param  - QueryCtx. Query Context
   @return - Boolean.
   @throws - None.
   <I><B>Experimental Interface</B></I><BR>   
 
      */
    Boolean evaluate(CIMInstance CI, QueryContext& QueryCtx);

    /**
        Returns true if this CQLPredicate does not contain other CQLPredicates, false otherwise.
                                                                                                                   
        @param  - None.
        @return - Boolean.
        @throws - None.
        <I><B>Experimental Interface</B></I><BR>
    */
    Boolean isTerminal()const;

    /**
	Returns true if this predicate has been inverted (NOT), false otherwise	

        @param  - None.
        @return - Boolean.
        @throws - None.
        <I><B>Experimental Interface</B></I><BR>
    */
    Boolean getInverted()const;

    /**
        Sets the inverted flag.  Logically NOTting this predicate.
                                                                                                                   
        @param  - None.
        @return - None.
        @throws - None.
        <I><B>Experimental Interface</B></I><BR>
    */
    void setInverted();

    /**
	Appends a predicate to the predicate array. This method should only
        be called by Bison.

        @param  - inPredicate.
        @return - None.
        @throws - None.
        <I><B>Experimental Interface</B></I><BR>
    */
    void appendPredicate(const CQLPredicate& inPredicate);

    /**
        Appends a predicate to the predicate array. This method should only
        be called by Bison.
                                                                                                                   
        @param  - inPredicate.
        @param  - inBooleanOperator
        @return - None.
        @throws - None.
        <I><B>Experimental Interface</B></I><BR>
    */
    void appendPredicate(const CQLPredicate& inPredicate, BooleanOpType inBooleanOperator);

    /**
        Appends a SimplePredicate to the predicate array. This method should only
        be called by Bison.

        NOTE: THIS METHOD IS NOT IMPLEMENTED
                                                                                                                   
        @param  - inSimplePredicate.
        @return - None.
        @throws - None.
        <I><B>Experimental Interface</B></I><BR>
    */
    void appendPredicate(CQLSimplePredicate inSimplePredicate, BooleanOpType inBooleanOperator);
  
    /**
        Gets the CQLPredicates contained within this CQLPredicate.
                                                                                                                   
        @param  - None.
        @return - Array<CQLPredicate>. 
        @throws - None.
        <I><B>Experimental Interface</B></I><BR>
    */
    Array<CQLPredicate> getPredicates()const;
  
     /**
        Gets the CQLSimplePredicate contained within this CQLPredicate. Only valid
        if this isSimple() = true.
                                                                                                                   
        @param  - None.
        @return - CQLSimplePredicate.
        @throws - None.
        <I><B>Experimental Interface</B></I><BR>
    */
    CQLSimplePredicate getSimplePredicate()const;
    
     /**
        Gets the Operators for this CQLPredicate.
        Given P1 AND P2 OR P3, this would return [AND][OR]
                                                                                                                   
        @param  - None.
        @return - Array<BooleanOpType>.
        @throws - None.
        <I><B>Experimental Interface</B></I><BR>
    */
    Array<BooleanOpType> getOperators()const;
    
     /**
        This method normalizes the CQLChainedIdentifier so that properties that require
        scoping are scoped.
                                                                                                                   
        @param  - queryContext.
        @return - None.
        @throws - None.
        <I><B>Experimental Interface</B></I><BR>
    */
    void applyContext(QueryContext& queryContext);

     /**
        Returns true if this CQLPredicate contains no CQLPredicates, ie. only has one SimplePredicate.                                                                                                                   
        @param  - None.
        @return - Boolean.
        @throws - None.
        <I><B>Experimental Interface</B></I><BR>
    */
    Boolean isSimple()const;

     /**
        Returns true if its SimplePredicate object tree only contains a CQLValue.
                                                                                                                   
        @param  - None.
        @return - Boolean.
        @throws - None.
        <I><B>Experimental Interface</B></I><BR>
    */
    Boolean isSimpleValue()const;

    String toString()const;

     /**
        Operator=
                                                                                                                   
        @param  - rhs. Right hand side of assignment operator.
        @return - CQLPredicate&.
        @throws - None.
        <I><B>Experimental Interface</B></I><BR>
    */
    CQLPredicate& operator=(const CQLPredicate& rhs);

    friend class CQLFactory;
  private:
	CQLPredicateRep* _rep;    

};

/*
#ifndef PEGASUS_ARRAY_T
#define PEGASUS_ARRAY_T CQLPredicate
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T
#endif
*/

PEGASUS_NAMESPACE_END
#endif
#endif 
