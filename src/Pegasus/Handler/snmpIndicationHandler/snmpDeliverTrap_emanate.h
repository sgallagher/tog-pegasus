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
// Modified By: Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "sr_conf.h"
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/param.h>

#include <signal.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#include "sr_snmp.h"
#include "comunity.h"
#include "v2clssc.h"
#include "sr_trans.h"
#include "context.h"
#include "method.h"
#include "diag.h"
#include "subagent.h"
#include "agentsoc.h"
#include "evq.h"
#include "sig.h"

#include <iostream>
#include <Pegasus/Common/Config.h>

#include "snmpDeliverTrap.h"

PEGASUS_NAMESPACE_BEGIN

// l10n
// lots of hacking to make up these messages

static const char _MSG_INITSUBAGENT_FAILED [] = "$0 Failed to initialize";
static const char _MSG_INITSUBAGENT [] = "InitSubagent";
static const char _MSG_INITSUBAGENT_FAILED_KEY [] = 
   "Handler.snmpIndicationHandler.snmpDeliverTrap_emanate._MSG_INITSUBAGENT_FAILED";

static const char _MSG_INVALID_TRAPOID [] = "Invalid $0.";
static const char _MSG_TRAPOID [] = "trapOid";
static const char _MSG_INVALID_KEY [] = 
   "Handler.snmpIndicationHandler.snmpDeliverTrap_emanate._MSG_INVALID";

static const char _MSG_DESTINATION_NOT_FOUND [] = "Can not find trap destination.";
static const char _MSG_DESTINATION_NOT_FOUND_KEY [] = 
   "Handler.snmpIndicationHandler.snmpDeliverTrap_emanate._MSG_DESTINATION_NOT_FOUND";

static const char _MSG_TARGETHOSTFORMAT_NOT_SUPPORTED [] = 
    "$0 and $1 are the only supported $2 values.";
static const char _MSG_HOSTNAME [] = "Host Name";
static const char _MSG_IPV4 [] = "IPV4 Address";
static const char _MSG_TARGETHOSTFORMAT [] = "TargetHostFormat";
static const char _MSG_TARGETHOSTFORMAT_NOT_SUPPORTED_KEY [] = 
   "Handler.snmpIndicationHandler.snmpDeliverTrap_emanate._MSG_TARGETHOSTFORMAT_NOT_SUPPORTED";

static const char _MSG_INVALID_SECURITY_NAME [] = "Invalid $0.";
static const char _MSG_SECURITY_NAME [] = "SNMP SecurityName";
static const char _MSG_INVALID_SECURITY_NAME_KEY [] = 
   "Handler.snmpIndicationHandler.snmpDeliverTrap_emanate._MSG_INVALID";

static const char _MSG_INVALID_OCTET_VALUE [] = 
    "Invalid octet value in trap destination.";
static const char _MSG_INVALID_OCTET_VALUE_KEY [] = 
   "Handler.snmpIndicationHandler.snmpDeliverTrap_emanate._MSG_INVALID_OCTET_VALUE";

static const char _MSG_CREATE_OCTET_FAILED [] = 
    "Creation of empty 4 length $0 failed.";
static const char _MSG_OCTETSTRING [] = "OctetString";
static const char _MSG_CREATE_OCTET_FAILED_KEY [] = 
   "Handler.snmpIndicationHandler.snmpDeliverTrap_emanate._MSG_CREATE_OCTET_FAILED";

static const char _MSG_INVALID_ENTERPRISEOID [] = "Invalid $0.";
static const char _MSG_ENTERPRISEOID [] = "enterpriseOid.";
static const char _MSG_INVALID_ENTERPRISEOID_KEY [] = 
   "Handler.snmpIndicationHandler.snmpDeliverTrap_emanate._MSG_INVALID";

static const char _MSG_INVALID_PROPERTYOID [] = "Invalid $0 of $1 Property.";
static const char _MSG_OID [] = "OID";
static const char _MSG_CIM [] = "CIM";
static const char _MSG_INVALID_PROPERTYOID_KEY [] = 
   "Handler.snmpIndicationHandler.snmpDeliverTrap_emanate._MSG_INVALID_PROPERTYOID";

static const char _MSG_INVALID_PROPERTYVALUE [] = "Invalid value of $0 Property.";
// declared above static const char _MSG_CIM [] = "CIM";
static const char _MSG_INVALID_PROPERTYVALUE_KEY [] = 
   "Handler.snmpIndicationHandler.snmpDeliverTrap_emanate._MSG_INVALID_PROPERTY";

static const char _MSG_MAKE_VARBIND_FAILED [] = 
    "Failed to $0 for type $1.";
static const char _MSG_MAKE_VARBIND_FAILED_KEY [] = 
   "Handler.snmpIndicationHandler.snmpDeliverTrap_emanate._MSG_MAKE_FAILED";
static const char _MSG_VARBIND [] = "MakeVarBindWithValue";

static const char _MSG_OCTET_PRIM_TYPE [] = "OCTET_PRIM_TYPE";

static const char _MSG_OBJECT_ID_TYPE [] = "OBJECT_ID_TYPE";

static const char _MSG_INTEGER_TYPE [] = "INTEGER_TYPE";

static const char _MSG_VERSION_NOT_SUPPORTED [] = 
    "$0 and $1 are the only supported $2 values.";
static const char _MSG_SNMPv1 [] = "SNMPv1 Trap";
static const char _MSG_SNMPv2C [] = "SNMPv2C Trap";
static const char _MSG_SNMPVersion [] = "SNMPVersion";
static const char _MSG_VERSION_NOT_SUPPORTED_KEY [] = 
    "Handler.snmpIndicationHandler.snmpDeliverTrap_emanate._MSG_VERSION_NOT_SUPPORTED";

// l10n
// lots of hacking to make up these messages



static const char _MSG_INITSUBAGENT_FAILED [] = "$0 Failed to initialize";
static const char _MSG_INITSUBAGENT [] = "InitSubagent";
static const char _MSG_INITSUBAGENT_FAILED_KEY [] = 
   "Handler.snmpIndicationHandler.snmpDeliverTrap_emanate._MSG_INITSUBAGENT_FAILED";

static const char _MSG_INVALID_TRAPOID [] = "Invalid $0.";
static const char _MSG_TRAPOID [] = "trapOid";
static const char _MSG_INVALID_KEY [] = 
   "Handler.snmpIndicationHandler.snmpDeliverTrap_emanate._MSG_INVALID";

static const char _MSG_DESTINATION_NOT_FOUND [] = "Can not find trap destination.";
static const char _MSG_DESTINATION_NOT_FOUND_KEY [] = 
   "Handler.snmpIndicationHandler.snmpDeliverTrap_emanate._MSG_DESTINATION_NOT_FOUND";

static const char _MSG_TARGETHOSTFORMAT_NOT_SUPPORTED [] = 
    "$0 and $1 are the only supported $2 values.";
static const char _MSG_HOSTNAME [] = "Host Name";
static const char _MSG_IPV4 [] = "IPV4 Address";
static const char _MSG_TARGETHOSTFORMAT [] = "TargetHostFormat";
static const char _MSG_TARGETHOSTFORMAT_NOT_SUPPORTED_KEY [] = 
   "Handler.snmpIndicationHandler.snmpDeliverTrap_emanate._MSG_TARGETHOSTFORMAT_NOT_SUPPORTED";

static const char _MSG_INVALID_SECURITY_NAME [] = "Invalid $0.";
static const char _MSG_SECURITY_NAME [] = "SNMP SecurityName";
static const char _MSG_INVALID_SECURITY_NAME_KEY [] = 
   "Handler.snmpIndicationHandler.snmpDeliverTrap_emanate._MSG_INVALID";

static const char _MSG_INVALID_OCTET_VALUE [] = 
    "Invalid octet value in trap destination.";
static const char _MSG_INVALID_OCTET_VALUE_KEY [] = 
   "Handler.snmpIndicationHandler.snmpDeliverTrap_emanate._MSG_INVALID_OCTET_VALUE";

static const char _MSG_CREATE_OCTET_FAILED [] = 
    "Creation of empty 4 length $0 failed.";
static const char _MSG_OCTETSTRING [] = "OctetString";
static const char _MSG_CREATE_OCTET_FAILED_KEY [] = 
   "Handler.snmpIndicationHandler.snmpDeliverTrap_emanate._MSG_CREATE_OCTET_FAILED";

static const char _MSG_INVALID_ENTERPRISEOID [] = "Invalid $0.";
static const char _MSG_ENTERPRISEOID [] = "enterpriseOid.";
static const char _MSG_INVALID_ENTERPRISEOID_KEY [] = 
   "Handler.snmpIndicationHandler.snmpDeliverTrap_emanate._MSG_INVALID";

static const char _MSG_INVALID_PROPERTYOID [] = "Invalid $0 of $1 Property.";
static const char _MSG_OID [] = "OID";
static const char _MSG_CIM [] = "CIM";
static const char _MSG_INVALID_PROPERTYOID_KEY [] = 
   "Handler.snmpIndicationHandler.snmpDeliverTrap_emanate._MSG_INVALID_PROPERTYOID";

static const char _MSG_INVALID_PROPERTYVALUE [] = "Invalid value of $0 Property.";
// declared above static const char _MSG_CIM [] = "CIM";
static const char _MSG_INVALID_PROPERTYVALUE_KEY [] = 
   "Handler.snmpIndicationHandler.snmpDeliverTrap_emanate._MSG_INVALID_PROPERTY";

static const char _MSG_MAKE_VARBIND_FAILED [] = 
    "Failed to $0 for type $1.";
static const char _MSG_MAKE_VARBIND_FAILED_KEY [] = 
   "Handler.snmpIndicationHandler.snmpDeliverTrap_emanate._MSG_MAKE_FAILED";
static const char _MSG_VARBIND [] = "MakeVarBindWithValue";

static const char _MSG_OCTET_PRIM_TYPE [] = "OCTET_PRIM_TYPE";

static const char _MSG_OBJECT_ID_TYPE [] = "OBJECT_ID_TYPE";

static const char _MSG_INTEGER_TYPE [] = "INTEGER_TYPE";

static const char _MSG_VERSION_NOT_SUPPORTED [] = 
    "$0 and $1 are the only supported $2 values.";
static const char _MSG_SNMPv1 [] = "SNMPv1 Trap";
static const char _MSG_SNMPv2C [] = "SNMPv2C Trap";
static const char _MSG_SNMPVersion [] = "SNMPVersion";
static const char _MSG_VERSION_NOT_SUPPORTED_KEY [] = 
    "Handler.snmpIndicationHandler.snmpDeliverTrap_emanate._MSG_VERSION_NOT_SUPPORTED";

class snmpDeliverTrap_emanate : public snmpDeliverTrap
{
public:

    snmpDeliverTrap_emanate();

    ~snmpDeliverTrap_emanate();

    void initialize();

    /**
	Send snmp trap to the target.
	@param  trapOid		snmp trap OID
	@param  securityName 	either an SNMPv1 or SNMPv2c community 
				name or an SNMPv3 user name
	@param 	targetHost	address of the trap/infom destination
	@param	targetHostFormat targetHost format
	@param  otherTargetHostFormat other target format type
	@param  portNumber 	UDP port number to send the trap/inform
	@param  snmpVersion	snmp version and format to use to send the indication
	@param 	engineID 	snmp engine ID used to create the SNMPv3 inform
	@param	vbOids		VarBind OIDs
	@param 	vbTypes		VarBind types
	@param 	vbValues	VarBind values
    */
    void deliverTrap(
        const String& trapOid,
        const String& securityName, 
        const String& targetHost, 
        const Uint16& targetHostFormat, 
        const String& otherTargetHostFormat, 
        const Uint32& portNumber,
        const Uint16& snmpVersion, 
        const String& engineID,
        Array<String>& vbOids,
        Array<String>& vbTypes,
        Array<String>& vbValues);

private:
    char * _getIPAddress(const CString& hostName);
    Boolean _isValidOctet(const Uint32& octetValue);

    /**
        Values for the TargetHostFormat property of the 
	PG_IndicationHandlerSNMPMapper class.
    */
    enum TargetHostFormat {_OTHER = 1, _HOST_NAME = 2,
         _IPV4_ADDRESS = 3, _IPV6_ADDRESS = 4};
    enum SNMPVersion {_SNMPv1_TRAP = 2, _SNMPv2C_TRAP = 3,
         _SNMPv2C_INFORM = 4, _SNMPv3_TRAP = 5, _SNMPv3_INFORM = 6};
};

PEGASUS_NAMESPACE_END
