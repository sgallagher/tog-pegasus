//%2004////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Modified By: Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/System.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/InternalException.h>
#include "snmpDeliverTrap_emanate.h"

// l10n
#include <Pegasus/Common/MessageLoader.h>

// EMANATE specific declarations and entry points - MUST be in begining
// and outside NAMESPACE.

// master agent needs these two declarations for communication with sub-agent.
// Following two declarations must be in the CODE. 

#include <prnt_lib.h>  // MUST be at the end in include list.

static const char *sr_filename = __FILE__;

IPCFunctionP IPCfp;  /* IPC functions pointer  */

// This code would normally be generated by Emanate from defined MIB objects. 
// Since we do not have MIB objects defined, just defined here to load 
// subagent as library. OidList[] provides objects for entry point to master agent.

// The objects internal to the agent 
ObjectInfo OidList[] = 
{
{ { 0, NULL },
#ifndef LIGHT
    NULL,
#endif /* LIGHT */
    0, 0, 0, 0,
    NULL, NULL }
};

// This code would normally be generated by Emanate in k_* routine from defined MIB objects.
// Since we do not have MIB objects defined, just define here to pass compile and
// enable entry point for master agent to start communication with library.

// Called by the master agent during initialization */
int k_initialize()
{
   return 1;
}

// END EMANATE specific declarations.

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;

snmpDeliverTrap_emanate::snmpDeliverTrap_emanate()
{
    //
    // Initialize Subagent and establish comunication with the Master Agent
    //
    initialize();
}

snmpDeliverTrap_emanate::~snmpDeliverTrap_emanate()
{
    //
    // Close the connection to the Master Agent and shut down the 
    // Subagent
    //
    EndSubagent();
}


// initialize sub-agent

// This also defines the communication protocol to be used between master
// and sub-agent.

void snmpDeliverTrap_emanate::initialize()
{
#ifndef SR_UDS_IPC
    InitIPCArrayTCP(&IPCfp);
#else /* SR_UDS_IPC */
    InitIPCArrayUDS(&IPCfp);
#endif /* SR_UDS_IPC */

    if(InitSubagent() == -1) 
    {

      // l10n

      // throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
      //   _MSG_INITSUBAGENT_FAILED);

      throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
				    MessageLoaderParms(_MSG_INITSUBAGENT_FAILED_KEY,
						       _MSG_INITSUBAGENT_FAILED));

    }
}


void snmpDeliverTrap_emanate::deliverTrap(
        const String& trapOid,
        const String& securityName, 
        const String& targetHost, 
        const Uint16& targetHostFormat, 
        const String& otherTargetHostFormat, 
        const Uint32& portNumber,
        const Uint16& snmpVersion, 
        const String& engineID,
        const Array<String>& vbOids,
        const Array<String>& vbTypes,
        const Array<String>& vbValues)
{
    VarBind *vbhead = NULL;
    VarBind *vb = NULL;
    VarBind *vblast = NULL;
    
    OID	    *object = NULL;

    // Translate a string into an OID
    OID *sendtrapOid = MakeOIDFromDot(trapOid.getCString());

    if (sendtrapOid == NULL)
    {

      // l10n

      // throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
      // _MSG_INVALID_TRAPOID);

      throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
				    MessageLoaderParms(_MSG_INVALID_KEY,
						       _MSG_INVALID_TRAPOID));
    }

    // Destination : convert targetHost into Transport

    CString trap_dest = targetHost.getCString();

    TransportInfo   global_ti;
    global_ti.type = SR_IP_TRANSPORT;

    switch (targetHostFormat)
    {
	case _HOST_NAME:
	{
	    char * ipAddr = _getIPAddress(trap_dest); 

	    if (ipAddr == NULL)
	    {

	      // l10n

	      // throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
	      //   _MSG_DESTINATION_NOT_FOUND);

	      throw PEGASUS_CIM_EXCEPTION_L (CIM_ERR_FAILED,
					     MessageLoaderParms(_MSG_DESTINATION_NOT_FOUND_KEY,
								_MSG_DESTINATION_NOT_FOUND));

 	    }
    	    global_ti.t_ipAddr = inet_addr(trap_dest);
	    break;	    
	}
	case _IPV4_ADDRESS:
	{
    	    global_ti.t_ipAddr = inet_addr(trap_dest);
	    break;	    
	}
	default:
	{

	  // l10n

	  // throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
	  // _MSG_TARGETHOSTFORMAT_NOT_SUPPORTED);

	  throw PEGASUS_CIM_EXCEPTION_L (CIM_ERR_NOT_SUPPORTED,
					 MessageLoaderParms(_MSG_TARGETHOSTFORMAT_NOT_SUPPORTED_KEY,
							    _MSG_TARGETHOSTFORMAT_NOT_SUPPORTED));

	    break;	    
	}
    } 

    global_ti.t_ipPort = htons((unsigned short)portNumber);

    // Community Name, default is public
    CString _community;
    if (securityName.size() == 0)
    {
	String community;
	community.assign("public");
	_community = community.getCString();
    }
    else
    {
	_community = securityName.getCString();
    }

    OctetString* community_name = MakeOctetStringFromText(_community);

    if (community_name == NULL)
    {

      // l10n

      // throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
      //   _MSG_INVALID_SECURITY_NAME);

      throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
				    MessageLoaderParms(_MSG_INVALID_SECURITY_NAME_KEY,
						       _MSG_INVALID_SECURITY_NAME));
						       
    }

    // getting IP address of the host

    CString hostname = System::getHostName().getCString();
    char* IP_string = _getIPAddress(hostname);

    // formatting agent(host) address into OctetString format

    OctetString* agent_addr;

    SR_INT32 s1, s2, s3, s4;
    SR_UINT32 ipaddr;

    // pull out each of the 4 octet values from IP address

    sscanf(IP_string,"%d.%d.%d.%d", &s1, &s2, &s3, &s4);
 
    // validate the values for s1, s2, s3, and s4 to make sure values are
    // between 0 and 255
    if (!_isValidOctet(s1) || !_isValidOctet(s2) ||
	!_isValidOctet(s3) || !_isValidOctet(s4))
    {

      // l10n

      // throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
      //   _MSG_INVALID_OCTET_VALUE);

      throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
				  MessageLoaderParms(_MSG_INVALID_OCTET_VALUE_KEY,
						     _MSG_INVALID_OCTET_VALUE));
    }
 
    // create an empty 4 length OctetString

    agent_addr = MakeOctetString(NULL,4);

    if (agent_addr == NULL)
    {

      // l10n

      // throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
      //   _MSG_CREATE_OCTET_FAILED);

      throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
				    MessageLoaderParms(_MSG_CREATE_OCTET_FAILED_KEY,
						       _MSG_CREATE_OCTET_FAILED));

    }
 
    // fill in values for OctetString

    agent_addr->octet_ptr[0] = (unsigned char)s1;
    agent_addr->octet_ptr[1] = (unsigned char)s2;
    agent_addr->octet_ptr[2] = (unsigned char)s3;
    agent_addr->octet_ptr[3] = (unsigned char)s4;

    // specTrap from trapOid. 

    SR_INT32 genTrap = 0;
    SR_INT32 specTrap = 0;
    
    OID* enterpriseOid ;

    Array<String> standard_traps;

    standard_traps.append(String("1.3.6.1.6.3.1.1.5.1"));
    standard_traps.append(String("1.3.6.1.6.3.1.1.5.2"));
    standard_traps.append(String("1.3.6.1.6.3.1.1.5.3"));
    standard_traps.append(String("1.3.6.1.6.3.1.1.5.4"));
    standard_traps.append(String("1.3.6.1.6.3.1.1.5.5"));
    standard_traps.append(String("1.3.6.1.6.3.1.1.5.6"));

    Array<String> oids;
    String tmpoid = trapOid;

    while(tmpoid.find(".") != PEG_NOT_FOUND)
    {
        oids.append(tmpoid.subString(0, tmpoid.find(".")));
        tmpoid = tmpoid.subString(tmpoid.find(".") + 1);
    }

    oids.append(tmpoid);

    String ent;
    if (Contains(standard_traps, trapOid))
    {
    	//
    	// if the trapOid is one of the standard traps,
    	// then the SNMPV1 enterprise parameter must be set 
    	// to the value of the trapOid, the generic-trap
    	// parameter must be set to one of (0 - 5), and the 
    	// specific-trap parameter must be set to 0
    	//

        enterpriseOid = sendtrapOid;

	// the generic trap is last sub-identifier of the 
	// trapOid minus 1
	genTrap = atoi(oids[oids.size() - 1].getCString()) - 1;
	specTrap = 0;
    }
    else
    {
	// 
	// if the trapOid is not one of the standard traps:
	// then 1) the generic-trap parameter must be set to 6, 
	// 2) if the next-to-last sub-identifier of the 
	// trapOid is zero, then the SNMPV1 enterprise 
 	// parameter is the trapOid with the last 2 
	// sub-identifiers removed, otherwise, the 
	// SNMPV1 enterprise parameter is the trapOid 
	// with the last sub-identifier removed;
	// 3) the SNMPv1 specific-trap parameter is the last
	// sub-identifier of the trapOid;
	//

	genTrap = 6;

        specTrap = atoi(oids[oids.size()-1].getCString());

	ent = oids[0];
	for (Uint8 i = 1; i < oids.size()-2; i++)
	{
	    ent = ent + "." + oids[i];
	}

	if (oids[oids.size()-2] != "0")
	{
	    ent = ent + "." + oids[oids.size()-2];
   	}

	enterpriseOid = MakeOIDFromDot(ent.getCString());
	
	if (enterpriseOid == NULL)
	{

	  // l10n

	  // throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
	  //   _MSG_INVALID_ENTERPRISEOID);

	  throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
					MessageLoaderParms(_MSG_INVALID_ENTERPRISEOID_KEY,
							   _MSG_INVALID_ENTERPRISEOID));
	}
    }

    // creates VarBind
    for(Uint32 i = 0; i < vbOids.size(); i++)
    {
	CString _vbOid = vbOids[i].getCString();
	CString _vbValue = vbValues[i].getCString();

	if ((object = MakeOIDFromDot(_vbOid)) == NULL)
        {

	  // l10n
	  
	  // throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
	  //   _MSG_INVALID_PROPERTYOID);

	  throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
					MessageLoaderParms(_MSG_INVALID_PROPERTYOID_KEY,
							   _MSG_INVALID_PROPERTYOID));
        } 

	if (String::equalNoCase(vbTypes[i], "OctetString"))
	{
    	     OctetString*    value;

             value = CloneOctetString(MakeOctetStringFromText(_vbValue));
             if (value == NULL)
             {
	       
	       // l10n

	       // throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
	       //   _MSG_INVALID_PROPERTYVALUE);

       	         throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
					       MessageLoaderParms(_MSG_INVALID_PROPERTYVALUE_KEY,
								  _MSG_INVALID_PROPERTYVALUE));
             }

             if ((vb = MakeVarBindWithValue(object, 
				  	(OID *) NULL, 
					OCTET_PRIM_TYPE, 
					value)) == NULL)
            {

	      // l10n

	      // throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
	      //    _MSG_MAKE_VARBIND_FAILED_FOR_OCTET_PRIM_TYPE);

	      throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
					    MessageLoaderParms(_MSG_MAKE_VARBIND_FAILED_FOR_OCTET_PRIM_TYPE_KEY,
							       _MSG_MAKE_VARBIND_FAILED_FOR_OCTET_PRIM_TYPE));
            }
    	}
	else if (String::equalNoCase(vbTypes[i], "OID"))
	{
            void* value = MakeOIDFromDot(_vbValue);
            if (value == NULL)
            {
	      // l10n

	      // throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
	      //    _MSG_INVALID_PROPERTYVALUE);

	      throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
					    MessageLoaderParms(_MSG_INVALID_PROPERTYVALUE_KEY,
							       _MSG_INVALID_PROPERTYVALUE));
							       
            }

            if ((vb = MakeVarBindWithValue(object, 
				  	(OID *) NULL, 
					OBJECT_ID_TYPE, 
					value)) == NULL)
            {

	      // l10n

	      // throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
	      //    _MSG_MAKE_VARBIND_FAILED_FOR_OBJECT_ID_TYPE);

	      throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
					    MessageLoaderParms(_MSG_MAKE_VARBIND_FAILED_FOR_OBJECT_ID_TYPE_KEY,
							       _MSG_MAKE_VARBIND_FAILED_FOR_OBJECT_ID_TYPE));
            }
	}
	else
	{
            int vbvalue = atoi(_vbValue);
            void* value = &vbvalue;

            if (value == NULL)
            {

	      // l10n

	      // throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
	      //    _MSG_INVALID_PROPERTYVALUE);

	      throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
					    MessageLoaderParms(_MSG_INVALID_PROPERTYVALUE_KEY,
							       _MSG_INVALID_PROPERTYVALUE));
            }

            if ((vb = MakeVarBindWithValue(object, 
				  	(OID *) NULL, 
					INTEGER_TYPE, 
					value)) == NULL)
            {

	      // l10n

	      // throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
	      //    _MSG_MAKE_VARBIND_FAILED_FOR_INTEGER_TYPE);

	      throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
					    MessageLoaderParms(_MSG_MAKE_VARBIND_FAILED_FOR_INTEGER_TYPE_KEY,
							       _MSG_MAKE_VARBIND_FAILED_FOR_INTEGER_TYPE));
            }
	}

        if (i == 0)
        {
            vbhead = vb;
            vblast = vb;
        }
        else
        {
            vblast->next_var = vb;
            vblast = vblast->next_var;
        }

    }

    vblast->next_var = NULL;

    // Now send the trap
    switch (snmpVersion)
    {
	case _SNMPv1_TRAP:
	{
            SendNotificationToDestSMIv1Params(
	    1,					// notifyType - TRAP
            genTrap,				// genTrap
            specTrap,				// specTrap
            enterpriseOid,			// enterprise
            agent_addr,				// agent_addr
            vbhead,				// vb
            NULL,				// contextName
            1,					// retryCount
            1,					// timeout
            community_name,			// securityName,
            SR_SECURITY_LEVEL_NOAUTH,		// securityLevel
            SR_SECURITY_MODEL_V1,		// securityModel
            &global_ti,				// Transport Info
            0);          			// cfg_chk
	    break;
	}
	case _SNMPv2C_TRAP:
	{
            SendNotificationToDestSMIv2Params(
	    (SR_INT32)SNMPv2_TRAP_TYPE,		// notifyType - NOTIFICATION
            sendtrapOid,			// snmpTrapOID
            agent_addr,				// agent_addr
            vbhead,				// vb
            NULL,				// contextName
            1,					// retryCount
            100,				// timeout
            community_name,			// securityName or community
            SR_SECURITY_LEVEL_NOAUTH,		// securityLevel
            SR_SECURITY_MODEL_V2C,		// securityModel
            &global_ti,				// TransportInfo
            0);					// cfg_chk
	    break;
	}
	default:
	{
	  // l10n

	  // throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
	  // _MSG_VERSION_NOT_SUPPORTED);

	  throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_NOT_SUPPORTED,	  
					MessageLoaderParms(_MSG_VERSION_NOT_SUPPORTED_KEY,
							   _MSG_VERSION_NOT_SUPPORTED));

	  break;
	}
    }

    // Free OID built by calls MakeOIDFromDot()
    FreeOID(sendtrapOid);
    FreeOID(enterpriseOid);
    FreeOID(object);

    // Free the data structures allocated and built by calls
    // MakeOctetString() and MakeOctetStringFrom Text()
    FreeOctetString(community_name);
    FreeOctetString(agent_addr);

    // Free the VarBind data structures allocated and built 
    // by calls MakeVarBindWithValue()
    FreeVarBindList(vbhead);
    FreeVarBindList(vb);
    FreeVarBindList(vblast);
}

// get the IP address of a host
char * snmpDeliverTrap_emanate::_getIPAddress(const CString& hostName)
{
    struct hostent *targetHostInfo;
    struct in_addr in;

#ifdef PEGASUS_OS_SOLARIS
#define HOSTENT_BUFF_SIZE	8192
    char			buf[HOSTENT_BUFF_SIZE];
    struct hostent		h_result;
    int				h_errorp;
    targetHostInfo=gethostbyname_r(hostName, &h_result, buff,
					HOSTENT_BUFF_SIZE, &h_errorp);
#else
    targetHostInfo = gethostbyname(hostName);
#endif

    if (targetHostInfo == NULL)
    {
	return(NULL);
    }
	    
    char ** networkAddr;
    networkAddr = targetHostInfo->h_addr_list;
    (void)memcpy(&in.s_addr, *networkAddr, sizeof(in.s_addr));
    char * ipAddr = inet_ntoa(in);
    return(ipAddr);
}

// check the value of each part of an IP address which should be 
// between 0 and 255
Boolean snmpDeliverTrap_emanate::_isValidOctet(const Uint32& octetValue)
{
    if (octetValue > 0 && octetValue < 255)
    {
	return true;
    }
    else
    {
	return false;
    }
}

PEGASUS_NAMESPACE_END
