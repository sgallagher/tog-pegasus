#include "RepositoryQueryContext.h"

PEGASUS_NAMESPACE_BEGIN
                                                                                                                           
                                                                                                                          
RepositoryQueryContext::RepositoryQueryContext(CIMNamespaceName inNS, CIMRepository* inCIMRep):QueryContext(inNS){
	_CIMRep = inCIMRep;
}

CIMClass RepositoryQueryContext::getClass(CIMName inClassName){
	/* Hardcoded defaults */
        Boolean localOnly = false;
        Boolean includeQualifiers = true;
        Boolean includeClassOrigin = false;
        CIMPropertyList _emptyCIMPropertyList;
                                                                                                                                       
        try{
                CIMClass _class = _CIMRep->getClass(
                                               getNamespace(),
                                               inClassName,
                                               localOnly,
                                               includeQualifiers,
                                               includeClassOrigin,
                                               _emptyCIMPropertyList);
		return _class;
        }catch(Exception e){
                // error
		PEGASUS_STD(cout) << e.getMessage() << PEGASUS_STD(endl);
		return CIMClass();
        }

}
                                                                                                                           
PEGASUS_NAMESPACE_END

