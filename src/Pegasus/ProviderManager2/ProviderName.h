//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Modified By:  Adrian Schuur (schuur@de.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ProviderName_h
#define Pegasus_ProviderName_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMObjectPath.h>

#include <Pegasus/ProviderManager2/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_PPM_LINKAGE ProviderName
{
public:
    ProviderName(void);
    
    ProviderName(
        const String & logicalName,
        const String & physicalName,
        const String & interfaceName,
        const Uint32 capabilities,
        const CIMName & method=CIMName());

    ProviderName(
        const CIMNamespaceName & nameSpace,
        const CIMName & className,
        const Uint32 capabilities,
        const CIMName & method=CIMName());
        
     ProviderName(
        const CIMObjectPath & path,
        const Uint32 capabilities,
        const CIMName & method=CIMName());
        
   ~ProviderName(void);

    String getLogicalName() const;
    String getPhysicalName() const;
    void   setPhysicalName(const String & physicalName);
    String getInterfaceName(void) const;
    String getLocation(void) const;
    void   setLocation(const String &);
    Uint32 getCapabilitiesMask(void) const;
    CIMName getMethodName(void) const;
    CIMNamespaceName getNameSpace() const;
    CIMName getClassName() const;

private:
    CIMNamespaceName _nameSpace;
    CIMName _className;
    String _physicalName;
    String _interfaceName;
    String _logicalName;
    String _location;
    Uint32 _capabilities;
    
    CIMName _method;

};

PEGASUS_NAMESPACE_END

#endif
