#include <Pegasus/CQL/CQLTerm.h>

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T FactorOpType
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

#define PEGASUS_ARRAY_T CQLTerm
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T



CQLTerm::CQLTerm(CQLFactor theFactor)
{
   _Factors.append(theFactor);
}

CQLValue CQLTerm::resolveValue(CIMInstance CI, QueryContext& QueryCtx)
{
   CQLValue returnVal = _Factors[0].resolveValue(CI,QueryCtx);

   for(Uint32 i = 0; i < _FactorOperators.size(); ++i)
   {
      switch(_FactorOperators[i])
      {
         case mult:
            returnVal = returnVal * 
                        _Factors[i+1].resolveValue(CI,QueryCtx);
            break;
         case divide:
            returnVal = returnVal / 
                        _Factors[i+1].resolveValue(CI,QueryCtx);
            break;
         case concat:
            returnVal = returnVal + 
                        _Factors[i+1].resolveValue(CI,QueryCtx);
            break;
         default:
            throw(1);
      }
   }
   return returnVal;
}

//##ModelId=40FC32BF038F
void CQLTerm::appendOperation(FactorOpType inFactorOpType, CQLFactor inFactor)
{
   _FactorOperators.append(inFactorOpType);
   _Factors.append(inFactor);
}

String CQLTerm::toString()
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

Boolean CQLTerm::isSimpleValue()
{
   if(_Factors.size() == 1 && 
      _Factors[0].isSimpleValue())
   {
      return true;
   }
   return false;
}

Array<CQLFactor> CQLTerm::getFactors()
{
   return _Factors;
}

Array<FactorOpType> CQLTerm::getOperators()
{
   return _FactorOperators;
}

void CQLTerm::applyScopes(Array<CQLScope> inScope)
{
   for(Uint32 i = 0; i < _Factors.size(); ++i)
   {
      _Factors[i].applyScopes(inScope);
   }
}



PEGASUS_NAMESPACE_END
