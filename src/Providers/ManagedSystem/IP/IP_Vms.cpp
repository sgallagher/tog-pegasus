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
// Author: Sean Keenan <sean.keenan@hp.com>
//
// Modified By:
//
//%////////////////////////////////////////////////////////////////////////////

/* ==========================================================================
   Includes.
   ========================================================================== */

#include "IPPlatform.h"

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

IPInterface::IPInterface()
{
}

IPInterface::~IPInterface()
{
}

/*
================================================================================
NAME              : getCaption
DESCRIPTION       :
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean IPInterface::getCaption(String& s) const
{
  s = String::EMPTY;
  return true;
}

/*
================================================================================
NAME              : getDescription
DESCRIPTION       :
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean IPInterface::getDescription(String& s) const
{
  s = String::EMPTY;
  return true;
}

/*
================================================================================
NAME              : getInstallDate
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean IPInterface::getInstallDate(CIMDateTime& d) const
{
  // Not supported. This property is inherited from
  // CIM_ManagedSystemElement, but has no useful meaning
  // for an IP address.

  return false;
}

/*
================================================================================
NAME              : getName
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean IPInterface::getName(String& s) const
{
  // not supported
  return false;
}

/*
================================================================================
NAME              : getStatus
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean IPInterface::getStatus(String& s) const
{
  // This property is inherited from CIM_ManagedSystemElement,
  // is not relevant.

  return false;
}


/*
================================================================================
NAME              : getSystemName
DESCRIPTION       : Platform-specific routine to get the System Name
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean IPInterface::getSystemName(String& s)
{
  // not supported
  return false;
}


/*
================================================================================
NAME              : getNameFormat
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean IPInterface::getNameFormat(String& s) const
{
  // not supported
  return false;
}

/*
================================================================================
NAME              : getProtocolType
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean IPInterface::getProtocolType(Uint16& i16) const
{
  // not supported
  return false;
}

/*
================================================================================
NAME              : getOtherTypeDesription
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean IPInterface::getOtherTypeDescription(String& s) const
{
  // not supported
  return false;
}

/*
================================================================================
NAME              : getAddress
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean IPInterface::getAddress(String& s) const
{
  // not supported
  return false;
}

/*
================================================================================
NAME              : getSubnetMask
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean IPInterface::getSubnetMask(String& s) const
{
  // not supported
  return false;
}

/*
================================================================================
NAME              : getAddressType
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean IPInterface::getAddressType(Uint16& i16) const
{
  // not supported
  return false;
}

/*
================================================================================
NAME              : getIPVersionSupport
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean IPInterface::getIPVersionSupport(Uint16& i16) const
{
  // not supported
  return false;
}

/*
================================================================================
NAME              : getFrameType
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean IPInterface::getFrameType(Uint16& i16) const
{
  // not supported
  return false;
}


/*
================================================================================
NAME              : get_LANInterfaceName
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
String IPInterface::get_LANInterfaceName(void) const
{
  // This routine must be written to return a value.
  return String::EMPTY;
}

/*
================================================================================
NAME              : bindsToLANInterface
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean IPInterface::bindsToLANInterface(void) const
{
  return false;
}


/*
================================================================================
NAME              : initSystemName
DESCRIPTION       : Platform-specific routine to get the System Name
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
void IPInterface::initSystemName(void)
{
  // Do nothing
}

/*
================================================================================
NAME              : InterfaceList Constructor
DESCRIPTION       : Build the list IP Interfaces
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
InterfaceList::InterfaceList()
{
}

/*
================================================================================
NAME              : InterfaceList Destructor
DESCRIPTION       : None
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
InterfaceList::~InterfaceList()
{
}


/*
================================================================================
NAME              : findInterface
DESCRIPTION       : find the requested interface
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean InterfaceList::findInterface(const String &ifName,
			     IPInterface &ipIfInst) const
{
    // Always return interface not found
    return false;
}


/*
================================================================================
NAME              : getInterface
DESCRIPTION       : Get an interface based on an index.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
IPInterface InterfaceList::getInterface(const int index) const
{
    // give an interface (this should never get called since size will
    // always be zero).
    IPInterface i;
    return i;
}

/*
================================================================================
NAME              : size
DESCRIPTION       : Find the size of the Interface List.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
int InterfaceList::size(void) const
{
    // no interfaces
    return 0;
}

/////////////////////////////////////////////////////////////////////////
//
// PG_IPRoute related methods
//
/////////////////////////////////////////////////////////////////////////

IPRoute::IPRoute()
{
}

IPRoute::~IPRoute()
{
}

/*
================================================================================
NAME              : getCaption
DESCRIPTION       :
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean IPRoute::getCaption(String& s) const
{
  s = String::EMPTY;
  return true;
}

/*
================================================================================
NAME              : getDescription
DESCRIPTION       :
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean IPRoute::getDescription(String& s) const
{
  s = String::EMPTY;
  return true;
}

/*
================================================================================
NAME              : getInstallDate
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean IPRoute::getInstallDate(CIMDateTime& d) const
{
  // Not supported. This property is inherited from
  // CIM_ManagedSystemElement, but has no useful meaning
  // for an IP Route.

  return false;
}

/*
================================================================================
NAME              : getName
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean IPRoute::getName(String& s) const
{
  // not supported
  return false;
}

/*
================================================================================
NAME              : getStatus
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean IPRoute::getStatus(String& s) const
{
  // This property, inherited from CIM_ManagedSystemElement,
  // is not relevant.

  return false;
}

/*
================================================================================
NAME              : getDestinationAddress
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean IPRoute::getDestinationAddress(String& s) const
{
  // not supported
  return false;
}

/*
================================================================================
NAME              : getDestinationMask
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean IPRoute::getDestinationMask(String& s) const
{
  // not supported
  return false;
}

/*
================================================================================
NAME              : getNextHop
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean IPRoute::getNextHop(String& s) const
{
  // not supported
  return false;
}

/*
================================================================================
NAME              : getIsStatic
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean IPRoute::getIsStatic(Boolean& s) const
{
  // not supported
  return false;
}

/*
================================================================================
NAME              : getAddressType
DESCRIPTION       : 
ASSUMPTIONS       : 
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean IPRoute::getAddressType(Uint16& i16) const
{
  // not supported
  return false;
}

/*
================================================================================
NAME              : RouteList Constructor
DESCRIPTION       : Build the list of IP Routes
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
RouteList::RouteList()
{
}

/*
================================================================================
NAME              : RouteList Destructor
DESCRIPTION       : None
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
RouteList::~RouteList()
{
}


/*
================================================================================
NAME              : findRoute
DESCRIPTION       : Find the requested IP Route based on the destination
		  : address, destination mask, and address type.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
Boolean RouteList::findRoute(const String &destAddr,
			     const String &destMask,
			     const Uint16 &addrType,
			     IPRoute      &ipRInst) const
{
    // Always return route not found
    return false;
}


/*
================================================================================
NAME              : getRoute
DESCRIPTION       : Get an IP Route based on an index.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
IPRoute RouteList::getRoute(const int index) const
{
    // give a route (this should never get called since size will
    // always be zero).
    IPRoute i;
    return i;
}

/*
================================================================================
NAME              : size
DESCRIPTION       : Find the size of the Route List.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
int RouteList::size(void) const
{
    return 0;
}

