//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ServiceCIMOMHandle_h
#define Pegasus_ServiceCIMOMHandle_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueue.h>

#include <Pegasus/Server/CIMServer.h>

#include <Pegasus/Repository/CIMRepository.h>

#include <Pegasus/Server/ProviderManager/ProviderManagerQueue.h>
#include <Pegasus/Server/ConfigurationManager/ConfigurationManagerQueue.h>

#include <Pegasus/Provider/CIMOMHandle.h>

PEGASUS_NAMESPACE_BEGIN

class CIMOperationRequestDispatcher;

class PEGASUS_SERVER_LINKAGE ServiceCIMOMHandle : public CIMOMHandle
{
public:
	/** */
	ServiceCIMOMHandle(void);

	/** */
	ServiceCIMOMHandle(
		MessageQueue * outputQueue,
		CIMServer * cimserver,
		CIMRepository * repository,
		ProviderManagerQueue * providerManager,
		ConfigurationManagerQueue * configurationManager);

	/** */
	virtual ~ServiceCIMOMHandle(void);

	ServiceCIMOMHandle & operator=(const ServiceCIMOMHandle & handle);

	CIMServer * getServer(void) { return(_server); }

	CIMRepository * getRepository(void) { return(_repository); }
	
	ProviderManagerQueue * getProviderManager(void) { return(_providerManager); }

	ConfigurationManagerQueue * getConfigurationManager(void) { return(_configurationManager); }

protected:	
	CIMServer * _server;
	CIMRepository * _repository;
	ProviderManagerQueue * _providerManager;
	ConfigurationManagerQueue * _configurationManager;

	friend CIMOperationRequestDispatcher;
};

PEGASUS_NAMESPACE_END

#endif
