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
// Author: Christopher Neufeld <neufeld@linuxcare.com>
// 	   Al Stone <ahs3@fc.hp.com>
//
// Modified By: Al Stone <ahs3@fc.hp.com>
// Modified by: Mike Glantz <michael_glantz@hp.com>
//
//%/////////////////////////////////////////////////////////////////////////

#ifndef _COMPUTERSYSTEM_H
#define _COMPUTERSYSTEM_H

#define CAPTION "Computer System"
#define DESCRIPTION "This is the CIM_ComputerSystem object"
#define NAME_FORMAT "IP"
#define STATUS "OK"

#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <stdlib.h>

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

class ComputerSystem
{
   public:
      ComputerSystem();
      ~ComputerSystem();

      Boolean getCaption(CIMProperty&);
      Boolean getDescription(CIMProperty&);
      Boolean getInstallDate(CIMProperty&);
      Boolean getCreationClassName(CIMProperty&);
      Boolean getName(CIMProperty&);
      Boolean getStatus(CIMProperty&);
      Boolean getNameFormat(CIMProperty&);
      Boolean getPrimaryOwnerName(CIMProperty&);
      Boolean setPrimaryOwnerName(const String&);
      Boolean getPrimaryOwnerContact(CIMProperty&);
      Boolean setPrimaryOwnerContact(const String&);
      Boolean getRoles(CIMProperty&);
      Boolean getOtherIdentifyingInfo(CIMProperty&);
      Boolean getIdentifyingDescriptions(CIMProperty&);
      Boolean getDedicated(CIMProperty&);
      Boolean getResetCapability(CIMProperty&);
      Boolean getPowerManagementCapabilities(CIMProperty&);

// CIM_UnitaryComputerSystem
      Boolean getInitialLoadInfo(CIMProperty&);
      Boolean getLastLoadInfo(CIMProperty&);
      Boolean getPowerManagementSupported(CIMProperty&);
      Boolean getPowerState(CIMProperty&);
      Boolean getWakeUpType(CIMProperty&);
      Boolean getPrimaryOwnerPager(CIMProperty&);
      Boolean setPrimaryOwnerPager(const String&);
      Boolean getSecondaryOwnerName(CIMProperty&);
      Boolean setSecondaryOwnerName(const String&);
      Boolean getSecondaryOwnerContact(CIMProperty&);
      Boolean setSecondaryOwnerContact(const String&);
      Boolean getSecondaryOwnerPager(CIMProperty&);
      Boolean setSecondaryOwnerPager(const String&);
      Boolean getSerialNumber(CIMProperty&);
      Boolean getIdentificationNumber(CIMProperty&);

      void initialize(void);
      String getHostName(void);

   protected:

};


#endif
