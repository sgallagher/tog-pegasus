#include "CIMOMHandleQueryContext.h"

PEGASUS_NAMESPACE_BEGIN                                                                                                                                     
CIMOMHandleQueryContext::CIMOMHandleQueryContext(CIMNamespaceName inNS, CIMOMHandle &handle):QueryContext(inNS){
	_CH = handle;
}

CIMClass CIMOMHandleQueryContext::getClass(CIMName inClassName){
	/* Hardcoded defaults */
	Boolean localOnly = false;
	Boolean includeQualifiers = true;
	Boolean includeClassOrigin = false;
	try{
		CIMClass _class = _CH.getClass(OperationContext(),
					       getNamespace(),
					       inClassName,
					       localOnly,
				       	       includeQualifiers,
				       	       includeClassOrigin,
				               CIMPropertyList());
		return _class;
	}catch(CIMException e){
		// error
		PEGASUS_STD(cout) << e.getMessage() << PEGASUS_STD(endl);
		return CIMClass();
	}
}

PEGASUS_NAMESPACE_END

