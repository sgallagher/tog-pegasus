#ifndef Pegasus_RepositoryQueryContext_h
#define Pegasus_RepositoryQueryContext_h
                                                                                                                                       
#include <Pegasus/Common/Config.h>
#include <Pegasus/CQL/Linkage.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/CQL/QueryContext.h>
#include <Pegasus/Repository/CIMRepository.h>                                                                                                                                       
PEGASUS_NAMESPACE_BEGIN


class PEGASUS_CQL_LINKAGE RepositoryQueryContext: public QueryContext
{
   public:

        RepositoryQueryContext(CIMNamespaceName inNS, CIMRepository* inCIMRep);
	
	~RepositoryQueryContext(){}

        CIMClass getClass (CIMName inClassName);
         
   private: 
	RepositoryQueryContext():QueryContext(){}
        CIMRepository* _CIMRep;
};

PEGASUS_NAMESPACE_END
#endif
