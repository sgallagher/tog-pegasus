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
// Author: Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//
// Modified By: David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//              Vijay Eli, (vijayeli@in.ibm.com) fix for #2572
//
//%/////////////////////////////////////////////////////////////////////////////

// define asprintf used to implement ultostr on Linux
#if defined(PEGASUS_LINUX_GENERIC_GNU)
#define _GNU_SOURCE
#include <features.h>
#include <stdio.h>
#endif

#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/SSLContext.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/PegasusVersion.h>

#include <Pegasus/getoopt/getoopt.h>
#include <Clients/cliutils/CommandException.h>
#include "SSLTrustMgr.h"

#ifdef PEGASUS_OS_OS400
#include "qycmutiltyUtility.H"
#include "qycmutilu2.H"
#include "vfyptrs.cinc"
#include <stdio.h>
#include "OS400ConvertChar.h"
#endif

PEGASUS_NAMESPACE_BEGIN

//l10n
/**
 * The CLI message resource name
 */
static const char MSG_PATH []                  = "pegasus/pegasusCLI";

/**
    The command name.
 */
const char   SSLTrustMgr::COMMAND_NAME []      = "ssltrustmgr";

/**
    Label for the usage string for this command.
 */
const char   SSLTrustMgr::_USAGE []            = "Usage: ";

/**
    The option character used to specify add certificate.
*/
const char   SSLTrustMgr::_OPTION_ADD          = 'a';

/**
    The option character used to specify remove certificate.
*/
const char   SSLTrustMgr::_OPTION_REMOVE       = 'r';

/**
    The option character used to specify list certificates.
*/
const char   SSLTrustMgr::_OPTION_LIST         = 'l';

/**
    The option character used to specify revoke certificate.
*/
const char   SSLTrustMgr::_OPTION_REVOKE       = 'R';

/**
    The option character used to display help info.
*/
const char   SSLTrustMgr::_OPTION_HELP         = 'h';

/**
    The option character used to display version info.
*/
const char   SSLTrustMgr::_OPTION_VERSION      = 'v';

/**
    The option character used to specify the trust store name.
 */
const char   SSLTrustMgr::_OPTION_TRUSTSTORE   = 't';

/**
    The option character used to specify the certificate file path
    or a CRL file path.
 */
const char   SSLTrustMgr::_OPTION_CERTFILE     = 'f';

/**
    The option character used to specify the certificate user name.
 */
const char   SSLTrustMgr::_OPTION_CERTUSER     = 'c';

/**
    The option character used to specify the client trust store path.
 */
const char   SSLTrustMgr::_OPTION_TRUSTPATH  = 'T';

/**
    The option character used to specify the issuer name.
 */
const char   SSLTrustMgr::_OPTION_ISSUERNAME   = 'i';

/**
    The option character used to specify the serial number.
 */
const char   SSLTrustMgr::_OPTION_SERIALNUMBER = 'n';


//
// Setting this flag will compile in the code that will enable the
// SSLTrustMgr command to connect to a remote host. This compile time
// flag is expected to be removed once the Certificate Management Provider
// MOF is moved from PG_Internal to PH_InterOp.
//
#ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION

/**
    The option character used to specify the hostname.
 */
const char   SSLTrustMgr::_OPTION_HOSTNAME     = 'h';

/**
    The option character used to specify the port number.
 */
const char   SSLTrustMgr::_OPTION_PORTNUMBER   = 'p';

/**
    The option character used to specify SSL usage.
 */
const char   SSLTrustMgr::_OPTION_SSL          = 's';

/**
    The option character used to specify the timeout value.
 */
const char   SSLTrustMgr::_OPTION_TIMEOUT      = 'o';

/**
    The option character used to specify the username.
 */
const char   SSLTrustMgr::_OPTION_USERNAME     = 'u';

/**
    The option character used to specify the password.
 */
const char   SSLTrustMgr::_OPTION_PASSWORD     = 'w';


/**
    The minimum valid portnumber.
 */
const Uint32 SSLTrustMgr::_MIN_PORTNUMBER      = 0;

/**
    The maximum valid portnumber.
 */
const Uint32 SSLTrustMgr::_MAX_PORTNUMBER      = 65535;

/**
    The maximum number of password retries.
*/
const Uint32 SSLTrustMgr::_MAX_PW_RETRIES      = 3;

#endif /* #ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION */


/**
    This constant signifies that an operation option has not
    been recorded
*/
const Uint32 SSLTrustMgr::_OPERATION_TYPE_UNINITIALIZED = 0;

/**
    The constant representing a add operation
*/
const Uint32 SSLTrustMgr::_OPERATION_TYPE_ADD = 1;

/**
    The constant representing a remove operation
*/
const Uint32 SSLTrustMgr::_OPERATION_TYPE_REMOVE = 2;

/**
    The constant representing a revoke operation
*/
const Uint32 SSLTrustMgr::_OPERATION_TYPE_REVOKE = 3;

/**
    The constant representing a list operation
*/
const Uint32 SSLTrustMgr::_OPERATION_TYPE_LIST = 4;

/**
    The constant representing a help operation
*/
const Uint32 SSLTrustMgr::_OPERATION_TYPE_HELP = 5;

/**
    The constant representing a version display operation
*/
const Uint32 SSLTrustMgr::_OPERATION_TYPE_VERSION = 6;


/**
    The name of the Method that implements add certificate to the
    trust store.
*/
static const CIMName ADD_CERTIFICATE_METHOD = CIMName ("addCertificate");

/**
    This constant represents the name of the Issuer name
    property in the schema
*/
static const CIMName PROPERTY_NAME_ISSUERNAME = CIMName ("IssuerName");

/**
    This constant represents the name of the Serial number
    property in the schema
*/
static const CIMName PROPERTY_NAME_SERIALNUMBER = CIMName ("SerialNumber");

/**
    This constant represents the name of the trust store type
    property in the schema
*/
static const CIMName PROPERTY_NAME_TRUSTTYPE    = CIMName ("TruststoreType");

/**
    This constant represents the name of the Subject name type
    property in the schema
*/
static const CIMName PROPERTY_NAME_SUBJECTNAME  = CIMName ("SubjectName");

/**
    This constant represents the name of the notBefore type
    property in the schema
*/
static const CIMName PROPERTY_NAME_NOTBEFORE    = CIMName ("NotBefore");

/**
    This constant represents the name of the notAfter type
    property in the schema
*/
static const CIMName PROPERTY_NAME_NOTAFTER     = CIMName ("NotAfter");

/**
    This constant represents the name of the lastUpdate type
    property in the schema
*/
static const CIMName PROPERTY_NAME_LASTUPDATE   = CIMName ("LastUpdate");

/**
    This constant represents the name of the nextUpdate type
    property in the schema
*/
static const CIMName PROPERTY_NAME_NEXTUPDATE   = CIMName ("NextUpdate");

/**
    This constant represents the name of the revokedSerialNumbers type
    property in the schema
*/
static const CIMName PROPERTY_NAME_REVOKED_SERIAL_NUMBERS =
                                      CIMName ("RevokedSerialNumbers");

/**
    This constant represents the name of the revocationDates type
    property in the schema
*/
static const CIMName PROPERTY_NAME_REVOCATION_DATES =
                                      CIMName ("RevocationDates");

/**
    This constant represents the name of the Truststore path type
    property in the schema
*/
static const CIMName PROPERTY_NAME_TRUSTPATH = CIMName ("TruststorePath");

/**
    The name of the Method that implements add revoked certificate to
    the CRL store.
*/
static const CIMName ADD_CRL_METHOD = CIMName ("addCertificateRevocationList");

/**
    The input parameter name for certificate content.
*/
static const char   CERT_CONTENTS []    = "certificateContents";

/**
    The input parameter name for registered certificate user.
*/
static const char   CERT_USERNAME []    = "userName";

/**
    The input parameter name for trust store type.
*/
static const char   CERT_TRUSTTYPE []   = "truststoreType";

/**
    The input parameter name for trust store path.
*/
static const char   CERT_TRUSTPATH []   = "truststorePath";

/**
    The input parameter name for CRL content.
*/
static const char   CRL_CONTENTS []     = "CRLContents";

/**
    The constants represeting the cimserver trust store and store name.
*/
static const Uint16 CIMSERVER_TRUST     = 2;

static const char CIMSERVER_TRUST_NAME []    = "cim_trust";

/**
    The constants represeting the indication trust store and store name.
*/
static const Uint16 INDICATION_TRUST         = 3;

static const char INDICATION_TRUST_NAME []   = "export_trust";

/**
    The constants represeting the client trust store and store name.
*/
static const Uint16 CLIENT_TRUST             = 4;

static const char CLIENT_TRUST_NAME []       = "client_trust";

/**
    The constants represeting the CRL store and the name.
*/
static const Uint16 CRL_STORE                = 5;

static const char CRL_STORE_NAME []          = "crl";


/**
    The constants representing the display messages.
*/
static const char   LONG_HELP []  = "help";

static const char   LONG_VERSION []  = "version";

static const char PASSWORD_PROMPT []  =
                "Please enter your password: ";

static const char PASSWORD_BLANK []  =
                "Password cannot be blank. Please re-enter your password.";

static const char REQUIRED_ARGS_MISSING [] =
                "Required arguments missing.";

static const char REQUIRED_ARGS_MISSING_KEY [] =
                "Clients.ssltrustmgr.SSLTrustMgr.REQUIRED_ARGS_MISSING";

static const char CIMOM_NOT_RUNNING [] =
                "CIM Server may not be running.";

static const char CIMOM_NOT_RUNNING_KEY [] =
                "Clients.ssltrustmgr.SSLTrustMgr.CIMOM_NOT_RUNNING";

static const char ADD_CERT_FAILURE []    =
                "Failed to add certificate.";

static const char ADD_CERT_FAILURE_KEY []    =
                "Clients.ssltrustmgr.SSLTrustMgr.ADD_CERT_FAILURE";

static const char ADD_CRL_FAILURE []    =
                "Failed to add CRL.";

static const char ADD_CRL_FAILURE_KEY []    =
                "Clients.ssltrustmgr.SSLTrustMgr.ADD_CRL_FAILURE";

static const char ADD_CERTIFICATE_SUCCESS [] =
                "Certificate added successfully.";

static const char ADD_CERTIFICATE_SUCCESS_KEY [] =
                "Clients.ssltrustmgr.SSLTrustMgr.ADD_CERTIFICATE_SUCCESS";

static const char ADD_CRL_SUCCESS [] =
                "CRL added successfully.";

static const char ADD_CRL_SUCCESS_KEY [] =
                "Clients.ssltrustmgr.SSLTrustMgr.ADD_CRL_SUCCESS";

static const char REMOVE_CERTIFICATE_SUCCESS [] =
                "Certificate removed successfully.";

static const char REMOVE_CERTIFICATE_SUCCESS_KEY [] =
                "Clients.ssltrustmgr.SSLTrustMgr.REMOVE_CERTIFICATE_SUCCESS";

static const char REMOVE_CRL_SUCCESS [] =
                "CRL removed successfully.";

static const char REMOVE_CRL_SUCCESS_KEY [] =
                "Clients.ssltrustmgr.SSLTrustMgr.REMOVE_CRL_SUCCESS";

static const char CERT_ALREADY_EXISTS []        =
                "Specified certificate already exist.";

static const char CERT_ALREADY_EXISTS_KEY []        =
                "Clients.ssltrustmgr.SSLTrustMgr.CERT_ALREADY_EXISTS";

static const char CERT_NOT_FOUND [] =
                "Specified certificate does not exist.";

static const char CERT_NOT_FOUND_KEY [] =
                "Clients.ssltrustmgr.SSLTrustMgr.CERT_NOT_FOUND";

static const char CRL_NOT_FOUND [] =
                "Specified certificate does not exist.";

static const char CRL_NOT_FOUND_KEY [] =
                "Clients.ssltrustmgr.SSLTrustMgr.CRL_NOT_FOUND";

static const char CERT_SCHEMA_NOT_LOADED []  =
                "Please restore the internal repository on the CIM Server.";

static const char CERT_SCHEMA_NOT_LOADED_KEY []  =
                "Clients.ssltrustmgr.SSLTrustMgr.CERT_SCHEMA_NOT_LOADED";

static const char REMOVE_CERT_FAILURE [] =
                "Failed to remove certificate.";

static const char REMOVE_CERT_FAILURE_KEY []  =
                "Clients.ssltrustmgr.SSLTrustMgr.REMOVE_CERT_FAILURE";

static const char REVOKE_CERT_FAILURE [] =
                "Failed to revoke certificate.";

static const char REVOKE_CERT_FAILURE_KEY []  =
                "Clients.ssltrustmgr.SSLTrustMgr.REVOKE_CERT_FAILURE";

static const char CERT_ALREADY_REVOKED [] =
                "Specified certificate already revoked.";

static const char CERT_ALREADY_REVOKED_KEY [] =
                "Clients.ssltrustmgr.SSLTrustMgr.CERT_ALREADY_REVOKED";

static const char REMOVE_CRL_FAILURE [] =
                "Failed to remove CRL.";

static const char REMOVE_CRL_FAILURE_KEY []  =
                "Clients.ssltrustmgr.SSLTrustMgr.REMOVE_CRL_FAILURE";

static const char LIST_CERT_FAILURE [] =
                "Failed to list certificate.";

static const char LIST_CERT_FAILURE_KEY [] =
                "Clients.ssltrustmgr.SSLTrustMgr.LIST_CERT_FAILURE";

static const char LIST_CRL_FAILURE [] =
                "Failed to list CRL.";

static const char LIST_CRL_FAILURE_KEY [] =
                "Clients.ssltrustmgr.SSLTrustMgr.LIST_CRL_FAILURE";

/**
    The constant representing the trust manager provider class name
*/
static const char PG_SSLTRUST_MGR_PROV_CLASS []    = "PG_SSLTrustManager";



#ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION
static Boolean verifyCertificate(SSLCertificateInfo &certInfo)
{
    //
    // If server certificate was found in CA trust store and validated, then
    // return 'true' to accept the certificate, otherwise return 'false'.
    //
    if (certInfo.getResponseCode() == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}
#endif /* #ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION */

/**

    Constructs a SSLTrustMgr and initializes instance variables.

 */
SSLTrustMgr::SSLTrustMgr ()
{
    //
    // Initialize the instance variables.
    //
    _operationType       = _OPERATION_TYPE_UNINITIALIZED;

    _trustStoreSet       = false;
    _trustPathSet        = false;
    _certFileSet         = false;
    _certUserSet         = false;;
    _issuerNameSet       = false;;
    _serialNumberSet     = false;;
    _crlSet              = false;;

#ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION
    _hostNameSet         = false;
    _portNumber          = WBEM_DEFAULT_HTTP_PORT;
    _portNumberSet       = false;

    char buffer[32];
    sprintf(buffer, "%lu", (unsigned long) _portNumber);
    _portNumberStr       = buffer;

    _userNameSet         = false;
    _passwordSet         = false;
    _useSSL              = false;
#endif /* #ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION */
    _timeout             = DEFAULT_TIMEOUT_MILLISECONDS;

    //
    // build Usage string.
    //
    _usage = String (_USAGE);
    _usage.append (COMMAND_NAME);

    //
    // Add option 1
    //
    _usage.append (" -").append (_OPTION_ADD);
    _usage.append (" [ -").append (_OPTION_TRUSTSTORE).append (" truststore")
         .append (" ]");
    _usage.append (" -").append (_OPTION_CERTUSER).append (" certuser");
    _usage.append (" -").append (_OPTION_CERTFILE).append (" certfile");

#ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION
    _usage.append (" [ -").append (_OPTION_HOSTNAME).append (" hostname")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_PORTNUMBER).append (" portnumber")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_TIMEOUT).append (" timeout")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_USERNAME).append (" username")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_PASSWORD).append (" password")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_SSL)
         .append (" ]");
#endif /* #ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION */
    _usage.append ("\n");


    //
    // Add option 2
    //
    //_usage.append ("                   -").append (_OPTION_ADD);
    _usage.append("       ").append (COMMAND_NAME);
    _usage.append (" -").append (_OPTION_ADD);
    _usage.append (" -").append (_OPTION_TRUSTPATH).append (" trustpath");
    _usage.append (" -").append (_OPTION_CERTFILE).append (" certfile");

#ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION
    _usage.append (" [ -").append (_OPTION_HOSTNAME).append (" hostname")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_PORTNUMBER).append (" portnumber")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_TIMEOUT).append (" timeout")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_USERNAME).append (" username")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_PASSWORD).append (" password")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_SSL)
         .append (" ]");
#endif /* #ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION */
    _usage.append ("\n");

    //
    // Add option 3
    //
    //_usage.append ("                   -").append (_OPTION_ADD);
    _usage.append("       ").append (COMMAND_NAME);
    _usage.append (" -").append (_OPTION_ADD);
    _usage.append (" -").append (_OPTION_REVOKE);
    _usage.append (" -").append (_OPTION_CERTFILE).append (" crlfile");

#ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION
    _usage.append (" [ -").append (_OPTION_HOSTNAME).append (" hostname")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_PORTNUMBER).append (" portnumber")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_TIMEOUT).append (" timeout")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_USERNAME).append (" username")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_PASSWORD).append (" password")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_SSL)
         .append (" ]");
#endif /* #ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION */
    _usage.append ("\n");


    //
    // Remove option 1
    //
    //_usage.append ("                   -").append (_OPTION_REMOVE);
    _usage.append("       ").append (COMMAND_NAME);
    _usage.append (" -").append (_OPTION_REMOVE);
    _usage.append (" [ -").append (_OPTION_TRUSTSTORE).append (" truststore")
         .append (" |");
    _usage.append (" -").append (_OPTION_TRUSTPATH).append (" trustpath")
         .append (" ]");
    _usage.append (" -").append (_OPTION_ISSUERNAME).append (" issuername");
    _usage.append (" -").append (_OPTION_SERIALNUMBER).append (" serialnumber");

#ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION
    _usage.append (" [ -").append (_OPTION_HOSTNAME).append (" hostname")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_PORTNUMBER).append (" portnumber")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_TIMEOUT).append (" timeout")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_USERNAME).append (" username")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_PASSWORD).append (" password")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_SSL)
         .append (" ]");
#endif /* #ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION */
    _usage.append ("\n");


    //
    // Remove option 2
    //
    //_usage.append ("                   -").append (_OPTION_REMOVE);
    _usage.append("       ").append (COMMAND_NAME);
    _usage.append (" -").append (_OPTION_REMOVE);
    _usage.append (" -").append (_OPTION_REVOKE);
    _usage.append (" -").append (_OPTION_ISSUERNAME).append (" issuername");

#ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION
    _usage.append (" [ -").append (_OPTION_HOSTNAME).append (" hostname")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_PORTNUMBER).append (" portnumber")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_TIMEOUT).append (" timeout")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_USERNAME).append (" username")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_PASSWORD).append (" password")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_SSL)
         .append (" ]");
#endif /* #ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION */
    _usage.append ("\n");


    //
    // List option 1
    //
    //_usage.append ("                   -").append (_OPTION_LIST);
    _usage.append("       ").append (COMMAND_NAME);
    _usage.append (" -").append (_OPTION_LIST);
    _usage.append (" [ -").append (_OPTION_TRUSTSTORE).append (" truststore")
         .append (" |");
    _usage.append (" -").append (_OPTION_TRUSTPATH).append (" trustpath")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_ISSUERNAME).append (" issuername");
    _usage.append (" [ -").append (_OPTION_SERIALNUMBER)
         .append (" serialnumber")
         .append (" ] ]");

#ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION
    _usage.append (" [ -").append (_OPTION_HOSTNAME).append (" hostname")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_PORTNUMBER).append (" portnumber")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_TIMEOUT).append (" timeout")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_USERNAME).append (" username")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_PASSWORD).append (" password")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_SSL)
         .append (" ]");
#endif /* #ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION */
    _usage.append ("\n");

    //
    // List option 2
    //
    //_usage.append ("                   -").append (_OPTION_LIST);
    _usage.append("       ").append (COMMAND_NAME);
    _usage.append (" -").append (_OPTION_LIST);
    _usage.append (" -").append (_OPTION_REVOKE);
    _usage.append (" [ -").append (_OPTION_ISSUERNAME).append (" issuername")
         .append (" ]");

#ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION
    _usage.append (" [ -").append (_OPTION_HOSTNAME).append (" hostname")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_PORTNUMBER).append (" portnumber")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_TIMEOUT).append (" timeout")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_USERNAME).append (" username")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_PASSWORD).append (" password")
         .append (" ]");
    _usage.append (" [ -").append (_OPTION_SSL)
         .append (" ]");
#endif /* #ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION */
    _usage.append ("\n");

    //
    // Version options
    //
    _usage.append("       ").append (COMMAND_NAME);
    _usage.append (" -").append (_OPTION_VERSION)
         .append(" |");
    _usage.append (" --").append (LONG_VERSION)
         .append("\n");

    //
    // Help option
    //
    _usage.append("       ").append (COMMAND_NAME);
    _usage.append (" -").append (_OPTION_HELP)
         .append(" |");
    _usage.append (" --").append (LONG_HELP)
         .append("\n");

    //
    // Options description
    //
    _usage.append("Options : \n");
    _usage.append("    -a              ");
    _usage.append("- Adds the specified certificate to the trust store\n");

    _usage.append("    -r              ");
    _usage.append("- Removes the specified certificate from the trust store\n");

    _usage.append("    -l              ");
    _usage.append("- Displays the certificates in the trust store\n");

    _usage.append("    -R              ");
    _usage.append("- Performs requested add, remove or list operation ");
    _usage.append("on the CRL store\n");

    _usage.append("    -t truststore   ");
    _usage.append("- Specifies the target trust store name\n");

    _usage.append("    -T trustpath    ");
    _usage.append("- Specifies the target trust store path\n");

    _usage.append("    -f certfile     ");
    _usage.append("- Specifies the PEM format file containing ");
    _usage.append("an X509 certificate\n");

    _usage.append("    -f crlfile      ");
    _usage.append("- Specifies the PEM format file containing a CRL\n");

    _usage.append("    -c certuser     ");
    _usage.append("- Specifies the user name to be associated with the ");
    _usage.append("certificate\n");

    _usage.append("    -i issuername   ");
    _usage.append("- Specifies the certificate or the CRL issuer name\n");

    _usage.append("    -n serialnumber ");
    _usage.append("- Specifies the certificate serial number\n");

    _usage.append("    -h, --help      - Display this help message\n");
    _usage.append("    -v, --version   - Display CIM Server version number\n");

    _usage.append("\nUsage note: The ssltrustmgr command requires that ");
    _usage.append("the CIM Server is running.\n");

//l10n localize usage
#ifdef PEGASUS_HAS_ICU

    MessageLoaderParms menuparms(
        "Clients.ssltrustmgr.SSLTrustMgr.MENU.STANDARD", _usage);
    menuparms.msg_src_path = MSG_PATH;
    _usage = MessageLoader::getMessage(menuparms);

#endif

    setUsage (_usage);
}

#ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION
/**
    Prompt for password.

    @param   outPrintWriter     the ostream to which output should be
                                written
    @return  String value of the user entered password
 */
String SSLTrustMgr::_promptForPassword( ostream& outPrintWriter )
{
    //
    // Password is not set, prompt for non-blank password
    //
    String pw = String::EMPTY;
    Uint32 retries = 1;

    do
    {
        pw = System::getPassword( PASSWORD_PROMPT );

        if ( pw == String::EMPTY || pw == "" )
        {
            if( retries < _MAX_PW_RETRIES )
            {
                retries++;
            }
            else
            {
                break;
            }
            outPrintWriter << PASSWORD_BLANK << endl;
            pw = String::EMPTY;
            continue;
        }
    }
    while ( pw == String::EMPTY );

    return( pw );
}
#endif /* #ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION */

/**
    Connect to cimserver.

    @param  client             the handle to CIMClient object

    @param  outPrintWriter     the ostream to which output should be
                               written

    @exception  Exception      if an error is encountered in creating
                               the connection
 */
void SSLTrustMgr::_connectToServer( CIMClient& client,
                                    ostream& outPrintWriter )
{
#ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION
    String                 host;
    Uint32                 portNumber            = 0;
    Boolean                connectToLocal        = false;

    //
    //  Construct host address
    //
    if ((!_hostNameSet) && (!_portNumberSet))
    {
        connectToLocal = true;
    }
    else
    {
        if (!_hostNameSet)
        {
           _hostName = System::getHostName();
        }

        if( !_portNumberSet )
        {
           if( _useSSL )
           {
               _portNumber = System::lookupPort( WBEM_HTTPS_SERVICE_NAME,
                                          WBEM_DEFAULT_HTTPS_PORT );
           }
           else
           {
               _portNumber = System::lookupPort( WBEM_HTTP_SERVICE_NAME,
                                          WBEM_DEFAULT_HTTP_PORT );
           }
           char buffer[32];
           sprintf( buffer, "%lu", (unsigned long) _portNumber );
           _portNumberStr = buffer;
        }
    }
    host = _hostName;
    portNumber = _portNumber;

    if( connectToLocal )
    {
        client.connectLocal();
    }
    else
    {
        if (!_userNameSet)
        {
            _userName = System::getEffectiveUserName();
        }

        if( _useSSL )
        {
            //
            // Get environment variables:
            //
            const char* pegasusHome = getenv("PEGASUS_HOME");

            String certpath = FileSystem::getAbsolutePath(
                pegasusHome, PEGASUS_SSLCLIENT_CERTIFICATEFILE);

            String randFile = String::EMPTY;

#ifdef PEGASUS_SSL_RANDOMFILE
            randFile = FileSystem::getAbsolutePath(
                pegasusHome, PEGASUS_SSLCLIENT_RANDOMFILE);
#endif
            SSLContext sslcontext (certpath, verifyCertificate, randFile);

            client.connect(host, portNumber, sslcontext,  _userName, _password );
        }
        else
        {
            client.connect(host, portNumber, _userName, _password );
        }
        //NOTE: User try block around connect() and prompt for password
        //      on HTTP exception 401 (HTTP Authentication challenge from
        //      the CIMServer).
    }
#else

    client.connectLocal();

#endif /* #ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION */

}


/**
    Convert CIMDateTime to user-readable string of the format
    month day-of-month, year  hour:minute:second (value-hrs-GMT-offset)

    @param  cimDateTimeStr  CIM formated DateTime String

    @return  String user-readable date time string.
 */
String SSLTrustMgr::_formatCIMDateTime(const String& cimDateTimeStr)
{
   Uint32 year = 0;
   Uint32 month = 0;
   Uint32 day = 0;
   Uint32 hour = 0;
   Uint32 minute = 0;
   Uint32 second = 0;
   Uint32 microsecond = 0;
   Uint32 timezone = 0;

   sscanf(cimDateTimeStr.getCString(), "%04d%02d%02d%02d%02d%02d.%06d%04d",
          &year, &month, &day, &hour, &minute, &second,
          &microsecond, &timezone);

   char monthString[5];

   switch (month)
   {
      case 1 :
          sprintf(monthString, "Jan");
          break;
      case 2 :
          sprintf(monthString, "Feb");
          break;
      case 3 :
          sprintf(monthString, "Mar");
          break;
      case 4 :
          sprintf(monthString, "Apr");
          break;
      case 5 :
          sprintf(monthString, "May");
          break;
      case 6 :
          sprintf(monthString, "Jun");
          break;
      case 7 :
          sprintf(monthString, "Jul");
          break;
      case 8 :
          sprintf(monthString, "Aug");
          break;
      case 9 :
          sprintf(monthString, "Sep");
          break;
      case 10 :
          sprintf(monthString, "Oct");
          break;
      case 11 :
          sprintf(monthString, "Nov");
          break;
      case 12 :
          sprintf(monthString, "Dec");
          break;

      // covered all known cases, if get to default, just
      // return the input string as received.
      default :
          return (cimDateTimeStr);
   }

   char dateTimeStr[80];
   sprintf(dateTimeStr, "%s %d, %d  %d:%02d:%02d (%03d%02d)",
           monthString, day, year, hour, minute, second,
           timezone/60, timezone%60);

   String retVal = String(dateTimeStr);

   return (retVal);
}


/**
    Read certificate content.

    @param  certFilePath     the certificate file path

    @return  Array<char> containing the certificate content

    @exception  Exception    if an error is encountered
                             in reading the certificate file
 */
Array<char> SSLTrustMgr::_readCertificateContent(const String &certFilePath)
{
    Array<char> content;

    //
    //  Check that cert file exists
    //
    if (!FileSystem::exists (certFilePath))

    {
        NoSuchFile e (certFilePath);
        throw e;
    }

    //
    //  Check that cert file is readable
    //
    if (!FileSystem::canRead (certFilePath))
    {
        FileNotReadable e (certFilePath);
        throw e;
    }

    //
    //  Load file context to memory
    //
    try
    {
        FileSystem::loadFileToMemory (content, certFilePath);
        content.append ('\0');
    }
    catch (const CannotOpenFile&)
    {
        throw;
    }

    return content;
}


/**
   Add a new certificate to the trust store

   @param  client           the handle to CIMClient object

   @param  outPrintWriter   the ostream to which output should be written

   @exception  Exception    if failed to add certificate
 */
void SSLTrustMgr::_addCertificate (
    CIMClient&   client,
    ostream&     outPrintWriter)
{
    try
    {
        Array<char>              content;
        Array<CIMKeyBinding>   kbArray;
        CIMKeyBinding          kb;
        Array<CIMParamValue>   inParams;
        Array<CIMParamValue>   outParams;

        content = _readCertificateContent(_certFile);

        String contentStr = String(content.getData());

        //
        // Build the input params
        //
        inParams.append ( CIMParamValue ( CERT_CONTENTS,
                                          CIMValue (contentStr)));
        inParams.append ( CIMParamValue ( CERT_USERNAME,
                                          CIMValue (_certUser)));

        if (_trustPathSet )
        {
            inParams.append ( CIMParamValue ( CERT_TRUSTTYPE,
                                              CIMValue (CLIENT_TRUST)));
        }
        else if (_trustStoreSet &&
                 String::equal(_trustStore, INDICATION_TRUST_NAME))
        {
            inParams.append ( CIMParamValue ( CERT_TRUSTTYPE,
                                              CIMValue (INDICATION_TRUST)));
        }
        else
        {
            inParams.append ( CIMParamValue ( CERT_TRUSTTYPE,
                                              CIMValue (CIMSERVER_TRUST)));
        }

        inParams.append ( CIMParamValue ( CERT_TRUSTPATH,
                                          CIMValue (_trustPath)));

        CIMObjectPath reference(
            _hostName, PEGASUS_NAMESPACENAME_CERTIFICATE,
            PEGASUS_CLASSNAME_CERTIFICATE, kbArray);

        //
        // Call the invokeMethod with the input parameters
        //

        //
        // Not checking for return code as all error conditions will
        // throw exceptions and will be handled by the catch block. If new
        // return codes are added in future, they need to be handled here.
        //
        CIMValue retValue = client.invokeMethod (
                                       PEGASUS_NAMESPACENAME_CERTIFICATE,
                                       reference,
                                       ADD_CERTIFICATE_METHOD,
                                       inParams,
                                       outParams );

        outPrintWriter << localizeMessage(MSG_PATH, ADD_CERTIFICATE_SUCCESS_KEY,
            ADD_CERTIFICATE_SUCCESS) << endl;
    }
    catch (const Exception& )
    {
        throw;
    }
}

/**
   Remove an existing certificate from the trust store

   @param  client           the handle to CIMClient object

   @param  outPrintWriter   the ostream to which output should be written

   @exception  Exception    if failed to remove certificate
 */
void SSLTrustMgr::_removeCertificate (
    CIMClient&   client,
    ostream&     outPrintWriter)
{
    Array<CIMKeyBinding> kbArray;
    CIMKeyBinding        kb;

    try
    {
        //
        // Build the input params
        //
        kb.setName(PROPERTY_NAME_ISSUERNAME);
        kb.setValue(_issuerName);
        kb.setType(CIMKeyBinding::STRING);

        kbArray.append(kb);

        kb.setName(PROPERTY_NAME_SERIALNUMBER);
        kb.setValue(_serialNumber);
        kb.setType(CIMKeyBinding::STRING);

        kbArray.append(kb);

        Uint16 trustType;

        if (_trustPathSet )
        {
            trustType = CLIENT_TRUST;
        }
        else if (_trustStoreSet &&
            String::equal(_trustStore, INDICATION_TRUST_NAME))
        {
            trustType = INDICATION_TRUST;
        }
        else
        {
            trustType = CIMSERVER_TRUST;
        }

        char trustStoreType[2];
        sprintf(trustStoreType, "%d", trustType);

        kb.setName(PROPERTY_NAME_TRUSTTYPE);
        kb.setValue(String(trustStoreType));
        kb.setType(CIMKeyBinding::NUMERIC);

        kbArray.append(kb);

        CIMObjectPath reference(
            _hostName, PEGASUS_NAMESPACENAME_CERTIFICATE,
            PEGASUS_CLASSNAME_CERTIFICATE, kbArray);

        client.deleteInstance(
            PEGASUS_NAMESPACENAME_CERTIFICATE,
            reference);

        outPrintWriter << localizeMessage(MSG_PATH, REMOVE_CERTIFICATE_SUCCESS_KEY,
            REMOVE_CERTIFICATE_SUCCESS) << endl;
    }
    catch (const Exception& )
    {
        throw;
    }
}

/**
   Add a CRL to the CRL store

   @param  client           the handle to CIMClient object

   @param  outPrintWriter   the ostream to which output should be written

   @exception  Exception    if failed to add CRL
 */
void SSLTrustMgr::_addCRL(
    CIMClient&   client,
    ostream&     outPrintWriter)
{
    try
    {
        Array<char>              content;
        Array<CIMKeyBinding>   kbArray;
        CIMKeyBinding          kb;
        Array<CIMParamValue>   inParams;
        Array<CIMParamValue>   outParams;

        content = _readCertificateContent(_certFile);

        String contentStr = String(content.getData());

        //
        // Build the input params
        //
        inParams.append ( CIMParamValue ( CRL_CONTENTS,
                                          CIMValue (contentStr)));

        CIMObjectPath reference(
            _hostName, PEGASUS_NAMESPACENAME_CERTIFICATE,
            PEGASUS_CLASSNAME_CRL, kbArray);

        //
        // Call the invokeMethod with the input parameters
        //

        //
        // Not checking for return code as all error conditions will
        // throw exceptions and will be handled by the catch block. If new
        // return codes are added in future, they need to be handled here.
        //
        CIMValue retValue = client.invokeMethod (
                                       PEGASUS_NAMESPACENAME_CERTIFICATE,
                                       reference,
                                       ADD_CRL_METHOD,
                                       inParams,
                                       outParams );

        outPrintWriter << localizeMessage(MSG_PATH, ADD_CRL_SUCCESS_KEY,
            ADD_CRL_SUCCESS) << endl;
    }
    catch (const Exception& )
    {
        throw;
    }
}

/**
   Remove an existing CRL from the CRL store

   @param  client           the handle to CIMClient object

   @param  outPrintWriter   the ostream to which output should be written

   @exception  Exception    if failed to remove CRL
 */
void SSLTrustMgr::_removeCRL (
    CIMClient&   client,
    ostream&     outPrintWriter)
{
    Array<CIMKeyBinding> kbArray;
    CIMKeyBinding        kb;

    try
    {
        //
        // Build the input params
        //
        kb.setName(PROPERTY_NAME_ISSUERNAME);
        kb.setValue(_issuerName);
        kb.setType(CIMKeyBinding::STRING);

        kbArray.append(kb);

        CIMObjectPath reference(
            _hostName, PEGASUS_NAMESPACENAME_CERTIFICATE,
            PEGASUS_CLASSNAME_CRL, kbArray);

        client.deleteInstance(
            PEGASUS_NAMESPACENAME_CERTIFICATE,
            reference);

        outPrintWriter << localizeMessage(MSG_PATH, REMOVE_CRL_SUCCESS_KEY,
            REMOVE_CRL_SUCCESS) << endl;
    }
    catch (const Exception& )
    {
        throw;
    }
}


/**
   List certificates in the trust store

   @param  client           the handle to CIMClient object

   @param  outPrintWriter   the ostream to which output should be written

   @exception  Exception    if failed to list certificates
 */
void SSLTrustMgr::_listCertificates (
    CIMClient&   client,
    ostream&     outPrintWriter)
{
    Array<CIMInstance> certificateNamedInstances;

    try
    {
        //
        // get all the instances of class PG_SSLCertificate
        //
        certificateNamedInstances =
            client.enumerateInstances(
                PEGASUS_NAMESPACENAME_CERTIFICATE,
                PEGASUS_CLASSNAME_CERTIFICATE);

        //
        // copy all the certificate contents
        //
        Uint32 numberInstances = certificateNamedInstances.size();

        for (Uint32 i = 0; i < numberInstances; i++)
        {
            CIMInstance& certificateInstance =
                certificateNamedInstances[i];

            Uint16 trustType;
            String issuerName;
            String serialNumber;
            String subjectName;
            String truststorePath;
            CIMDateTime notBefore;
            CIMDateTime notAfter;

            //
            // Check if trust store type match
            //
            Uint32 pos =
                certificateInstance.findProperty(PROPERTY_NAME_TRUSTTYPE);
            CIMProperty prop = (CIMProperty)certificateInstance.getProperty(pos);
            prop.getValue().get(trustType);

            if ( (String::equal(_trustStore, CIMSERVER_TRUST_NAME) &&
                     trustType != CIMSERVER_TRUST) ||
                 (String::equal(_trustStore, INDICATION_TRUST_NAME) &&
                     trustType != INDICATION_TRUST) ||
                 (String::equal(_trustStore, CLIENT_TRUST_NAME) &&
                     trustType != CLIENT_TRUST)
               )
            {
                continue;
            }

            //
            //
            pos = certificateInstance.findProperty(PROPERTY_NAME_TRUSTPATH);
            prop = (CIMProperty)certificateInstance.getProperty(pos);
            truststorePath.assign(prop.getValue().toString());

            if ( _trustPathSet && trustType == CLIENT_TRUST )
            {
                if ( !String::equal(_trustPath, truststorePath) )
                {
                    continue;
                }
            }

            //
            // Check if issuer name and serial number are specified
            // and they match
            //
            pos = certificateInstance.findProperty(PROPERTY_NAME_ISSUERNAME);
            prop = (CIMProperty)certificateInstance.getProperty(pos);
            issuerName.assign(prop.getValue().toString());

            pos = certificateInstance.findProperty(PROPERTY_NAME_SERIALNUMBER);
            prop = (CIMProperty)certificateInstance.getProperty(pos);
            serialNumber.assign(prop.getValue().toString());

            if ( _issuerNameSet )
            {
                if (String::equal(_issuerName, issuerName))
                {
                    if (_serialNumberSet &&
                        !String::equal(_serialNumber, serialNumber))
                    {
                        continue;
                    }
                }
                else
                {
                    continue;
                }
            }

            //
            // Get the remaining properties and display them.
            //
            pos = certificateInstance.findProperty(PROPERTY_NAME_SUBJECTNAME);
            prop = (CIMProperty)certificateInstance.getProperty(pos);
            subjectName.assign(prop.getValue().toString());

            pos = certificateInstance.findProperty(PROPERTY_NAME_NOTBEFORE);
            prop = (CIMProperty)certificateInstance.getProperty(pos);
            prop.getValue().get(notBefore);

            String notBeforeStr = _formatCIMDateTime(notBefore.toString());

            pos = certificateInstance.findProperty(PROPERTY_NAME_NOTAFTER);
            prop = (CIMProperty)certificateInstance.getProperty(pos);
            prop.getValue().get(notAfter);

            String notAfterStr = _formatCIMDateTime(notAfter.toString());

            //
            // Display the certificate content
            //
            outPrintWriter << "Issuer: " << issuerName << endl;
            outPrintWriter << "Serial Number: " << serialNumber << endl;
            outPrintWriter << "Subject: " << subjectName << endl;
            outPrintWriter << "Validity:" << endl;
            outPrintWriter << "    NotBefore: " << notBeforeStr << endl;
            outPrintWriter << "    NotAfter : " << notAfterStr << endl ;
            outPrintWriter <<
                "---------------------------------------------"<< endl;
        }
    }
    catch (const Exception& )
    {
        throw;
    }
}

/**
   List CRL's in the CRL store

   @param  client           the handle to CIMClient object

   @param  outPrintWriter   the ostream to which output should be written

   @exception  Exception    if failed to list CRL's
 */
void SSLTrustMgr::_listCRL (
    CIMClient&   client,
    ostream&     outPrintWriter)
{
    Array<CIMInstance> crlNamedInstances;

    try
    {
        //
        // get all the instances of class PG_SSLCertificateRevocationList
        //
        crlNamedInstances =
            client.enumerateInstances(
                PEGASUS_NAMESPACENAME_CERTIFICATE,
                PEGASUS_CLASSNAME_CRL);

        //
        // copy the CRL content
        //
        Uint32 numberInstances = crlNamedInstances.size();

        for (Uint32 i = 0; i < numberInstances; i++)
        {
            CIMInstance& crlInstance = crlNamedInstances[i];

            String issuerName;
            CIMDateTime lastUpdate;
            CIMDateTime nextUpdate;

            //
            // Check if issuer name is specified
            //
            Uint32 pos = crlInstance.findProperty(PROPERTY_NAME_ISSUERNAME);
            CIMProperty prop = (CIMProperty)crlInstance.getProperty(pos);
            issuerName.assign(prop.getValue().toString());

            if ( _issuerNameSet && !String::equal(_issuerName, issuerName) )
            {
                continue;
            }

            //
            // Get the remaining properties and display them.
            //
            pos = crlInstance.findProperty(PROPERTY_NAME_LASTUPDATE);
            prop = (CIMProperty)crlInstance.getProperty(pos);
            prop.getValue().get(lastUpdate);

            String lastUpdateStr = _formatCIMDateTime(lastUpdate.toString());

            pos = crlInstance.findProperty(PROPERTY_NAME_NEXTUPDATE);
            prop = (CIMProperty)crlInstance.getProperty(pos);
            prop.getValue().get(nextUpdate);

            String nextUpdateStr = _formatCIMDateTime(nextUpdate.toString());

            //
            // Display the CRL issuer name and update dates
            //
            outPrintWriter << "Issuer:  " << issuerName << endl;
            outPrintWriter << "Last update: " << lastUpdateStr << endl;
            outPrintWriter << "Next update: " << nextUpdateStr << endl;

            Array<String> revokedSerialNumbers;
            Array<CIMDateTime> revocationDates;

            pos = crlInstance.findProperty(PROPERTY_NAME_REVOKED_SERIAL_NUMBERS);
            prop = (CIMProperty)crlInstance.getProperty(pos);
            prop.getValue().get(revokedSerialNumbers);

            pos = crlInstance.findProperty(PROPERTY_NAME_REVOCATION_DATES);
            prop = (CIMProperty)crlInstance.getProperty(pos);
            prop.getValue().get(revocationDates);

            outPrintWriter << "Revoked Certificates:" << endl;

            for (Uint32 i = 0; i < revokedSerialNumbers.size(); i++)
            {
                String revocationDateStr =
                    _formatCIMDateTime(revocationDates[i].toString());

                //
                // Display the revoked serial numbers and the revocation dates
                //
                outPrintWriter <<
                    "    Serial Number: " << revokedSerialNumbers[i] << endl;
                outPrintWriter <<
                    "        Revocation Date: " << revocationDateStr << endl;
                outPrintWriter << endl;
            }
            outPrintWriter <<
                "---------------------------------------------"<< endl;
        }
    }
    catch (const Exception& )
    {
        throw;
    }

}

/**
    Parses the command line, validates the options, and sets instance
    variables based on the option arguments.

    @param   argc  the number of command line arguments

    @param   argv  the string vector of command line arguments

    @exception  CommandFormatException  if an error is encountered in parsing
                                        the command line
 */
void SSLTrustMgr::setCommand (Uint32 argc, char* argv [])
{
    Uint32         i               = 0;
    Uint32         c               = 0;
    String         timeoutStr;
    String         serialNumberStr;
    String         optString;
    getoopt        getOpts;

    //
    //  Construct optString
    //
    optString.append (_OPTION_ADD);
    optString.append (getoopt::NOARG);
    optString.append (_OPTION_TRUSTSTORE);
    optString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    optString.append (_OPTION_TRUSTPATH);
    optString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    optString.append (_OPTION_CERTFILE);
    optString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    optString.append (_OPTION_CERTUSER);
    optString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);

    optString.append (_OPTION_ADD);
    optString.append (getoopt::NOARG);
    optString.append (_OPTION_REVOKE);
    optString.append (getoopt::NOARG);
    optString.append (_OPTION_CERTFILE);
    optString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);

    optString.append (_OPTION_REMOVE);
    optString.append (getoopt::NOARG);
    optString.append (_OPTION_TRUSTSTORE);
    optString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    optString.append (_OPTION_TRUSTPATH);
    optString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    optString.append (_OPTION_ISSUERNAME);
    optString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    optString.append (_OPTION_SERIALNUMBER);
    optString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);

    optString.append (_OPTION_REMOVE);
    optString.append (getoopt::NOARG);
    optString.append (_OPTION_REVOKE);
    optString.append (getoopt::NOARG);
    optString.append (_OPTION_ISSUERNAME);
    optString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);

    optString.append (_OPTION_LIST);
    optString.append (getoopt::NOARG);
    optString.append (_OPTION_TRUSTSTORE);
    optString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    optString.append (_OPTION_TRUSTPATH);
    optString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    optString.append (_OPTION_ISSUERNAME);
    optString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    optString.append (_OPTION_SERIALNUMBER);
    optString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);

    optString.append (_OPTION_LIST);
    optString.append (getoopt::NOARG);
    optString.append (_OPTION_REVOKE);
    optString.append (getoopt::NOARG);
    optString.append (_OPTION_ISSUERNAME);
    optString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);

    optString.append (_OPTION_HELP);
    optString.append (getoopt::NOARG);
    optString.append (_OPTION_VERSION);
    optString.append (getoopt::NOARG);

#ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION
    optString.append (_OPTION_HOSTNAME);
    optString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    optString.append (_OPTION_PORTNUMBER);
    optString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    optString.append (_OPTION_SSL);
    optString.append (_OPTION_TIMEOUT);
    optString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    optString.append (_OPTION_USERNAME);
    optString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    optString.append (_OPTION_PASSWORD);
    optString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);
#endif /* #ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION */

    //
    //  Initialize and parse getOpts
    //
    getOpts = getoopt ();
    getOpts.addFlagspec (optString);

    //
    // Add long flag options for 'help' and 'version'
    //
    getOpts.addLongFlagspec (LONG_HELP, getoopt::NOARG);
    getOpts.addLongFlagspec (LONG_VERSION, getoopt::NOARG);

    getOpts.parse (argc, argv);

    if (getOpts.hasErrors ())
    {
        CommandFormatException e (getOpts.getErrorStrings () [0]);
        throw e;
    }
    _operationType = _OPERATION_TYPE_UNINITIALIZED;

    //
    //  Get options and arguments from the command line
    //
    for (i =  getOpts.first (); i <  getOpts.last (); i++)
    {
        if (getOpts [i].getType () == Optarg::LONGFLAG)
        {
            if (getOpts [i].getopt () == LONG_HELP)
            {
                if (_operationType != _OPERATION_TYPE_UNINITIALIZED)
                {
                    String param = String (LONG_HELP);
                    //
                    // More than one operation option was found
                    //
                    UnexpectedOptionException e (param);
                    throw e;
                }

               _operationType = _OPERATION_TYPE_HELP;
            }
            else if (getOpts [i].getopt () == LONG_VERSION)
            {
                if (_operationType != _OPERATION_TYPE_UNINITIALIZED)
                {
                    String param = String (LONG_VERSION);
                    //
                    // More than one operation option was found
                    //
                    UnexpectedOptionException e (param);
                    throw e;
                }

               _operationType = _OPERATION_TYPE_VERSION;
            }
        }
        else if (getOpts [i].getType () == Optarg::REGULAR)
        {
            //
            //  The ssltrustmgr command has no non-option argument options
            //
            UnexpectedArgumentException e (getOpts [i].Value ());
            throw e;
        }
        else /* getOpts [i].getType () == FLAG */
        {
            c = getOpts [i].getopt () [0];

            switch (c)
            {
                case _OPTION_ADD:
                {
                    if (_operationType != _OPERATION_TYPE_UNINITIALIZED)
                    {
                        //
                        // More than one operation option was found
                        //
                        UnexpectedOptionException e (_OPTION_ADD);
                        throw e;
                    }

                    if (getOpts.isSet (_OPTION_ADD) > 1)
                    {
                        //
                        // More than one add option was found
                        //
                        DuplicateOptionException e (_OPTION_ADD);
                        throw e;
                    }

                    _operationType = _OPERATION_TYPE_ADD;
                    break;
                }

                case _OPTION_REMOVE:
                {
                    if (_operationType != _OPERATION_TYPE_UNINITIALIZED)
                    {
                        //
                        // More than one operation option was found
                        //
                        UnexpectedOptionException e (_OPTION_REMOVE);
                        throw e;
                    }

                    if (getOpts.isSet (_OPTION_REMOVE) > 1)
                    {
                        //
                        // More than one remove option was found
                        //
                        DuplicateOptionException e (_OPTION_REMOVE);
                        throw e;
                    }

                    _operationType = _OPERATION_TYPE_REMOVE;
                    break;
                }

                case _OPTION_LIST:
                {
                    if (_operationType != _OPERATION_TYPE_UNINITIALIZED)
                    {
                        //
                        // More than one operation option was found
                        //
                        UnexpectedOptionException e (_OPTION_LIST);
                        throw e;
                    }

                    if (getOpts.isSet (_OPTION_LIST) > 1)
                    {
                        //
                        // More than one list option was found
                        //
                        DuplicateOptionException e (_OPTION_LIST);
                        throw e;
                    }

                    _operationType = _OPERATION_TYPE_LIST;
                    break;
                }

                case _OPTION_REVOKE:
                {
                    if (getOpts.isSet (_OPTION_REVOKE) > 1)
                    {
                        //
                        // More than one revoke option was found
                        //
                        DuplicateOptionException e (_OPTION_REVOKE);
                        throw e;
                    }

                    _crlSet = true;
                    break;
                }

                case _OPTION_TRUSTSTORE:
                {
                    if (getOpts.isSet (_OPTION_TRUSTSTORE) > 1)
                    {
                        //
                        // More than one trust store option was found
                        //
                        DuplicateOptionException e (_OPTION_TRUSTSTORE);
                        throw e;
                    }

                    _trustStore = getOpts [i].Value ();
                    _trustStoreSet = true;
                    break;
                }

                case _OPTION_TRUSTPATH:
                {
                    if (getOpts.isSet (_OPTION_TRUSTPATH) > 1)
                    {
                        //
                        // More than one trust path option was found
                        //
                        DuplicateOptionException e (_OPTION_TRUSTPATH);
                        throw e;
                    }

                    _trustPath = getOpts [i].Value ();
                    _trustPathSet = true;
                    break;
                }

                case _OPTION_CERTUSER:
                {
                    if (getOpts.isSet (_OPTION_CERTUSER) > 1)
                    {
                        //
                        // More than one certificate user option was found
                        //
                        DuplicateOptionException e (_OPTION_CERTUSER);
                        throw e;
                    }

                    _certUser = getOpts [i].Value ();
                    _certUserSet = true;
                    break;
                }

                case _OPTION_CERTFILE:
                {
                    if (getOpts.isSet (_OPTION_CERTFILE) > 1)
                    {
                        //
                        // More than one cert file option was found
                        //
                        DuplicateOptionException e (_OPTION_CERTFILE);
                        throw e;
                    }

                    _certFile = getOpts [i].Value ();
                    _certFileSet = true;
                    break;
                }

                case _OPTION_ISSUERNAME:
                {
                    if (getOpts.isSet (_OPTION_ISSUERNAME) > 1)
                    {
                        //
                        // More than one issuer name option was found
                        //
                        DuplicateOptionException e (_OPTION_ISSUERNAME);
                        throw e;
                    }

                    _issuerName = getOpts [i].Value ();
                    _issuerNameSet = true;
                    break;
                }

                case _OPTION_SERIALNUMBER:
                {
                    if (getOpts.isSet (_OPTION_SERIALNUMBER) > 1)
                    {
                        //
                        // More than one serial number option was found
                        //
                        DuplicateOptionException e (_OPTION_SERIALNUMBER);
                        throw e;
                    }

                    _serialNumber = getOpts [i].Value ();
                    _serialNumberSet = true;
                    break;
                }

#ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION
                case _OPTION_HOSTNAME:
                {
                    if (getOpts.isSet (_OPTION_HOSTNAME) > 1)
                    {
                        //
                        // More than one hostname option was found
                        //
                        DuplicateOptionException e (_OPTION_HOSTNAME);
                        throw e;
                    }
                    _hostName = getOpts [i].Value ();
                    _hostNameSet = true;
                    break;
                }

                case _OPTION_PORTNUMBER:
                {
                    if (getOpts.isSet (_OPTION_PORTNUMBER) > 1)
                    {
                        //
                        // More than one portNumber option was found
                        //
                        DuplicateOptionException e (_OPTION_PORTNUMBER);
                        throw e;
                    }

                    _portNumberStr = getOpts [i].Value ();

                    try
                    {
                        getOpts [i].Value (_portNumber);
                    }
                    catch (TypeMismatchException& it)
                    {
                        InvalidOptionArgumentException e (_portNumberStr,
                            _OPTION_PORTNUMBER);
                        throw e;
                    }
		    _portNumberSet = true;
                    break;
                }

                case _OPTION_SSL:
                {
                    //
                    // Use port 5989 as the default port for SSL
                    //
                    _useSSL = true;
                    if (!_portNumberSet)
                       _portNumber = 5989;
                    break;
                }

                case _OPTION_TIMEOUT:
                {
                    if (getOpts.isSet (_OPTION_TIMEOUT) > 1)
                    {
                        //
                        // More than one timeout option was found
                        //
                        DuplicateOptionException e (_OPTION_TIMEOUT);
                        throw e;
                    }

                    timeoutStr = getOpts [i].Value ();

                    try
                    {
                        getOpts [i].Value (_timeout);
                    }
                    catch (TypeMismatchException& it)
                    {
                        InvalidOptionArgumentException e (timeoutStr,
                            _OPTION_TIMEOUT);
                        throw e;
                    }
                    break;
                }

                case _OPTION_USERNAME:
                {
                    if (getOpts.isSet (_OPTION_USERNAME) > 1)
                    {
                        //
                        // More than one username option was found
                        //
                        DuplicateOptionException e (_OPTION_USERNAME);
                        throw e;
                    }
                    _userName = getOpts [i].Value ();
                    _userNameSet = true;
                    break;
                }

                case _OPTION_PASSWORD:
                {
                    if (getOpts.isSet (_OPTION_PASSWORD) > 1)
                    {
                        //
                        // More than one password option was found
                        //
                        DuplicateOptionException e (_OPTION_PASSWORD);
                        throw e;
                    }
                    _password = getOpts [i].Value ();
                    _passwordSet = true;
                    break;
                }
#endif /* #ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION */

                case _OPTION_HELP:
                {
                    if (_operationType != _OPERATION_TYPE_UNINITIALIZED)
                    {
                        //
                        // More than one operation option was found
                        //
                        UnexpectedOptionException e (_OPTION_HELP);
                        throw e;
                    }

                    if (getOpts.isSet (_OPTION_HELP) > 1)
                    {
                        //
                        // More than one list option was found
                        //
                        DuplicateOptionException e (_OPTION_HELP);
                        throw e;
                    }
                    _operationType = _OPERATION_TYPE_HELP;
                    break;
                }

                case _OPTION_VERSION:
                {
                    if (_operationType != _OPERATION_TYPE_UNINITIALIZED)
                    {
                        //
                        // More than one operation option was found
                        //
                        UnexpectedOptionException e (_OPTION_VERSION);
                        throw e;
                    }

                    if (getOpts.isSet (_OPTION_VERSION) > 1)
                    {
                        //
                        // More than one version option was found
                        //
                        DuplicateOptionException e (_OPTION_VERSION);
                        throw e;
                    }
                    _operationType = _OPERATION_TYPE_VERSION;
                    break;
                }

                default:
                {
                    //
                    //  This path should not be hit
                    //
                    break;
                }
            }
        }
    }

    if ( _operationType == _OPERATION_TYPE_UNINITIALIZED )
    {
        //
        // No operation type was specified
        // Show the usage
        //
        CommandFormatException e ( localizeMessage ( MSG_PATH,
            REQUIRED_ARGS_MISSING_KEY, REQUIRED_ARGS_MISSING ) );

        throw e;
    }

    if ( _operationType == _OPERATION_TYPE_ADD )
    {
        char option = 0;

        //
        // For -a option, the next required option is -f,
        // make sure it is set.
        //
        if ( !_certFileSet )
        {
            //
            // A required option is missing
            //
            MissingOptionException e (_OPTION_CERTFILE);
            throw e;
        }
        else
        {
            //
            // If -f option is set, -t, -T and -R options are not set then
            // -t cim_trust is assumed default and -c option is expected
            //
            if ( !_trustStoreSet && !_trustPathSet && !_crlSet )
            {
                if ( !_certUserSet )
                {
                    //
                    // A required option is missing
                    //
                    MissingOptionException e (_OPTION_CERTUSER);
                    throw e;
                }
            }
        }

        option = 0;

        if ( _crlSet )
        {
            //
            // With -R option, options other than -f are invalid.
            //
            if ( _trustStoreSet )
            {
                option = _OPTION_TRUSTSTORE;
            }
            else if ( _trustPathSet )
            {
                option = _OPTION_TRUSTPATH;
            }
            else if ( _certUserSet )
            {
                option = _OPTION_CERTUSER;
            }

            //
            //  Unexpected option specified.
            //
            if ( option )
            {
                UnexpectedOptionException e (option);
                throw e;
            }
        }
        else if ( _trustStoreSet )
        {
            //
            // With -t option, -c is a required option and the options
            // -T and -R are invalid.
            //
            if ( !_certUserSet )
            {
                //
                // A required option is missing
                //
                MissingOptionException e (_OPTION_CERTUSER);
                throw e;
            }

            if ( _trustPathSet )
            {
                option = _OPTION_TRUSTPATH;
            }
            else if ( _crlSet )
            {
                option = _OPTION_REVOKE;
            }

            //
            //  Unexpected option specified.
            //
            if ( option )
            {
                UnexpectedOptionException e (option);
                throw e;
            }
        }
        else if ( _trustPathSet )
        {
            //
            // With -T option, the options -t, -R and -c  are invalid.
            //
            if ( _trustStoreSet )
            {
                option = _OPTION_TRUSTSTORE;
            }
            else if ( _crlSet )
            {
                option = _OPTION_REVOKE;
            }
            else if ( _certUserSet )
            {
                option = _OPTION_CERTUSER;
            }

            //
            //  Unexpected option specified.
            //
            if ( option )
            {
                UnexpectedOptionException e (option);
                throw e;
            }
        }
    }

    if ( _operationType == _OPERATION_TYPE_REMOVE ||
         _operationType == _OPERATION_TYPE_LIST )
    {
        char option = 0;

        if ( _operationType == _OPERATION_TYPE_REMOVE )
        {
            //
            // For -r option, the required option is -i,
            // make sure it is set.
            //
            if ( !_issuerNameSet )
            {
                //
                // A required option is missing
                //
                MissingOptionException e (_OPTION_ISSUERNAME);
                throw e;
            }

            //
            // For -r option, if -R is not set, then the next required
            // option is -n. make sure it is set.
            //
            if ( ( _trustPathSet || !_crlSet ) && !_serialNumberSet )
            {
                //
                // A required option is missing
                //
                MissingOptionException e (_OPTION_SERIALNUMBER);
                throw e;
            }
        }
        else if ( _operationType == _OPERATION_TYPE_LIST )
        {
            //
            // For -l option, the is no required option.
            //

            //
            //  Serial number specified without issuer name
            //
            if ( _serialNumberSet && !_issuerNameSet )
            {
                InvalidOptionArgumentException e (_serialNumber,
                                                  _OPTION_SERIALNUMBER);
                throw e;
            }

            //
            // If trust path is set, then use client_trust as the
            // trust store type.
            //
            if ( _trustPathSet )
            {
                _trustStore.assign(String(CLIENT_TRUST_NAME));
            }
        }


        if ( _crlSet )
        {
            //
            // With -R option, the options -t, -T and -n  are invalid.
            //
            if ( _trustStoreSet )
            {
                option = _OPTION_TRUSTSTORE;
            }
            else if ( _trustPathSet )
            {
                option = _OPTION_TRUSTPATH;
            }
            else if ( _serialNumberSet )
            {
                option = _OPTION_SERIALNUMBER;
            }

            //
            //  Unexpected option specified.
            //
            if ( option )
            {
                UnexpectedOptionException e (option);
                throw e;
            }
        }
        else if ( _trustStoreSet )
        {
            //
            // With -t option, the options -R and -T are invalid.
            //
            if ( _trustPathSet )
            {
                option = _OPTION_TRUSTPATH;
            }
            else if ( _crlSet )
            {
                option = _OPTION_REVOKE;
            }

            //
            //  Unexpected option specified.
            //
            if ( option )
            {
                UnexpectedOptionException e (option);
                throw e;
            }
        }
        else if ( _trustPathSet )
        {
            //
            // With -T option, the options -t and -R are invalid.
            //
            if ( _trustStoreSet )
            {
                option = _OPTION_TRUSTSTORE;
            }
            else if ( _crlSet )
            {
                option = _OPTION_REVOKE;
            }

            //
            //  Unexpected option specified.
            //
            if ( option )
            {
                UnexpectedOptionException e (option);
                throw e;
            }
        }
    }

    if ( _operationType == _OPERATION_TYPE_ADD ||
         _operationType == _OPERATION_TYPE_REMOVE ||
         _operationType == _OPERATION_TYPE_LIST )
    {
        if ( _trustStoreSet &&
             (!String::equal(_trustStore, CIMSERVER_TRUST_NAME) &&
             !String::equal(_trustStore, INDICATION_TRUST_NAME))
           )
        {
            //
            //  Invalid or no trust store name specified
            //
            InvalidOptionArgumentException e (_trustStore,
                                              _OPTION_TRUSTSTORE);
            throw e;
        }
    }

    //
    // If the trust store name is not specified for add, remove and list
    // operations, then use cimserver_trust as the default trust store.
    //
    if ( (_operationType == _OPERATION_TYPE_ADD ||
         _operationType == _OPERATION_TYPE_REMOVE ||
         _operationType == _OPERATION_TYPE_LIST) &&
         (!_trustStoreSet && !_trustPathSet) )
    {
         _trustStore.assign(String(CIMSERVER_TRUST_NAME));
         _trustStoreSet = true;
    }

#ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION

    if ( !getOpts.isSet (_OPTION_PORTNUMBER) )
    {
        //
        //  No portNumber specified
        //  Default to WBEM_DEFAULT_PORT
        //  Already done in constructor
        //
    }
    else
    {
        if (_portNumber > _MAX_PORTNUMBER)
        {
            //
            //  Portnumber out of valid range
            //
            InvalidOptionArgumentException e (_portNumberStr,
                _OPTION_PORTNUMBER);
            throw e;
        }
    }

    if ( !getOpts.isSet (_OPTION_TIMEOUT) )
    {
        //
        //  No timeout specified
        //  Default to WbemExecClient::DEFAULT_TIMEOUT_MILLISECONDS
        //  Already done in constructor
        //
    }
    else
    {
        if (_timeout <= 0)
        {
            //
            //  Timeout out of valid range
            //
            InvalidOptionArgumentException e (timeoutStr,
                _OPTION_TIMEOUT);
            throw e;
        }
    }
#endif /* #ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION */

}


/**

    Executes the command and writes the results to the PrintWriters.

    @param   outPrintWriter     the ostream to which output should be
                                written
    @param   errPrintWriter     the ostream to which error output should be
                                written

    @return  0                  if the command is successful
             1                  if an error occurs in executing the command

 */
Uint32 SSLTrustMgr::execute (
    ostream& outPrintWriter,
    ostream& errPrintWriter)
{
    if ( _operationType == _OPERATION_TYPE_UNINITIALIZED )
    {
        //
        // The command was not initialized
        //
        return 1;
    }

    //
    // Options HELP and VERSION
    // CIMServer need not be running for these options to work
    //
    else if (_operationType == _OPERATION_TYPE_HELP)
    {
        errPrintWriter << _usage << endl;
        return (RC_SUCCESS);
    }
    else if(_operationType == _OPERATION_TYPE_VERSION)
    {
        errPrintWriter << "Version " << PEGASUS_PRODUCT_VERSION << endl;
        return (RC_SUCCESS);
    }

    CIMClient client;

    try
    {
        client.setTimeout( _timeout );
        client.setRequestDefaultLanguages(); //l10n
    }
    catch (Exception & e)
    {
        errPrintWriter << e.getMessage() << endl;
        return ( RC_ERROR );
    }

    try
    {
        _connectToServer( client, outPrintWriter);
    }
    catch(Exception& )
    {
        //l10n
        outPrintWriter << localizeMessage(MSG_PATH,
                                          CIMOM_NOT_RUNNING_KEY,
                                          CIMOM_NOT_RUNNING) << endl;
        return (RC_ERROR);
    }

    //
    // Perform the requested operation
    //
    switch ( _operationType )
    {
        case _OPERATION_TYPE_ADD:
            try
            {
                if ( !_crlSet )
                {
                    _addCertificate( client, outPrintWriter );
                }
                else
                {
                    _addCRL( client, outPrintWriter );
                }
            }
            catch (CIMException& e)
            {
                CIMStatusCode code = e.getCode();

                if (code == CIM_ERR_FAILED || code == CIM_ERR_NOT_SUPPORTED)
                {
                    if ( !_crlSet )
                    {
                        outPrintWriter << localizeMessage(MSG_PATH,
                            ADD_CERT_FAILURE_KEY, ADD_CERT_FAILURE) << endl;
                    }
                    else
                    {
                        outPrintWriter << localizeMessage(MSG_PATH,
                            ADD_CRL_FAILURE_KEY, ADD_CRL_FAILURE) << endl;
                    }
                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_ALREADY_EXISTS)
                {
                    if ( !_crlSet )
                    {
                        outPrintWriter << localizeMessage(MSG_PATH,
                            ADD_CERT_FAILURE_KEY, ADD_CERT_FAILURE) << endl;

                        outPrintWriter << localizeMessage(MSG_PATH,
                            CERT_ALREADY_EXISTS_KEY,
                            CERT_ALREADY_EXISTS) << endl;
                    }
                    errPrintWriter << e.getMessage()  << endl;
                }
                else if (code == CIM_ERR_INVALID_CLASS)
                {
                    if ( !_crlSet )
                    {
                        outPrintWriter << localizeMessage(MSG_PATH,
                            ADD_CERT_FAILURE_KEY, ADD_CERT_FAILURE) << endl;
                    }
                    else
                    {
                        outPrintWriter << localizeMessage(MSG_PATH,
                            ADD_CRL_FAILURE_KEY, ADD_CRL_FAILURE) << endl;
                    }
                    outPrintWriter << localizeMessage(MSG_PATH,
                       CERT_SCHEMA_NOT_LOADED_KEY,
                       CERT_SCHEMA_NOT_LOADED) << endl;
                }
                else
                {
                        errPrintWriter << e.getMessage() << endl;
                }

                return ( RC_ERROR );
            }
            catch (Exception& e)
            {
                if ( !_crlSet )
                {
                    outPrintWriter << localizeMessage(MSG_PATH,
                        ADD_CERT_FAILURE_KEY,
                        ADD_CERT_FAILURE) << endl << e.getMessage() << endl;
                }
                else
                {
                    outPrintWriter << localizeMessage(MSG_PATH,
                        ADD_CRL_FAILURE_KEY,
                        ADD_CRL_FAILURE) << endl << e.getMessage() << endl;
                }
                return ( RC_ERROR );
            }
            break;

        case _OPERATION_TYPE_REMOVE:
            try
            {
                if ( !_crlSet )
                {
                    _removeCertificate ( client, outPrintWriter );
                }
                else
                {
                    _removeCRL ( client, outPrintWriter );
                }
            }
            catch (CIMException& e)
            {
                CIMStatusCode code = e.getCode();

                if (code == CIM_ERR_FAILED || code == CIM_ERR_NOT_SUPPORTED)
                {
                    if ( !_crlSet )
                    {
                        outPrintWriter << localizeMessage(MSG_PATH,
                            REMOVE_CERT_FAILURE_KEY,
                            REMOVE_CERT_FAILURE) << endl;
                    }
                    else
                    {
                        outPrintWriter << localizeMessage(MSG_PATH,
                            REMOVE_CRL_FAILURE_KEY,
                            REMOVE_CRL_FAILURE) << endl;
                    }
                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_NOT_FOUND)
                {
                    if ( !_crlSet )
                    {
                        outPrintWriter << localizeMessage(MSG_PATH,
                            REMOVE_CERT_FAILURE_KEY,
                            REMOVE_CERT_FAILURE) << endl;

                        outPrintWriter << localizeMessage(MSG_PATH,
                            CERT_NOT_FOUND_KEY, CERT_NOT_FOUND) << endl;
                    }
                    else
                    {
                        outPrintWriter << localizeMessage(MSG_PATH,
                            REMOVE_CRL_FAILURE_KEY,
                            REMOVE_CRL_FAILURE) << endl;

                        outPrintWriter << localizeMessage(MSG_PATH,
                            CRL_NOT_FOUND_KEY, CRL_NOT_FOUND) << endl;
                    }
                    errPrintWriter << e.getMessage()  << endl;
                }
                else if (code == CIM_ERR_INVALID_CLASS)
                {
                    if ( !_crlSet )
                    {
                        outPrintWriter << localizeMessage(MSG_PATH,
                            REMOVE_CERT_FAILURE_KEY,
                            REMOVE_CERT_FAILURE) << endl;
                    }
                    else
                    {
                        outPrintWriter << localizeMessage(MSG_PATH,
                            REMOVE_CRL_FAILURE_KEY,
                            REMOVE_CRL_FAILURE) << endl;
                    }
                    outPrintWriter << localizeMessage(MSG_PATH,
                        CERT_SCHEMA_NOT_LOADED_KEY,
                            CERT_SCHEMA_NOT_LOADED) << endl;
                }
                else
                {
                    errPrintWriter << e.getMessage() << endl;
                }
                return ( RC_ERROR );
            }
            catch (Exception& e)
            {
                if ( !_crlSet )
                {
                    outPrintWriter << localizeMessage(MSG_PATH,
                        REMOVE_CERT_FAILURE_KEY,
                        REMOVE_CERT_FAILURE) << endl
                        << e.getMessage() << endl;
                }
                else
                {
                    outPrintWriter << localizeMessage(MSG_PATH,
                        REMOVE_CRL_FAILURE_KEY,
                        REMOVE_CRL_FAILURE) << endl << e.getMessage() << endl;
                }
                return ( RC_ERROR );
            }
            break;

        case _OPERATION_TYPE_LIST:
            try
            {
                if ( !_crlSet )
                {
                    _listCertificates ( client, outPrintWriter );
                }
                else
                {
                    _listCRL ( client, outPrintWriter );
                }
            }
            catch (CIMException& e)
            {
                CIMStatusCode code = e.getCode();

                if (code == CIM_ERR_FAILED || code == CIM_ERR_NOT_SUPPORTED)
                {
                    if ( !_crlSet )
                    {
                        outPrintWriter << localizeMessage(MSG_PATH,
                            LIST_CERT_FAILURE_KEY, LIST_CERT_FAILURE) << endl;
                    }
                    else
                    {
                        outPrintWriter << localizeMessage(MSG_PATH,
                            LIST_CRL_FAILURE_KEY, LIST_CRL_FAILURE) << endl;
                    }
                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_INVALID_CLASS)
                {
                    if ( !_crlSet )
                    {
                        outPrintWriter << localizeMessage(MSG_PATH,
                            LIST_CERT_FAILURE_KEY, LIST_CERT_FAILURE) << endl;
                    }
                    else
                    {
                        outPrintWriter << localizeMessage(MSG_PATH,
                            LIST_CRL_FAILURE_KEY, LIST_CRL_FAILURE) << endl;
                    }
                    outPrintWriter << localizeMessage(MSG_PATH,
                        CERT_SCHEMA_NOT_LOADED_KEY,
                        CERT_SCHEMA_NOT_LOADED) << endl;
                }
                else
                {
                    errPrintWriter << e.getMessage() << endl;
                }

                return ( RC_ERROR );
            }
            catch (Exception& e)
            {
                if ( !_crlSet )
                {
                    outPrintWriter << localizeMessage(MSG_PATH,
                        LIST_CERT_FAILURE_KEY,
                        LIST_CERT_FAILURE) << endl << e.getMessage() << endl;
                }
                else
                {
                    outPrintWriter << localizeMessage(MSG_PATH,
                        LIST_CRL_FAILURE_KEY,
                        LIST_CRL_FAILURE) << endl << e.getMessage() << endl;
                }
                return ( RC_ERROR );
            }
            break;

        default:
            //
            //  This path should not be hit
            //
            break;
    }

    return (RC_SUCCESS);
}

PEGASUS_NAMESPACE_END

// exclude main from the Pegasus Namespace


PEGASUS_USING_PEGASUS;

PEGASUS_USING_STD;

///////////////////////////////////////////////////////////////////////////
/**

    Parses the command line, and executes the command.

    @param   argc  the number of command line arguments
    @param   argv  the string vector of command line arguments

    @return  0                  if the command is successful
             1                  if an error occurs in executing the command

 */
///////////////////////////////////////////////////////////////////////////

int main (int argc, char* argv [])
{
    SSLTrustMgr    command = SSLTrustMgr();
    int            retCode;

    //l10n set message loading to process locale
    MessageLoader::_useProcessLocale = true;

#ifdef PEGASUS_OS_OS400

    VFYPTRS_INCDCL;               // VFYPTRS local variables

  // verify pointers
#pragma exception_handler (qsyvp_excp_hndlr,qsyvp_excp_comm_area,\
    0,_C2_MH_ESCAPE)
    for( int arg_index = 1; arg_index < argc; arg_index++ ){
	  VFYPTRS(VERIFY_SPP_NULL(argv[arg_index]));
    }
#pragma disable_handler

    // Convert the args to ASCII
    for(Uint32 i = 0;i< argc;++i)
    {
      EtoA(argv[i]);
    }

    // Set the stderr stream to buffered with 32k.
    // Allows utf-8 to be sent to stderr. (P9A66750)
    setvbuf(stderr, new char[32768], _IOLBF, 32768);

    // Check to ensure the user is authorized to use the command
    // ycmCheckSecurityAuthorities() will send a diagnostic message to qsh
    if(FALSE == ycmCheckSecurityAuthorities())
    {
	exit(CPFDF80_RC);
    }
#endif

    try
    {
        command.setCommand (argc, argv);
    }
    catch (CommandFormatException& cfe)
    {
        cerr << SSLTrustMgr::COMMAND_NAME << ": " << cfe.getMessage ()
             << endl;
        cerr << "Use '-h' or '--help' to obtain command syntax." << endl;
        exit (Command::RC_ERROR);
    }

    retCode = command.execute (cout, cerr);

    exit (retCode);
    return 0;
}
