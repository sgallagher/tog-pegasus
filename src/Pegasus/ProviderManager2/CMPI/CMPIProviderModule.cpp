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
//              Josephine Eskaline Joyce (jojustin@in.ibm.com) for PEP#101
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CMPI_Version.h"

#include "CMPIProviderModule.h"

#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/MessageLoader.h> //l10n
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/ProviderManager2/CMPI/CMPIProvider.h>
#include <Pegasus/ProviderManager2/CMPI/CMPIProviderManager.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

CMPIProviderModule::CMPIProviderModule(const String & fileName)
    : _fileName(fileName),
    _ref_count(0),
    _library(0)
{
   genericProviderModule=0;
#ifdef PEGASUS_OS_TYPE_WINDOWS
   if (fileName[1] != ':')
#else
   if (fileName[0]!='/')
#endif
      _fileName=ProviderManager::_resolvePhysicalName(fileName);
   else _fileName=fileName;
}

CMPIProviderModule::~CMPIProviderModule(void)
{
}

ProviderVector CMPIProviderModule::load(const String & providerName)
{
   String realProviderName(providerName);
   realProviderName.remove(0,1);
   if (!genericProviderModule)
    _library = System::loadDynamicLibrary((const char *)_fileName.getCString());

    if(_library == 0)
    {

        throw Exception(MessageLoaderParms("ProviderManager.CMPI.CMPIProviderModule.CANNOT_LOAD_LIBRARY",
            "ProviderLoadFailure: ($0:$1):Cannot load library, error: $2",
            _fileName,
            realProviderName,
            System::dynamicLoadError()));
    }

    char symbolName[512];
    CString mName=realProviderName.getCString();
    ProviderVector miVector;
    memset(&miVector,0,sizeof(ProviderVector));
    int specificMode=0;

    if ((miVector.createGenInstMI=(CREATE_GEN_INST_MI)
           System::loadDynamicSymbol(_library,_Generic_Create_InstanceMI))) {
       if (miVector.createGenInstMI)
         miVector.miTypes|=CMPI_MIType_Instance;
       miVector.genericMode=1;
    }

    strcpy(symbolName,(const char*)mName);
    strcat(symbolName,_Create_InstanceMI);
    if ((miVector.createInstMI=(CREATE_INST_MI)
          System::loadDynamicSymbol(_library,symbolName))) {
       if (miVector.createInstMI)
          miVector.miTypes|=CMPI_MIType_Instance;
       specificMode=1;
    }


    if ((miVector.createGenAssocMI=(CREATE_GEN_ASSOC_MI)
          System::loadDynamicSymbol(_library,_Generic_Create_AssociationMI))) {
       if (miVector.createGenAssocMI)
          miVector.miTypes|=CMPI_MIType_Association;
       miVector.genericMode=1;
    }

    strcpy(symbolName,(const char*)mName);
    strcat(symbolName,_Create_AssociationMI);
    if ((miVector.createAssocMI=(CREATE_ASSOC_MI)
          System::loadDynamicSymbol(_library,symbolName))) {
       if (miVector.createAssocMI)
         miVector.miTypes|=CMPI_MIType_Association;
       specificMode=1;
    }


    if ((miVector.createGenMethMI=(CREATE_GEN_METH_MI)
          System::loadDynamicSymbol(_library,_Generic_Create_MethodMI))) {
       if (miVector.createGenMethMI)
          miVector.miTypes|=CMPI_MIType_Method;
       miVector.genericMode=1;
    }

    strcpy(symbolName,(const char*)mName);
    strcat(symbolName,_Create_MethodMI);
    if ((miVector.createMethMI=(CREATE_METH_MI)
          System::loadDynamicSymbol(_library,symbolName))) {
       if (miVector.createMethMI)
          miVector.miTypes|=CMPI_MIType_Method;
       specificMode=1;
    }


    if ((miVector.createGenPropMI=(CREATE_GEN_PROP_MI)
          System::loadDynamicSymbol(_library,_Generic_Create_PropertyMI))) {
       if (miVector.createGenPropMI)
          miVector.miTypes|=CMPI_MIType_Property;
       miVector.genericMode=1;
    }

    strcpy(symbolName,(const char*)mName);
    strcat(symbolName,_Create_PropertyMI);
    if ((miVector.createPropMI=(CREATE_PROP_MI)
              System::loadDynamicSymbol(_library,symbolName))) {
       if (miVector.createPropMI)
          miVector.miTypes|=CMPI_MIType_Property;
       specificMode=1;
    }


    if ((miVector.createGenIndMI=(CREATE_GEN_IND_MI)
          System::loadDynamicSymbol(_library,_Generic_Create_IndicationMI))) {
       if (miVector.createGenIndMI)
          miVector.miTypes|=CMPI_MIType_Indication;
       miVector.genericMode=1;
    }

    strcpy(symbolName,(const char*)mName);
    strcat(symbolName,_Create_IndicationMI);
    if ((miVector.createIndMI=(CREATE_IND_MI)
           System::loadDynamicSymbol(_library,symbolName))) {
       if (miVector.createIndMI)
          miVector.miTypes|=CMPI_MIType_Indication;
       specificMode=1;
    }

    if (miVector.miTypes==0) {
	throw Exception(MessageLoaderParms("ProviderManager.CMPI.CMPIProviderModule.WRONG_LIBRARY",
            "ProviderLoadFailure: ($0) Provider is not a CMPI style provider. Cannot find $1_Create<mi-type>MI symbol.",
	    _fileName,
	    realProviderName));
    }

    if (miVector.genericMode && specificMode) {
        throw Exception(MessageLoaderParms("ProviderManager.CMPI.CMPIProviderModule.CONFLICTING_CMPI_STYLE",
		"ProviderLoadFailure: ($0:$1) conflicting generic/specfic CMPI style provider.",
		_fileName,
		realProviderName));
    }

    genericProviderModule=miVector.genericMode!=0;
    _ref_count++;

    return miVector;
}

void CMPIProviderModule::unloadModule(void)
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
