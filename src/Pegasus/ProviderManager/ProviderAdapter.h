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
// Author: Markus Mueller (sedgewick_de@yahoo.de)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ProviderAdapter_h
#define Pegasus_ProviderAdapter_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Sharable.h>
#include <Pegasus/Common/IPC.h>

#include <Pegasus/Provider/CIMBaseProvider.h>

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T DynamicLibraryHandle
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

// 
// the ProviderAdapter serves as a coupling between a
// CIMBaseProvider and an arbitrary file

class PEGASUS_SERVER_LINKAGE ProviderAdapter
{
public:
	ProviderAdapter(const String & adapterName,
                        const String & providerName,
                        const String & className = String::EMPTY);

	virtual ~ProviderAdapter(void);

	const String & getAdapterName(void) const;

	const String & getProviderName(void) const;

	//void load(void);
	//void unload(void);

	virtual CIMBaseProvider * getBaseProvider(void) const;

protected:
	String _adapterName;
	String _providerName;
	String _className;
	CIMBaseProvider * _adapter;
};

//
// this class manages all adapters
//

//#define PEGASUS_ARRAY_T ProviderAdapter * 
//#include <Pegasus/Common/ArrayInter.h>
//#undef PEGASUS_ARRAY_T


class PEGASUS_SERVER_LINKAGE ProviderAdapterManager
{
public:
	ProviderAdapterManager(void);
	virtual ~ProviderAdapterManager(void);

	ProviderAdapter * addAdapter(const String & adapterName,
	                             const String & adapterFileName,
	                             const String & providerName,
                                     const String & className = String::EMPTY);
	void deleteAdapter(const String & adapterName);
	void deleteAdapter(const DynamicLibraryHandle & adapterlib);

	//void load(void);
	//void unload(void);

	//virtual CIMBaseProvider * getProvider(void) const;
        static ProviderAdapterManager * get_pamgr();
        void list(void);

private:
	Array<String> _listOfAdapterNames;
	Array<ProviderAdapter *> _listOfAdapters;
	Array<DynamicLibraryHandle> _listOfAdapterLibs;
	Array<Uint32> _listOfAdapterCounts;
        Mutex _listMutex;
        DynamicLibraryHandle _loadlibrary(const String & adapterName,
                                          const String & adapterFileName);
        ProviderAdapter * _load(const DynamicLibraryHandle & library,
                                const String & adapterName,
	                        const String & providerName,
                                const String & className = String::EMPTY);
};

PEGASUS_NAMESPACE_END

#endif /* ProviderAdapter_h */
