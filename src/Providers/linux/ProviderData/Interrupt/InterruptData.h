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

#ifndef Pegasus_InterruptData_h
#define Pegasus_InterruptData_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMDateTime.h>
#include "ProviderData.h"
#include "DeviceLocator.h"
#include "InterruptInformation.h"

PEGASUS_NAMESPACE_BEGIN

/** InterruptData class.  This class is both a sort of iterator for 
 *  all interrupts on the system as well as a wrapper for holding the
 *  information of a specific interrupt.  */
class InterruptData : public ProviderData
{

public:
   InterruptData(void);
   ~InterruptData(void);

   int initialize(void);
   void terminate(void);

   Array<InterruptData *> GetAllInterrupts(void);
   InterruptData *GetFirstInterrupt(void);
   InterruptData *GetNextInterrupt(void);
   void EndGetInterrupt(void);
   InterruptData *GetInterrupt(const String &inIrq);

   String getIRQNumber() const;
   String getAvailability() const;
   String getTriggerType() const;

   /** getTriggerLevel() is hardcoded to return unknown.  Linux can't
    *  determine the trigger level.  */
   String getTriggerLevel() const {return "2";};

   /** getShareable() is hardcoded to return false.  */
   String getShareable() const {return "FALSE";};

   /** getHardware() is hardcoded to return true.  Linux does not
   expose the software interrupts to userspace.  As a result, these
   are all hardware interrupts. */
   String getHardware() const {return "TRUE";};

private:

   DeviceLocator deviceLocator;
   InterruptInformation *irq;

};

PEGASUS_NAMESPACE_END

#endif
