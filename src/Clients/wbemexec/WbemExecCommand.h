//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM, 
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
// Author: Carol Ann Krug Graves, Hewlett-Packard Company 
//         (carolann_graves@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_WbemExecCommand_h
#define Pegasus_WbemExecCommand_h

#include <Pegasus/Common/String.h>
#include <Clients/cliutils/Command.h>
#include <Clients/cliutils/CommandException.h>
#include "WbemExecException.h"

#if defined(PEGASUS_PLATFORM_LINUX_IX86_GNU)
char * ultostr(unsigned long int ulint, int width)
{
    char * retval = NULL;
    asprintf(&retval,"%*ld", width, ulint); // allocates buffer automatically
    return retval;
}
#endif

PEGASUS_NAMESPACE_BEGIN

/**
  
    WbemExecCommand provides a command line interface to the CIM Server.
    The input to the command consists of a CIM request encoded in XML.  The 
    request is submitted to the CIM Server for execution.  The result of the 
    operation is returned to stdout, and consists of the CIM response encoded 
    in XML.
  
    @author  Hewlett-Packard Company
  
 */
class WbemExecCommand : public Command 
{
public:

    /**
      
        Constructs a WbemExecCommand and initializes instance variables.
      
     */
    WbemExecCommand ();

    /**
        
        Parses the command line, validates the options, and sets instance 
        variables based on the option arguments.
      
        @param   argc  the number of command line arguments
        @param   argv  the string vector of command line arguments
      
        @exception  CommandFormatException  if an error is encountered in 
                                            parsing the command line
      
     */
    void setCommand (Uint32 argc, char* argv []) throw (CommandFormatException);

    /**
        
        Executes the command and writes the results to the PrintWriters.
      
        @param   outPrintWriter     the ostream to which output should be
                                    written
        @param   errPrintWriter     the ostream to which error output should be
                                    written
      
        @return  0                  if the command is successful
                 1                  if an error occurs in executing the command
      
     */
    Uint32 execute (ostream& outPrintWriter, ostream& errPrintWriter);

    /**
        The command name.
     */
    static const char   COMMAND_NAME [];
    
private:

    /**
        
        Creates a channel for an HTTP connection.
      
        @param   outPrintWriter     the ostream to which error output should be
                                    written
      
        @return  the Channel created
      
        @exception  WbemExecException  if an error is encountered in creating
                                       the connection 
      
     */
    Channel* _getHTTPChannel (ostream& outPrintWriter) 
        throw (WbemExecException);

    /**
        
        Executes the command using HTTP.  A CIM request encoded in XML is read
        from the input, and encapsualted in an HTTP request message.  A channel
        is obtained for an HTTP connection, and the message is written to the
        channel.  The response is written to the specified outPrintWriter, and
        consists of the CIM response encoded in XML.  
      
        @param   outPrintWriter     the ostream to which output should be
                                    written
        @param   errPrintWriter     the ostream to which error output should be
                                    written
      
        @exception  WbemExecException  if an error is encountered in executing
                                       the command 
      
     */
    void _executeHttp (ostream& outPrintWriter, ostream& errPrintWriter) 
        throw (WbemExecException);

    /**
        The host on which the command is to be executed.  A CIM Server must be
        running on this host when the command is executed.  The default host is
        the local host.
     */
    String _hostName;

    /**
        A Boolean indicating whether a hostname was specified on the command 
        line.  The default host is the local host.
     */
    Boolean _hostNameSet;

    /**
        The port to be used when the command is executed, specified as a
        string.  The port number must be the port number on which the CIM 
        Server is running on the specified host.  The default port number 
        is 5988.
     */
    String _portNumberStr;

    /**
        The port to be used when the command is executed.  The port number must
        be the port number on which the CIM Server is running on the specified
        host.  The default port number is 5988.
     */
    Uint32 _portNumber;

    /**
        Indicates that an HTTP/1.1 version request should be generated.  If 
        true, an HTTP/1.1 request is generated.  Otherwise, an HTTP/1.0 request
        is generated.  
     */
    Boolean _useHTTP11;

    /**
        Indicates that the HTTP M-POST method should be used for the request.  
        If true, an HTTP M-POST request is generated.  Otherwise, an HTTP POST
        request is generated.  
     */
    Boolean _useMPost;

    /**
        The timeout value to be used in milliseconds.
        The default timeout value is 20000.
     */
    Uint32 _timeout;

    /**
        Indicates that debug output is requested.  If true, the output of
        wbemexec includes the HTTP encapsulation of the original XML request.
     */
    Boolean _debugOutput1;

    /**
        Indicates that debug output is requested.  If true, the output of
        wbemexec includes the HTTP encapsulation of the XML response.
        Otherwise, only the XML response (if any) is included in the output of
        wbemexec.
     */
    Boolean _debugOutput2;

    /**
        The username to be used for authorization of the operation.
     */
    String _userName;

    /**
        The password to be used for authorization of the operation.
     */
    String _password;

    /**
        The path of the input file containing a CIM request encoded in XML.
        By default, stdin is used as the input.
     */
    String _inputFilePath;

    /**
        A Boolean indicating whether an input file path was specified on the 
        command line.
     */
    Boolean _inputFilePathSet;

    /**
        The option character used to specify the hostname.
     */
    static const char   _OPTION_HOSTNAME;

    /**
        The option character used to specify the port number.
     */
    static const char   _OPTION_PORTNUMBER;

    /**
        The option character used to specify the HTTP version for the request.
     */
    static const char   _OPTION_HTTPVERSION;

    /**
        The option character used to specify the HTTP method for the request.
     */
    static const char   _OPTION_HTTPMETHOD;

    /**
        The option character used to specify the timeout value.
     */
    static const char   _OPTION_TIMEOUT;

    /**
        The option character used to specify that debug output is requested.
     */
    static const char   _OPTION_DEBUG;

    /**
        The option character used to specify the username.
     */
    static const char   _OPTION_USERNAME;

    /**
        The option character used to specify the password.
     */
    static const char   _OPTION_PASSWORD;

    /**
        Label for the usage string for this command.
     */
    static const char   _USAGE [];
    
    /**
        The minimum valid portnumber.
     */
    static const Uint32 _MIN_PORTNUMBER;

    /**
        The maximum valid portnumber.
     */
    static const Uint32 _MAX_PORTNUMBER;

    /**
        The integer representing the default port number.
     */
    static const Uint32 _DEFAULT_PORT;

    /**
        The debug option argument value used to specify that the HTTP 
        encapsulation of the original XML request be included in the output.
     */
    static const char   _DEBUG_OPTION1;

    /**
        The debug option argument value used to specify that the HTTP 
        encapsulation of the XML response be included in the output.
     */
    static const char   _DEBUG_OPTION2;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_WbemExecCommand_h */
