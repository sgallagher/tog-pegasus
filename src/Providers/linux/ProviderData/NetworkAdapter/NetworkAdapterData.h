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
// Network adapter data collection tool.
//

#ifndef Pegasus_NetworkAdapterData_h
#define Pegasus_NetworkAdapterData_h

#include <vector>

#include <Pegasus/Common/String.h>

#include "IPV4NetInformation.h"
#include "LogicalDeviceData.h"

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

PEGASUS_NAMESPACE_BEGIN


/** A list of the different types of network adapters.  Note that this list
 *  is separate from the list of different network adapter providers.  A
 *  NetworkAdapterData object must be assigned a value from this type. */
enum network_adapter_types {
  NETWORK_ADAPTER_LO, NETWORK_ADAPTER_ETHERNET, NETWORK_ADAPTER_TOKENRING,
  NETWORK_ADAPTER_FIBRECHANNEL,
  NETWORK_ADAPTER_INVALID
};


/** The ProviderData class for network adapters.  Some types of adapters
 *  might subclass this, extending it.  At present, only the ethernet
 *  adapter type does so.  All other types are treated simply as their
 *  generic network adapter types. */
class NetworkAdapterData : public LogicalDeviceData {

protected:
  /// The unique system name of the adapter.  Under Linux, this might be eth0
  String name;

  /// The type of adapter
  enum network_adapter_types adapter_type;

  validated<Uint64> speed, max_speed;
  validated<bool> full_duplex, autosense;
  validated<Uint64> bytes_sent, bytes_received;
  String permanent_address;

  /// The network addresses associated with this interface.
  Array<String> network_addresses;
  
public:

  NetworkAdapterData(String const &adapter_name) { 
    adapter_type = NETWORK_ADAPTER_INVALID;
    permanent_address = "";
    name = adapter_name;
  }

  virtual ~NetworkAdapterData(void) {}
  
  int initialize(void);
  int initialize(IPV4IFInformation const *interface);
  virtual void terminate(void) {}

  /** list_all_adapters() is a static method which returns the unique
   *  system names of all adapters in the system */
  static vector<String> list_all_adapters(void);

  virtual String const &GetPermanentAddress(void) const { 
    return permanent_address; }
  virtual Array<String> const &GetNetworkAddresses(void) const {
    return network_addresses;
  }
  
  String GetName(void) const { return name; }
  Uint64 GetSpeed(void) const { return speed.getValue(); }
  Uint64 GetMaxSpeed(void) const { return max_speed.getValue(); }
  bool GetFullDuplex(void) const { return full_duplex.getValue(); }
  bool GetAutoSense(void) const { return autosense.getValue(); }
  Uint64 GetOctetsTransmitted(void) const { return bytes_sent.getValue(); }
  Uint64 GetOctetsReceived(void) const { return bytes_received.getValue(); }

  void SetMaxSpeed(Uint64 val) { max_speed.setValue(val); }
  void SetFullDuplex(bool val) { full_duplex.setValue(val); }
  void SetAutoSense(bool val) { autosense.setValue(val); }

  enum network_adapter_types data_type(void) const { return adapter_type; }

};



PEGASUS_NAMESPACE_END


#endif  /* Pegasus_NetworkAdapterData_h  */
