#ifndef _Pegasus_CIMNameUnchecked_h
#define _Pegasus_CIMNameUnchecked_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMName.h>

PEGASUS_NAMESPACE_BEGIN

class CIMNameUnchecked : public CIMName
{
public:

#ifdef PEGASUS_DEBUG

    CIMNameUnchecked(const String& str) : CIMName(str)
    {
	// Call base class constructor for force CIMName::legal() call.
    }

#else /* PEGASUS_DEBUG */

    CIMNameUnchecked(const String& str)
    {
	// Bypass CIMName::legal() call. Must be friend to do this.
	cimName = str;
    }

#endif /* PEGASUS_DEBUG */

};

PEGASUS_NAMESPACE_END

#endif /* _Pegasus_CIMNameUnchecked_h */
