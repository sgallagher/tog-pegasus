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
// Author: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//	      : Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/PegasusVersion.h>

#include <iostream>
#include <Pegasus/Handler/CIMHandler.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Common/Tracer.h>

#include "snmpIndicationHandler.h"

#ifdef HPUX_EMANATE
#include "snmpDeliverTrap_emanate.h"
#else
#include "snmpDeliverTrap_stub.h"
#endif

// l10n
#include <Pegasus/Common/MessageLoader.h>

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;

void snmpIndicationHandler::initialize(CIMRepository* repository)
{
    _repository = repository;
}

// l10n - note: ignoring indication language
void snmpIndicationHandler::handleIndication(
    const OperationContext& context,
    const String nameSpace,
    CIMInstance& indication,
    CIMInstance& handler, 
    CIMInstance& subscription,
    ContentLanguages & contentLanguages)
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

    PEG_METHOD_ENTER (TRC_IND_HANDLER, 
	"snmpIndicationHandler::handleIndication");

    try
    {
    	CIMClass indicationClass = _repository->getClass(
	    nameSpace, indication.getClassName(), false, true, 
	    false, CIMPropertyList());

    	Uint32 propertyCount = indication.getPropertyCount();

    	for (Uint32 i=0; i < propertyCount; i++)
    	{
	    prop = indication.getProperty(i);

	    if (!prop.isUninitialized())
            {
                CIMName propName = prop.getName();
                Uint32 propPos = indicationClass.findProperty(propName);
                if (propPos != PEG_NOT_FOUND)
                {
                    CIMProperty trapProp = indicationClass.getProperty(propPos);

                    qualifierPos = trapProp.findQualifier(CIMName ("MappingStrings"));
                    if (qualifierPos != PEG_NOT_FOUND)
                    {
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
        static snmpDeliverTrap_emanate emanateTrap;
#else
        static snmpDeliverTrap_stub emanateTrap;
#endif

        Uint32 targetHostPos = handler.findProperty(CIMName ("TargetHost"));
        Uint32 targetHostFormatPos = handler.findProperty(CIMName ("TargetHostFormat"));
        Uint32 otherTargetHostFormatPos = handler.findProperty(CIMName (
				      "OtherTargetHostFormat"));
        Uint32 portNumberPos = handler.findProperty(CIMName ("PortNumber"));
        Uint32 snmpVersionPos = handler.findProperty(CIMName ("SNMPVersion"));
        Uint32 securityNamePos =  handler.findProperty(CIMName ("SNMPSecurityName"));
        Uint32 engineIDPos =  handler.findProperty(CIMName ("SNMPEngineID"));

        if ((targetHostPos != PEG_NOT_FOUND) &&
            (targetHostFormatPos != PEG_NOT_FOUND) && 
            (snmpVersionPos != PEG_NOT_FOUND) && 
            (indicationClass.findQualifier(CIMName ("MappingStrings")) != 
                PEG_NOT_FOUND))
        {
    	    // properties from the handler instance
            String targetHost;
	    String otherTargetHostFormat = String();
	    String securityName = String();
	    String engineID = String();
	    Uint16 targetHostFormat = 0;
	    Uint16 snmpVersion = 0;
	    Uint32 portNumber;

	    String trapOid;
	    //
            //  Get snmpTrapOid from context
            //
	    try
	    {
                SnmpTrapOidContainer trapContainer = context.get
                    (SnmpTrapOidContainer::NAME);

                trapOid = trapContainer.getSnmpTrapOid();
	    }
	    catch (Exception& e)
            {
	        // get trapOid from indication Class

	        Uint32 pos = indicationClass.findQualifier(CIMName ("MappingStrings"));
	        if (pos != PEG_NOT_FOUND)
	        {
	            trapOid = indicationClass.getQualifier(pos).getValue().toString();

		    trapOid = trapOid.subString(11, PEG_NOT_FOUND);

                    if ((String::compare(trapOid, "SNMP.", 5)) == 0)
                    {
                        trapOid = trapOid.subString(5, (trapOid.size()-6));
                    }
	            else
	            {
			PEG_TRACE_STRING(TRC_IND_HANDLER, Tracer::LEVEL4,
			    		 "Invalid MappingStrings Value " + trapOid);
			PEG_METHOD_EXIT();
		        // l10n
	                // throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Invalid MappingStrings Value");
		        throw PEGASUS_CIM_EXCEPTION_L (CIM_ERR_FAILED,
						   MessageLoaderParms("Handler.snmpIndicationHandler.snmpIndicationHandler.INVALID_MS_VALUE",
								       "Invalid MappingStrings Value")); 
	            }
	        }
	        else
	        {
		    PEG_TRACE_STRING(TRC_IND_HANDLER, Tracer::LEVEL4,
		    		 "Qualifier MappingStrings can not be found.");
		    PEG_METHOD_EXIT();
	    	    //L10N_ TODO DONE
	            //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Qualifier MappingStrings can not be found");
		    MessageLoaderParms parms("Handler.snmpIndicationHandler.snmpIndicationHandler.QUALIFIER_MAPPINGS_NOT_FOUND",
								 "Qualifier MappingStrings can not be found");
		    throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, parms);
	        }
	    }

	    handler.getProperty(targetHostPos).getValue().get(targetHost);
	    handler.getProperty(targetHostFormatPos).getValue().get(targetHostFormat);
	    if (otherTargetHostFormatPos != PEG_NOT_FOUND)
	    { 
	        handler.getProperty(otherTargetHostFormatPos).getValue().get
		    (otherTargetHostFormat);
	    }
	    if (portNumberPos != PEG_NOT_FOUND)
	    {
	        handler.getProperty(portNumberPos).getValue().get(portNumber);
	    }
	    else
	    {
	        // default port
	        portNumber = SNMP_TRAP_DEFAULT_PORT;
	    }

	    handler.getProperty(snmpVersionPos).getValue().get(snmpVersion);
	    if (securityNamePos != PEG_NOT_FOUND)
	    {
	        handler.getProperty(securityNamePos).getValue().get(securityName);
	    }
	    if (engineIDPos != PEG_NOT_FOUND)
	    {
	        handler.getProperty(engineIDPos).getValue().get(engineID);
	    }

	    emanateTrap.deliverTrap(
                trapOid,
                securityName,
                targetHost,
                targetHostFormat,
	        otherTargetHostFormat,
	        portNumber,
	        snmpVersion,
	        engineID,
                propOIDs,  
                propTYPEs, 
                propVALUEs);
        }
        else
        {
	    PEG_TRACE_STRING(TRC_IND_HANDLER, Tracer::LEVEL4,
		"Invalid IndicationHandlerSNMPMapper instance.");
	    PEG_METHOD_EXIT();
          // l10n

          // throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, 
          // "Invalid IndicationHandlerSNMPMapper instance");

          throw PEGASUS_CIM_EXCEPTION_L (CIM_ERR_FAILED, 
				     MessageLoaderParms("Handler.snmpIndicationHandler.snmpIndicationHandler.INVALID_SNMP_INSTANCE", 
							"Invalid IndicationHandlerSNMPMapper instance"));
        }
    } 
    catch (CIMException & c)
    {
	PEG_TRACE_STRING(TRC_IND_HANDLER, Tracer::LEVEL4, c.getMessage()); 
	PEG_METHOD_EXIT();

	throw PEGASUS_CIM_EXCEPTION (CIM_ERR_FAILED, c.getMessage());
    }
    catch (Exception& e)
    {
	PEG_TRACE_STRING(TRC_IND_HANDLER, Tracer::LEVEL4, e.getMessage());
	PEG_METHOD_EXIT();

	throw PEGASUS_CIM_EXCEPTION (CIM_ERR_FAILED, e.getMessage());
    }
    catch (...)
    {
	PEG_TRACE_STRING(TRC_IND_HANDLER, Tracer::LEVEL4,
		"Failed to deliver trap.");
	PEG_METHOD_EXIT();

	throw PEGASUS_CIM_EXCEPTION_L (CIM_ERR_FAILED,
		MessageLoaderParms("Handler.snmpIndicationHandler.snmpIndicationHandler.FAILED_TO_DELIVER_TRAP", 
				   "Failed to deliver trap."));
    }
}

// This is the dynamic entry point into this dynamic module. The name of
// this handler is "snmpIndicationHandler" which is appended to "PegasusCreateHandler_"
// to form a symbol name. This function is called by the HandlerTable
// to load this handler.

extern "C" PEGASUS_EXPORT CIMHandler* 
    PegasusCreateHandler_snmpIndicationHandler() {
    return new snmpIndicationHandler;
}

PEGASUS_NAMESPACE_END
