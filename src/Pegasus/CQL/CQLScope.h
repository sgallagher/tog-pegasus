#ifndef Pegasus_CQLScope_h
#define Pegasus_CQLScope_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/CQL/Linkage.h>
#include <Pegasus/CQL/CQLChainedIdentifier.h>
#include <Pegasus/Common/CIMName.h>

PEGASUS_NAMESPACE_BEGIN

class QueryContext;

class CQLScope{

    public:
	CQLScope(){}
    	CQLScope(CIMName _inCIMName, CQLChainedIdentifier _inChainedIdentifier, QueryContext *inQuertContext);
	~CQLScope(){}
	CIMName getScope();
	CQLChainedIdentifier getTarget();
	QueryContext* getQueryContext();
	Boolean isDefault();

    private:
	CIMName _name;	
	CQLChainedIdentifier _chainedIdentifier;
	QueryContext *_queryContext;
};

#ifndef PEGASUS_ARRAY_T
#define PEGASUS_ARRAY_T CQLScope
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T
#endif




PEGASUS_NAMESPACE_END
#endif
