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
//  Controller information class
//

#ifndef Pegasus_ControllerInformation_h
#define Pegasus_ControllerInformation_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/CIMDateTime.h>

#include "DeviceInformation.h"


PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

PEGASUS_NAMESPACE_BEGIN


/** These defines hold the CIM25 class identifiers for controller
 *  protocols. */

#define CONTROLLER_PROTOCOL_OTHER 1
#define CONTROLLER_PROTOCOL_UNKNOWN 2
#define CONTROLLER_PROTOCOL_EISA 3
#define CONTROLLER_PROTOCOL_ISA 4
#define CONTROLLER_PROTOCOL_PCI 5
#define CONTROLLER_PROTOCOL_ATA_ATAPI 6
#define CONTROLLER_PROTOCOL_FLEXIBLE_DISKETTE 7
#define CONTROLLER_PROTOCOL_1496 8
#define CONTROLLER_PROTOCOL_SCSI_PARALLEL 9
#define CONTROLLER_PROTOCOL_SCSI_FIBRECHANNEL 10
#define CONTROLLER_PROTOCOL_SCSI_SERIAL 11
#define CONTROLLER_PROTOCOL_SCSI_SERIAL_PROTOCOL2 12
#define CONTROLLER_PROTOCOL_SCSI_SERIAL_STORAGE 13
#define CONTROLLER_PROTOCOL_VESA 14
#define CONTROLLER_PROTOCOL_PCMCIA 15
#define CONTROLLER_PROTOCOL_USB 16
#define CONTROLLER_PROTOCOL_PARALLEL 17
#define CONTROLLER_PROTOCOL_ESCON 18
#define CONTROLLER_PROTOCOL_DIAGNOSTIC 19
#define CONTROLLER_PROTOCOL_I2C 20
#define CONTROLLER_PROTOCOL_POWER 21
#define CONTROLLER_PROTOCOL_HIPPI 22
#define CONTROLLER_PROTOCOL_MULTIBUS 23
#define CONTROLLER_PROTOCOL_VME 24
#define CONTROLLER_PROTOCOL_IPI 25
#define CONTROLLER_PROTOCOL_IEEE_488 26
#define CONTROLLER_PROTOCOL_RS232 27
#define CONTROLLER_PROTOCOL_IEEE_802_3_10BASE5 28
#define CONTROLLER_PROTOCOL_IEEE_802_3_10BASE2 29
#define CONTROLLER_PROTOCOL_IEEE_802_3_1BASE5 30
#define CONTROLLER_PROTOCOL_IEEE_802_3_10BROAD36 31
#define CONTROLLER_PROTOCOL_IEEE_802_3_100BASEVG 32
#define CONTROLLER_PROTOCOL_IEEE_802_3_TOKEN_RING 33
#define CONTROLLER_PROTOCOL_ANSI_X3T9_5_FDDI 34
#define CONTROLLER_PROTOCOL_MCA 35
#define CONTROLLER_PROTOCOL_ESDI 36
#define CONTROLLER_PROTOCOL_IDE 37
#define CONTROLLER_PROTOCOL_CMD 38
#define CONTROLLER_PROTOCOL_ST506 39
#define CONTROLLER_PROTOCOL_DSI 40
#define CONTROLLER_PROTOCOL_QIC2 41
#define CONTROLLER_PROTOCOL_ENHANCED_ATA_IDE 42
#define CONTROLLER_PROTOCOL_AGP 43
#define CONTROLLER_PROTOCOL_TWIRP 44
#define CONTROLLER_PROTOCOL_FIR 45
#define CONTROLLER_PROTOCOL_SIR 46
#define CONTROLLER_PROTOCOL_IRBUS 47


/** The DeviceInformation class for controllers, objects from this class
 *  are returned by DeviceLocator objects. */
class ControllerInformation : public DeviceInformation {

private:
  validated<CIMDateTime> t_of_last_reset;
  validated<Uint16> protocol_supported;
  validated<Uint32> max_num_ctrld;
  validated<String> protocol_desc;
  
public:
  ControllerInformation(void) { max_num_ctrld.setValue(0); }
  virtual ~ControllerInformation(void) {}
  
  void SetProtocolSupported(Uint16 val) { protocol_supported.setValue(val); }

  CIMDateTime GetTimeOfLastReset(void) { return t_of_last_reset.getValue(); }
  Uint16 GetProtocolSupported(void) { return protocol_supported.getValue(); }
  Uint32 GetMaxNumberControlled(void) { return max_num_ctrld.getValue(); }
  String GetProtocolDescription(void) { return protocol_desc.getValue(); }
};



PEGASUS_NAMESPACE_END


#endif  /* Pegasus_ControllerInformation_h  */
