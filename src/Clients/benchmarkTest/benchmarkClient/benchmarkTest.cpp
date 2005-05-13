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
// Author: Denise Eckstein, Hewlett-Packard Company
//         Significant portions of the code in this application were copied
//         from the wbemexec application.
//
// Modified By: David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////
//
//L10N TODO: Internal benchmark utility. Although this module contains
//messages, there is currently no expectation that it will be
//localized.

#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Common/SSLContext.h>
#include <Pegasus/Common/Stopwatch.h>

#include <Pegasus/Common/Tracer.h>

#include <Pegasus/getoopt/getoopt.h>
#include <Clients/cliutils/CommandException.h>
#include "benchmarkTest.h"
#include "../benchmarkDefinition/benchmarkDefinition.h"


#ifndef PLATFORM_PRODUCT_VERSION
 #define PLATFORM_PRODUCT_VERSION "1.0.0"
#endif

PEGASUS_NAMESPACE_BEGIN

#define NAMESPACE CIMNamespaceName ("root/benchmark")
#define MODULENAME String ("benchmarkProviderModule")

benchmarkDefinition test;

/**
    The command name.
 */
const char   benchmarkTestCommand::COMMAND_NAME []      = "benchmarkTest";

//    Label for the usage string for this command.
const char   benchmarkTestCommand::_USAGE []            = "usage: ";

//    The option character used to specify the hostname.
const char   benchmarkTestCommand::_OPTION_HOSTNAME     = 'h';

//    The option character used to specify the port number.
const char   benchmarkTestCommand::_OPTION_PORTNUMBER   = 'p';

//    The option character used to specify SSL usage.
const char   benchmarkTestCommand::_OPTION_SSL          = 's';

//    The option character to request that the version be displayed.
const char   benchmarkTestCommand::_OPTION_VERSION      = 'v';

//    The option character used to specify the timeout value.
const char   benchmarkTestCommand::_OPTION_TIMEOUT      = 't';

//    The option character used to specify the username.
const char   benchmarkTestCommand::_OPTION_USERNAME     = 'u';

//    The option character used to specify the password.
const char   benchmarkTestCommand::_OPTION_PASSWORD     = 'w';

//    The option character used to specify the iteration parameter.
const char benchmarkTestCommand::_OPTION_ITERATIONS     = 'i';

//    The option character used to specify the TESTID parameter.
const char benchmarkTestCommand::_OPTION_TESTID         = 'n';

//    The option character used to specify that a debug option is requested.
const char   benchmarkTestCommand::_OPTION_DEBUG        = 'd';

//    The minimum valid portnumber.
const Uint32 benchmarkTestCommand::_MIN_PORTNUMBER      = 0;

//    The maximum valid portnumber.
const Uint32 benchmarkTestCommand::_MAX_PORTNUMBER      = 65535;

static const char PASSWORD_PROMPT []  =
                     "Please enter your password: ";

static const char PASSWORD_BLANK []  =
                     "Password cannot be blank. Please re-enter your password.";

//    The debug option argument value used to enable client-side tracing.
const char   benchmarkTestCommand::_DEBUG_OPTION1       = '1';

//    The debug option argument value used to disable printing verbose report.
const char   benchmarkTestCommand::_DEBUG_OPTION2       = '2';


static const Uint32 MAX_PW_RETRIES = 3;

static Boolean verifyCertificate(SSLCertificateInfo &certInfo)
{

#if 0
    outPrintWriter << certInfo.getSubjectName() << endl;
#endif
    return true;
}

/**

    Constructs a benchmarkTestCommand and initializes instance variables.

 */
benchmarkTestCommand::benchmarkTestCommand ()
{

    _hostName            = String ();
    _hostNameSet         = false;
    _portNumber          = WBEM_DEFAULT_HTTP_PORT;
    _portNumberSet       = false;

    char buffer[32];
    sprintf(buffer, "%lu", (unsigned long) _portNumber);
    _portNumberStr       = buffer;

    _testID              = 0;
    _testIDSet           = false;

    _timeout             = DEFAULT_TIMEOUT_MILLISECONDS;
    _userName            = String ();
    _userNameSet         = false;
    _password            = String ();
    _passwordSet         = false;
    _useSSL              = false;
    _displayVersion      = false;
    _iterations          = DEFAULT_NUMBER_OF_ITERATIONS;
    _iterationsSet       = false;

    _debugOption1        = false;
    _generateReport      = true;

    String usage = String (_USAGE);
    usage.append (COMMAND_NAME);
    usage.append (" [ -");
    usage.append (_OPTION_SSL);
    usage.append (" ] [ -");
    usage.append (_OPTION_VERSION);
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
    usage.append (_OPTION_ITERATIONS);
    usage.append (" iterations ] [ -");
    usage.append (_OPTION_TESTID);
    usage.append (" test number ] [ -");
    usage.append (_OPTION_DEBUG);
    usage.append (_DEBUG_OPTION1);
    usage.append (" [ -");
    usage.append (_OPTION_DEBUG);
    usage.append (_DEBUG_OPTION2);
    usage.append (" ]");
    setUsage (usage);
}

String benchmarkTestCommand::_promptForPassword( ostream& outPrintWriter )
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
 void benchmarkTestCommand::_connectToServer( CIMClient& client,
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
void benchmarkTestCommand::setCommand (Uint32 argc, char* argv [])
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
    GetOptString.append (_OPTION_HOSTNAME);
    GetOptString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    GetOptString.append (_OPTION_PORTNUMBER);
    GetOptString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    GetOptString.append (_OPTION_VERSION);
    GetOptString.append (_OPTION_SSL);
    GetOptString.append (_OPTION_TIMEOUT);
    GetOptString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    GetOptString.append (_OPTION_USERNAME);
    GetOptString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    GetOptString.append (_OPTION_PASSWORD);
    GetOptString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    GetOptString.append (_OPTION_ITERATIONS);
    GetOptString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    GetOptString.append (_OPTION_TESTID);
    GetOptString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    GetOptString.append (_OPTION_DEBUG);
    GetOptString.append (getoopt::GETOPT_ARGUMENT_DESIGNATOR);


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

                case _OPTION_VERSION:
                {
            _displayVersion = true;
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

                case _OPTION_ITERATIONS:
                {
                    if (getOpts.isSet (_OPTION_ITERATIONS) > 1)
                    {
                        //
                        // More than one iteration option was found
                        //
                        DuplicateOptionException e (_OPTION_ITERATIONS);
                        throw e;
                    }

                    _iterationsStr = getOpts [i].Value ();

                    try
                    {
                        getOpts [i].Value (_iterations);
                    }
                    catch (const TypeMismatchException&)
                    {
                        InvalidOptionArgumentException e (_iterationsStr,
                            _OPTION_ITERATIONS);
                        throw e;
                    }
            _iterationsSet = true;
                    break;
                }

                case _OPTION_TESTID:
                {
                    if (getOpts.isSet (_OPTION_TESTID) > 1)
                    {
                        //
                        // More than one TESTID option was found
                        //
                        DuplicateOptionException e (_OPTION_TESTID);
                        throw e;
                    }

                    _testIDStr = getOpts [i].Value ();

                    try
                    {
                        getOpts [i].Value (_testID);
                    }
                    catch (const TypeMismatchException&)
                    {
                        InvalidOptionArgumentException e (_testIDStr,
                            _OPTION_TESTID);
                        throw e;
                    }
            _testIDSet = true;
                    break;
                }

                case _OPTION_DEBUG:
                {
                    //
                    //
                    String debugOptionStr;

                    debugOptionStr = getOpts [i].Value ();

                    if (debugOptionStr.size () != 1)
                    {
                        //
                        //  Invalid debug option
                        //
                        InvalidOptionArgumentException e (debugOptionStr,
                            _OPTION_DEBUG);
                        throw e;
                    }

                    if (debugOptionStr [0] == _DEBUG_OPTION1)
                    {
                        _debugOption1 = true;
                    }
                    else if (debugOptionStr [0] == _DEBUG_OPTION2)
                    {
                        _generateReport = false;
                    }
                    else
                    {
                        //
                        //  Invalid debug option
                        //
                        InvalidOptionArgumentException e (debugOptionStr,
                            _OPTION_DEBUG);
                        throw e;
                    }
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

    if (_displayVersion)
    {
        cout << PLATFORM_PRODUCT_VERSION << __DATE__ << endl;
    }

    if (_generateReport)
    {
        for (Uint32 i = 0; i < argc; i++)
        {
            cout << argv[i] << " ";
        }
        cout << endl <<  endl;
    }

}

/** ErrorExit - Print out the error message and exits.
    @param   errPrintWriter     The ostream to which error output should be
                                written
    @param   message            Text for error message
    @return - None, Terminates the program
    @exception - This function terminates the program
*/
void benchmarkTestCommand::errorExit( ostream& errPrintWriter,
                               const String& message)
{
    errPrintWriter << "benchmarkTest error: " << message << endl;
    exit(1);
}

void benchmarkTestCommand::_getCSInfo(ostream& outPrintWriter,
                                      ostream& errPrintWriter)
{

#define CSINFO_NAMESPACE CIMNamespaceName ("root/cimv2")
#define CSINFO_CLASSNAME CIMName ("PG_ComputerSystem")

    Boolean isConnected = false;
    CIMClient client;
    client.setTimeout( _timeout );

    try
    {
        _connectToServer( client, outPrintWriter);
        isConnected = true;

        Boolean deepInheritance = true;
        Boolean localOnly = false;
        Boolean includeQualifiers = false;
        Boolean includeClassOrigin = false;
        Array<CIMInstance> cimNInstances =
               client.enumerateInstances(CSINFO_NAMESPACE, CSINFO_CLASSNAME,
                                         deepInheritance,
                                         localOnly, includeQualifiers,
                                         includeClassOrigin );

        if (cimNInstances.size() != 1)
        {
           client.disconnect();
           return;
        }

        CIMObjectPath instanceRef = cimNInstances[0].getPath();
        if ( !(instanceRef.getClassName().equal(CSINFO_CLASSNAME)))
        {
           errPrintWriter << "Returned ClassName = " << instanceRef.getClassName().getString() << endl;
           errPrintWriter << "Expected ClassName = " << CSINFO_CLASSNAME.getString() << endl;
           errorExit(errPrintWriter, "EnumerateInstances failed.  Incorrect class name returned.");
        }

        for (Uint32 j = 0; j < cimNInstances[0].getPropertyCount(); j++)
        {
           CIMName propertyName = cimNInstances[0].getProperty(j).getName();
           if (propertyName.equal(CIMName("OtherIdentifyingInfo")))
           {
               outPrintWriter << "Model = "
                 << cimNInstances[0].getProperty(j).getValue().toString() << endl;
           }
        }

    }  // end try

    catch(...)
    {
       if (isConnected)
          client.disconnect();
       return;
    }

    client.disconnect();
}


void benchmarkTestCommand::_getOSInfo(ostream& outPrintWriter,
                                      ostream& errPrintWriter)
{

#define OSINFO_NAMESPACE CIMNamespaceName ("root/cimv2")
#define OSINFO_CLASSNAME CIMName ("PG_OperatingSystem")

    Boolean isConnected = false;
    CIMClient client;
    client.setTimeout( _timeout );

    try
    {
        _connectToServer( client, outPrintWriter);
        isConnected = true;

        Boolean deepInheritance = true;
        Boolean localOnly = false;
        Boolean includeQualifiers = false;
        Boolean includeClassOrigin = false;
        Array<CIMInstance> cimNInstances =
               client.enumerateInstances(OSINFO_NAMESPACE, OSINFO_CLASSNAME,
                                         deepInheritance,
                                         localOnly, includeQualifiers,
                                         includeClassOrigin );

        // while we only have one instance (the running OS), we can take the
        // first instance.  When the OSProvider supports installed OSs as well,
        // will need to select the runningOS instance

        if (cimNInstances.size() != 1)
        {
           client.disconnect();
           return;
        }

        CIMObjectPath instanceRef = cimNInstances[0].getPath();
        if ( !(instanceRef.getClassName().equal(OSINFO_CLASSNAME)) )
        {
           errPrintWriter << "Returned ClassName = " << instanceRef.getClassName().getString() << endl;
           errPrintWriter << "Expected ClassName = " << OSINFO_CLASSNAME.getString() << endl;
           errorExit(errPrintWriter, "enumerateInstances failed.  Incorrect class name returned.");
        }

        for (Uint32 j = 0; j < cimNInstances[0].getPropertyCount(); j++)
        {
           CIMName propertyName = cimNInstances[0].getProperty(j).getName();
           if (propertyName.equal(CIMName("CSName")))
           {
               outPrintWriter << "Computer System Name = "
                 << cimNInstances[0].getProperty(j).getValue().toString() << endl;
           }

           if (propertyName.equal(CIMName("Version")))
           {
               outPrintWriter << "Version = "
                 << cimNInstances[0].getProperty(j).getValue().toString() << endl;
           }
        }

    }  // end try

    catch(...)
    {
       if (isConnected)
          client.disconnect();
       return;
    }

    client.disconnect();
}

void benchmarkTestCommand::_getSystemConfiguration(
                                      ostream& outPrintWriter,
                                      ostream& errPrintWriter)
{
   benchmarkTestCommand::_getOSInfo(outPrintWriter, errPrintWriter);
   benchmarkTestCommand::_getCSInfo(outPrintWriter, errPrintWriter);
   outPrintWriter << endl << endl;
}

void benchmarkTestCommand::_getTestConfiguration(
                      ostream& outPrintWriter,
                                      ostream& errPrintWriter)
{
    Boolean isConnected = false;
    CIMClient client;
    client.setTimeout( _timeout );

    try
    {
        _connectToServer( client, outPrintWriter);
        isConnected = true;

        Boolean deepInheritance = true;

        Array<CIMName> classNames = client.enumerateClassNames(
                  NAMESPACE, CIMName(), deepInheritance);


        Uint32 numberOfProperties;
        Uint32 sizeOfPropertyValue;
        Uint32 numberOfInstances;

        for (Uint32 i = 0, n = classNames.size(); i < n; i++)
    {
            if (CIM_ERR_SUCCESS == test.getConfiguration(classNames[i],
                 numberOfProperties, sizeOfPropertyValue, numberOfInstances))
            {
               _testClassNames.append (classNames[i]);
            }
        }

    }  // end try

    catch (const Exception&)
    {
        if (isConnected)
           client.disconnect();
        throw;
    }

    catch (const exception&)
    {
        if (isConnected)
           client.disconnect();
        throw;
    }

    client.disconnect();
}


CIMObjectPath benchmarkTestCommand::_buildObjectPath(
                         const CIMName& className,
                         CIMValue Identifier)
{
    Array<CIMKeyBinding> keys;

    keys.append(CIMKeyBinding("Identifier", Identifier.toString(),
                              CIMKeyBinding::NUMERIC));

    return CIMObjectPath(String(), CIMNamespaceName(NAMESPACE), className, keys);
}

Boolean benchmarkTestCommand::_invokeProviderModuleMethod(
                                         const String& moduleName,
                                         const CIMName& methodName,
                                         ostream&    outPrintWriter,
                                         ostream&    errPrintWriter)
{
    CIMClient client;
    Boolean isConnected = false;
    Sint16 retValue = -1;
    client.setTimeout( _timeout );
    CIMObjectPath moduleRef;
    try
    {
        _connectToServer( client, outPrintWriter);
        isConnected = true;

        //
        // disable the module
        //

        moduleRef.setNameSpace(PEGASUS_NAMESPACENAME_PROVIDERREG);
        moduleRef.setClassName(PEGASUS_CLASSNAME_PROVIDERMODULE);
        CIMKeyBinding kb1(CIMName ("Name"), moduleName,
                CIMKeyBinding::STRING);
        Array<CIMKeyBinding> keys;
        keys.append(kb1);

        moduleRef.setKeyBindings(keys);

        Array<CIMParamValue> inParams;
        Array<CIMParamValue> outParams;

        CIMValue ret_value = client.invokeMethod(
                PEGASUS_NAMESPACENAME_PROVIDERREG,
                moduleRef,
                methodName,
                inParams,
                outParams);

        ret_value.get(retValue);
        client.disconnect();
    }

    catch (const Exception&)
    {
        if (isConnected)
           client.disconnect();
        throw;
    }

    if ((retValue == 0) || (retValue == 1))
        return true;
    else return false;
}

void benchmarkTestCommand::dobenchmarkTest1(
                              Uint32 testID,
                              ostream& outPrintWriter,
                              ostream& errPrintWriter)
{
    CIMClient client;
    client.setTimeout( _timeout );

    try
    {
        Stopwatch stopwatchTime;

        stopwatchTime.start();

        for (Uint32 i = 0; i < _iterations; i++)
        {
           _connectToServer( client, outPrintWriter);
           client.disconnect();
        }

        stopwatchTime.stop();

        double elapsedTime = stopwatchTime.getElapsed();

        outPrintWriter << testID;
        outPrintWriter <<  ": Benchmark Test #1: Connect/Disconnect Test" << endl;
        outPrintWriter << _iterations << " requests processed in "
                       << stopwatchTime.getElapsed() << " Seconds "
                       << "(Average Elapse Time = " << elapsedTime/_iterations
                       << ")" << endl << endl;
    }  // end try

    catch(const Exception& e)
    {
      errorExit(errPrintWriter, e.getMessage());
    }
}

void benchmarkTestCommand::dobenchmarkTest2(
                              Uint32 testID,
                              const CIMName& className,
                              ostream& outPrintWriter,
                              ostream& errPrintWriter)
{
    CIMClient client;
    client.setTimeout( _timeout );

    try
    {
        Stopwatch stopwatchTime;

        stopwatchTime.start();

        _connectToServer(client, outPrintWriter);

        stopwatchTime.stop();

        double connectTime = stopwatchTime.getElapsed();

        stopwatchTime.reset();
        stopwatchTime.start();

        if (_invokeProviderModuleMethod(MODULENAME, CIMName("stop"),
                                        outPrintWriter, errPrintWriter))
        {
            _invokeProviderModuleMethod(MODULENAME, CIMName("start"),
                                        outPrintWriter, errPrintWriter);
        }

        stopwatchTime.stop();

        double _unloadModuleTime = stopwatchTime.getElapsed();

        stopwatchTime.reset();
        stopwatchTime.start();

        CIMObjectPath reference =
           benchmarkTestCommand::_buildObjectPath(className, CIMValue(99));
        CIMInstance cimInstance = client.getInstance(NAMESPACE, reference);

        CIMObjectPath instanceRef = cimInstance.getPath();
        if ( !(instanceRef.getClassName().equal(className)))
        {
            outPrintWriter << "Returned ClassName = " << instanceRef.getClassName().getString() << endl;
            outPrintWriter << "Expected ClassName = " << className.getString() << endl;
            errorExit(errPrintWriter, "getInstance failed.  Incorrect class name returned.");
        }

        stopwatchTime.stop();

        double elapsedTime = stopwatchTime.getElapsed();

        outPrintWriter << testID << ": Benchmark Test #2: Load Provider Test on class "
                       << className.getString() << endl;
        outPrintWriter << "Connect time = " << connectTime << endl;
        outPrintWriter << "Unload Module time = " << _unloadModuleTime << endl;
        outPrintWriter << "First getInstance request processed in "
                       << elapsedTime << " Seconds "
                       << endl << endl;

        client.disconnect();

    }  // end try

    catch(const Exception& e)
    {
      errorExit(errPrintWriter, e.getMessage());
    }
}

void benchmarkTestCommand::dobenchmarkTest3(
                              Uint32 testID,
                              const CIMName& className,
                              ostream& outPrintWriter,
                              ostream& errPrintWriter)
{
    CIMClient client;
    client.setTimeout( _timeout );

    try
    {
        Stopwatch stopwatchTime;

        stopwatchTime.reset();
        stopwatchTime.start();

        _connectToServer( client, outPrintWriter);

        stopwatchTime.stop();

        double connectTime = stopwatchTime.getElapsed();

        Uint32 numberOfProperties;
        Uint32 sizeOfPropertyValue;
        Uint32 expectedNumberOfInstances;

        test.getConfiguration(className, numberOfProperties,
                 sizeOfPropertyValue, expectedNumberOfInstances);

        stopwatchTime.reset();
        stopwatchTime.start();

        CIMObjectPath reference =
            benchmarkTestCommand::_buildObjectPath(className, CIMValue(99));

        for (Uint32 i = 0; i < _iterations; i++)
        {
            CIMInstance cimInstance = client.getInstance(NAMESPACE, reference);
            CIMObjectPath instanceRef = cimInstance.getPath();
            if ( !(instanceRef.getClassName().equal(className)))
            {
                outPrintWriter << "Returned ClassName = " << instanceRef.getClassName().getString() << endl;
                outPrintWriter << "Expected ClassName = " << className.getString() << endl;
                errorExit(errPrintWriter, "getInstance failed. Incorrect class name returned.");
            }

            if ( cimInstance.getPropertyCount() != numberOfProperties+1)
            {
                errorExit(errPrintWriter, "getInstance failed. Incorrect number of properties returned.");
            }
        }

        stopwatchTime.stop();

        double elapsedTime = stopwatchTime.getElapsed();

        outPrintWriter << testID << ": Benchmark Test #3: getInstance Test on "
                       << className.getString() << endl;
        outPrintWriter << "Connect time = " << connectTime << endl;
        outPrintWriter << "Number of Non-Key Properties Returned  = " << numberOfProperties << endl
                       << "Size of Each Non-Key Property Returned  = " << sizeOfPropertyValue << endl
                       << "Number of Instances Returned = " << 1 << endl;
        outPrintWriter << _iterations << " requests processed in "
                       << elapsedTime << " Seconds "
                       << "(Average Elapse Time = " << elapsedTime/_iterations
                       << ")" << endl << endl;

        client.disconnect();

    }  // end try

    catch(const Exception& e)
    {
      errorExit(errPrintWriter, e.getMessage());
    }
}

void benchmarkTestCommand::dobenchmarkTest4(
                              Uint32 testID,
                              const CIMName& className,
                              ostream& outPrintWriter,
                              ostream& errPrintWriter)
{

    CIMClient client;
    client.setTimeout( _timeout );

    try
    {
        Stopwatch stopwatchTime;

        stopwatchTime.reset();
        stopwatchTime.start();

        _connectToServer( client, outPrintWriter);

        stopwatchTime.stop();

        double connectTime = stopwatchTime.getElapsed();

        Uint32 numberOfProperties;
        Uint32 sizeOfPropertyValue;
        Uint32 expectedNumberOfInstances;

        test.getConfiguration(className, numberOfProperties,
                 sizeOfPropertyValue, expectedNumberOfInstances);

        Uint32 numberInstances;

        stopwatchTime.reset();
        stopwatchTime.start();

        for (Uint32 i = 0; i < _iterations; i++)
        {
           Array<CIMObjectPath> cimInstanceNames =
                  client.enumerateInstanceNames(NAMESPACE, className);

           numberInstances = cimInstanceNames.size();
           if (numberInstances != expectedNumberOfInstances)
           {
              errorExit(errPrintWriter, "enumerateInstanceNames failed. Incorrect number of instances returned.");
           }

           for (Uint32 j = 0; j < numberInstances; j++)
           {
              if ( !(cimInstanceNames[j].getClassName().equal(className)))
              {
                 errorExit(errPrintWriter, "enumerateInstanceNames failed. Incorrect class name returned.");
              }

            }   // end for looping through instances
        }

        stopwatchTime.stop();

        double elapsedTime = stopwatchTime.getElapsed();

        outPrintWriter << testID << ": Benchmark Test #4: enumerateInstanceNames Test on class "
                       << className.getString() << endl;
        outPrintWriter << "Connect time = " << connectTime << endl;
        outPrintWriter << "Number of Non-Key Properties Returned  = " << 0 << endl
                       << "Number of Instances Returned = " << expectedNumberOfInstances << endl;
        outPrintWriter << _iterations << " requests processed in "
                       << elapsedTime << " Seconds "
                       << "(Average Elapse Time = " << elapsedTime/_iterations
                       << ")" << endl << endl;

        client.disconnect();
    }  // end try

    catch(const Exception& e)
    {
      errorExit(errPrintWriter, e.getMessage());
    }

}

void benchmarkTestCommand::dobenchmarkTest5(
                              Uint32 testID,
                              const CIMName& className,
                              ostream& outPrintWriter,
                              ostream& errPrintWriter)
{

    CIMClient client;
    client.setTimeout( _timeout );

    try
    {
        Stopwatch stopwatchTime;

        stopwatchTime.reset();
        stopwatchTime.start();

        _connectToServer(client, outPrintWriter);

        stopwatchTime.stop();

        double connectTime = stopwatchTime.getElapsed();

        Uint32 numberOfProperties;
        Uint32 sizeOfPropertyValue;
        Uint32 expectedNumberOfInstances;

        test.getConfiguration(className, numberOfProperties,
                 sizeOfPropertyValue, expectedNumberOfInstances);

        Uint32 numberInstances;

        stopwatchTime.reset();
        stopwatchTime.start();

        Boolean deepInheritance = true;
        Boolean localOnly = false;
        Boolean includeQualifiers = false;
        Boolean includeClassOrigin = false;

        for (Uint32 i = 0; i < _iterations; i++)
        {
           Array<CIMInstance> cimNInstances =
                  client.enumerateInstances(NAMESPACE, className,
                                         deepInheritance,
                                         localOnly,  includeQualifiers,
                                         includeClassOrigin );

           numberInstances = cimNInstances.size();
           if (numberInstances != expectedNumberOfInstances)
           {
              errorExit(errPrintWriter, "enumerateInstances failed. Incorrect number of instances returned.");
           }

           for (Uint32 j = 0; j < numberInstances; j++)
           {
              CIMObjectPath instanceRef = cimNInstances[j].getPath ();
              if ( !(instanceRef.getClassName().equal(className)))
              {
                 errorExit(errPrintWriter, "enumerateInstances failed. Incorrect class name returned.");
              }

              if ( cimNInstances[j].getPropertyCount() != numberOfProperties+1)
              {
                 errorExit(errPrintWriter, "enumerateInstances failed. Incorrect number of properties returned.");
              }

            }   // end for looping through instances
        }

        stopwatchTime.stop();

        double elapsedTime = stopwatchTime.getElapsed();

        outPrintWriter << testID << ": Benchmark Test #5: enumerateInstances Test on class "
                       << className.getString() << endl;
        outPrintWriter << "Connect time = " << connectTime << endl;
        outPrintWriter << "Number of Non-Key Properties Returned  = " << numberOfProperties << endl
                       << "Size of Each Non-Key Property Returned  = " << sizeOfPropertyValue << endl
                       << "Number of Instances Returned = " << expectedNumberOfInstances << endl;
        outPrintWriter << _iterations << " requests processed in "
                       << elapsedTime << " Seconds "
                       << "(Average Elapse Time = " << elapsedTime/_iterations
                       << ")" << endl << endl;

        client.disconnect();
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
Uint32 benchmarkTestCommand::execute (ostream& outPrintWriter,
                                      ostream& errPrintWriter)
{

     benchmarkDefinition test;

    try
    {
        if (_debugOption1)
        {
          Tracer::setTraceFile("benchmarkTest.trc");
          Tracer::setTraceComponents("ALL");
          Tracer::setTraceLevel(Tracer::LEVEL4);
        }

    Uint32 testID = 0;

    if (_generateReport)
    {
           benchmarkTestCommand::_getSystemConfiguration(outPrintWriter, errPrintWriter);
        }

        benchmarkTestCommand::_getTestConfiguration(outPrintWriter, errPrintWriter);

    testID++;
    if (!_testIDSet || (testID == _testID))
    {
           benchmarkTestCommand::dobenchmarkTest1(testID, outPrintWriter, errPrintWriter);
        }

    testID++;
    if (!_testIDSet || (testID == _testID))
    {
           benchmarkTestCommand::dobenchmarkTest2(testID, _testClassNames[0],
                                                  outPrintWriter, errPrintWriter);
        }

        for (Uint32 i = 0, n = _testClassNames.size(); i < n; i++)

    {
        testID++;
        if (!_testIDSet || (testID == _testID))
        {
                benchmarkTestCommand::dobenchmarkTest3(testID, _testClassNames[i],
                                                  outPrintWriter, errPrintWriter);
            }

        testID++;
        if (!_testIDSet || (testID == _testID))
        {
                benchmarkTestCommand::dobenchmarkTest4(testID, _testClassNames[i],
                                                  outPrintWriter, errPrintWriter);
            }

        testID++;
        if (!_testIDSet || (testID == _testID))
        {
                benchmarkTestCommand::dobenchmarkTest5(testID, _testClassNames[i],
                                                  outPrintWriter, errPrintWriter);
            }
        }

    }
    catch (const benchmarkTestException& e)
    {
      errPrintWriter << benchmarkTestCommand::COMMAND_NAME << ": " <<
            e.getMessage () << endl;
      return (RC_ERROR);
    }

    catch (const Exception& e)
    {
      errPrintWriter << benchmarkTestCommand::COMMAND_NAME << ": " <<
            e.getMessage () << endl;
      return (RC_ERROR);
    }

    catch (const exception& e)
    {
      errPrintWriter << benchmarkTestCommand::COMMAND_NAME << ": " <<
            e.what () << endl;
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
    benchmarkTestCommand    command = benchmarkTestCommand ();
    int                rc;

    try
    {
        command.setCommand (argc, argv);
    }
    catch (const CommandFormatException& cfe)
    {
        cerr << benchmarkTestCommand::COMMAND_NAME << ": " << cfe.getMessage ()
             << endl;
        cerr << command.getUsage () << endl;
        exit (Command::RC_ERROR);
    }

    rc = command.execute (cout, cerr);
    return (rc);
}
