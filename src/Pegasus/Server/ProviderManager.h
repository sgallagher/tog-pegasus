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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//              Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//              Yi Zhou, Hewlett-Packard Company(yi_zhou@hp.com)
//              Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ProviderManager_h
#define Pegasus_ProviderManager_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Thread.h>

#include <Pegasus/Server/ProviderModule.h>
#include <Pegasus/Server/ServiceCIMOMHandle.h>
#include <Pegasus/Server/ProviderBlockedEntry.h>

#include <Pegasus/Provider/ProviderHandle.h>
#include <Pegasus/Provider/ProviderException.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_SERVER_LINKAGE ProviderManager
{
public:
	ProviderManager(MessageQueue * outputQueue, CIMRepository * repository, CIMServer * server);
	virtual ~ProviderManager(void);

	ProviderHandle * getProvider(const String & providerName, const String & className);

	void addProviderToTable(const String & providerName, Boolean BlockFlag);
	void removeProviderFromTable(const String & providerName);
	Uint32 blockProvider(const String & providerName);
	Uint32 unblockProvider(const String & providerName);
	Uint32 stopProvider(const String & providerName);
	Boolean isProviderBlocked(const String & providerName);
	void createProviderBlockTable(Array<CIMNamedInstance> & instances);

        void shutdownAllProviders(const String & providerName, const String & className);

protected:
	static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL monitorThread(void * arg);

	CIMOMHandle _cimom;
	ServiceCIMOMHandle _serviceCimom;
	Array<ProviderModule> _providers;
	Array<ProviderBlockedEntry> _providerBT;

private:
	void _addProviderToTable(const String & providerName, Boolean blockFlag);
	Uint32 _stopProvider(Uint32 providerIndex);

};

PEGASUS_NAMESPACE_END

#endif
