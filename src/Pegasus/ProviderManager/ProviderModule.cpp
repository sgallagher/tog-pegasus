//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
//      Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//		Yi Zhou, Hewlett-Packard Company(yi_zhou@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ProviderModule.h"

#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>

PEGASUS_NAMESPACE_BEGIN

ProviderModule::ProviderModule(const String & fileName)
   : _fileName(fileName), 
     _ref_count(0),
     _library(0)
{
    _library = System::loadDynamicLibrary((const char *)_fileName.getCString());
}

ProviderModule::~ProviderModule(void)
{ 

}

CIMBaseProvider *ProviderModule::load(const String & providerName)
{
   CIMBaseProvider *provider = 0;


    // dynamically load the provider library

    if(_library == 0)
    {
       _library = System::loadDynamicLibrary((const char *)_fileName.getCString());
    }
    
    if(_library == 0)
    {

        // ATTN: does unload() need to be called?

        String errorString = "Cannot load library, error: " + System::dynamicLoadError();
        throw Exception("ProviderLoadFailure (" + _fileName + ":" + providerName + "):" + errorString);
    }

    // find libray entry point
    CIMBaseProvider * (*createProvider)(const String &) = 0;

    createProvider = (CIMBaseProvider * (*)(const String &))System::loadDynamicSymbol(
        _library, "PegasusCreateProvider");

    if(createProvider == 0)
    {
        String errorString = "entry point not found.";
        throw Exception("ProviderLoadFailure (" + _fileName + ":" + providerName + "):" + errorString);
    }

    // invoke the provider entry point
    provider = createProvider(providerName);
    
    if(provider == 0)
    {
        String errorString = "entry point returned null.";
        throw Exception("ProviderLoadFailure (" + _fileName + ":" + providerName + "):" + errorString);
    }

    // test for the appropriate interface
    if(dynamic_cast<CIMBaseProvider *>(provider) == 0)
    {
        String errorString = "provider is not a CIMBaseProvider.";
        throw Exception("ProviderLoadFailure (" + _fileName + ":" + providerName + "):" + errorString);
    }
    _ref_count++;
    return provider;
}

void ProviderModule::unloadModule(void)
{
  _ref_count--;
   if( _ref_count.value() > 0)
      return;
   _ref_count = 0;
   if(_library != 0)
     {
       System::unloadDynamicLibrary(_library);
       _library = 0;
     }
}

Boolean ProviderModule::operator == (const void *key) const 
{
   String *prov = reinterpret_cast<String *>(const_cast<void *>(key));
   if(String::equalNoCase(_fileName, *prov))
      return true;
   return false;
}

Boolean ProviderModule::operator == (const ProviderModule &pm) const
{
   if(String::equalNoCase(_fileName, pm._fileName))
      return true;
   return false;
}

PEGASUS_NAMESPACE_END
