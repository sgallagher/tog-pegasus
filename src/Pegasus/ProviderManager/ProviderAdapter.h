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
// Author: Markus Mueller (sedgewick_de@yahoo.de)
//
// Modified By: Adrian Schuur - schuur@de.ibm.com
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ProviderAdapter_h
#define Pegasus_ProviderAdapter_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Sharable.h>
#include <Pegasus/Common/IPC.h>

#include <Pegasus/Provider/CIMProvider.h>
#include <Pegasus/Server/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

// 
// the ProviderAdapter serves as a coupling between a
// CIMProvider and an arbitrary file

class PEGASUS_SERVER_LINKAGE ProviderAdapter
{
public:
	ProviderAdapter(const String & adapterName,
                        const String & providerName,
                        const String & modName);
	virtual ~ProviderAdapter(void);
	const String & getAdapterName(void) const;
	const String & getProviderName(void) const;
	virtual CIMProvider * getProvider(void) const;

protected:
	String _adapterName; //_adapterName;
	String _providerLocation;
	String _providerName;
	String _className;
	CIMProvider * _adapter;
};

//
// this class manages all adapters
//

class PEGASUS_SERVER_LINKAGE ProviderAdapterManager
{
public:
	ProviderAdapterManager(void);
	virtual ~ProviderAdapterManager(void);

	ProviderAdapter * addAdapter(const String & adapterName,
	                             const String & adapterFileName,
	                             const String & providerLocation,
                                     const String & providerName);
	void deleteAdapter(const String & adapterName);
	void deleteAdapter(const DynamicLibraryHandle & adapterlib);
        static ProviderAdapterManager * get_pamgr();
        void list(void);

private:
	Array<String> _listOfAdapterNames;
	Array<ProviderAdapter *> _listOfAdapters;
	Array<DynamicLibraryHandle> _listOfAdapterLibs;
	Array<Uint32> _listOfAdapterCounts;
        Mutex _listMutex;
        DynamicLibraryHandle loadAdapter(const String & adapterName,
                                          const String & adapterFileName);
        ProviderAdapter * loadProvider(const DynamicLibraryHandle & library,
                                const String & adapterName,
	                        const String & providerLocation,
	                        const String & providerName);
};

PEGASUS_NAMESPACE_END

#endif /* ProviderAdapter_h */
