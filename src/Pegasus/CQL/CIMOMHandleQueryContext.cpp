#include "CIMOMHandleQueryContext.h"

PEGASUS_NAMESPACE_BEGIN

CIMOMHandleQueryContext::CIMOMHandleQueryContext(CIMNamespaceName& inNS, CIMOMHandle& handle)
  :QueryContext(inNS),
   _CH(handle)
{
}

CIMOMHandleQueryContext::CIMOMHandleQueryContext(const CIMOMHandleQueryContext& handle)
  :QueryContext(handle),
   _CH(handle._CH)
{
}

CIMOMHandleQueryContext::~CIMOMHandleQueryContext()
{
}

CIMOMHandleQueryContext& CIMOMHandleQueryContext::operator=(const CIMOMHandleQueryContext& rhs)
{
  if (this == &rhs)
    return *this;

  QueryContext::operator=(rhs);

  _CH = rhs._CH;

  return *this;
}

CIMClass CIMOMHandleQueryContext::getClass(const CIMName& inClassName)
{
  /* Hardcoded defaults */
  Boolean localOnly = false;
  Boolean includeQualifiers = true;
  Boolean includeClassOrigin = false;

  // ATTN - constructing OperationContext from scratch may be a problem
  // once security is added to that object

  CIMClass _class = _CH.getClass(OperationContext(),
				 getNamespace(),
				 inClassName,
				 localOnly,
				 includeQualifiers,
				 includeClassOrigin,
				 CIMPropertyList());
  return _class;
}

PEGASUS_NAMESPACE_END

