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
//              Josephine Eskaline Joyce (jojustin@in.ibm.com) for PEP#101
//
//%////////////////////////////////////////////////////////////////////////////
//
//  Generic network adapter
//


#include "PCIControllerData.h"
#include <Pegasus/Common/AutoPtr.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

#define CLASSNAME "PCIControllerData"


/** The initialize(void) method is used to load the ProviderData object
 *  with data from the next PCI device to be located in the scan. */
int PCIControllerData::initialize(void)
{
  if (searcher == NULL)
    searcher = new DeviceLocator(WILDCARD_PCI_DEVICE);

  info = (ControllerInformation *) searcher->getNextDevice();
  if (info == NULL)
    return 1;

  SetLogicalDeviceID(info->getUniqueKey());

  return 0;
}


/** The GetNext() method creates a new PCIControllerData object, and passes
 *  the searcher to it.  Note that it hands the searcher value off as a
 *  token, NULLing out its own copy of the pointer to the searcher, so that
 *  calling the destructor on the original object won't close the data
 *  stream for its Next object. */
PCIControllerData *PCIControllerData::GetNext(void)
{ 
  AutoPtr<PCIControllerData> retval; 

  retval.reset(new PCIControllerData());

  if (retval->initialize(searcher) == 1) {
    searcher = NULL;
    return NULL;
  }

  searcher = NULL; /* We've passed it to the new one, we may no longer
		    * delete it ourselves */
  return retval.release();
}


PEGASUS_NAMESPACE_END
