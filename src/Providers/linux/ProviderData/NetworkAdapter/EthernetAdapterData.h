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
// Ethernet adapter data collection tool.
//

#ifndef Pegasus_EthernetAdapterData_h
#define Pegasus_EthernetAdapterData_h

#include <vector>

#include "NetworkAdapterData.h"

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

PEGASUS_NAMESPACE_BEGIN

/** The ethernet adapter class, which extends the network adapter class
 *  with ethernet-specific data.  Most of this data is only available if
 *  the mii-diag executable is found on the system.  That program is
 *  available from http://www.scyld.com/diag/. */
class EthernetAdapterData : public NetworkAdapterData {


  // We don't know many of these, so I make them "validated" data types,
  // any attempt to read uninitialized values will throw an exception.
protected:
  validated<Uint32> maxdatasize;
  validated<Uint32> symbol_errors;
  validated<Uint64> total_packets_sent, total_packets_received;
  validated<Uint32> alignment_errors;
  validated<Uint32> FCS_errors;
  validated<Uint32> single_collisions;
  validated<Uint32> multiple_collisions;
  validated<Uint32> sqe_errors;
  validated<Uint32> deferred_xmits;
  validated<Uint32> late_collisions;
  validated<Uint32> excessive_collisions;
  validated<Uint32> int_mac_xmit_errs;
  validated<Uint32> int_mac_recv_errs;
  validated<Uint32> carrier_sense_errors;
  validated<Uint32> frame_too_longs;
  Array<Uint16> capabilities;
  Array<String> capability_descriptions;
  Array<Uint16> enabled_capabilities;
  
public:
  EthernetAdapterData(String const &adap_name);

  ~EthernetAdapterData(void) {}

  int initialize(void);
  int initialize(IPV4IFInformation const *interface);
  void terminate(void) {}
  

  Uint32 GetMaxDataSize(void) const { return maxdatasize.getValue(); }
  Array<Uint16> const &GetCapabilities(void) const;
  Array<String> const &GetCapabilityDescriptions(void) const;
  Array<Uint16> const &GetEnabledCapabilities(void) const;
  Uint32 GetSymbolErrors(void) const { return symbol_errors.getValue(); }
  Uint64 GetTotalPacketsTransmitted(void) const { 
    return total_packets_sent.getValue(); 
  }
  Uint64 GetTotalPacketsReceived(void) const {
    return total_packets_received.getValue(); 
  }
  Uint32 GetAlignmentErrors(void) const { return alignment_errors.getValue(); }
  Uint32 GetFCSErrors(void) const { return FCS_errors.getValue(); }
  Uint32 GetSingleCollisionFrames(void) const {
    return single_collisions.getValue(); 
  }
  Uint32 GetMultipleCollisionFrames(void) const {
    return multiple_collisions.getValue(); 
  }
  Uint32 GetSQETestErrors(void) const { return sqe_errors.getValue(); }
  Uint32 GetDeferredTransmissions(void) const {
    return deferred_xmits.getValue(); 
  }
  Uint32 GetLateCollisions(void) const { return late_collisions.getValue(); }
  Uint32 GetExcessiveCollisions(void) const {
    return excessive_collisions.getValue(); 
  }
  Uint32 GetInternalMACTransmitErrors(void) const {
    return int_mac_xmit_errs.getValue(); 
  }
  Uint32 GetInternalMACReceiveErrors(void) const {
    return int_mac_recv_errs.getValue(); 
  }
  Uint32 GetCarrierSenseErrors(void) const {
    return carrier_sense_errors.getValue();
  }
  Uint32 GetFrameTooLongs(void) const { return frame_too_longs.getValue(); }

};

PEGASUS_NAMESPACE_END


#endif  /* Pegasus_EthernetAdapterData_h  */
