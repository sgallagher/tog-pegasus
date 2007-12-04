//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//=============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Rudy Schuet (rudy.schuet@compaq.com) 11/12/01
//              added nsk platform support
//              Sean Keenan Hewlett-Packard Company (sean.keenan@hp.com)
//
//%////////////////////////////////////////////////////////////////////////////

#include "ComputerSystem.h"
#if defined(PEGASUS_PLATFORM_HPUX_ACC)
# include "ComputerSystem_HPUX.cpp"
#else
# if defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
#  include "ComputerSystem_Linux.cpp"
# elif defined(PEGASUS_OS_VMS)
#  include "ComputerSystem_Vms.cpp"
# else
#  include "ComputerSystem_Stub.cpp"
# endif
CIMInstance ComputerSystem::buildInstance(const CIMName& className)
{
    CIMInstance instance(className);
    CIMProperty p;

    //-- fill in properties for CIM_ComputerSystem
    if (getCaption(p)) instance.addProperty(p);

    if (getDescription(p)) instance.addProperty(p);

    if (getInstallDate(p)) instance.addProperty(p);

    if (getStatus(p)) instance.addProperty(p);

    if (getOperationalStatus(p)) instance.addProperty(p);

    if (getStatusDescriptions(p)) instance.addProperty(p);
    
    if (getElementName(p)) instance.addProperty(p);

    if (getCreationClassName(p)) instance.addProperty(p);

    if (getName(p)) instance.addProperty(p);

    if (getNameFormat(p)) instance.addProperty(p);

    if (getPrimaryOwnerName(p)) instance.addProperty(p);

    if (getPrimaryOwnerContact(p)) instance.addProperty(p);

    if (getRoles(p)) instance.addProperty(p);

    if (getOtherIdentifyingInfo(p)) instance.addProperty(p);

    if (getIdentifyingDescriptions(p)) instance.addProperty(p);

    if (getDedicated(p)) instance.addProperty(p);

    if (getResetCapability(p)) instance.addProperty(p);

    if (getPowerManagementCapabilities(p)) instance.addProperty(p);

    // Done if we are servicing CIM_ComputerSystem
    if (className.equal (CLASS_CIM_COMPUTER_SYSTEM))
      return instance;

    // Fill in properties for CIM_UnitaryComputerSystem
    if (getInitialLoadInfo(p)) instance.addProperty(p);

    if (getLastLoadInfo(p)) instance.addProperty(p);

    if (getPowerManagementSupported(p)) instance.addProperty(p);

    if (getPowerState(p)) instance.addProperty(p);

    if (getWakeUpType(p)) instance.addProperty(p);

    // Done if we are servicing CIM_UnitaryComputerSystem
    if (className.equal (CLASS_CIM_UNITARY_COMPUTER_SYSTEM))
      return instance;

    // Fill in properties for <Extended>_ComputerSystem
    if (className.equal (CLASS_EXTENDED_COMPUTER_SYSTEM))
    {
       if(getPrimaryOwnerPager(p)) instance.addProperty(p);
       if(getSecondaryOwnerName(p)) instance.addProperty(p);
       if(getSecondaryOwnerContact(p)) instance.addProperty(p);
       if(getSecondaryOwnerPager(p)) instance.addProperty(p);
       if(getSerialNumber(p)) instance.addProperty(p);
       if(getIdentificationNumber(p)) instance.addProperty(p);
    }

    return instance;
}
#endif
