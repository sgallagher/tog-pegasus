#include <Pegasus/CQL/CQLTerm.h>
#include <Pegasus/CQL/CQLTermRep.h>
#include <Pegasus/CQL/CQLFactory.h>
#include <Pegasus/CQL/QueryContext.h>
PEGASUS_NAMESPACE_BEGIN

CQLTermRep::CQLTermRep(){}

CQLTermRep::CQLTermRep(CQLFactor theFactor)
{
   _Factors.append(theFactor);
}

CQLTermRep::CQLTermRep(const CQLTermRep& rep){
	_Factors = rep._Factors;
	_FactorOperators = rep._FactorOperators;
}

CQLTermRep::~CQLTermRep(){
}
CQLValue CQLTermRep::resolveValue(CIMInstance CI, QueryContext& QueryCtx)
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

void CQLTermRep::appendOperation(FactorOpType inFactorOpType, CQLFactor inFactor)
{
   _FactorOperators.append(inFactorOpType);
   _Factors.append(inFactor);
}

String CQLTermRep::toString()
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

Boolean CQLTermRep::isSimple()
{
   return (_Factors.size() == 1);
}

Boolean CQLTermRep::isSimpleValue()
{
   if(_Factors.size() == 1) 
      return _Factors[0].isSimpleValue();
   return false;
}

Array<CQLFactor> CQLTermRep::getFactors()
{
   return _Factors;
}

Array<FactorOpType> CQLTermRep::getOperators()
{
   return _FactorOperators;
}

void CQLTermRep::applyScopes(Array<CQLScope> inScope)
{
   for(Uint32 i = 0; i < _Factors.size(); ++i)
   {
      _Factors[i].applyScopes(inScope);
   }
}

PEGASUS_NAMESPACE_END
