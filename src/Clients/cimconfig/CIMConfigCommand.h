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
// Author: Nag Boranna (nagaraja_boranna@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


#ifndef Pegasus_CIMConfigCommand_h
#define Pegasus_CIMConfigCommand_h

#include <iostream>
#include <Pegasus/Common/String.h>
#include <Clients/cliutils/Command.h>
#include <Clients/cliutils/CommandException.h>
#include <Pegasus/Config/UnrecognizedPropertyException.h>
#include <Pegasus/Config/InvalidPropertyValueException.h>


/**
This is a configuration CLI used to manage the configuration of the
CIM Server.  This command supports operations to get, set, unset,
and list properties.  The APIs in the ConfigFile class are used to
access the CIM Server configuration file.

@author Nag Boranna, Hewlett-Packard Company
*/

class CIMConfigCommand : public Command 
{

public:

    /**    
        Constructs a CIMConfigCommand and initializes instance variables.
    */
    CIMConfigCommand ();

    /**
    Parses the command line, validates the options, and sets instance 
    variables based on the option arguments.

    @param   args  the string array containing the command line arguments
    @param   argc  the int containing the arguments count

    @throws  CommandFormatException  if an error is encountered in parsing
                                     the command line
    */
    void setCommand (Uint32 argc, char* argv []) throw (CommandFormatException);

    /**
    Executes the command and writes the results to the output streams.

    @param ostream    The stream to which command output is written.
    @param ostream    The stream to which command errors are written.
    @return  0      if the command is successful
             1      if an error occurs in executing the command
    */
    Uint32 execute (PEGASUS_STD(ostream)& outPrintWriter,
        PEGASUS_STD(ostream)& errPrintWriter);


private:

    /**
    Get property values for the specified property from the CIM Server.

    @param ostream        The stream to which command output is written.
    @param ostream        The stream to which command errors are written.
    @param propertyName The name of the property to get.
    @param propertyValues A string array to store the values.

    @exception CIMException  if failed to get the properties from CIMOM.
    */
    void _getPropertiesFromCIMServer
        (
        ostream&    outPrintWriter, 
        ostream&    errPrintWriter,
        const String&    propertyName,
        Array <String>&    propertyValues
        ) throw (CIMException);

    /**
    Send an updated property value to the CIM Server.
    
    @param ostream          The stream to which command output is written.
    @param ostream          The stream to which command errors are written.
    @param propertyName   The name of the property to update.
    @param propertyValue  The new value of the property.

    @exception CIMException  if failed to get the properties from CIMOM.
    */
    void _updatePropertyInCIMServer
        (
        ostream&    outPrintWriter, 
        ostream&    errPrintWriter,
        const String&   propertyName,
        const String&   propertyValue
        ) throw (CIMException);

    /**
    Get a list of all property names and value from the CIM Server.

    @param propertyNames   The string array to store property names.
    @param propertyValues  The string array to store property values.
    */
    void _listAllPropertiesInCIMServer
    (
        Array <String>&   propertyNames,
        Array <String>&   propertyValues
    );

    /**
        Configuration File handler
    */
    ConfigFileHandler* _configFileHandler;

    /**
    The CIM Client reference
    */
    CIMClient*    _client;

    /**
    The host name. 
    */
    String        _hostName;

    /**
    The name of a config property. 
    */
    String        _propertyName;

    /**
    The value of a config property. 
    */
    String        _propertyValue;

    /**
    The flag to indicate whether the default value is set or not. 
    */
    Boolean       _defaultValueSet;

    /**
    The flag to indicate whether the current value is set or not. 
    */
    Boolean       _currentValueSet;

    /**
    The flag to indicate whether the planned value is set or not. 
    */
    Boolean       _plannedValueSet;

    /**
    The type of operation specified on the command line. 
    */
    Uint32        _operationType;

};

#endif /* Pegasus_CIMConfigCommand_h */
