//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 - 2003 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ProviderName_h
#define Pegasus_ProviderName_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>

#include <Pegasus/Server/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_SERVER_LINKAGE ProviderName
{
public:
    ProviderName(void);
    //ProviderName(const String & s);
    /*
    physicalName - the fully qualified name to the physical component that is responsible for providing
        for the object named in objectName.

    logicalName - the logical name associated with the physical component. It is possible for one physical
        component to have one or more logical components.

    objectName - the relative or fully qualified CIM object name of interest.
    */
    ProviderName(
        const String & objectName,
        const String & logicalName,
        const String & physicalName,
        const String & interfaceName,
        const Uint32 capabilities);

    ~ProviderName(void);

    //String toString(void) const;

    String getObjectName(void) const;
    void setObjectName(const String & objectName);

    String getLogicalName(void) const;
    void setLogicalName(const String & logicalName);

    String getPhysicalName(void) const;
    void setPhysicalName(const String & physicalName);

    String getInterfaceName(void) const;
    void setInterfaceName(const String & interfaceName);

    Uint32 getCapabilitiesMask(void) const;
    void setCapabilitiesMask(const Uint32 capabilities);

private:
    String _physicalName;
    String _interfaceName;

    String _logicalName;
    String _objectName;

    Uint32 _capabilities;

};

PEGASUS_NAMESPACE_END

#endif
