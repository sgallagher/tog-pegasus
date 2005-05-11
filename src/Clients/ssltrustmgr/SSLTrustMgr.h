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
// Author: Nag Boranna, Hewlett-Packard Company (nagaraja.boranna@hp.com)
//
// Modified By: David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//              Vijay Eli, vijayeli@in.ibm.com, fix for #2572
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_SSLTrustMgr_h
#define Pegasus_SSLTrustMgr_h

#include <Pegasus/Common/String.h>
#include <Pegasus/Client/CIMClient.h>
#include <Clients/cliutils/Command.h>
#include <Clients/cliutils/CommandException.h>

PEGASUS_NAMESPACE_BEGIN

/**

    The SSLTrustMgr command provides a command line interface to manage X509
    certificates in a PEM format trust store or a Certificate Revocation
    List (CRL). The command supports add, remove, revoke and list operations
    on the target trust store or a CRL store. The SSLTrustMgr command
    requires the CIM Server to be running on the local host.

    @author  Hewlett-Packard Company

 */
class SSLTrustMgr : public Command
{
public:

    enum { DEFAULT_TIMEOUT_MILLISECONDS = 200000 };

    /**
        Constructs a SSLTrustMgr and initializes instance variables.
     */
    SSLTrustMgr ();

    /**
        Parses the command line, validates the options, and sets instance
        variables based on the option arguments.

        @param  argc   the number of command line arguments
        @param  argv   the string vector of command line arguments

        @exception  CommandFormatException  if an error is encountered in
                                            parsing the command line
     */
    void setCommand (Uint32 argc, char* argv []);

    /**
        Executes the command and writes the results to the PrintWriters.

        @param  outPrintWriter    the ostream to which output should be
                                  written
        @param  errPrintWriter    the ostream to which error output should be
                                  written

        @return  0                if the command is successful
                 1                if an error occurs in executing the command
     */
    Uint32 execute (
        PEGASUS_STD(ostream)& outPrintWriter,
        PEGASUS_STD(ostream)& errPrintWriter);

    /**
        The command name.
     */
    static const char   COMMAND_NAME [];

private:

//
// Setting this flag will compile in the code that will enable the
// SSLTrustMgr command to connect to a remote host. This compile time
// flag is expected to be removed once the Certificate Management Provider
// MOF is moved from PG_Internal to PH_InterOp.
//
#ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION
    /**
        Prompt for password.

        @param  outPrintWriter  the ostream to which output should be written

        @return  String value of the user entered password
     */
    String _promptForPassword( PEGASUS_STD(ostream)&  outPrintWriter);
#endif /* #ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION */

    /**
        Connect to cimserver.

        @param  client          the handle to CIMClient object

        @param  outPrintWriter  the ostream to which output should be
                                written

        @exception  Exception   if an error is encountered in creating
                                the connection
     */
    void _connectToServer(
        CIMClient& client,
        PEGASUS_STD(ostream)& outPrintWriter );

    /**
        Convert CIMDateTime to user-readable string of the format
        month day-of-month, year  hour:minute:second (value-hrs-GMT-offset)

        @param  cimDateTimeStr  CIM formated DateTime String

        @return  String user-readable date time string.
     */
    String _formatCIMDateTime(const String& cimDateTimeStr);


    /**
        Read certificate content.

        @param  certFilePath    the certificate file path

        @return  Array<char> containing the certificate content

        @exception  Exception   if an error is encountered
                                in reading the certificate file
     */
    Array<char> _readCertificateContent(const String &certFilePath);

    /**
       Add a new certificate to the trust store

       @param  client           the handle to CIMClient object

       @param  outPrintWriter   the ostream to which output should be written

       @exception  Exception    if failed to add certificate
     */
    void _addCertificate (
        CIMClient&               client,
        PEGASUS_STD(ostream)&    outPrintWriter);

    /**
       Remove an existing certificate from the trust store

       @param  client           the handle to CIMClient object

       @param  outPrintWriter   the ostream to which output should be written

       @exception  Exception    if failed to remove certificate
     */
    void _removeCertificate (
        CIMClient&               client,
        PEGASUS_STD(ostream)&    outPrintWriter);

    /**
       List certificates in the trust store

       @param  client           the handle to CIMClient object

       @param  outPrintWriter   the ostream to which output should be written

       @exception  Exception    if failed to list certificates
     */
    void _listCertificates (
        CIMClient&               client,
        PEGASUS_STD(ostream)&    outPrintWriter);

    /**
       Add a CRL to the CRL store

       @param  client           the handle to CIMClient object

       @param  outPrintWriter   the ostream to which output should be written

       @exception  Exception    if failed to add a CRL
     */
    void _addCRL (
        CIMClient&               client,
        PEGASUS_STD(ostream)&    outPrintWriter);

    /**
       Remove an existing CRL from the CRL store

       @param  client           the handle to CIMClient object

       @param  outPrintWriter   the ostream to which output should be written

       @exception  Exception    if failed to remove a CRL
     */
    void _removeCRL (
        CIMClient&               client,
        PEGASUS_STD(ostream)&    outPrintWriter);

    /**
       List CRL's in the CRL store

       @param  client           the handle to CIMClient object

       @param  outPrintWriter   the ostream to which output should be written

       @exception  Exception    if failed to list CRL's
     */
    void _listCRL (
        CIMClient&               client,
        PEGASUS_STD(ostream)&    outPrintWriter);

    /**
        The trust store on which the operation is requested.
     */
    String _trustStore;

    /**
        Indicates that the trust store is set.
     */
    Boolean _trustStoreSet;

    /**
        The trust path on which the operation is requested.
     */
    String _trustPath;

    /**
        Indicates that the trust path is set.
     */
    Boolean _trustPathSet;

    /**
        The certificate file to be added to the trust store.
     */
    String _certFile;

    /**
        Indicates that the certificate file is set.
     */
    Boolean _certFileSet;

    /**
        The user name to be mapped with the certificate being added
        to the trust store.
     */
    String _certUser;

    /**
        Indicates that the mapping certificate user is set.
     */
    Boolean _certUserSet;

    /**
        The issuer name of the certificate to be removed or listed
        from the trust store.
     */
    String _issuerName;

    /**
        Indicates that the issuer name is set.
     */
    Boolean _issuerNameSet;

    /**
        The serial number of the certificate to be removed or listed
        from the trust store for a specified issuer.
     */
    String _serialNumber;

    /**
        Indicates that the serial number is set.
     */
    Boolean _serialNumberSet;

    /**
        Indicates that the CRL flag is set.
     */
    Boolean _crlSet;

    /**
        The host on which the command is to be executed.  A CIM Server must be
        running on this host when the command is executed.  The default host is
        the local host.
     */
    String _hostName;

#ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION
    /**
        A Boolean indicating whether a hostname was specified on the command
        line.  The default host is the local host.
     */
    Boolean _hostNameSet;

    /**

        The port to be used when the command is executed. The
        port number must be the port number on which the
        target CIM Server is running.

        If no port is specified, ssltrustmgr will attempt to locate the port
        associated with the service "wbem-http" for non-SSL connections
        and "wbem-https" for SSL connections. If not found, the DMTF
        recommended default port number will be used.

        _portNumberStr and _portNumber are not used with
        connectLocal().

        The default port for non-SSL 5988 and 5989 for SSL.

     */
    String _portNumberStr;
    Uint32 _portNumber;


    /**
        A Boolean indicating whether a port number was specified on the
	command line.
     */
    Boolean _portNumberSet;

    /**
        The username to be used for authorization of the operation.
     */
    String _userName;

    /**
        Indicates that the user name is set.
     */
    Boolean _userNameSet;

    /**
        The password to be used for authorization of the operation.
     */
    String _password;

    /**
        Indicates that the password is set.
     */
    Boolean _passwordSet;

    /**
        A Boolean indicating whether an SSL connection was specified on the
        command line.
     */
    Boolean _useSSL;

#endif  /* #ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION */

    /**
        The timeout value to be used in milliseconds.
        The default timeout value is DEFAULT_TIMEOUT_MILLISECONDS.
     */
    Uint32 _timeout;

    /**
        The type of operation specified on the command line.
     */
    Uint32 _operationType;

    /**
        The usage string.
     */
    String _usage;

    /**
        The option character used to specify the add option.
     */
    static const char   _OPTION_ADD;

    /**
        The option character used to specify the remove option.
     */
    static const char   _OPTION_REMOVE;

    /**
        The option character used to specify the revoke option.
     */
    static const char   _OPTION_REVOKE;

    /**
        The option character used to specify the list option.
     */
    static const char   _OPTION_LIST;

    /**
        The option character used to display help info.
     */
    static const char   _OPTION_HELP;

    /**
        The option character used to display version info.
     */
    static const char   _OPTION_VERSION;

    /**
        The option character used to specify the trust store name.
     */
    static const char   _OPTION_TRUSTSTORE;

    /**
        The option character used to specify the trust path.
     */
    static const char   _OPTION_TRUSTPATH;

    /**
        The option character used to specify the certificate file.
     */
    static const char   _OPTION_CERTFILE;

    /**
        The option character used to specify the certificate user name.
     */
    static const char   _OPTION_CERTUSER;

    /**
        The option character used to specify the issuer name.
     */
    static const char   _OPTION_ISSUERNAME;

    /**
        The option character used to specify the serial number.
     */
    static const char   _OPTION_SERIALNUMBER;

#ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION

    /**
        The option character used to specify the hostname.
     */
    static const char   _OPTION_HOSTNAME;

    /**
        The option character used to specify the port number.
     */
    static const char   _OPTION_PORTNUMBER;

    /**
        The option character used to specify the timeout value.
     */
    static const char   _OPTION_TIMEOUT;

    /**
        The option character used to specify the username.
     */
    static const char   _OPTION_USERNAME;

    /**
        The option character used to specify the password.
     */
    static const char   _OPTION_PASSWORD;

    /**
        The option character used to specify whether SSL should be used.
     */
    static const char   _OPTION_SSL;

    /**
        The minimum valid portnumber.
     */
    static const Uint32 _MIN_PORTNUMBER;

    /**
        The maximum valid portnumber.
     */
    static const Uint32 _MAX_PORTNUMBER;

    /**
        The maximum number of password retries.
     */
    static const Uint32 _MAX_PW_RETRIES;

#endif /* #ifdef PEGASUS_SSLTRUSTMGR_REMOTE_CONNECTION */

    /**
        Label for the usage string for this command.
     */
    static const char   _USAGE [];

    /**
        This constant signifies that an operation option has not been recorded
     */
    static const Uint32 _OPERATION_TYPE_UNINITIALIZED;

    /**
        The constant representing a add operation
     */
    static const Uint32 _OPERATION_TYPE_ADD;

    /**
        The constant representing a remove operation
     */
    static const Uint32 _OPERATION_TYPE_REMOVE;

    /**
        The constant representing a revoke operation
     */
    static const Uint32 _OPERATION_TYPE_REVOKE;

    /**
        The constant representing a list operation
     */
    static const Uint32 _OPERATION_TYPE_LIST;

    /**
        The constant representing a help operation
     */
    static const Uint32 _OPERATION_TYPE_HELP;

    /**
        The constant representing a version display operation
     */
    static const Uint32 _OPERATION_TYPE_VERSION;

};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_SSLTrustMgr_h */
