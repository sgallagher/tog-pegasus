//%2004////////////////////////////////////////////////////////////////////////
//
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
// Author: Dave Rosckes (rosckes@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/CQL/CQLExpression.h>
#include <Pegasus/CQL/CQLFactory.h>
PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T TermOpType
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

#define PEGASUS_ARRAY_T CQLExpression
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T




CQLExpression::CQLExpression(CQLTerm& theTerm)
{
   _CQLTerms.append(theTerm);

}

CQLExpression::CQLExpression(const CQLExpression& inExpress) 
{
   _TermOperators = inExpress._TermOperators;

   _CQLTerms = inExpress._CQLTerms;
}


CQLValue CQLExpression::resolveValue(CIMInstance CI, QueryContext& QueryCtx)
{
   CQLValue returnVal = _CQLTerms[0].resolveValue(CI,QueryCtx);

   for(Uint32 i = 0; i < _TermOperators.size(); ++i)
   {
      switch(_TermOperators[i])
      {
         case plus:
            returnVal = returnVal + 
                        _CQLTerms[i+1].resolveValue(CI,QueryCtx);
            break;
         case minus:
            returnVal = returnVal - 
                        _CQLTerms[i+1].resolveValue(CI,QueryCtx);
            break;
         default:
            throw(1);
      }
   }
   return returnVal;
}

void CQLExpression::appendOperation(TermOpType theTermOpType, CQLTerm& theTerm)
{
   _TermOperators.append(theTermOpType);
   _CQLTerms.append(theTerm);
}

String CQLExpression::toString() 
{
   String returnStr;

   returnStr.append(_CQLTerms[0].toString());

   for(Uint32 i = 0; i < _TermOperators.size(); ++i)
   {
      returnStr.append(_TermOperators[i] == plus ? String(" + ") : String(" - "));
      returnStr.append(_CQLTerms[i+1].toString());
   }

   return returnStr;
}

Boolean CQLExpression::isSimpleValue()
{
   if(_CQLTerms.size() == 1 && 
      _CQLTerms[0].isSimpleValue())
   {
      return true;
   }
   return false;
}

Array<CQLTerm> CQLExpression::getTerms()
{
   return _CQLTerms;
}

Array<TermOpType> CQLExpression::getOperators()
{
   return _TermOperators;
}

void CQLExpression::applyScopes(Array<CQLScope> inScopes)
{
   for(Uint32 i = 0; i < _CQLTerms.size(); ++i)
   {
      _CQLTerms[i].applyScopes(inScopes);
   }
}

Boolean CQLExpression::operator==(const CQLExpression& expr){
	return true;
}

Boolean CQLExpression::operator!=(const CQLExpression& expr){
	return (!operator==(expr));
}


PEGASUS_NAMESPACE_END

