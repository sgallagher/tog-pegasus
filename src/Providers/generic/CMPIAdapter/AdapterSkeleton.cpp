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
// Author: Markus Mueller
//
//%////////////////////////////////////////////////////////////////////////////


#include <Pegasus/Common/Config.h>
#include "CMPIAdapter.h"

#define DDD(X) X

PEGASUS_USING_STD;


void CMPI_sigsegv( int s_n, siginfo_t * s_info, void * sig)
{
    fprintf(stderr, "Segmentation fault caught at %p in pid %d\n",
            s_info->si_addr, getpid());
    pthread_exit(NULL);
}

void CMPI_sigint( int s_n, siginfo_t * s_info, void * sig)
{
    fprintf(stderr, "Terminal interrupt caught at %p in pid %d\n",
            s_info->si_addr, getpid());
}

void CMPI_sigabrt(int s_n, siginfo_t * s_info, void * sig)
{
   fprintf(stderr, "Abort caught at %p in pid %d\n",
           s_info->si_addr, getpid());
}


PEGASUS_NAMESPACE_BEGIN

/**
Standard initialization function for the provider.
*/

void CMPIAdapter::initialize(CIMOMHandle& cimomHandle)
{
    DDD (cerr << "CMPIAdapter::initialize() at " << (void *)(this) <<endl;)
    DDD (cerr << "for provider " << _providerName <<endl;)

    // register SignalHandlers
    _signal = new SignalHandler();
    _signal->registerHandler(11,CMPI_sigsegv);
    _signal->activate(11);
    _signal->registerHandler(2,CMPI_sigint);
    _signal->activate(2);
    _signal->registerHandler(2,CMPI_sigabrt);
    _signal->activate(6);

    _cimom = cimomHandle;

    // load the library

    #ifdef PEGASUS_OS_TYPE_WINDOWS
    _libraryName = _providerName + String(".dll");
    #elif defined(PEGASUS_OS_HPUX)
    _libraryName = ConfigManager::getHomedPath(
        ConfigManager::getInstance()->getCurrentValue("providerDir"));
    _libraryName.append(
        String("/lib") + _providerName + String(".sl"));
    #else
    _libraryName = ConfigManager::getHomedPath(
        ConfigManager::getInstance()->getCurrentValue("providerDir"));
    _libraryName.append(
        String("/lib") + _providerName + String(".so"));
    #endif

    cerr << "try to load file " << _libraryName << endl;
    // dynamically load the provider library
    _library = System::loadDynamicLibrary(_libraryName.getCString());

    if(_library == 0)
    {
        String errorString = "Cannot load library, error: " + System::dynamicLoadError();
        throw Exception("ProviderLoadFailure (" + _libraryName + ":" + _providerName + "):" + errorString);
    }

    return;
}


void CMPIAdapter::terminate(void)
{
    // unload the library
    if (_library != 0)
    {
        System::unloadDynamicLibrary(_library);
        _library = 0;
    }

    // get rid of the signal handler
    delete _signal;
}

// CIMInstanceProvider interface

void CMPIAdapter::getInstance(
        const OperationContext & context,
        const CIMObjectPath & instanceReference,
        const Boolean includeQualifiers,
        const Boolean includeClassOrigin,
        const CIMPropertyList & propertyList,
        InstanceResponseHandler & handler)
{
   cerr<<"--- TestProvider::getInstance"<<endl;
   handler.processing();
   handler.complete();
}

void CMPIAdapter::deleteInstance(
        const OperationContext & context,
        const CIMObjectPath & instanceReference,
        InstanceResponseHandler & handler)
{
   cerr<<"--- TestProvider::deleteInstances"<<endl;
   //handler.processing();
   //handler.complete();
}

void CMPIAdapter::createInstance(
        const OperationContext & context,
        const CIMObjectPath & instanceReference,
        const CIMInstance & instanceObject,
        ObjectPathResponseHandler & handler)
{
   cerr<<"--- TestProvider::createInstances"<<endl;
   //handler.processing();
   //handler.complete();
}

void CMPIAdapter::modifyInstance(
        const OperationContext & context,
        const CIMObjectPath & instanceReference,
        const CIMInstance & instanceObject,
        const Boolean includeQualifiers,
        const CIMPropertyList & propertyList,
        InstanceResponseHandler & handler)
{
   cerr<<"--- TestProvider::modifyInstances"<<endl;
   handler.processing();
   handler.complete();
}

void CMPIAdapter::enumerateInstances(
        const OperationContext & context,
        const CIMObjectPath & classReference,
        const Boolean includeQualifiers,
        const Boolean includeClassOrigin,
        const CIMPropertyList & propertyList,
        InstanceResponseHandler & handler)
{
   cerr<<"--- TestProvider::enumerateInstances"<<endl;
   handler.processing();
   handler.complete();
}

void CMPIAdapter::enumerateInstanceNames(
        const OperationContext & context,
        const CIMObjectPath & classReference,
        ObjectPathResponseHandler & handler)
{
   cerr<<"--- TestProvider::enumerateInstanceNames"<<endl;
   handler.processing();
   handler.complete();
}

//
// CIMAssociationProvider interface
//

void CMPIAdapter::associators(
        const OperationContext & context,
        const CIMObjectPath & objectName,
        const String & associationClass,
        const String & resultClass,
        const String & role,
        const String & resultRole,
        const Boolean includeQualifiers,
        const Boolean includeClassOrigin,
        const CIMPropertyList & propertyList,
        ObjectResponseHandler & handler)
{
   cerr<<"--- TestProvider::associators"<<endl;
   handler.processing();
   handler.complete();
}

void CMPIAdapter::associatorNames(
        const OperationContext & context,
        const CIMObjectPath & objectName,
        const String & associationClass,
        const String & resultClass,
        const String & role,
        const String & resultRole,
        ObjectPathResponseHandler & handler)
{
   cerr<<"--- TestProvider::associatorNames"<<endl;
   handler.processing();
   handler.complete();
}

void CMPIAdapter::references(
        const OperationContext & context,
        const CIMObjectPath & objectName,
        const String & resultClass,
        const String & role,
        const Boolean includeQualifiers,
        const Boolean includeClassOrigin,
        const CIMPropertyList & propertyList,
        ObjectResponseHandler & handler)
{
   cerr<<"--- TestProvider::references"<<endl;
   handler.processing();
   handler.complete();
}

void CMPIAdapter::referenceNames(
        const OperationContext & context,
        const CIMObjectPath & objectName,
        const String & resultClass,
        const String & role,
        ObjectPathResponseHandler & handler)
{
   cerr<<"--- TestProvider::referenceNames"<<endl;
   handler.processing();
   handler.complete();
}
//
// CIMMethodProvider interface
//

void CMPIAdapter::invokeMethod(
        const OperationContext & context,
        const CIMObjectPath & objectReference,
        const String & methodName,
        const Array<CIMParamValue> & inParameters,
        MethodResultResponseHandler & handler)
{
   cerr<<"--- TestProvider::invokeMethod"<<endl;
   handler.processing();
   handler.complete();
}


// This is the dynamic entry point into this dynamic module. The name of
// this provider is "ConfigSettingProvider" which is appened to
// "PegasusCreateProvider_" to form a symbol name. This function is called
// by the ProviderTable to load this provider.
//
// NOTE: The name of the provider must be correct to be loadable.

extern "C" PEGASUS_EXPORT ProviderAdapter * PegasusCreateProviderAdapter(
         const String & adapterName, const String & providerName)
{
    cerr << adapterName << " activated with provider " << providerName << endl;
    return (new CMPIAdapter(adapterName ,providerName ));
}

PEGASUS_NAMESPACE_END

