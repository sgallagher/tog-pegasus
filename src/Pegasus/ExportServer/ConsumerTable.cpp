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
// Author: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
// Modified By: Sushma Fernandes, Hewlett-Packard Company
//                 sushma_fernandes@hp.com
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cstdlib>
#include <cstdlib>
#include <cctype>
#include <fstream>
#include <cstdio>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/Common/Tracer.h>
#include "ConsumerTable.h"

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

ConsumerTable::ConsumerTable()
{
    ConsumerList regConsumers;

    String    consumerDir   = String::EMPTY;

    //
    // Get environment variable
    //
    consumerDir = ConfigManager::getPegasusHome();

    FileSystem::translateSlashes(consumerDir);
    consumerFile.clear();
    consumerFile.append(consumerDir + "/" + CONSUMER_LIST_FILE);

    String line;

    //
    // Delete the backup configuration file
    //
    if (FileSystem::exists(consumerDir + "/" + CONSUMER_LIST_FILE + ".bak"))
    {
        FileSystem::removeFile(consumerDir + "/" + CONSUMER_LIST_FILE + ".bak");
    }

    //
    // Open the config file
    //
    ArrayDestroyer<char> p(consumerFile.allocateCString());
    ifstream ifs(p.getPointer());
    if (!ifs)
    {
        return;
    }

    //
    // Read each line of the file
    //
    for (Uint32 lineNumber = 1; GetLine(ifs, line); lineNumber++)
    {
        // Get the property name and value

        //
        // Skip leading whitespace
        //
        const Char16* p = line.getData();

        while (*p && isspace(*p))
        {
            p++;
        }

        if (!*p)
        {
            continue;
        }

        //
        // Skip comment lines
        //
        if (*p == '#')
        {
            continue;
        }

        //
        // Get the property name
        //
        String name = String::EMPTY;

        name += *p++;

        while (isalnum(*p) || *p == '_' || *p == '/')
        {
            name += *p++;
        }

        //
        // Skip whitespace after property name
        //
        while (*p && isspace(*p))
        {
            p++;
        }

        p++;

        //
        // Skip whitespace after equal sign
        //
        while (*p && isspace(*p))
        {
            p++;
        }

        //
        // Get the value
        //
        String value = String::EMPTY;

        while (*p)
        {
            value += *p++;
        }

        //
        // Store the property name and value in the table
        //

	regConsumers.consumerId = name;
	regConsumers.consumerLocation = value;
	_consumerList.append(regConsumers);
    }

    ifs.close();
}

void ConsumerTable::set(Boolean dynamicReg, Boolean staticConsumers, Boolean persistence)
{
    _dynamicReg = dynamicReg;
    _staticConsumers = staticConsumers;
    _persistence = persistence;
}

CIMIndicationConsumer* ConsumerTable::lookupConsumer(const String& consumerId)
{
    for (Uint32 i = 0, n = _consumers.size(); i < n; i++)
    {
	if (String::equal(_consumers[i].consumerId, consumerId))
	    return _consumers[i].consumer;
    }

    return 0;
}

CIMStatusCode ConsumerTable::registerConsumer(
    const String& consumerId,
    const String& consumerLocation,
    const String& action,
    String& errorDescription)
{
    PEG_METHOD_ENTER(TRC_EXP_REQUEST_DISP,
       "ConsumerTable::registerConsumer");

    PEG_TRACE_STRING(TRC_EXP_REQUEST_DISP, Tracer::LEVEL4,
       "consumerId = " + consumerId + ", consumerLocation = " +
       consumerLocation + ", action = " + action );

    CIMStatusCode errorCode = CIM_ERR_SUCCESS;

    if (action == String("2"))
    {
	// Check if already registered

	for (Uint8 i = 0; i < _consumerList.size(); i++)
	{
	    if (String::equalNoCase(_consumerList[i].consumerId, consumerId))
	    {
		errorCode = CIM_ERR_FAILED;
		errorDescription = "Consumer Already registered";
                PEG_METHOD_EXIT();
		return errorCode;
	    }
	}

	// New consumer

       // ATTN-DME-P1-20020504: path needs to be appended to consumerLocation
       // for this test to work.

	//if (FileSystem::existsNoCase(consumerLocation))
	//{
	    ConsumerList newConsumer;
	    newConsumer.consumerId = consumerId;
	    newConsumer.consumerLocation = consumerLocation;
	    _consumerList.append(newConsumer);

	    //
	    // Open the config file
	    //
            ArrayDestroyer<char> p(consumerFile.allocateCString());
            ofstream ofs(p.getPointer());

            if (!ofs)
	    {
                errorCode = CIM_ERR_FAILED;
                errorDescription = "Unable to open/generate consumer_list.dat file";
                PEG_METHOD_EXIT();
                return errorCode;
	    }

            ofs << consumerId;
            ofs << " = ";
            ofs << consumerLocation << endl;

	    ofs.close();
	//}
	//else
	//{
	//    errorCode = CIM_ERR_FAILED;
	//    errorDescription = "Invalid Consumer Path or Library Name";
        //    PEG_METHOD_EXIT();
	//    return errorCode;
	//}
    }

    PEG_METHOD_EXIT();
    return errorCode;
}

typedef CIMIndicationConsumer* (*CreateIndicationConsumerFunc)();

CIMIndicationConsumer* ConsumerTable::loadConsumer(const String& consumerId)
{
    PEG_METHOD_ENTER(TRC_EXP_REQUEST_DISP,
       "ConsumerTable::loadConsumer");

    String consumerName = _GetConsumerName(consumerId);

    if (consumerName.size() != 0)
    {
	// Load the dynamic library:

#ifdef PEGASUS_OS_TYPE_WINDOWS
	ArrayDestroyer<char> libraryName = consumerName.allocateCString();
#else
        String unixLibName = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir"));
	unixLibName += "/lib";
	unixLibName += consumerName;
#ifdef PEGASUS_OS_HPUX
	unixLibName += ".sl";
#else
	unixLibName += ".so";
#endif
	ArrayDestroyer<char> libraryName = unixLibName.allocateCString();
#endif

	DynamicLibraryHandle libraryHandle = 
	    System::loadDynamicLibrary(libraryName.getPointer());

	if (!libraryHandle) {
#ifdef PEGASUS_OS_TYPE_WINDOWS
            PEG_METHOD_EXIT();
	    throw DynamicLoadFailed(libraryName.getPointer());
#else
	    unixLibName = System::dynamicLoadError();
	    ArrayDestroyer<char> errorMsg = unixLibName.allocateCString();
            PEG_METHOD_EXIT();
	    throw DynamicLoadFailed(errorMsg.getPointer());
#endif
	}

	// Lookup the create consumer symbol:

	String tmp = "PegasusCreateIndicationConsumer_";
	tmp.append(consumerName);
	ArrayDestroyer<char> functionName = tmp.allocateCString();

	CreateIndicationConsumerFunc func = 
	    (CreateIndicationConsumerFunc)System::loadDynamicSymbol(
	    libraryHandle, functionName.getPointer());

	if (!func)
        {
            PEG_METHOD_EXIT();
	    throw DynamicLookupFailed(functionName.getPointer());
        }

	// Create the consumer:

	CIMIndicationConsumer* consumer = func();

	if (!consumer) 
        {
            PEG_METHOD_EXIT();
	    throw CreateIndicationConsumerReturnedNull(
		libraryName.getPointer(), 
		functionName.getPointer());
        }

	if (consumer)
	{
	    Entry entry;
	    entry.consumerId = consumerId;
	    entry.consumer = consumer;
	    _consumers.append(entry);
	}
        PEG_METHOD_EXIT();
        return consumer;
    }
    else
    {
        PEG_METHOD_EXIT();
        return 0;
    }
}

String ConsumerTable::_GetConsumerName(const String& consumerId)
{
    PEG_METHOD_ENTER(TRC_EXP_REQUEST_DISP,
       "ConsumerTable::_GetConsumerName");

    for (Uint8 i = 0; i < _consumerList.size(); i++)
    {
	if (String::equal(_consumerList[i].consumerId, consumerId))
            {
            PEG_METHOD_EXIT();
	    return _consumerList[i].consumerLocation;
            }
    }
    PEG_METHOD_EXIT();
    return String();
}

PEGASUS_NAMESPACE_END
