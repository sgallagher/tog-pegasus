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

#ifndef Pegasus_CQLFactor_h
#define Pegasus_CQLFactor_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/CQL/CQLValue.h>
#include <Pegasus/CQL/Linkage.h>
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

   CQLFunctionRep(const CQLFunctionRep* rep);

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
    CQLValue resolveValue(const CIMInstance& CI, const QueryContext& queryCtx);

   Array<CQLPredicate> getParms()const;

   FunctionOpType getFunctionType()const;

   String toString()const;

   void applyContext(QueryContext& inContext);

   Boolean operator==(const CQLFunctionRep& func)const;

   Boolean operator!=(const CQLFunctionRep& func)const;

   friend class CQLFactory;

 private:
   CQLValue dateTimeToMicrosecond(const CIMInstance& CI, const QueryContext& queryCtx);
   CQLValue stringToUint(const CIMInstance& CI, const QueryContext& queryCtx);
   CQLValue stringToSint(const CIMInstance& CI, const QueryContext& queryCtx);
   CQLValue stringToReal(const CIMInstance& CI, const QueryContext& queryCtx);
   CQLValue stringToNumeric(const CIMInstance& CI, const QueryContext& queryCtx);
   CQLValue upperCase(const CIMInstance& CI, const QueryContext& queryCtx);
   CQLValue numericToString(const CIMInstance& CI, const QueryContext& queryCtx);
   CQLValue referenceToString(const CIMInstance& CI, const QueryContext& queryCtx);
   CQLValue className(const CIMInstance& CI, const QueryContext& queryCtx);
   CQLValue nameSpaceName(const CIMInstance& CI, const QueryContext& queryCtx);
   CQLValue nameSpaceType(const CIMInstance& CI, const QueryContext& queryCtx);
   CQLValue hostPort(const CIMInstance& CI, const QueryContext& queryCtx);
   CQLValue modelPath(const CIMInstance& CI, const QueryContext& queryCtx);
   CQLValue classPath(const CIMInstance& CI, const QueryContext& queryCtx);
   CQLValue objectPath(const CIMInstance& CI, const QueryContext& queryCtx);
   CQLValue instanceToReference(const CIMInstance& CI, const QueryContext& queryCtx);
   CQLValue currentDateTime(const CIMInstance& CI, const QueryContext& queryCtx);
   CQLValue dateTime(const CIMInstance& CI, const QueryContext& queryCtx);
   CQLValue microsecondToTimestamp(const CIMInstance& CI, const QueryContext& queryCtx);
   CQLValue microsecondToInterval(const CIMInstance& CI, const QueryContext& queryCtx);

   FunctionOpType _funcOpType;

   Array<CQLPredicate> _parms;

};

PEGASUS_NAMESPACE_END

#endif 
