#ifndef Pegasus_CQLScope_h
#define Pegasus_CQLScope_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/CQL/Linkage.h>
#include <Pegasus/CQL/CQLChainedIdentifier.h>
#include <Pegasus/Common/CIMName.h>

PEGASUS_NAMESPACE_BEGIN

class CQLScope{

    public:
	CQLScope(){}
    	CQLScope(CIMName _inCIMName, Array<CIMName> _inList, 
               CQLChainedIdentifier _inChainedIdentifier);
	~CQLScope();
	CIMName getScope();
   Array<CIMName> getList();
	CQLChainedIdentifier getTarget();
	Boolean isDefault();

    private:
	CIMName _name;	
   Array<CIMName> _list;
	CQLChainedIdentifier* _chainedIdentifier;
};

#ifndef PEGASUS_ARRAY_T
#define PEGASUS_ARRAY_T CQLScope
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T
#endif

PEGASUS_NAMESPACE_END
#endif
