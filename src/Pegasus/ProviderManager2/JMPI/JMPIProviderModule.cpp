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
// Modified By:
//      Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//		Yi Zhou, Hewlett-Packard Company(yi_zhou@hp.com)
//     Mike Day, IBM (mdday@us.ibm.com)
//     Adrian Schuur, IBM (schuur@de.ibm.com)
//     Josephine Eskaline Joyce, IBM (jojustin@in.ibm.com) for PEP#101
//
//%/////////////////////////////////////////////////////////////////////////////

#include "JMPIProviderModule.h"

#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/MessageLoader.h> //l10n
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/ProviderManager2/JMPI/JMPIProvider.h>
#include <Pegasus/ProviderManager2/JMPI/JMPIProviderManager.h>

PEGASUS_NAMESPACE_BEGIN

JMPIProviderModule::JMPIProviderModule(const String & fileName,
                                       const String & interfaceName)
    : _fileName(fileName),
    _interfaceName(interfaceName),
    _ref_count(0),
    _library(0)
{
   Uint32 i=fileName.find(".jar:");
   if (i==PEG_NOT_FOUND) {
      String msg="Invalid Location format for Java providers: "+fileName;
      throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,msg);
   }
   String jar=fileName.subString(0,i+4);
   _className=fileName.subString(i+5);

   if (jar[0]!='/')
      _fileName=JMPIProviderManager::resolveFileName(jar);
   else _fileName=jar;
}

JMPIProviderModule::~JMPIProviderModule(void)
{
}

ProviderVector JMPIProviderModule::load(const String & providerName)
{
   ProviderVector pv;
   JvmVector *jv;

   JNIEnv *env=JMPIjvm::attachThread(&jv);
   pv.jProvider=JMPIjvm::getProvider(env,_fileName,_className,
        providerName.getCString(),&pv.jProviderClass);
   JMPIjvm::detachThread();

    if (pv.jProvider == 0) {
        String s0 = "ProviderLoadFailure";
        throw Exception(MessageLoaderParms("ProviderManager.JMPIProviderModule.CANNOT_LOAD_LIBRARY",
            "$0 ($1:$2):Cannot load library",
            s0,
            _fileName,
            providerName));
    }

    _ref_count++;

    return pv;
}

void JMPIProviderModule::unloadModule(void)
{
    if (_ref_count.DecAndTestIfZero())
    {
        if(_library != 0)
        {
            System::unloadDynamicLibrary(_library);
            _library = 0;
        }
    }
}

PEGASUS_NAMESPACE_END
