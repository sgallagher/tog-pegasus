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

#include <Pegasus/CQL/CQLTerm.h>
#include <Pegasus/CQL/CQLTermRep.h>
#include <Pegasus/CQL/CQLFactory.h>
#include <Pegasus/CQL/QueryContext.h>
#include <Pegasus/CQL/QueryException.h>
#include <Pegasus/Common/Tracer.h>

PEGASUS_NAMESPACE_BEGIN

CQLTermRep::CQLTermRep(){}

CQLTermRep::CQLTermRep(const CQLFactor& theFactor)
{
   _Factors.append(theFactor);
}

CQLTermRep::CQLTermRep(const CQLTermRep& rep){
	_Factors = rep._Factors;
	_FactorOperators = rep._FactorOperators;
}

CQLTermRep::~CQLTermRep(){
}

CQLValue CQLTermRep::resolveValue(const CIMInstance& CI, const QueryContext& QueryCtx)
{
  PEG_METHOD_ENTER(TRC_CQL,"CQLTermRep:resolveValue()");
   CQLValue returnVal = _Factors[0].resolveValue(CI,QueryCtx);

   for(Uint32 i = 0; i < _FactorOperators.size(); ++i)
   {
      switch(_FactorOperators[i])
      {/*
         case mult:
            returnVal = returnVal * 
                        _Factors[i+1].resolveValue(CI,QueryCtx);
            break;
         case divide:
            returnVal = returnVal / 
                        _Factors[i+1].resolveValue(CI,QueryCtx);
            break;
       */
         case concat:
            returnVal = returnVal + 
                        _Factors[i+1].resolveValue(CI,QueryCtx);
            break;
       
         default:
	   MessageLoaderParms mload(String("CQL.CQLTermRep.OPERATION_NOT_SUPPORTED"),
				    String("Operation is not supported."));
	   throw CQLSyntaxErrorException(mload);
      }
   }

   PEG_METHOD_EXIT();
   return returnVal;
}

void CQLTermRep::appendOperation(FactorOpType inFactorOpType, CQLFactor inFactor)
{
   _FactorOperators.append(inFactorOpType);
   _Factors.append(inFactor);
}

String CQLTermRep::toString()const
{
   String returnStr;

   returnStr.append(_Factors[0].toString());

   for(Uint32 i = 0; i < _FactorOperators.size(); ++i)
   {
      returnStr.append(_FactorOperators[i] == 
            mult ? String(" * ") : divide ? String(" / ") : String(" concat "));
      returnStr.append(_Factors[i+1].toString());
   }

   return returnStr;

}

Boolean CQLTermRep::isSimple()const
{
   return (_Factors.size() == 1);
}

Boolean CQLTermRep::isSimpleValue()const
{
   if(_Factors.size() == 1) 
      return _Factors[0].isSimpleValue();
   return false;
}

Array<CQLFactor> CQLTermRep::getFactors()const
{
   return _Factors;
}

Array<FactorOpType> CQLTermRep::getOperators()const
{
   return _FactorOperators;
}

void CQLTermRep::applyContext(QueryContext& inContext,
                              CQLChainedIdentifier& inCid)
{
   for(Uint32 i = 0; i < _Factors.size(); ++i)
   {
      _Factors[i].applyContext(inContext, inCid);
   }
}

PEGASUS_NAMESPACE_END
