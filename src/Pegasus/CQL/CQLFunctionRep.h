#ifndef Pegasus_CQLFactor_h
#define Pegasus_CQLFactor_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/CQL/CQLValue.h>
#include <Pegasus/CQL/Linkage.h>
#include <Pegasus/CQL/CQLScope.h>
//#include <Pegasus/CQL/CQLExpression.h>
#include <Pegasus/CQL/CQLPredicate.h>


PEGASUS_NAMESPACE_BEGIN

class PEGASUS_CQL_LINKAGE CQLFactory;
//class PEGASUS_CQL_LINKAGE CQLPredicate;
/*
   CQLFunction objects are populated by the
   Bison code.

   Supported functions are in accordance with the
   DMTF CQL Specification.
   TODO:  THIS LIST IS SUBJECT TO CHANGE
 
    classname( <expr> )
    classname( )
    count(*)
    count( distinct * )
    count( distinct <expr> )
    something for createarray
    something for datetime
    something for hostname
    max( <expr> )
    mean( <expr> )
    median( <expr> )
    min( <expr> )
    something for modelpath
    something for namespacename
    something for namespacepath
    something for objectpath
    something for scheme
    sum( <expr> )
    something for userinfo
    uppercase( <expr> )

  */


class PEGASUS_CQL_LINKAGE CQLFunctionRep
{
  public:
   

    CQLFunctionRep() {};
   CQLFunctionRep(const CQLFunctionRep& inFunc);
    //CQLFunctionRep(FunctionOpType inFunctionOpType, Array<CQLExpression> inParms);
    CQLFunctionRep(CQLIdentifier inOpType, Array<CQLPredicate> inParms);
  ~CQLFunctionRep();
    /** 
       The getValue method validates the parms versus FunctionOpType.
               (A) resolves prarameter  types
               (B) number of parms
        and then actually executes the function.
        Returns a CQLValue object that has already been resolved.
      */
    CQLValue resolveValue(CIMInstance CI, QueryContext& queryCtx);
   Array<CQLPredicate> getParms();
   FunctionOpType getFunctionType();
   String toString();
   void applyScopes(Array<CQLScope> inScopes);
   Boolean operator==(const CQLFunctionRep& func);
   Boolean operator!=(const CQLFunctionRep& func);
   friend class CQLFactory;

  private:

   FunctionOpType _funcOpType;

    Array<CQLPredicate> _parms;

};

PEGASUS_NAMESPACE_END

#endif 
