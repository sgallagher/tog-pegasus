//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett Packard, IBM, The Open Group,
// Tivoli Systems
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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <iostream>
#include <Pegasus/Handler/CIMHandler.h>
#include <Pegasus/Repository/CIMRepository.h>

#include "snmpIndicationHandler.h"

#ifdef HPUX_EMANATE
#include "snmpDeliverTrap_emanate.h"
#else
#include "snmpDeliverTrap_stub.h"
#endif

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;

//#define DDD(X) X
#define DDD(X) // X

DDD(static const char* _SNMPINDICATIONHANDLER = "snmpIndicationHandler::";)

void snmpIndicationHandler::initialize(CIMRepository* repository)
{
    _repository = repository;
    DDD(cout << _SNMPINDICATIONHANDLER << "initialize()" << endl;)
}

void snmpIndicationHandler::handleIndication(CIMInstance& handler, 
    CIMInstance& indication, String nameSpace)
{
    Array<String> propOIDs;
    Array<String> propTYPEs;
    Array<String> propVALUEs;

    CIMProperty prop;
    CIMQualifier trapQualifier;

    Uint32 qualifierPos;
    
    String propValue;

    String mapstr1;
    String mapstr2;

    CIMClass indicationClass = _repository->getClass(
	nameSpace, indication.getClassName(), false);

    for (Uint32 i=0; i<indication.getPropertyCount();i++)
    {
	prop = indication.getProperty(i);

	if (prop)
        {
            String propName = prop.getName();
            Uint32 propPos = indicationClass.findProperty(propName);
            if (propPos != PEG_NOT_FOUND)
            {
            CIMProperty trapProp = indicationClass.getProperty(propPos);

            if (trapProp.existsQualifier("MappingStrings"))
            {
		qualifierPos = trapProp.findQualifier("MappingStrings");
		trapQualifier = trapProp.getQualifier(qualifierPos);
		
		mapstr1.clear();
		mapstr1 = trapQualifier.getValue().toString();

		if ((mapstr1.find("OID.IETF") != PEG_NOT_FOUND) &&
		    (mapstr1.find("DataType.IETF") != PEG_NOT_FOUND))
		{
		    if (mapstr1.subString(0, 8) == "OID.IETF")
		    {
			mapstr1 = mapstr1.subString(mapstr1.find("SNMP.")+5);
                        if (mapstr1.find("|") != PEG_NOT_FOUND)
                        {
			    mapstr2.clear();
			    mapstr2 = mapstr1.subString(0, 
				mapstr1.find("DataType.IETF")-1);
			    propOIDs.append(mapstr2);
                            
			    propValue.clear();
                            propValue = prop.getValue().toString();
			    propVALUEs.append(propValue);
                            
			    mapstr2 = mapstr1.subString(mapstr1.find("|")+2);
                            mapstr2 = mapstr2.subString(0, mapstr2.size()-1);
			    propTYPEs.append(mapstr2);
                        }
		    }
		}
	    }
            }
        }
    }

        // Collected complete data in arrays and ready to send the trap.
        // trap destination and SNMP type are defined in handlerInstance
        // and passing this instance as it is to deliverTrap() call

#ifdef HPUX_EMANATE
        snmpDeliverTrap_emanate emanateTrap;
#else
        snmpDeliverTrap_stub emanateTrap;
#endif

    if ((handler.findProperty("TrapDestination") != PEG_NOT_FOUND) &&
        (handler.findProperty("SNMPVersion") != PEG_NOT_FOUND) && 
        (indicationClass.findQualifier("MappingStrings") != PEG_NOT_FOUND))
    {
        String community, trapType, destination;   // from handler instance
	String trapOid;	    // from indication Class

	trapOid = indicationClass.getQualifier(
	    indicationClass.findQualifier("MappingStrings")).getValue().toString();

        trapOid = trapOid.subString(trapOid.find("OID.IETF | SNMP.")+16);

	community = handler.getProperty(
	    handler.findProperty("SNMPCommunityName")).getValue().toString();

	destination = handler.getProperty(
	    handler.findProperty("TrapDestination")).getValue().toString();

	trapType = handler.getProperty(
	    handler.findProperty("SNMPVersion")).getValue().toString();

	emanateTrap.deliverTrap(
            trapOid,
            community,
            destination,
            trapType,
            propOIDs,  
            propTYPEs, 
            propVALUEs);
    }
    else
        cout << "Invalid Indication" << endl;
}

// This is the dynamic entry point into this dynamic module. The name of
// this handler is "snmpIndicationHandler" which is appened to "PegasusCreateHandler_"
// to form a symbol name. This function is called by the HandlerTable
// to load this handler.

extern "C" PEGASUS_EXPORT CIMHandler* 
    PegasusCreateHandler_snmpIndicationHandler() {
    DDD(cout << "Called PegasusCreateHandler_snmpIndicationHandler" << endl;)
    return new snmpIndicationHandler;
}

PEGASUS_NAMESPACE_END
