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
//  Generic CIM_LogicalDevice data elements.
//

#ifndef Pegasus_LogicalDeviceData_h
#define Pegasus_LogicalDeviceData_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/String.h>

#include "ProviderData.h"


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN


/** This macro can be used in any Get*() method when the appropriate data
 *  is not implemented.  The function calling the method should catch
 *  AccessedInvalidData exceptions and do whatever is appropriate when the
 *  data in question is not available. */
#define UNIMPLEMENTED throw AccessedInvalidData()

/** Implements the data elements in the CIM_LogicalDevice class as a type
 *  of ProviderData.  Well, actually, not a single data element is actually
 *  implemented, so "implements" is a bit of an overstatement.  This is the
 *  framework for logical devices. */
class LogicalDeviceData : public ProviderData {

private:
  String logical_id;
  
public:

  LogicalDeviceData(String const &name = "Unnamed Logical Device") { 
    logical_id = name; 
  }
  virtual ~LogicalDeviceData(void) {}

  String const &GetLogicalDeviceID(void) const { return logical_id; }
  void SetLogicalDeviceID(String const &s) { logical_id = s; }

  virtual bool GetPowerManagementSupported(void) const { UNIMPLEMENTED; }
  virtual Array<Uint16> const &GetPowerManagementCapabilities(void) const {
    UNIMPLEMENTED; }

  virtual Uint16 GetAvailability(void) const { UNIMPLEMENTED; }
  virtual Uint16 GetStatusInfo(void) const { UNIMPLEMENTED; }
  virtual Uint32 GetLastErrorCode(void) const { UNIMPLEMENTED; }
  virtual String const &GetErrorDescription(void) const { UNIMPLEMENTED; }
  virtual bool GetErrorCleared(void) const { UNIMPLEMENTED; }
  virtual Array<String> const &GetOtherIdentifyingInfo(void) const { 
    UNIMPLEMENTED; }
  virtual Array<String> const &GetIdentifyingDescriptions(void) const {
    UNIMPLEMENTED; }

  virtual Uint64 GetPowerOnHours(void) const { UNIMPLEMENTED; }
  virtual Uint64 GetTotalPowerOnHours(void) const { UNIMPLEMENTED; }
  virtual Array<Uint16> const &GetAdditionalAvailability(void) const {
    UNIMPLEMENTED; }

  virtual Uint64 GetMaxQuiesceTime(void) const { UNIMPLEMENTED; }

  // user-invoked actions  (unsupported by default)
  virtual Uint32 SetPowerState(Uint32 new_state) { return 1; }
  virtual Uint32 Reset(void) { return 1; }
  virtual Uint32 EnableDevice(bool newsetting) { return 1; }
  virtual Uint32 OnlineDevice(bool set_to_online) { return 1; }
  virtual Uint32 QuiesceDevice(bool cease_ops) { return 1; }
  virtual Uint32 SaveProperties(void) { return 1; }
  virtual Uint32 RestoreProperties(void) { return 1; }

};


PEGASUS_NAMESPACE_END

#endif  /* !Pegasus_LogicalDeviceData_h */
