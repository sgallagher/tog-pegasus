//%////////////-*-c++-*-///////////////////////////////////////////////////////////
//
// Copyright (c) 2000 - 2003 BMC Software, Hewlett-Packard Company, IBM,
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
//     Mike Day, IBM (mdday@us.ibm.com)
//     Adrian Schuur, IBM (schuur@de.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CMPIProviderModule.h"

#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/MessageLoader.h> //l10n
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/ProviderManager2/CMPI/CMPIProvider.h>
#include <Pegasus/ProviderManager2/CMPI/CMPIProviderManager.h>

PEGASUS_NAMESPACE_BEGIN

CMPIProviderModule::CMPIProviderModule(const String & fileName,
                                       const String & interfaceName)
    : _fileName(fileName),
    _interfaceName(interfaceName),
    _ref_count(0),
    _library(0)
{
   if (fileName[0]!='/')
      _fileName=CMPIProviderManager::resolveFileName(fileName);
   else _fileName=fileName;
}

CMPIProviderModule::~CMPIProviderModule(void)
{
}

ProviderVector CMPIProviderModule::load(const String & providerName)
{
    _library = System::loadDynamicLibrary((const char *)_fileName.getCString());

    if(_library == 0)
    {

        String s0 = "ProviderLoadFailure";
        throw Exception(MessageLoaderParms("ProviderManager.CMPIProviderModule.CANNOT_LOAD_LIBRARY",
            "$0 ($1:$2):Cannot load library, error: $3",
            s0,
            _fileName,
            providerName,
            System::dynamicLoadError()));
    }

    char symbolName[512];
    CString mName=providerName.getCString();
    ProviderVector miVector;
    memset(&miVector,0,sizeof(ProviderVector));
    int specificMode=0;

    if ((miVector.createGenInstMI=(CREATE_GEN_INST_MI)
           System::loadDynamicSymbol(_library,"_Generic_Create_InstanceMI"))) {
       miVector.miTypes|=CMPI_MIType_Instance;
       miVector.genericMode=1;
    }

    strcpy(symbolName,(const char*)mName);
    strcat(symbolName,"_Create_InstanceMI");
    if ((miVector.createInstMI=(CREATE_INST_MI)
          System::loadDynamicSymbol(_library,symbolName))) {
       miVector.miTypes|=CMPI_MIType_Instance;
       specificMode=1;
    }


    if ((miVector.createGenAssocMI=(CREATE_GEN_ASSOC_MI)
          System::loadDynamicSymbol(_library,"_Generic_Create_AssociationMI"))) {
       miVector.miTypes|=CMPI_MIType_Association;
       miVector.genericMode=1;
    }

    strcpy(symbolName,(const char*)mName);
    strcat(symbolName,"_Create_AssociationMI");
    if ((miVector.createAssocMI=(CREATE_ASSOC_MI)
          System::loadDynamicSymbol(_library,symbolName))) {
       miVector.miTypes|=CMPI_MIType_Association;
       specificMode=1;
    }


    if ((miVector.createGenMethMI=(CREATE_GEN_METH_MI)
          System::loadDynamicSymbol(_library,"_Generic_Create_MethodMI"))) {
       miVector.miTypes|=CMPI_MIType_Method;
       miVector.genericMode=1;
    }

    strcpy(symbolName,(const char*)mName);
    strcat(symbolName,"_Create_MethodMI");
    if ((miVector.createMethMI=(CREATE_METH_MI)
          System::loadDynamicSymbol(_library,symbolName))) {
       miVector.miTypes|=CMPI_MIType_Method;
       specificMode=1;
    }


    if ((miVector.createGenPropMI=(CREATE_GEN_PROP_MI)
          System::loadDynamicSymbol(_library,"_Generic_Create_PropertyMI"))) {
       miVector.miTypes|=CMPI_MIType_Property;
       miVector.genericMode=1;
    }

    strcpy(symbolName,(const char*)mName);
    strcat(symbolName,"_Create_PropertyMI");
    if ((miVector.createPropMI=(CREATE_PROP_MI)
              System::loadDynamicSymbol(_library,symbolName))) {
       miVector.miTypes|=CMPI_MIType_Property;
       specificMode=1;
    }


    if ((miVector.createGenIndMI=(CREATE_GEN_IND_MI)
          System::loadDynamicSymbol(_library,"_Generic_Create_IndicationMI"))) {
       miVector.miTypes|=CMPI_MIType_Indication;
       miVector.genericMode=1;
    }

    strcpy(symbolName,(const char*)mName);
    strcat(symbolName,"_Create_IndicationMI");
    if ((miVector.createIndMI=(CREATE_IND_MI)
           System::loadDynamicSymbol(_library,symbolName))) {
       miVector.miTypes|=CMPI_MIType_Indication;
       specificMode=1;
    }

    if (miVector.miTypes==0) {
       throw Exception("ProviderLoadFailure "+_fileName+":"+_providerName+
           " Provider is a not CMPI style provider");
    }

    if (miVector.genericMode && specificMode) {
       throw Exception("ProviderLoadFailure "+_fileName+":"+_providerName+
           " onflicting generic/specfic CMPI style provider");
    }

/*    // test for the appropriate interface
    if(dynamic_cast<CIMProvider *>(provider) == 0)
    {
        //l10n
        //String errorString = "provider is not a CIMProvider.";
        //throw Exception("ProviderLoadFailure (" + _fileName + ":" + providerName + "):" + errorString);
        String s0 = "ProviderLoadFailure";
        String s3 = "CIMProvider";
        throw Exception(MessageLoaderParms("ProviderManager.CMPIProviderModule.PROVIDER_IS_NOT_A",
            "$0 ($1:$2):provider is not a $3.",
            s0,
            _fileName,
            providerName,
            s3));
    }
*/
    _ref_count++;

    return miVector;
}

void CMPIProviderModule::unloadModule(void)
{
    _ref_count--;

    if(_ref_count.value() > 0)
        return;

    _ref_count = 0;

    if(_library != 0)
    {
        System::unloadDynamicLibrary(_library);
        _library = 0;
    }
}

PEGASUS_NAMESPACE_END
