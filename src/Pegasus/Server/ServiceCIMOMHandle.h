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

PEGASUS_NAMESPACE_BEGIN

class MessageQueue;
class CIMServer;
class CIMRepository;
class ProviderManagerQueue;
class ConfigurationManagerQueue;

// ATTN: the ServiceCIMOMHandle does not derive from CIMOMHandle
// because it can go directly to a component.
class PEGASUS_SERVER_LINKAGE ServiceCIMOMHandle
{
public:
	/** */
	ServiceCIMOMHandle(void);

	/** */
	ServiceCIMOMHandle(
		MessageQueue * outputQueue,
		CIMServer * cimserver,
		CIMRepository * repository);

	/** */
	virtual ~ServiceCIMOMHandle(void);

	ServiceCIMOMHandle & operator=(const ServiceCIMOMHandle & handle);

	CIMServer * getServer(void) { return(_server); }

	CIMRepository * getRepository(void) { return(_repository); }
	
	ProviderManagerQueue * getProviderManager(void)
	{
		if(_providerManager == 0)
		{
			// ATTN: temporary solution to avoid passing component pointers in the
			// constructor.
			_providerManager =
				(ProviderManagerQueue *)MessageQueue::lookup("Server_ProviderManagerQueue");
		}
		
		return(_providerManager);
	}

	ConfigurationManagerQueue * getConfigurationManager(void)
	{
		if(_configurationManager == 0)
		{

			// ATTN: temporary solution to avoid passing component pointers in the
			// constructor.
			_configurationManager =
				(ConfigurationManagerQueue *)MessageQueue::lookup("Server_ConfiguratioknManagerQueue");
		}

		return(_configurationManager);
	}


protected:	
	CIMServer * _server;
	CIMRepository * _repository;
	ProviderManagerQueue * _providerManager;
	ConfigurationManagerQueue * _configurationManager;

};

PEGASUS_NAMESPACE_END

#endif
