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

#include <iostream>
#include <Pegasus/Common/Config.h>
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

void snmpIndicationHandler::handleIndication(CIMInstance& handlerInstance,
    CIMInstance& indicationInstance,
    String nameSpace)
{
    String enterprise, trapOid, destination, trapType;
    
    Array<String> propOIDs;
    Array<String> propTYPEs;
    Array<String> propVALUEs;

    String propName;
    
    Uint32 propPos;
    Uint32 qualifierPos;
    Uint32 typePos;
    CIMValue propValue;
    CIMProperty trapProp;
    CIMQualifier trapQualifier;

    CIMClass indicationClass = _repository->getClass(
	nameSpace, 
	indicationInstance.getClassName(), 
	false);

    //filling the array with enterprise first
    propPos = indicationClass.findProperty("enterprise");
    if (propPos != PEG_NOT_FOUND)
    {
        trapProp = indicationClass.getProperty(propPos);
        qualifierPos = trapProp.findQualifier("OID");
        trapQualifier = trapProp.getQualifier(qualifierPos);
        enterprise = trapQualifier.getValue().toString();

        int i;
        for (i=0; i<indicationInstance.getPropertyCount();i++)
        {
	    propValue = indicationInstance.getProperty(i).getValue();
	    
	    if (!propValue.isNull())
            {
                propName = indicationInstance.getProperty(i).getName();
                propPos = indicationClass.findProperty(propName);
                trapProp = indicationClass.getProperty(propPos);

                if (trapProp.existsQualifier("OID"))
                {
                    if (propName == "trapOid")
		    {
			trapOid = propValue.toString();
		    }
		    else
		    {
			qualifierPos = trapProp.findQualifier("OID");
			trapQualifier = trapProp.getQualifier(qualifierPos);
			propOIDs.append(trapQualifier.getValue().toString());
			propVALUEs.append(propValue.toString());
			if (trapProp.existsQualifier("SNMPTYPE"))
			{
			    typePos = trapProp.findQualifier("SNMPTYPE");
			    trapQualifier = trapProp.getQualifier(typePos);
			    propTYPEs.append(trapQualifier.getValue().toString());
			}
			else
			    propTYPEs.append(TypeToString(trapProp.getType()));
		    }
		}
            }
        }
        
        destination = handlerInstance.getProperty(
	    handlerInstance.findProperty("destination")).getValue().toString();

	trapType = handlerInstance.getProperty(
	    handlerInstance.findProperty("trapType")).getValue().toString();

	// Collected complete data in arrays and ready to send the trap.
        // trap destination and SNMP type are defined in handlerInstance
        // and passing this instance as it is to deliverTrap() call
#ifdef HPUX_EMANATE
	snmpDeliverTrap_emanate emanateTrap;
#else
	snmpDeliverTrap_stub emanateTrap;
#endif
        
	cout << "Trap to deliver " << endl;
        
	emanateTrap.deliverTrap(trapOid, 
            enterprise,
            destination,
            trapType, 
            propOIDs,  
            propTYPEs, 
            propVALUEs);
        
	cout << "Trap Delivered " << endl;
    }
    else
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, 
	    "Indication is without enterprise OID");
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
