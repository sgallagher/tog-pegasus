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
//         David Kennedy       <dkennedy@linuxcare.com>
//
// Modified By:
//         David Kennedy       <dkennedy@linuxcare.com>
//         Christopher Neufeld <neufeld@linuxcare.com>
//         Al Stone, Hewlett-Packard Company <ahs3@fc.hp.com>
//         Jim Metcalfe, Hewlett-Packard Company
//         Carlos Bonilla, Hewlett-Packard Company
//         Mike Glantz, Hewlett-Packard Company <michael_glantz@hp.com>
//         Lyle Wilkinson, Hewlett-Packard Company <lyle_wilkinson@hp.com>
//
//%////////////////////////////////////////////////////////////////////////////

#ifndef PG_IP_STUB_H
#define PG_IP_STUB_H

// =============================================================================
// Includes.
// =============================================================================

#include <Pegasus/Provider/CIMInstanceProvider.h>

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

// =============================================================================
// Defines.
// =============================================================================

// Support IP PROTOCOL TYPES

#define PROTOCOL_IPV4  "IPv4"
#define PROTOCOL_IPV6  "IPv6"


// =============================================================================
// Type Definitions.
// =============================================================================

class IPInterface
{

  public:

    IPInterface();
    ~IPInterface();

    // Property Accessor Methods

    Boolean getCaption(String&) const;
    Boolean getDescription(String&) const;
    Boolean getInstallDate(CIMDateTime&) const;
    Boolean getName(String&) const;
    Boolean getStatus(String&) const;
    Boolean getNameFormat(String&) const;
    Boolean getProtocolType(Uint16&) const;
    Boolean getOtherTypeDescription(String&) const;
    Boolean getAddress(String&) const;
    Boolean getSubnetMask(String&) const;
    Boolean getAddressType(Uint16&) const;
    Boolean getIPVersionSupport(Uint16&) const;
    Boolean getFrameType(Uint16&) const;

    // System name is the same for all instances
    static Boolean getSystemName(String&);

    // Method to Get the Simple Interface Name before the colon (":")
    String get_LANInterfaceName(void) const;

    // Method to determine if this IP interface ties to a LAN Interface
    Boolean bindsToLANInterface(void) const;

    // Method to obtain/save the fully qualified host name
    static void initSystemName(void);

    // Methods to Load Instances

    void set_address(const String& addr);
    void set_subnetMask(const String& snm);
    void set_protocol(const String& proto);
    void set_simpleIfName(const String& name);

};

class InterfaceList
{

  public:

    InterfaceList();	// Constructor - should load list of interfaces

    ~InterfaceList();

    // Method to get a particular element based on an Interface Name
    // Returns false if one is not found.
    Boolean findInterface(const String &ifName,
                          IPInterface &ipIfInst ) const;

    // Method to get a particular element based on an index
    IPInterface getInterface(const int index) const;
 
    // Number of Elements in the InterfaceList
    int size(void) const;

};

class IPRoute
{

  public:

    IPRoute();
    ~IPRoute();

    // Property Accessor Methods

    Boolean getCaption(String&) const;
    Boolean getDescription(String&) const;
    Boolean getInstallDate(CIMDateTime&) const;
    Boolean getName(String&) const;
    Boolean getStatus(String&) const;
    Boolean getDestinationAddress(String&) const;
    Boolean getDestinationMask(String&) const;
    Boolean getNextHop(String&) const;
    Boolean getIsStatic(Boolean&) const;
    Boolean getAddressType(Uint16&) const;

    // Methods to Load Instances

    void set_destAddr(const String& addr);
    void set_destMask(const String& mask);
    void set_nextHop(const String& nhop);
    void set_protocolType(const String& pt);

};

class RouteList
{

  public:

    RouteList();	// Constructor - should load list of interfaces

    ~RouteList();

    // Method to find a particular IP Route based on a Destination
    // Address, Destination Mask, and Address Type (e.g. IPv4):
    // Returns false if a match is not found.
    Boolean findRoute(const String &destAddr,
		      const String &destMask,
		      const Uint16 &addrType,
                           IPRoute &ipRInst ) const;

    // Method to get a particular element based on an index
    IPRoute getRoute(const int index) const;
 
    // Number of Elements in the IP Route
    int size(void) const;

};

#endif  /* #ifndef PG_IP_STUB_H */
