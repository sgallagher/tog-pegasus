//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ProviderManager.h"
#include <Pegasus/Common/FileSystem.h>

PEGASUS_NAMESPACE_BEGIN

ProviderManager::ProviderManager(void)
{
}

ProviderManager::~ProviderManager(void)
{
}

Message * ProviderManager::processMessage(Message * message, ProviderName providerName)
{
    // subclasses should implement this method. if not implented, the base
    // classes generates a generic failure.

    return(message);
}

ProviderName ProviderManager::findProvider(const ProviderName & providerName)
{
    return(_registrar.findProvider(providerName));
}

ProviderName ProviderManager::findProvider(const String & destinationPath)
{
    return(_registrar.findConsumerProvider(destinationPath));
}

Boolean ProviderManager::insertProvider(const ProviderName & providerName)
{
    return(_registrar.insertProvider(providerName));
}

Boolean ProviderManager::removeProvider(const ProviderName & providerName)
{
    return(_registrar.removeProvider(providerName));
}

String ProviderManager::_resolvePhysicalName(String physicalName)
{
    String temp;
    String root = ".";

    // fully qualify physical provider name (module), if not already done so.
    #if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
    temp = physicalName + String(".dll");
    #elif defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
    temp =  String("lib") + physicalName + String(".so");
    #elif defined(PEGASUS_OS_HPUX)
    # ifdef PEGASUS_PLATFORM_HPUX_PARISC_ACC
    temp =  String("lib") + physicalName + String(".sl");
    # else
    temp =  String("lib") + physicalName + String(".so");
    # endif
    #elif defined(PEGASUS_OS_OS400)
    // do nothing
    #else
    temp =  String("lib") + physicalName + String(".so");
    #endif

    temp =  FileSystem::getAbsoluteFileName(
                ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir")), temp);
    return temp;
}


PEGASUS_NAMESPACE_END
