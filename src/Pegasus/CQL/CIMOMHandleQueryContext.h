#ifndef Pegasus_CIMOMHandleQueryContext_h
#define Pegasus_CIMOMHandleQueryContext_h
                                                                                                                           
#include <Pegasus/Common/Config.h>
#include <Pegasus/CQL/Linkage.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/CQL/QueryContext.h> 
#include <Pegasus/Provider/CIMOMHandle.h>
                                                                                                                          
PEGASUS_NAMESPACE_BEGIN


class PEGASUS_CQL_LINKAGE CIMOMHandleQueryContext: public QueryContext
{
  public:

        CIMOMHandleQueryContext(CIMNamespaceName inNS, CIMOMHandle &handle);

	~ CIMOMHandleQueryContext(){}

        CIMClass getClass (CIMName inClassName);
       
  private:                                                                                                                                
	CIMOMHandleQueryContext():QueryContext(){}

        // members
        CIMOMHandle _CH;
};

PEGASUS_NAMESPACE_END
#endif
