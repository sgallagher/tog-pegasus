#include "CQLScope.h"

PEGASUS_NAMESPACE_BEGIN


#define PEGASUS_ARRAY_T CQLScope
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

CQLScope::CQLScope(CIMName _inCIMName, Array<CIMName> inList,
                  CQLChainedIdentifier _inChainedIdentifier)
{
   _name = _inCIMName;
   _list = _list;
   _chainedIdentifier = new CQLChainedIdentifier(_inChainedIdentifier);
}

CQLScope::~CQLScope()
{
   if(_chainedIdentifier != NULL)
   {
      delete _chainedIdentifier;
      _chainedIdentifier = NULL;
   }
}

CIMName CQLScope::getScope()
{
	return _name;
}

Array<CIMName> CQLScope::getList()
{
   return _list;
}

CQLChainedIdentifier CQLScope::getTarget()
{
	return* _chainedIdentifier;
}

Boolean CQLScope::isDefault()
{

}

PEGASUS_NAMESPACE_END
