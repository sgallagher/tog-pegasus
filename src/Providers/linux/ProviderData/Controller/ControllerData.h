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
//  Controller Data class
//

#ifndef Pegasus_ControllerData_h
#define Pegasus_ControllerData_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Array.h>


#include "ProviderData.h"
#include "LogicalDeviceData.h"
#include "ControllerInformation.h"

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

PEGASUS_NAMESPACE_BEGIN

/** A macro to create a function which simply returns the corresponding
 *  method in the "info" variable. */
#define BOUNCE_TO_INFO(type, x, y) type x y const { return info->x(); }


/** A class which implements the data in the CIM_Controller class. */
class ControllerData : public LogicalDeviceData {

protected:
  /// The ControllerInformation data associated with this ProviderData type
  ControllerInformation *info;
  
public:
  ControllerData(void) { info = NULL; }
  virtual ~ControllerData(void) { delete info; }

  virtual int initialize(void) { return 0; }
  virtual void terminate(void) {}

  void SetControllerInformation(ControllerInformation *obj) { info = obj; }

  BOUNCE_TO_INFO(CIMDateTime, GetTimeOfLastReset, (void));
  BOUNCE_TO_INFO(Uint16, GetProtocolSupported, (void));
  BOUNCE_TO_INFO(Uint32, GetMaxNumberControlled, (void));
  BOUNCE_TO_INFO(String, GetProtocolDescription, (void));
};



PEGASUS_NAMESPACE_END


#endif  /* Pegasus_ControllerData_h  */
