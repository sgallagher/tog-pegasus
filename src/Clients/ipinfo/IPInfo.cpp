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
// Author: Mike Brasher (mbrasher@bmc.com)
//         Carol Ann Krug Graves, Hewlett-Packard Company 
//         (carolann_graves@hp.com)
//
// Modified By:
//         Warren Otsuka (warren_otsuka@hp.com)
//         Sushma Fernandes, Hewlett-Packard Company
//         (sushma_fernandes@hp.com)
//         Mike Day (mdday@us.ibm.com)
//         Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//         Bapu Patil, Hewlett-Packard Company ( bapu_patil@hp.com )
//         Warren Otsuka, Hewlett-Packard Company (warren_otsuka@hp.com)
//         Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//         Susan Campbell, Hewlett-Packard Company (scampbell@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Common/SSLContext.h>

#include <Pegasus/getoopt/getoopt.h>
#include <Clients/cliutils/CommandException.h>
#include "IPInfo.h"

// To build a version of ipinfo that does not
// support remote connections the
// DISABLE_SUPPORT_FOR_REMOTE_CONNECTIONS
// flag can be enabled.
//#define DISABLE_SUPPORT_FOR_REMOTE_CONNECTIONS

//#define DEBUG

PEGASUS_NAMESPACE_BEGIN

#define NAMESPACE CIMNamespaceName ("root/cimv2")
#define CLASSNAME CIMName ("PG_OperatingSystem")

/**
    The command name.
 */
const char   IPInfoCommand::COMMAND_NAME []      = "ipinfo";

/**
    Label for the usage string for this command.
 */
const char   IPInfoCommand::_USAGE []            = "usage: ";

/**
    The option character used to specify the hostname.
 */
const char   IPInfoCommand::_OPTION_HOSTNAME     = 'h';

/**
    The option character used to specify the port number.
 */
const char   IPInfoCommand::_OPTION_PORTNUMBER   = 'p';

/**
    The option character used to specify SSL usage.
 */
const char   IPInfoCommand::_OPTION_SSL          = 's';

/**
    The option character used to specify the timeout value.
 */
const char   IPInfoCommand::_OPTION_TIMEOUT      = 't';

/**
    The option character used to specify the username.
 */
const char   IPInfoCommand::_OPTION_USERNAME     = 'u';

/**
    The option character used to specify the password.
 */
const char   IPInfoCommand::_OPTION_PASSWORD     = 'w';

/**
    The option character used to specify debug information.
 */
const char   IPInfoCommand::_OPTION_DEBUG        = 'd';

/**
    The minimum valid portnumber.
 */
const Uint32 IPInfoCommand::_MIN_PORTNUMBER      = 0;

/**
    The maximum valid portnumber.
 */
const Uint32 IPInfoCommand::_MAX_PORTNUMBER      = 65535;

static const char PASSWORD_PROMPT []  =
                     "Please enter your password: ";

static const char PASSWORD_BLANK []  = 
                     "Password cannot be blank. Please re-enter your password.";

static const Uint32 MAX_PW_RETRIES = 3;

static Boolean verifyCertificate(SSLCertificateInfo &certInfo)
{

#ifdef DEBUG
    cout << certInfo.getSubjectName() << endl;
#endif
    //ATTN-NB-03-05132002: Add code to handle server certificate verification.
    return true;
}

/**
  
    Constructs a IPInfoCommand and initializes instance variables.
  
 */
IPInfoCommand::IPInfoCommand ()
{

    _hostName            = String ();
    _hostNameSet         = false;
    _portNumber          = WBEM_DEFAULT_HTTP_PORT;
    _portNumberSet       = false;

    char buffer[32];
    sprintf(buffer, "%lu", (unsigned long) _portNumber);
    _portNumberStr       = buffer;

    _timeout             = DEFAULT_TIMEOUT_MILLISECONDS;
    _userName            = String ();
    _userNameSet         = false;
    _password            = String ();
    _passwordSet         = false;
    _useSSL              = false;
    _enableDebug         = false;

    String usage = String (_USAGE);
    usage.append (COMMAND_NAME);
    usage.append (" [ -");
#ifndef DISABLE_SUPPORT_FOR_REMOTE_CONNECTIONS
    usage.append (_OPTION_SSL);
    usage.append (" ] [ -");
    usage.append (_OPTION_HOSTNAME);
    usage.append (" hostname ] [ -");
    usage.append (_OPTION_PORTNUMBER);
    usage.append (" portnumber ] [ -");
    usage.append (_OPTION_USERNAME);
    usage.append (" username ] [ -");
    usage.append (_OPTION_PASSWORD);
    usage.append (" password ] [ -");
    usage.append (_OPTION_TIMEOUT);
    usage.append (" timeout ] [ -");
#endif
    usage.append (_OPTION_DEBUG);
    usage.append (" ]");
    setUsage (usage);
}

String IPInfoCommand::_promptForPassword( ostream& outPrintWriter ) 
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
          if( retries < MAX_PW_RETRIES )
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

/**
  
    Connects to cimserver.
  
    @param   outPrintWriter     the ostream to which error output should be
                                written
  
    @exception       Exception  if an error is encountered in creating
                               the connection
  
 */
void IPInfoCommand::_connectToServer( CIMClient& client,
				         ostream& outPrintWriter )
{
    String                 host                  = String ();
    Uint32                 portNumber            = 0;
    Boolean                connectToLocal        = false;

    //
    //  Construct host address
    //

    if ((!_hostNameSet) && (!_portNumberSet) && (!_userNameSet) && (!_passwordSet))
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
    else if( _useSSL )
    {
        //
        // Get environment variables:
        //
        const char* pegasusHome = getenv("PEGASUS_HOME");
	
	String certpath = FileSystem::getAbsolutePath(
           pegasusHome, PEGASUS_SSLCLIENT_CERTIFICATEFILE);
	
	String randFile = String::EMPTY;

	randFile = FileSystem::getAbsolutePath(
            pegasusHome, PEGASUS_SSLCLIENT_RANDOMFILE);
        SSLContext  sslcontext (certpath, verifyCertificate, randFile);

        if (!_userNameSet)
        {
           _userName = System::getEffectiveUserName();
        }

        if (!_passwordSet)
        {
            _password = _promptForPassword( outPrintWriter );
        }
	client.connect(host, portNumber, sslcontext,  _userName, _password );
    }
    else
    { 
        if (!_passwordSet)
        {
            _password = _promptForPassword( outPrintWriter );
        }
        client.connect(host, portNumber, _userName, _password );
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
void IPInfoCommand::setCommand (Uint32 argc, char* argv [])
{
    Uint32         i              = 0;
    Uint32         c              = 0;
    String         httpVersion    = String ();
    String         httpMethod     = String ();
    String         timeoutStr     = String ();
    String         GetOptString   = String ();
    getoopt        getOpts;

    //
    //  Construct GetOptString
    //
#ifndef DISABLE_SUPPORT_FOR_REMOTE_CONNECTIONS
    GetOptString.append (_OPTION_HOSTNAME);
    GetOptString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    GetOptString.append (_OPTION_PORTNUMBER);
    GetOptString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    GetOptString.append (_OPTION_SSL);
    GetOptString.append (_OPTION_TIMEOUT);
    GetOptString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    GetOptString.append (_OPTION_USERNAME);
    GetOptString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    GetOptString.append (_OPTION_PASSWORD);
    GetOptString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);
#endif
    GetOptString.append (_OPTION_DEBUG);

    //
    //  Initialize and parse getOpts
    //
    getOpts = getoopt ();
    getOpts.addFlagspec (GetOptString);
    getOpts.parse (argc, argv);

    if (getOpts.hasErrors ())
    {
        CommandFormatException e (getOpts.getErrorStrings () [0]);
        throw e;
    }
    
    //
    //  Get options and arguments from the command line
    //
    for (i =  getOpts.first (); i <  getOpts.last (); i++)
    {
        if (getOpts [i].getType () == Optarg::LONGFLAG)
        {
            UnexpectedArgumentException e (
                         getOpts [i].Value ());
            throw e;
        } 
        else if (getOpts [i].getType () == Optarg::REGULAR)
        {
            UnexpectedArgumentException e (
                         getOpts [i].Value ());
            throw e;
        } 
        else /* getOpts [i].getType () == FLAG */
        {
            c = getOpts [i].getopt () [0];
    
            switch (c) 
            {
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
                    catch (const TypeMismatchException&)
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
      
                case _OPTION_DEBUG:
                {
		    _enableDebug = true;
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
                    catch (const TypeMismatchException&)
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
    
                default:
                    //
                    //  This path should not be hit
                    //
                    break;
            }
        }
    }

    if (getOpts.isSet (_OPTION_PORTNUMBER) < 1)
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

    if (getOpts.isSet (_OPTION_TIMEOUT) < 1)
    {
        //
        //  No timeout specified
        //  Default to DEFAULT_TIMEOUT_MILLISECONDS
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
}

/** ErrorExit - Print out the error message and exits.
    @param   errPrintWriter     The ostream to which error output should be
                                written
    @param   message            Text for error message
    @return None, Terminates the program
*/
void IPInfoCommand::errorExit( ostream& errPrintWriter,
                               const String& message)
{
    errPrintWriter << "ipinfo error: " << message << endl;
    exit(1);
}

void IPInfoCommand::getIPInfo(ostream& outPrintWriter,
                              ostream& errPrintWriter)
{

    CIMClient client;
    client.setTimeout( _timeout );

    try
    {
        _connectToServer( client, outPrintWriter);
        IPRouteInfo ipr (client, _enableDebug, outPrintWriter, errPrintWriter);
        IPPEpInfo ippep(client, _enableDebug, outPrintWriter, errPrintWriter);
        BIPTLEpInfo ipbiptle(client, _enableDebug, outPrintWriter, errPrintWriter);

    }  // end try

    catch(const Exception& e)
    {
      errorExit(errPrintWriter, e.getMessage());
    }

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
Uint32 IPInfoCommand::execute (ostream& outPrintWriter, 
                                 ostream& errPrintWriter) 
{
    try
    {
        IPInfoCommand::getIPInfo( outPrintWriter, errPrintWriter );
    }
    catch (const IPInfoException& e)
    {
      errPrintWriter << IPInfoCommand::COMMAND_NAME << ": " << 
        e.getMessage () << endl;
        return (RC_ERROR);
    }
    return (RC_SUCCESS);
}

/**
    
    Parses the command line, and executes the command.
  
    @param   argc  the number of command line arguments
    @param   argv  the string vector of command line arguments
  
    @return  0                  if the command is successful
             1                  if an error occurs in executing the command
  
 */
PEGASUS_NAMESPACE_END

// exclude main from the Pegasus Namespace
PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

int main (int argc, char* argv []) 
{
    IPInfoCommand    command = IPInfoCommand ();
    int                rc;

    try 
    {
        command.setCommand (argc, argv);
    } 
    catch (const CommandFormatException& cfe) 
    {
        cerr << IPInfoCommand::COMMAND_NAME << ": " << cfe.getMessage () 
             << endl;
        cerr << command.getUsage () << endl;
        exit (Command::RC_ERROR);
    }

    rc = command.execute (cout, cerr);
    exit (rc);
    return 0;
}
