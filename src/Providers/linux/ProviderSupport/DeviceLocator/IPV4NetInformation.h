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
// Modified By: David Kennedy       <dkennedy@linuxcare.com>
//              Christopher Neufeld <neufeld@linuxcare.com>
//              Al Stone            <ahs3@fc.hp.com>
//
//%////////////////////////////////////////////////////////////////////////////
//
//  Information about network parameters
//

#ifndef Pegasus_NetInformation_h
#define Pegasus_NetInformation_h

#include <string.h>

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>

#include "ProviderData.h"
#include "DeviceInformation.h"

/* Device list */
#include "DeviceTypes.h"


PEGASUS_NAMESPACE_BEGIN



/// used to represent 0.0.0.0 IPv4 addresses where that value is meaningless
#define INVALID_ADDRESS_STRING "<<<UNDEFINED>>>"

static const struct {
  String ifconfig_string;
  Uint32 prog_if_value;
} encap_lookup[] = {
  { "Local Loopback", Device_IPV4NetworkSettings_Interfaces_LocalLoopback },
  { "Ethernet", Device_IPV4NetworkSettings_Interfaces_Ethernet },
};

typedef struct {
  unsigned char octets[4];
} dottedquad_t;


enum net_param_type {
  IPV4_INVALID_TYPE, IPV4_ROUTE_INFO, IPV4_INTERFACE_INFO
};


/** A generic information type for IPv4 pseudo-devices (IP routes and
 *  network interfaces). */
class IPV4NetInformation : public DeviceInformation {

 protected:
  /// what sort of pseudo-device this is.
  enum net_param_type ptype;

 public:
  IPV4NetInformation(void) { ptype = IPV4_INVALID_TYPE; }
  virtual ~IPV4NetInformation(void) { }

 public:
  enum net_param_type GetParamType(void) const { return ptype; }

  /// Produce a printable string describing the information about the object
  virtual String print(void) const = 0;

 protected:
  /// Convert the String representation of an IPv4 address to 4 octets
  void ParseDottedQuad(String const &quad_string, 
		       validated<dottedquad_t> *parsed) const;

  /// Convert the 4 octets of an IPv4 address to a String
  String RenderDottedQuad(validated<dottedquad_t> const *object) const;

  /// Returns true of the 4 octets of an IPv4 address are all zero.
  bool DottedQuadAllZeroes(validated<dottedquad_t> const *address) const;
};


/** Information about IPv4 network routes. */
class IPV4RouteInformation : public IPV4NetInformation {

 private:
  validated<dottedquad_t> destination;
  validated<dottedquad_t> gateway;
  validated<dottedquad_t> mask;

  /// The interface through which packets on this route should be sent
  String interface;

  /// Set if the route is dynamic, maintained by daemons on the system
  bool route_is_dynamic;

 public:
  IPV4RouteInformation(void);
  virtual ~IPV4RouteInformation(void) { }

  int SetDestination(String const &dest_string);
  int SetGateway(String const &gateway_string);
  int SetMask(String const &mask_string);
  void SetInterface(String const &interface_string) 
    { 
      interface = interface_string; 
    }
  void SetRouteDynamicFromFlags(String const &flags);

  String GetDestination(void) const { return RenderDottedQuad(&destination);}
  String GetGateway(void) const { return RenderDottedQuad(&gateway); }
  String GetMask(void) const { return RenderDottedQuad(&mask); }
  String const &GetInterface(void) const { return interface; }
  bool GetRouteIsDynamic(void) const { return route_is_dynamic; }

  virtual String print(void) const;
};


/** Information class holding data about network interfaces configured for
 *  IPv4 transactions. */
class IPV4IFInformation : public IPV4NetInformation {

 private:
  /// A unique identifier for the interface
  String interface;

  String encapsulation;
  String hwaddr;
  validated<dottedquad_t> address;
  validated<dottedquad_t> bcast;
  validated<dottedquad_t> netmask;
  validated<Uint32> mtu;
  validated<Uint32> interrupt;
  validated<Uint32> base_address;
  String status;
  String flags;
  validated<Uint64> tx_bytes, rx_bytes, tx_packs, rx_packs;
  validated<Uint32> tx_errors, tx_drops, tx_overruns, tx_carrier;
  validated<Uint32> rx_errors, rx_drops, rx_overruns, rx_frame;
  validated<Uint32> collisions;
  
  
 public:
  IPV4IFInformation(void);
  virtual ~IPV4IFInformation(void) { }

  void SetInterface(String const &interface_string) { interface = interface_string; }
  void SetEncapsulation(String const &encap_string) { encapsulation = encap_string; }
  void SetHWAddr(String const &hwaddr_string) { hwaddr = hwaddr_string; }
  int SetAddress(String const &addr_string);
  int SetBcast(String const &bcast_string);
  int SetNetmask(String const &netmask_string);
  int SetMTU(String const &mtu_string);
  int SetInterrupt(String const &interrupt_string);
  int SetBaseAddr(String const &baseaddr_string);
  void SetStatus(String const &status_string) { status = status_string; }
  void SetFlags(String const &flags_string) { flags = flags_string; }
  void SetRxPackets(Uint64 npacks) { rx_packs.setValue(npacks); }
  void SetTxPackets(Uint64 npacks) { tx_packs.setValue(npacks); }
  void SetRxBytes(Uint64 nbytes) { rx_bytes.setValue(nbytes); }
  void SetTxBytes(Uint64 nbytes) { tx_bytes.setValue(nbytes); }
  void SetRxErrStats(Uint32 err, Uint32 drop, Uint32 over, Uint32 frame);
  void SetTxErrStats(Uint32 err, Uint32 drop, Uint32 over, Uint32 frame);
  void SetCollisions(Uint32 colls) { collisions.setValue(colls); }
  
  String const &GetInterface(void) const { return interface; }
  String const &GetEncapsulation(void) const { return encapsulation; }
  String const &GetHWAddr(void) const { return hwaddr; }
  String GetAddress(void) const { return RenderDottedQuad(&address); }
  String GetBcast(void) const { return RenderDottedQuad(&bcast); }
  String GetNetmask(void) const { return RenderDottedQuad(&netmask); }

  // If you use one of these next three functions, be sure to catch the
  // UninitializedValue exception, which can be thrown if that data was not
  // loaded.
  Uint32 GetMTU(void) const;
  Uint32 GetInterrupt(void) const;
  Uint32 GetBaseAddr(void) const;

  String const &GetStatus(void) const { return status; }
  String const &GetFlags(void) const { return flags; }

  Uint64 GetRxPackets(void) const { return rx_packs.getValue(); }
  Uint64 GetTxPackets(void) const { return tx_packs.getValue(); }
  Uint64 GetRxBytes(void) const { return rx_bytes.getValue(); }
  Uint64 GetTxBytes(void) const { return tx_bytes.getValue(); }
  Uint64 GetCollisions(void) const { return collisions.getValue(); }


  virtual String print(void) const;
};



PEGASUS_NAMESPACE_END

#endif  /* Pegasus_NetInformation_h */
