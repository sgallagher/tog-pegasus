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
//  IPV4 data classes
//

#include <stdio.h>

#include <Pegasus/Common/System.h>
#include <Pegasus/Common/String.h>
#include "IPV4NetInformation.h"


PEGASUS_NAMESPACE_BEGIN


//////////////////////////////////////////
//
//   IPV4NetInformation methods
//
//////////////////////////////////////////

void IPV4NetInformation::ParseDottedQuad(String const &quad_string, 
					 validated<dottedquad_t> *quad) const
{
  int int_octets[4];
  int fields, i;
  dottedquad_t byte_octets;

  fields = sscanf(quad_string.getCString(), "%d.%d.%d.%d",
		  &int_octets[0], &int_octets[1],
		  &int_octets[2], &int_octets[3]);


  if (fields != 4)
    return;

  for (i = 0; i < 4; i++) {
    if (int_octets[i] < 0 || int_octets[i] > 255)
      return;
    byte_octets.octets[i] = (unsigned char) int_octets[i];
  }

  /* If we make it here, we have a valid dotted quad, set the value on the
   * validated type. */
  quad->setValue(byte_octets);
}


#define IPV4_ADDR_MAXSIZE 20

String IPV4NetInformation::RenderDottedQuad(validated<dottedquad_t> const *quad) const
{
  char renderbuffer[IPV4_ADDR_MAXSIZE];
  
  if (!quad->is_valid())
    return INVALID_ADDRESS_STRING;

  snprintf(renderbuffer, IPV4_ADDR_MAXSIZE, "%u.%u.%u.%u",
	   (int) quad->getValue().octets[0], (int) quad->getValue().octets[1],
	   (int) quad->getValue().octets[2], (int) quad->getValue().octets[3]);

  return String(renderbuffer);
}


bool IPV4NetInformation::DottedQuadAllZeroes(validated<dottedquad_t> const *address) const
{
  return (address->getValue().octets[0] == 0 &&
	  address->getValue().octets[1] == 0 &&
	  address->getValue().octets[2] == 0 &&
	  address->getValue().octets[3] == 0);
}


//////////////////////////////////////////
//
//   IPV4RouteInformation methods
//
//////////////////////////////////////////


IPV4RouteInformation::IPV4RouteInformation(void)
{
  ptype = IPV4_ROUTE_INFO;

  route_is_dynamic = false;
  interface = "";
}


// Parses a dotted-quad format address and assigns it to destination.
// Returns 0 on success, -1 on failure.
int IPV4RouteInformation::SetDestination(String const &dest_string)
{
  ParseDottedQuad(dest_string, &destination);

  if (destination.is_valid())
    return 0;
  else
    return -1;
}

// Parses a dotted-quad format address and assigns it to destination.
// Returns 0 on success, -1 on failure.
int IPV4RouteInformation::SetGateway(String const &gw_string)
{
  ParseDottedQuad(gw_string, &gateway);

  if (gateway.is_valid())
    return 0;
  else
    return -1;
}

// Parses a dotted-quad format address and assigns it to destination.
// Returns 0 on success, -1 on failure.
int IPV4RouteInformation::SetMask(String const &mask_string)
{
  ParseDottedQuad(mask_string, &mask);

  if (mask.is_valid())
    return 0;
  else
    return -1;
}


void IPV4RouteInformation::SetRouteDynamicFromFlags(String const &flags)
{
  route_is_dynamic = (flags.find('D') != PEG_NOT_FOUND ||
		      flags.find('R') != PEG_NOT_FOUND ||
		      flags.find('M') != PEG_NOT_FOUND);
}


String IPV4RouteInformation::print(void) const
{
  String retval = "";

  if (!destination.is_valid() ||
      !mask.is_valid() ||
      !gateway.is_valid())
    return "";

  if (route_is_dynamic)
    retval = "(dynamic) ";

  if (DottedQuadAllZeroes(&destination)) {
    if (!DottedQuadAllZeroes(&mask))
      return "Unrecognized route.  default destination with non-default mask.";

    retval.append("Default route:  out on interface \"" + GetInterface() + "\"");
    retval.append(" through the gateway machine at " + RenderDottedQuad(&gateway));
    return retval;
  }

  retval.append("Route to addresses " + RenderDottedQuad(&destination) + "/");
  retval.append(RenderDottedQuad(&mask) + ":  out on interface \"");
  retval.append(GetInterface() + "\"");
  if (!DottedQuadAllZeroes(&gateway))
    retval.append(" through the gateway machine at " + RenderDottedQuad(&gateway));

  return retval;
}


//////////////////////////////////////////
//
//   IPV4IFInformation methods
//
//////////////////////////////////////////

IPV4IFInformation::IPV4IFInformation(void)
{
  ptype = IPV4_INTERFACE_INFO;

  interface = encapsulation = hwaddr = "";
  status = flags = "";
}


int IPV4IFInformation::SetAddress(String const &addr_string)
{
  ParseDottedQuad(addr_string, &address);

  if (address.is_valid())
    return 0;
  else
    return -1;
}

int IPV4IFInformation::SetBcast(String const &bcast_string)
{
  ParseDottedQuad(bcast_string, &bcast);

  if (bcast.is_valid())
    return 0;
  else
    return -1;
}


int IPV4IFInformation::SetNetmask(String const &mask_string)
{
  ParseDottedQuad(mask_string, &netmask);

  if (netmask.is_valid())
    return 0;
  else
    return -1;
}


int IPV4IFInformation::SetMTU(String const &mtu_string)
{
  char *eptr;
  Uint32 hold_mtu;

  CString p;
  p = mtu_string.getCString();
  hold_mtu = strtoul(p, &eptr, 10);
  
  if (*eptr == 0) {
    mtu.setValue(hold_mtu);
    return 0;
  } else {
    return -1;
  }
}


int IPV4IFInformation::SetInterrupt(String const &interrupt_string)
{
  char *eptr;
  CString p;
  Uint32 int_hold;

  p = interrupt_string.getCString();
  int_hold = strtoul(p, &eptr, 10);
  
  if (*eptr == 0) {
    interrupt.setValue(int_hold);
    return 0;
  } else {
    return -1;
  }
}


int IPV4IFInformation::SetBaseAddr(String const &base_address_string)
{
  char *eptr;
  CString p;
  Uint32 hold_base;

  p = base_address_string.getCString();
  hold_base = strtoul(p, &eptr, 16);
  
  if (*eptr == 0) {
    base_address.setValue(hold_base);
    return 0;
  } else {
    return -1;
  }
}

void IPV4IFInformation::SetRxErrStats(Uint32 err, Uint32 drop, 
				 Uint32 over, Uint32 frame)
{
  rx_errors.setValue(err);
  rx_drops.setValue(drop);
  rx_overruns.setValue(over); 
  rx_frame.setValue(frame);
}


void IPV4IFInformation::SetTxErrStats(Uint32 err, Uint32 drop, 
				 Uint32 over, Uint32 frame)
{
  rx_errors.setValue(err); 
  rx_drops.setValue(drop);
  rx_overruns.setValue(over);
  rx_frame.setValue(frame);
}

Uint32 IPV4IFInformation::GetMTU(void) const
{
  return mtu.getValue();
}


Uint32 IPV4IFInformation::GetInterrupt(void) const
{
  return interrupt.getValue();
}


Uint32 IPV4IFInformation::GetBaseAddr(void) const
{
  return base_address.getValue();
}

String IPV4IFInformation::print(void) const
{
  String retval, holdvar;
  Uint32 hold_int;
  char render_uint32[10];  /* big enough to hold a uint32 in decimal, plus
			    * a few bytes */

  retval = "Interface \"" + GetInterface() + "\":  " + GetEncapsulation();
  if (GetHWAddr() != "")
    retval.append(" on hardware address " + GetHWAddr());
  retval.append(".");
  if (address.is_valid()) {
    retval.append("  Configured on address " + GetAddress());
    if (bcast.is_valid()) {
      retval.append(" with broadcast address " + GetBcast());
    } else {
      retval.append(" with no broadcast address ");
    }
    retval.append(" and netmask " + GetNetmask() + ".");
  }

  retval.append("  MTU is ");
  try {
    hold_int = GetMTU();
    sprintf(render_uint32, "%u.", hold_int);
    holdvar.append(render_uint32);
  } 
  catch (AccessedInvalidData &e) {
    holdvar = "unset.";
  }
  retval.append(holdvar);

  holdvar = "";
  try {
    hold_int = GetInterrupt();
    sprintf(render_uint32, "%u", hold_int);
    holdvar = "  Configured on interrupt " + String(render_uint32) + ".";
  }
  catch (AccessedInvalidData &e) {
  }
  retval.append(holdvar);

  holdvar = "";
  try {
    hold_int = GetBaseAddr();
    sprintf(render_uint32, "%x", hold_int);
    holdvar = "  Located at base address 0x" + String(render_uint32) + ".";
  }
  catch (AccessedInvalidData &e) {
  }
  retval.append(holdvar);

  return retval;
}


PEGASUS_NAMESPACE_END
