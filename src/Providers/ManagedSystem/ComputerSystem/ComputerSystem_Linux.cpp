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
// Author: Al Stone <ahs3@fc.hp.com>
//         Christopher Neufeld <neufeld@linuxcare.com>
//
// Modified By: David Kennedy       <dkennedy@linuxcare.com>
//              Christopher Neufeld <neufeld@linuxcare.com>
//              Al Stone            <ahs3@fc.hp.com>
//              Mike Glantz         <michael_glantz@hp.com>
//              Chad Smith         <chad_smith@hp.com>
//
//%////////////////////////////////////////////////////////////////////////////

#include "ComputerSystemProvider.h"
#include "ComputerSystem.h"
#include <Pegasus/Common/Logger.h> // for Logger
#include <Pegasus/Common/FileSystem.h>
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/param.h>     // for MAXHOSTNAMELEN
#include <errno.h>         // for errno

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;



static String _hostName;

ComputerSystem::ComputerSystem()
{
}

ComputerSystem::~ComputerSystem()
{
}


Boolean ComputerSystem::getCaption(CIMProperty& p)
{
  // hardcoded
  p = CIMProperty(PROPERTY_CAPTION, String(CAPTION));
  return true;
}

Boolean ComputerSystem::getDescription(CIMProperty& p)
{
  static char description[256];

  ifstream version("/proc/version");

  if (!version) {
	Logger::put(Logger::ERROR_LOG, CLASS_EXTENDED_COMPUTER_SYSTEM, Logger::WARNING,
		"error opening file \"/proc/version\" (%0)", errno);
	return false;
  }
	
  version.getline(description, sizeof(description));
  p = CIMProperty(PROPERTY_DESCRIPTION, String(description));
  return true;
}

Boolean ComputerSystem::getInstallDate(CIMProperty& p)
{
  // not supported
  return false;
}

Boolean ComputerSystem::getCreationClassName(CIMProperty& p)
{
  // can vary, depending on class
  p = CIMProperty(PROPERTY_CREATION_CLASS_NAME,
                  String(CLASS_CIM_COMPUTER_SYSTEM));
    return true;
}


Boolean ComputerSystem::getName(CIMProperty& p)
{
  p = CIMProperty(PROPERTY_NAME,String(getHostName()));
  return true;
}

Boolean ComputerSystem::getStatus(CIMProperty& p)
{
  // hardcoded 
  p = CIMProperty(PROPERTY_STATUS,String(STATUS));
  return true;
}

Boolean ComputerSystem::getNameFormat(CIMProperty& p)
{
  // hardcoded
  p = CIMProperty(PROPERTY_NAME_FORMAT,String(NAME_FORMAT));
  return true;
}

Boolean ComputerSystem::getPrimaryOwnerName(CIMProperty& p)
{
  // not supported
  return false;
}

Boolean ComputerSystem::setPrimaryOwnerName(const String& name)
{
  // not supported
  return false;
}

Boolean ComputerSystem::getPrimaryOwnerContact(CIMProperty& p)
{
  // not supported
  return false;
}

Boolean ComputerSystem::setPrimaryOwnerContact(const String& contact)
{
  // not supported
  return false;
}

Boolean ComputerSystem::getRoles(CIMProperty& p)
{
  // not supported
  return false;
}

Boolean ComputerSystem::getOtherIdentifyingInfo(CIMProperty& p)
{
  // not supported
  return false;
}

Boolean ComputerSystem::getIdentifyingDescriptions(CIMProperty& p)
{
  // not supported
  return false;
}

Boolean ComputerSystem::getDedicated(CIMProperty& p)
{
  // not supported
  return false;
}

Boolean ComputerSystem::getResetCapability(CIMProperty& p)
{
  // not supported
  return false;
}

Boolean ComputerSystem::getPowerManagementCapabilities(CIMProperty& p)
{
  Array<Uint16> capabilities;

  // Since PowerManagementSupported == FALSE
  // Capabilities must be defined to return "1" which signifies
  // "Not Supported" from Capabilities enum
  capabilities.append(1);
  p = CIMProperty(PROPERTY_POWER_MANAGEMENT_CAPABILITIES,capabilities);
  return true;
}

Boolean ComputerSystem::getInitialLoadInfo(CIMProperty& p)
{
  // not supported
  return false;
}

Boolean ComputerSystem::getLastLoadInfo(CIMProperty& p)
{
  // not supported
  return false;
}

Boolean ComputerSystem::getPowerManagementSupported(CIMProperty& p)
{
  // hardcoded
  p = CIMProperty(PROPERTY_POWER_MANAGEMENT_SUPPORTED,false); // on PA-RISC!!
  return true;
}

Boolean ComputerSystem::getPowerState(CIMProperty& p)
{
  // hardcoded
/*
ValueMap {"1", "2", "3", "4", "5", "6", "7", "8"},
Values {"Full Power", "Power Save - Low Power Mode", 
                   "Power Save - Standby", "Power Save - Other", 
                   "Power Cycle", "Power Off", "Hibernate", "Soft Off"}
*/
  p = CIMProperty(PROPERTY_POWER_STATE,Uint16(1));  
  return true;
}

Boolean ComputerSystem::getWakeUpType(CIMProperty& p)
{
  // not supported
  return false;
}

Boolean ComputerSystem::getPrimaryOwnerPager(CIMProperty& p)
{
  // not supported
  return false;
}

Boolean ComputerSystem::setPrimaryOwnerPager(const String& pager)
{
  // not supported
  return false;
}

Boolean ComputerSystem::getSecondaryOwnerName(CIMProperty& p)
{
  // not supported
  return false;
}

Boolean ComputerSystem::setSecondaryOwnerName(const String& name)
{
  // not supported
  return false;
}

Boolean ComputerSystem::getSecondaryOwnerContact(CIMProperty& p)
{
  // not supported
  return false;
}

Boolean ComputerSystem::setSecondaryOwnerContact(const String& contact)
{
  // not supported
  return false;
}

Boolean ComputerSystem::getSecondaryOwnerPager(CIMProperty& p)
{
  // not supported
  return false;
}

Boolean ComputerSystem::setSecondaryOwnerPager(const String& pager)
{
  // not supported
  return false;
}

Boolean ComputerSystem::getSerialNumber(CIMProperty& p)
{
  // not supported
  return false;
}

Boolean ComputerSystem::getIdentificationNumber(CIMProperty& p)
{
  // not supported
  return false;
}

/**
 * initialize primarily functions to initialize static global variables
 * that will not be changed frequently. These variables are currently
 * _hostName.
 *
 */
void ComputerSystem::initialize(void)
{
  char    hostName[PEGASUS_MAXHOSTNAMELEN];
  struct  hostent *he;

  if (gethostname(hostName, PEGASUS_MAXHOSTNAMELEN) != 0)
  {
      _hostName.assign("Not initialized");
  }

  // Now get the official hostname.  If this call fails then return
  // the value from gethostname().

  he=gethostbyname(hostName);
  if (he)
  {
     strcpy(hostName, he->h_name);
  }

  _hostName.assign(hostName);
}

String ComputerSystem::getHostName(void)
{
  return _hostName;
}


