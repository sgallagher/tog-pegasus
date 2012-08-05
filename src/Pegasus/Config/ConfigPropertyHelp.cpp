//%LICENSE////////////////////////////////////////////////////////////////
//
// Licensed to The Open Group (TOG) under one or more contributor license
// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
// this work for additional information regarding copyright ownership.
// Each contributor licenses this file to you under the OpenPegasus Open
// Source License; you may not use this file except in compliance with the
// License.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////
//
//%/////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//
// This file defines the table of help descriptions.  It is maintained as
// a separate file so that the help info can be modified without touching
// any of the code of Config
//
///////////////////////////////////////////////////////////////////////////////

#include "ConfigPropertyHelp.h"

PEGASUS_NAMESPACE_BEGIN

// This table defines help descriptions for cimconfig propertis which do
// not have more detailed help in their property owner classes.
// FUTURE: This should be loadable from an external file or otherwise
// part of clients rather than in memory. Big waste of memory.
//
// NOTE: The internationalization message bundle for this set of
// messages SHOULD BE generated with the tool BldMsgBundle which is
// a test program in Config/tests. Please use this tool to generate
// this component of the message bundle for Config if any of the messages
// change rather than manually editing the individual messages in the bundle.
//
// Generally format the messages as they will be displayed with NL characters
// to provide formatting for a terminal output (i.e ~< 60 characters). NL
// characters should be used at any point the user wants to insure that
// there is a new line and spaces used for indenting. Do not put an NL at
// the end of the message.
//
struct configPropertyDescription configPropertyDescriptionList[] =
{
    {"traceComponents",
        "Defines OpenPegasus components to be traced. Multiple components\n"
        "input as comma-separated list. 'ALL' traces all components."},

    {"traceLevel",
        "Defines Level of server tracing Enabled.\n"
        "Possible Values:\n"
        "    0 Trace off (default)\n"
        "    1 Severe and log messages\n"
        "    2 Basic flow trace messages, low data detail\n"
        "    3 Inter-function logic flow, medium data detail\n"
        "    4 High data detail\n"
        "    5 High data detail + Method Enter & Exit"},

    {"traceFilePath", "Specifies location and name of OpenPegasus trace file."},

    {"traceMemoryBufferKbytes",
        "Defines size of buffer for in-memory tracing in kbytes.\n"
        "Smallest value is 16 (10240 bytes). Ignored if traceFacility!=memory"},

    {"traceFacility", "Keyword configures the trace destination.\n"
            "Possible Values:\n"
            "    'File' - Trace output to file defined by traceFilePath\n"
            "    'Log'  - Trace output to log file\n"
            "    'Memory' - Trace output to memory"},

    {"logDir", "Specifies name of directory to be used for OpenPegasus\n"
            "specific log files. Not supported if PEGASUS_USE_SYSLOGS\n"
            "defined"},

    {"maxLogFileSizeKBytes",
        "Maximum size of the logfile in kbytes. If logfile size exceeds\n"
        "maxLogFileSizeKBytes logfile will be pruned. Minimum value is\n"
        "32 KB. Not supported if PEGASUS_USE_SYSLOGS defined."},

    {"logLevel","Keyword defines the desired level of logging.\n"
                "Possible Values:\n"
                "    TRACE, INFORMATION, WARNING, SEVERE, FATAL"},

    {"enableHttpConnection",
        "If 'true', allows connection to CIM Server using HTTP protocol"},

    {"enableHttpsConnection",
        "If 'true', allows connection to the CIM Server using HTTPS protocol\n"
        "(HTTP using Secure Socket Layer encryption)"},

    {"httpPort",
        "OpenPegasus first attempts to look up port number for HTTP\n"
        "using getservbyname for the 'wbem-http' service. httpPort.\n"
        "configuration setting used only when getservbyname lookup fails.\n"
        "Use of port 5988 recommended by DMTF"},

    {"httpsPort","OpenPegasus first attempts to look up port number for\n"
        "HTTP using getservbyname for the 'wbem-https' service. httpPorts\n"
        "configuration setting used only when the getservbyname lookup fails.\n"
        "Use of port 5989 recommended by DMTF"},

    {"daemon",
        "If 'true' enables forking of the CIM Server to create a background\n"
        "daemon process."},

    {"slp",
        "If 'true', OpenPegasus activates an SLP SA and issues DMTF\n"
        "defined SLP advertisements to this SA on startup."},

    {"enableAssociationTraversal",
        "If 'true', CIM Server supports the association traversal\n"
        "operators: Associators, AssociatorNames,References, and\n"
        "ReferenceNames."},

    {"enableIndicationService",
        "If 'true', the CIM Server will support CIM Indications."},

    {"enableAuthentication",
        "If 'true', a Client must be authenticated to access the CIM Server."},

    {"enableNamespaceAuthorization",
        "If 'true', CIM Server restricts access to namespaces based on\n"
        "configured user authorizations [user authorizations may be\n"
        "configured using the cimauth command]"},

    {"httpAuthType",  "Type of HTTP authentication. Currently = 'basic'\n"
        "indicating basic authentication"},

    {"passwordFilePath",  "Path to password file if password file used for\n"
        "user authentication."},

    {"sslCertificateFilePath",
        "File containing the CIM Server SSL Certificate."},

    {"sslKeyFilePath",
        "File Containing private key for CIM Server SSL Certificate."},

    {"sslTrustStore",
        "Specifies location of OpenSSL truststore. Truststore can be either\n"
         "file or directory. If the truststore is a directory, all\n"
         "certificates in the directory are trusted."},

    {"crlStore",  "Specifies location of OpenSSL certificate revocation list.\n"
         "if enabled."},

    {"sslClientVerificationMode",
        "Level of support for certificate-based authentication.\n"
        "Valid only if enableHttpsConnection=true\n"
        "Options are:\n"
        "'required': client MUST present a trusted certificate to access\n"
        "    CIM Server. If client fails to send a certificate or sends an\n"
        "    untrusted certificate, the connection will be rejected.\n"
        "'optional': server supports, but does not require, certificate-based\n"
        "   client authentication. Server requests and validates client \n"
        "   certificate, but the connection will be accepted if no\n"
        "   certificate or an untrusted certificate issent. Server will\n"
        "   then seek to authenticate client via authentication header.\n"
        "'disabled': Server does not support certificate-based client\n"
        "   authentication."},

    {"sslTrustStoreUserName",
        "System user name to be associated with all certificate-based\n"
        "authenticated requests. No default; for security reasons, system\n"
        "administrator must explicitly specify this value.  Allows a single\n"
        "user name to be specified.  This user will be associated with all\n"
        "certificates in the truststore"},

    {"kerberosServiceName",  "TBD"},

    {"enableCFZAPPLID",  "TBD"},

    {"sslCipherSuite",
        "String containing OpenSSL cipher specifications to configure\n"
        "cipher suite the client is permitted to negotiate with the server\n"
        "during SSL handshake phase. Enclose values in single quotes to"
        "avoid issues with special characters"},

    {"repositoryIsDefaultInstanceProvider",
        "If 'true', Repository functions as Provider (for instance and\n"
        "association operations) for all classes without an explicitly\n"
        "registered Provider."},

    {"enableBinaryRepository",
        "Setting this to 'true' will activate the binary repository support\n"
        "on the next restart of cimserver. OpenPegasus supports repositories\n"
        "with a mixture of binary and XML objects."},

    {"shutdownTimeout",
        "When 'cimserver -s' shutdown command is issued, specifies maximum\n"
        "time in seconds for CIM Server to complete outstanding CIM\n"
        "operation requests before shutting down. If specified timeout\n"
        "period expires, CIM Server shuts down, even with CIM operations\n"
        "in progress. Minimum value is 2 seconds."},

    {"repositoryDir",
        "Name of directory to be used for the OpenPegasus repository."},

    {"providerManagerDir",
        "Name of the directory containing ProviderManager plugin libraries.\n"
        "Should be a dedicated directory to minimize inspection of\n"
        "non-applicable files."},

    {"providerDir",
        "Names of directories that contains Provider executables. If\n"
        "multiple directories they should be comma-separated."},

    {"enableRemotePrivilegedUserAccess",
        "If 'true', the CIM Server allows access by a privileged user from a\n"
        "remote system. Many management operations require privileged user\n"
        "access. Disabling remote access by privileged user could\n"
        "significantly affect functionality."},

    {"enableSubscriptionsForNonprivilegedUsers",
        "If 'true', operations on indication filter, listener destination,\n"
        "and subscription instances may be performed by non-privileged\n"
        "users.Otherwise, these operations limited to privileged users."},

    {"authorizedUserGroups",
        "If set, the value is list of comma-separated user\n"
        "groups whose members may issue CIM requests. A user not a member of\n"
        "any of these groups is restricted from issuing CIM requests, except\n"
        "for privileged users (root user). If not set, any user may issue\n"
        "CIM requests."},

    {"messageDir",
        "Name of the directory to be used for the OpenPegasus translated\n"
        "message bundles."},

    {"enableNormalization",
        "If 'true', objects returned from instance providers are validated\n"
        "and normalized. Errors encountered during normalization cause\n"
        "CIMException with the status code set to CIM_ERR_FAILED. Provider\n"
        "modules in excludeMOdulesFromNormalizationList excluded from\n"
        "normalization."},

    {"excludeModulesFromNormalization",
        "If not Null, the value is interpreted as comma-separated list of\n"
        "Provider Module names to exclude from validation and normalization."},

    {"forceProviderProcesses",
        "If 'true', CIM Server runs Providers in separate processes rather\n"
        "than loading and calling Provider libraries directly within\n"
        "CIM Server process."},

    {"maxProviderProcesses",
        "Limits number of provider processes (see 'forceProviderProcesses')\n"
        "that may run concurrently. Value '0' indicates that the number of\n"
        " Provider Agent processes unlimited"},

    {"enableAuditLog",  "If 'true', audit logging at run time enabled."},

    {"socketWriteTimeout",
        "If CIM Server receives EWOULDBLOCK/EAGAIN error on a non-blocking\n"
        "write, socketWriteTimeout defines the number of seconds the CIM\n"
        "Server will wait for the socket to get ready and resume writing\n"
        "data."},

    {"idleConnectionTimeout",
        "If set to a positive integer, value specifies a minimum timeout\n"
        "value for idle client connections. If set to zero, idle client\n"
        "connections do not time out."},

    {"maxFailedProviderModuleRestarts",
        "If set to a positive integer, this value specifies the number of\n"
        "times a failed provider module with indications enabled is restarted\n"
        "automatically before being moved to Degraded state. If set to zero,\n"
        "failed provider module is not restarted with indications enabled\n"
        "automatically and will be moved to Degraded state immediately."},

    {"maxIndicationDeliveryRetryAttempts",
        "If set to a positive integer, value defines the number of times\n"
        "indication service will try to deliver an indication to a\n"
        "particular listener destination. This does not effect the original\n"
        "delivery attempt, thus if set to 0, cimserver will only try to\n"
        "deliver the indication once."},

    {"minIndicationDeliveryRetryInterval",
        "If set to a positive integer, defines the minimal time interval in\n"
        "seconds indication service waits before retrying delivery of\n"
        "indication to a listener destination that previously failed.\n"
        "Cimserver may take longer due to QoS or other processing."}
};

Uint32 configPropertyDescriptionListSize =
    sizeof(configPropertyDescriptionList) /
        sizeof(configPropertyDescriptionList[0]);

PEGASUS_NAMESPACE_END

