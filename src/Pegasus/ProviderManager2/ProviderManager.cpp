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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Sean Keenan, Hewlett-Packard Company <sean.keenan@hp.com>
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ProviderManager.h"
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Config/ConfigManager.h>

PEGASUS_NAMESPACE_BEGIN

ProviderManager::ProviderManager(void)
{
}

ProviderManager::~ProviderManager(void)
{
}

String ProviderManager::_resolvePhysicalName(String physicalName)
{
    String fileName = FileSystem::buildLibraryFileName(physicalName);

#if defined(PEGASUS_OS_VMS)
    String temp, temp2;

    temp =  String("/") + fileName + String(".exe");

    temp2 = FileSystem::getAbsoluteFileName(
                           ConfigManager::getInstance()->getCurrentValue("providerDir"),
                         temp);
    if (temp2 == String::EMPTY)
    {
      return temp2;
    }
    else
    {
      return (ConfigManager::getInstance()->getCurrentValue("providerDir") + temp);
    }
#else
#ifndef PEGASUS_OS_OS400
    fileName = FileSystem::getAbsoluteFileName(
        ConfigManager::getHomedPath(
            ConfigManager::getInstance()->getCurrentValue("providerDir")),
        fileName);
#endif

    return fileName;
#endif
}

void ProviderManager::setIndicationCallback(
        PEGASUS_INDICATION_CALLBACK indicationCallback)
{
    _indicationCallback = indicationCallback;
}

void ProviderManager::setSubscriptionInitComplete
    (Boolean subscriptionInitComplete)
{
    _subscriptionInitComplete = subscriptionInitComplete;
}

PEGASUS_NAMESPACE_END
