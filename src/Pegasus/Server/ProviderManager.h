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
// Author: Chip Vincent
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ProviderManager_h
#define Pegasus_ProviderManager_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/CIMOMHandle.h>
#include <Pegasus/Provider/CIMProvider.h>

PEGASUS_NAMESPACE_BEGIN

class ProviderFailure : public Exception
{
public:
	ProviderFailure(const String & fileName, const String & className);

};

class ProviderLoadFailure : public ProviderFailure
{
public:
    ProviderLoadFailure(const String & fileName, const String & className);

};

class ProviderCreateFailure : public ProviderFailure
{
public:
	ProviderCreateFailure(const String & fileName, const String & className);

};

class ProviderInitializationFailure : public ProviderFailure
{
public:
	ProviderInitializationFailure(const String & fileName, const String & className);

};

class ProviderTerminationFailure : public ProviderFailure
{
public:
	ProviderTerminationFailure(const String & fileName, const String & className);

};

class ProviderModule
{
public:
	ProviderModule(const String & fileName, const String & className) throw();
	virtual ~ProviderModule(void) throw();
	
	operator CIMProvider * (void) const throw() { return(_provider); }
	
	const String & getFileName(void) const throw() { return(_fileName); }
	const String & getClassName(void) const throw() { return(_className); }
		
protected:
	void load(void);
	void unload(void);

protected:
	String _fileName;
	String _className;
		
	DynamicLibraryHandle _libraryHandle;
	CIMProvider * _provider;
	
};

class PEGASUS_SERVER_LINKAGE ProviderManager
{
public:
    ProviderManager(CIMOMHandle & cimomHandle) throw();
	virtual ~ProviderManager(void) throw();

    CIMProvider * getProvider(const String & fileName, const String & className) throw(ProviderFailure);

protected:
	CIMOMHandle				_cimom;
	Array<ProviderModule>	_providers;

};

PEGASUS_NAMESPACE_END

#endif
