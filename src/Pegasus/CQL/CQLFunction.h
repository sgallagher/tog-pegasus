#ifndef CQLFUNCTION_H_HEADER_INCLUDED_BEE5F11B
#define CQLFUNCTION_H_HEADER_INCLUDED_BEE5F11B

#include <Pegasus/Common/Config.h>
#include <Pegasus/CQL/CQLValue.h>
#include <Pegasus/CQL/Linkage.h>
#include <Pegasus/CQL/CQLScope.h>
#include <Pegasus/CQL/CQLExpression.h>


PEGASUS_NAMESPACE_BEGIN

//class CQLExpression;

 /** The Enum is private, the definition is public.
      */
    //##ModelId=40FC3AB3010B
    enum  FunctionOpType { CLASSNAMEEXP, CLASSNAME, CLASSPATH, COUNT, COUNTDISTINCT, COUNTDISTINCTEXPR, CREATEARRAY, DATETIME, HOSTNAME, MAX, MEAN, MEDIAN, MIN, MODELPATH, NAMESPACENAME, NAMESPACEPATH, OBJECTPATH, SCHEME, SUM, USERINFO, UPPERCASE };
/**
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


//##ModelId=40FC3A8803C7
class PEGASUS_CQL_LINKAGE CQLFunction
{
  public:
   

    //##ModelId=40FD653E0390
    CQLFunction() {};
   CQLFunction(const CQLFunction& inFunc);
    CQLFunction(FunctionOpType inFunctionOpType, Array<CQLExpression> inParms);

    /** 
       The getValue method validates the parms versus FunctionOpType.
               (A) resolves prarameter  types
               (B) number of parms
        and then actually executes the function.
        Returns a CQLValue object that has already been resolved.
      */
    //##ModelId=40FC3BEA01F9
    CQLValue resolveValue(CIMInstance CI, QueryContext& queryCtx);
   Array<CQLExpression> getParms();
   FunctionOpType getFunctionType();
   String toString();
   void applyScopes(Array<CQLScope> inScopes);

  private:

   FunctionOpType _funcOpType;

    Array<CQLExpression> _parms;

};

PEGASUS_NAMESPACE_END

#endif /* CQLFUNCTION_H_HEADER_INCLUDED_BEE5F11B */
