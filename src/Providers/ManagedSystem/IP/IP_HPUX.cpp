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
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%////////////////////////////////////////////////////////////////////////////

/* ==========================================================================
   Includes.
   ========================================================================== */

#include "IPPlatform.h"
#include <errno.h>

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

String IPInterface::_hostname = String::EMPTY;  // Allocate this static

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
  s = _address;
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
  String sn;

  if (getSystemName(sn))
  {
      s = "IP Protocol Endpoint for " + sn + " (" + _address + ")";
      return true;
  }
  else
      return false;
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
  s = _protocol + "_" + _simpleIfName;
  return true;
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
    struct hostent *he;
    char hn[PEGASUS_MAXHOSTNAMELEN + 1];

    // fill in hn with what this system thinks is its name
    gethostname(hn, PEGASUS_MAXHOSTNAMELEN);
    hn[PEGASUS_MAXHOSTNAMELEN] = 0;

    // find out what the nameservices think is its full name
    // but if that failed, return what gethostname said
    if (he = gethostbyname(hn))
    {
        s = String(he->h_name);
    }
    else
    {
        s = String(hn);
    }

    return true;
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
  s = "<Protocol>_<InterfaceName>";
  return true;
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

/*
  From the MOF for CIM_ProtocolEndpoint.ProtocolType:
   ValueMap {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", 
    "10", "11", "12", "13", "14", "15", "16", "17", "18", 
    "19", "20", "21", "22", "23", "24", "25", "26", "27"},
  Values {"Unknown", "Other", "IPv4", "IPv6", "IPX", 
   "AppleTalk", "DECnet", "SNA", "CONP", "CLNP",
   "VINES", "XNS", "ATM", "Frame Relay",
   "Ethernet", "TokenRing", "FDDI", "Infiniband", 
   "Fibre Channel", "ISDN BRI Endpoint", 
   "ISDN B Channel Endpoint", "ISDN D Channel Endpoint", 
    // 22
    "IPv4/v6", "BGP", "OSPF", "MPLS", "UDP", "TCP"},
*/

  if (String::equal(_protocol,PROTOCOL_IPV4))
  {
	i16 = 2;  // IPv4
	return true;
  }
  else if (String::equal(_protocol,PROTOCOL_IPV6))
  {
	i16 = 3;  // IPv6
	return true;
  }
  else return false;

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
  // The caller must know to set the value to NULL (XML: no <VALUE> element)
  s = String::EMPTY;
  return true;
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
  s = _address;
  return true;
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
  s = _subnetMask;
  return true;
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
  /*
      From CIM v2.6.0 MOF for CIM_IPProtocolEndpoint.AddressType:
	 ValueMap {"0", "1", "2"},
	 Values {"Unknown", "IPv4", "IPv6"} ]
  */

  if (String::equal(_protocol,PROTOCOL_IPV4))
  {
	i16 = 1;  // IPv4
	return true;
  }
  else if (String::equal(_protocol,PROTOCOL_IPV6))
  {
	i16 = 2;  // IPv6
	return true;
  }
  else return false;

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
  /*
      From CIM v2.6.0 MOF for CIM_IPProtocolEndpoint.IPVersionSupport:
	 ValueMap {"0", "1", "2"},
	 Values {"Unknown", "IPv4 Only", "IPv6 Only"} ]
  */

  if (String::equal(_protocol,PROTOCOL_IPV4))
  {
	i16 = 1;  // IPv4 Only
	return true;
  }
  else if (String::equal(_protocol,PROTOCOL_IPV6))
  {
	i16 = 2;  // IPv6 Only
	return true;
  }
  else return false;

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
  /*
     From CIM v2.6.0 MOF for CIM_BindsToLANEndpoint.FrameType
        ValueMap {"0", "1", "2", "3", "4"},
        Values {"Unknown", "Ethernet", "802.2", "SNAP", "Raw802.3"} ]
  */

  i16 = 1;  // Ethernet
  return true;
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
  // Get rid of everything after the colon (":") if the name is of the
  // form "lanX:Y", e.g. "lan0:1".

  Uint32 pos = _simpleIfName.find(":");

  if (pos == PEG_NOT_FOUND)
    return _simpleIfName;
  else
  {
    String s = _simpleIfName;
    s.remove(pos,PEG_NOT_FOUND);
    return s;
  }
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
  // if this is a local ("lo") interface, then it doesn't bind to
  // an actual LAN Interface

  if (_simpleIfName.find("lo") == PEG_NOT_FOUND)
	return true;
  else
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
}

/*
================================================================================
NAME              : set_address
DESCRIPTION       : Platform-specific routine to set the IP Address
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
void IPInterface::set_address(const String& addr)
{
	_address = addr;
}

/*
================================================================================
NAME              : set_subnetMask
DESCRIPTION       : Platform-specific routine to set the Subnet Mask
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
void IPInterface::set_subnetMask(const String& snm)
{
	_subnetMask = snm;
}

/*
================================================================================
NAME              : set_protocol
DESCRIPTION       : Platform-specific routine to set the IP Protocol
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
void IPInterface::set_protocol(const String& proto)
{
	_protocol = proto;
}

/*
================================================================================
NAME              : set_simpleIfName
DESCRIPTION       : Platform-specific routine to set the Interface Name
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
void IPInterface::set_simpleIfName(const String& name)
{
	_simpleIfName = name;
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
  int fd,                      // file descriptor
      i, j,                    // general purpose indicies
      numif,                   // number of interfaces
      numip;                   // number of IP addresses
  struct ifconf ifconf;        // interface configuration
  unsigned int len;            // length of get_mib_info() buffer
  struct nmparms parms;        // get_mib_info() arguments
  mib_ipAdEnt * addr_buf;      // IP Address Buffer
  struct in_addr t;            // temporary variable for extracting
			       //     IP Address Buffer contents
  struct sockaddr_in *sin;     // temporary variable for extracting
                               //     interface name

#ifdef DEBUG
  cout << "InterfaceList::InterfaceList()" << endl;
#endif

  // Load the interface name structures.

  if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
	throw CIMOperationFailedException("Error in opening socket: " +
				String(strerror(errno)));
  }

  if (ioctl(fd, SIOCGIFNUM, &numif) < 0) {
      throw CIMOperationFailedException
          ("Error in ioctl() request SIOCGIFNUM: " + String(strerror(errno)));
  }

  ifconf.ifc_len = numif * sizeof (struct ifreq);
  ifconf.ifc_req = (struct ifreq *) calloc(numif, sizeof (struct ifreq));

  if (ioctl (fd, SIOCGIFCONF, &ifconf) < 0) {
	free (ifconf.ifc_req);
	throw CIMOperationFailedException
            ("Error in ioctl() request SIOCGIFCONF: " +
				String(strerror(errno)));
  }

  close(fd);

  if ((fd = open_mib("/dev/ip", O_RDONLY, 0, 0)) < 0) {
	free (ifconf.ifc_req);
	throw CIMOperationFailedException("Can't open /dev/ip: " +
				String(strerror(errno)));
  }

  parms.objid = ID_ipAddrNumEnt;
  parms.buffer = (char *) &numip;
  len = sizeof(numip);
  parms.len = &len;

  if (get_mib_info (fd, &parms) < 0) {
	free (ifconf.ifc_req);
	throw CIMOperationFailedException(
	    "Can't get ID_ipAddrNumEnt from get_mib_info(): " +
	    String(strerror(errno)));
  }

  addr_buf = (mib_ipAdEnt *)malloc(numip*sizeof(mib_ipAdEnt));

  if (addr_buf == 0)
  {
	free (ifconf.ifc_req);
	free (addr_buf);
	throw CIMOperationFailedException(
	    "Error in allocating space for the kernel interface table: " +
	    String(strerror(errno)));
  }

  parms.objid = ID_ipAddrTable;
  parms.buffer = (char *) addr_buf;
  len = numip * sizeof(mib_ipAdEnt);
  parms.len = &len;

  if (get_mib_info (fd, &parms) < 0) {
	free (ifconf.ifc_req);
	free (addr_buf);
	throw CIMOperationFailedException(
	    "Can't get ID_ipAddrTable from get_mib_info(): " +
	    String(strerror(errno)));
  }

  // Create the interface list entries

  for (i=0; i < numip ; i++) {

    IPInterface _ipif;

    t.s_addr = addr_buf[i].Addr;
    _ipif.set_address(inet_ntoa(t));

    // ATTN-LEW-2002-07-30: Enhance this to deal with IPv6 too.
    _ipif.set_protocol(PROTOCOL_IPV4);

    for (j = 0; j < (int)(numif-1); j++) {
	sin = reinterpret_cast<struct sockaddr_in*>(
            &ifconf.ifc_req[j].ifr_addr);
	if (sin->sin_addr.s_addr == t.s_addr)
	{
	    _ipif.set_simpleIfName(ifconf.ifc_req[j].ifr_name);
        }
    } /* for */

    t.s_addr = addr_buf[i].NetMask;
    _ipif.set_subnetMask(inet_ntoa(t));

    _ifl.push_back(_ipif);   // Add another IP interface to the list

  } /* for */

  close_mib(fd);
  free (ifconf.ifc_req);
  free (addr_buf);

#ifdef DEBUG
  cout << "InterfaceList::InterfaceList() -- done" << endl;
#endif

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
    // ifName has the format <Protocol>_<InterfaceName>,
    // for example "IPv4_lan0".

    int i;

    for (i = 0; i < _ifl.size(); i++)
    {
       String s;

       if (_ifl[i].getName(s) && String::equal(s,ifName)) 
       {
	  ipIfInst = _ifl[i];
	  return true;
       }
    }

    // Interface not found
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
    return _ifl[index];
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
    return _ifl.size();
}

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
  s = _destAddr;
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
  s = "IP Route for Destination Address: " + _destAddr +
      " (" + _protocolType + ")";
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
  s = _destAddr;
  return true;
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
  s = _destAddr;
  return true;
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
  s = _destMask;
  return true;
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
  s = _nextHop;
  return true;
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
  // Don't know how to get this property.
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
  /*
      From CIM v2.6.0 MOF for CIM_IPRoute.AddressType:
	 ValueMap {"0", "1", "2"},
	 Values {"Unknown", "IPv4", "IPv6"} ]
  */

  if (String::equal(_protocolType,PROTOCOL_IPV4))
  {
	i16 = 1;  // IPv4
	return true;
  }
  else if (String::equal(_protocolType,PROTOCOL_IPV6))
  {
	i16 = 2;  // IPv6
	return true;
  }
  else return false;

}

/*
================================================================================
NAME              : set_destAddress
DESCRIPTION       : Platform-specific routine to set the IP Destination Address
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
void IPRoute::set_destAddr(const String& addr)
{
	_destAddr = addr;
}

/*
================================================================================
NAME              : set_destMask
DESCRIPTION       : Platform-specific routine to set the IP Destination Mask
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
void IPRoute::set_destMask(const String& dm)
{
	_destMask = dm;
}

/*
================================================================================
NAME              : set_nextHop
DESCRIPTION       : Platform-specific routine to set the Next Hop Address
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
void IPRoute::set_nextHop(const String& nh)
{
	_nextHop = nh;
}

/*
================================================================================
NAME              : set_protocolType
DESCRIPTION       : Platform-specific routine to set the Protocol Type
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
void IPRoute::set_protocolType(const String& pt)
{
	_protocolType = pt;
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
  int fd,                      // file descriptor
      i,                       // general purpose indicies
      count;                   // number of raw IP Routes
  struct nmparms parms;        // get_mib_info() arguments
  mib_ipRouteEnt * route_buf;  // IP Route Buffer
  unsigned int len;            // length of get_mib_info() buffer
  struct in_addr t;            // temporary variable for extracting
			       //   IP route buffer contents

#ifdef DEBUG
  cout << "RouteList::RouteList()" << endl;
#endif

  // Load the interface name structures.

  if ((fd = open_mib("/dev/ip", O_RDONLY, 0, 0)) < 0) {
	throw CIMOperationFailedException("Can't open /dev/ip: " +
				String(strerror(errno)));
  }

  parms.objid = ID_ipRouteNumEnt;
  parms.buffer = (char *) &count;
  len = sizeof(count);
  parms.len = (unsigned int *) &len;

  if (get_mib_info (fd, &parms) < 0) {
	throw CIMOperationFailedException(
	    "Can't get ID_ipRouteNumEnt from get_mib_info(): " +
	    String(strerror(errno)));
  }

  route_buf = (mib_ipRouteEnt *)malloc(count*sizeof(mib_ipRouteEnt));

  if (route_buf == 0)
  {
	free (route_buf);
	throw CIMOperationFailedException(
	    "Error in allocating space for the kernel interface table: " +
	    String(strerror(errno)));
  }

  parms.objid = ID_ipRouteTable;
  parms.buffer = (char *) route_buf;
  len = count * sizeof(mib_ipRouteEnt);
  parms.len = &len;

  if (get_mib_info (fd, &parms) < 0) {
	free (route_buf);
	throw CIMOperationFailedException(
	    "Can't get ID_ipRouteTable from get_mib_info(): " +
	    String(strerror(errno)));
  }

  // Create the IP Route List entries

  for (i=0; i < count ; i++)
  {

    IPRoute _ipr;

    // check to see that this is a valid type to represent
    if (route_buf[i].Type == 3 || route_buf[i].Type == 4)
    {
        t.s_addr = route_buf[i].Dest;
	_ipr.set_destAddr(inet_ntoa(t));

	t.s_addr = route_buf[i].Mask;
	_ipr.set_destMask(inet_ntoa(t));

	t.s_addr = route_buf[i].NextHop;
	_ipr.set_nextHop(inet_ntoa(t));

        // ATTN-LEW-2002-09-13: Enhance this to deal with IPv6 too.
	_ipr.set_protocolType(PROTOCOL_IPV4);

        _iprl.push_back(_ipr);   // Add another IP Route to the list
    }

  } /* for */

  close_mib(fd);
  free (route_buf);

#ifdef DEBUG
  cout << "RouteList::RouteList() -- done" << endl;
#endif

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
    int i;

    for (i = 0; i < _iprl.size(); i++)
    {
       String sda, sdm;
       Uint16 sat;

       if ( _iprl[i].getDestinationAddress(sda) &&
                  	String::equal(sda,destAddr) &&
            _iprl[i].getDestinationMask(sdm) &&
	                String::equal(sdm,destMask) &&
            _iprl[i].getAddressType(sat) &&
	                sat == addrType ) 
       {
	  ipRInst = _iprl[i];
	  return true;
       }

    }

#ifdef DEBUG
   cout << "RouteList::findRoute(): NOT FOUND destAddr=" << destAddr <<
    	", destMask=" << destMask <<
    	", addrType=" << addrType << endl;
#endif

    // IP Route not found
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
    return _iprl[index];
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
    return _iprl.size();
}

